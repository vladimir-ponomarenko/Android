#include <jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <poll.h>
#include <pthread.h>
#include <endian.h>

#include "consts.h"
#include "diagcfgwr.h"
#include "log_packet.h"
#include "hdlc.h"
#include "ws_wrap.h"

#define  LOG_TAG    "DiagRevealerCPP"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define DIAG_IOCTL_SWITCH_LOGGING	7
#define DIAG_IOCTL_REMOTE_DEV		32

#define MEMORY_DEVICE_MODE		2

#define USER_SPACE_DATA_TYPE	0x00000020

#define DIAG_CON_APSS		(0x0001)
#define DIAG_CON_MPSS		(0x0002)
#define DIAG_CON_LPASS		(0x0004)
#define DIAG_CON_WCNSS		(0x0008)
#define DIAG_CON_SENSORS	(0x0010)
#define DIAG_CON_ALL		(DIAG_CON_APSS | DIAG_CON_MPSS \
				| DIAG_CON_LPASS | DIAG_CON_WCNSS \
				| DIAG_CON_SENSORS)
#define DIAG_MD_LOCAL		0

#define BUFFER_SIZE	65536

std::atomic<int> fd_atomic(-1);
char buf_read[BUFFER_SIZE] = {};
char buf_send[BUFFER_SIZE] = {};
volatile bool must_stop = false;
std::atomic<bool> worker_thread_finished(false);
volatile int worker_thread_result = -1;
volatile bool epan_initialized = false;
static uint16_t remote_dev = 0;

struct diag_logging_mode_param_t {
    uint32_t req_mode;
    uint32_t peripheral_mask;
    uint8_t mode_param;
} __attribute__((packed));

struct diag_logging_mode_param_t_pie {
    uint32_t req_mode;
    uint32_t peripheral_mask;
    uint32_t pd_mask;
    uint8_t mode_param;
    uint8_t diag_id;
    uint8_t pd_val;
    uint8_t reserved;
    int peripheral;
} __attribute__((packed));

struct diag_logging_mode_param_t_q {
    uint32_t req_mode;
    uint32_t peripheral_mask;
    uint32_t pd_mask;
    uint8_t mode_param;
    uint8_t diag_id;
    uint8_t pd_val;
    uint8_t reserved;
    int peripheral;
    int device_mask;
} __attribute__((packed));

typedef struct {
    char *p;
    size_t len;
} binaryBuffer;


jobject NewInteger(JNIEnv* env, int value);
bool _diag_switch_logging(int fd_val, int log_mode);
void _gather_diag_data(JavaVM* jvm, jobject obj, int fd_val);
std::string decode_msg(std::string msgType, std::string msgData);
static binaryBuffer read_diag_cfg (const char *filename);
static int write_commands (int fd_val, binaryBuffer *pbuf_write);
void handle_signal_interrupt(int sig);
jobjectArray CreateErrorReturnArray(JNIEnv *env, const char* errorMessage);

/*
 * Обрабатывает сигнал SIGUSR2 для прерывания потока.
 */
void handle_signal_interrupt(int sig) {
    if (sig == SIGUSR2) {
        LOGD("Caught signal SIGUSR2, attempting to exit thread.");
        pthread_exit((void*)-2);
    }
}

/*
 * Получает текущее POSIX время в миллисекундах.
 */
static unsigned long long get_posix_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long) tv.tv_sec * 1000L + tv.tv_usec / 1000;
}

/*
 * Читает содержимое файла конфигурации Diag в бинарный буфер.
 */
static binaryBuffer read_diag_cfg (const char *filename) {
    binaryBuffer ret = {nullptr, 0};
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        LOGE("Unable to open diag config '%s': %s", filename, strerror(errno));
        return ret;
    }

    fseek(fp, 0L, SEEK_END);
    long file_sz_long = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (file_sz_long <= 0 || file_sz_long > BUFFER_SIZE) {
        LOGE("Diag config file size inappropriate or error (%ld bytes)", file_sz_long);
        fclose(fp);
        return ret;
    }
    size_t file_sz = (size_t)file_sz_long;

    ret.p = (char *) malloc(file_sz);
    if (!ret.p) {
        LOGE("Failed to malloc %zu bytes for diag config", file_sz);
        fclose(fp);
        return ret;
    }

    size_t bytes_read = fread(ret.p, 1, file_sz, fp);
    if (bytes_read != file_sz) {
        LOGE("Error reading diag config '%s': expected %zu, got %zu. Error: %s",
             filename, file_sz, bytes_read, ferror(fp) ? strerror(errno) : "Short read");
        free(ret.p);
        ret.p = nullptr;
        ret.len = 0;
    } else {
        ret.len = bytes_read;
        LOGD("Reading diag config '%s' OK (%zu bytes)", filename, ret.len);
    }

    fclose(fp);
    return ret;
}

/*
 * Записывает команды из бинарного буфера в файловый дескриптор устройства Diag.
 */
static int write_commands (int fd_val, binaryBuffer *pbuf_write) {
    worker_thread_finished = false;
    worker_thread_result = -1;

    size_t i = 0;
    char *p = pbuf_write->p;
    int write_ret = 0;
    int read_ret = 0;

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_signal_interrupt;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR2);
    act.sa_flags = 0;
    sigaction(SIGUSR2, &act, NULL);

    size_t offset = remote_dev ? 8 : 4;
    size_t send_buf_size = BUFFER_SIZE + offset;
    char *send_buf = (char *) malloc(send_buf_size);
    if (!send_buf) {
        LOGE("malloc failed in write_commands (%zu bytes)", send_buf_size);
        worker_thread_result = -1;
        worker_thread_finished = true;
        return -1;
    }

    *((uint32_t *)send_buf) = htole32(USER_SPACE_DATA_TYPE);
    if(remote_dev) {
        *((int32_t *)(send_buf + 4)) = htole32(-remote_dev);
    }

    while (i < pbuf_write->len) {
        size_t cmd_len = 0;
        while (i + cmd_len < pbuf_write->len && p[i + cmd_len] != 0x7e) cmd_len++;
        if (i + cmd_len >= pbuf_write->len) break;
        cmd_len++;

        if (cmd_len >= 3) {
            if (cmd_len + offset > send_buf_size) {
                LOGE("write_commands: Command too large (%zu bytes)", cmd_len);
                i += cmd_len;
                continue;
            }
            memcpy(send_buf + offset, p + i, cmd_len);

            write_ret = write(fd_val, (const void *) send_buf, cmd_len + offset);

            if (write_ret < 0) {
                LOGE("write_commands error writing: %s (errno %d)", strerror(errno), errno);
                free(send_buf);
                worker_thread_result = -1;
                worker_thread_finished = true;
                return -1;
            }

            usleep(10000);
            int flags = fcntl(fd_val, F_GETFL, 0);
            fcntl(fd_val, F_SETFL, flags | O_NONBLOCK);
            read_ret = read(fd_val, buf_read, sizeof(buf_read));
            fcntl(fd_val, F_SETFL, flags);

            if (read_ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                LOGE("write_commands error reading response: %s (errno %d)", strerror(errno), errno);
            } else if (read_ret > 0) {
                LOGD("Read %d bytes of response after command write", read_ret);
            }
        }
        i += cmd_len;
    }

    LOGD("write_commands: Finished writing commands.");
    free(send_buf);
    worker_thread_result = 0;
    worker_thread_finished = true;
    return 0;
}

/*
 * Переключает режим логирования Diag, пробуя различные структуры ioctl для совместимости с разными версиями Android.
 */
bool _diag_switch_logging(int fd_val, int log_mode) {
    int ret = -1;

    LOGD("_diag_switch_logging: Trying sequential ioctl calls, starting with newest.");

    LOGD("Trying Android 10/11 struct (q)");
    struct diag_logging_mode_param_t_q mode_q;
    memset(&mode_q, 0, sizeof(mode_q));
    mode_q.req_mode = log_mode;
    mode_q.peripheral_mask = DIAG_CON_ALL;
    mode_q.pd_mask = 0;
    mode_q.mode_param = 1;
    mode_q.diag_id = 0;
    mode_q.pd_val = 0;
    mode_q.peripheral = 0;
    mode_q.device_mask = (1 << DIAG_MD_LOCAL);
    ret = ioctl(fd_val, DIAG_IOCTL_SWITCH_LOGGING, &mode_q);
    if (ret >= 0) {
        LOGI("Android 10/11 ioctl (struct q) succeeded (ret=%d).", ret);
        return true;
    }
    LOGW("Android 10/11 ioctl (struct q) failed: %s (errno %d)", strerror(errno), errno);


    LOGD("Trying Android 9 struct (pie)");
    struct diag_logging_mode_param_t_pie mode_pie;
    memset(&mode_pie, 0, sizeof(mode_pie));
    mode_pie.req_mode = log_mode;
    mode_pie.peripheral_mask = DIAG_CON_ALL;
    mode_pie.pd_mask = 0;
    mode_pie.mode_param = 0;
    ret = ioctl(fd_val, DIAG_IOCTL_SWITCH_LOGGING, &mode_pie);
    if (ret >= 0) {
        LOGI("Android 9 ioctl (struct pie) succeeded (ret=%d).", ret);
        return true;
    }
    LOGW("Android 9 ioctl (struct pie) failed: %s (errno %d)", strerror(errno), errno);


    LOGD("Trying Android 7/8 struct (t/nougat)");
    struct diag_logging_mode_param_t mode_nougat;
    memset(&mode_nougat, 0, sizeof(mode_nougat));
    mode_nougat.req_mode = log_mode;
    mode_nougat.peripheral_mask = DIAG_CON_ALL;
    mode_nougat.mode_param = 0;
    ret = ioctl(fd_val, DIAG_IOCTL_SWITCH_LOGGING, &mode_nougat);
    if (ret >= 0) {
        LOGI("Android 7/8 ioctl (struct t) succeeded (ret=%d).", ret);
        return true;
    }
    LOGW("Android 7/8 ioctl (struct t) failed: %s (errno %d)", strerror(errno), errno);


    LOGD("Trying Android 6 (pointer to int)");
    int mode_m = log_mode;
    ret = ioctl(fd_val, DIAG_IOCTL_SWITCH_LOGGING, &mode_m);
    if (ret >= 0) {
        LOGI("Android 6 (pointer) ioctl succeeded (ret=%d).", ret);
        return true;
    }
    LOGW("Android 6 (pointer) ioctl failed: %s (errno %d)", strerror(errno), errno);


    LOGD("Trying direct long/int mode (S5)");
    ret = ioctl(fd_val, DIAG_IOCTL_SWITCH_LOGGING, (long)log_mode);
    if (ret >= 0) {
        LOGI("Direct long/int ioctl succeeded (ret=%d).", ret);
        return true;
    }
    LOGW("Direct long/int ioctl failed: %s (errno %d)", strerror(errno), errno);

    LOGE("_diag_switch_logging: All attempts failed.");
    return false;
}

/*
 * Основная функция потока для чтения данных из /dev/diag, обработки пакетов и передачи их в Java через JNI callback.
 */
void _gather_diag_data(JavaVM* jvm, jobject obj, int fd_val) {
    JNIEnv *env = nullptr;
    jclass cls = nullptr;
    jmethodID mid = nullptr;
    bool attached = false;
    bool first_read_done = false;

    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        if (jvm->AttachCurrentThread(&env, NULL) != 0) { LOGE("_gather_diag_data: Failed to attach JNI thread"); goto exit_thread; }
        attached = true;
    }
    cls = env->GetObjectClass(obj);
    if(!cls){ LOGE("_gather_diag_data: unable to get Java object class!"); goto exit_thread_detach; }
    mid = env->GetMethodID(cls, "logRevealer", "([B[B)V");
    if(!mid){ LOGE("_gather_diag_data: unable to get callback method!"); env->DeleteLocalRef(cls); goto exit_thread_detach; }
    env->DeleteLocalRef(cls);
    if (attached) { jvm->DetachCurrentThread(); attached = false; }

    LOGD("_gather_diag_data: Starting log reading loop (blocking read).");

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_signal_interrupt;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR2);
    act.sa_flags = 0;
    sigaction(SIGUSR2, &act, NULL);

    while(!must_stop) {

        int read_len = read(fd_val, buf_read, sizeof(buf_read));

        if (read_len < 0) {
            if (errno == EINTR) {
                LOGD("_gather_diag_data: read interrupted (EINTR), checking stop flag...");
                continue;
            }
            LOGE("_gather_diag_data: read error: %s (errno %d)", strerror(errno), errno);
            break;
        }

        if (read_len == 0) {
            usleep(100000);
            continue;
        }

        if (!first_read_done && read_len > 4) {
            uint32_t packet_type = 0;
            memcpy(&packet_type, buf_read, sizeof(uint32_t));
            LOGD("_gather_diag_data: First packet received (len=%d), type=0x%X (%u)", read_len, packet_type, packet_type);
            first_read_done = true;
        }

        uint32_t current_packet_type = 0;
        if (read_len >= 4) memcpy(&current_packet_type, buf_read, sizeof(uint32_t));

        if (current_packet_type == USER_SPACE_DATA_TYPE) {
            int num_data = 0;
            if (read_len >= 8) memcpy(&num_data, buf_read + 4, sizeof(int));
            else { LOGW("_gather_diag_data: Short USER_SPACE_DATA_TYPE packet (%d bytes)", read_len); continue; }

            if (num_data < 0 || num_data > 10000) {
                LOGE("_gather_diag_data: Invalid num_data = %d. Skipping packet.", num_data);
                continue;
            }

            long long current_offset = remote_dev ? 12 : 8;
            int send_buf_offset = 0;

            for (int i = 0; i < num_data; i++) {
                if (current_offset + sizeof(int) > read_len) {
                    LOGW("_gather_diag_data: Packet ended reading msg_len for sub %d/%d", i + 1, num_data);
                    break;
                }
                int msg_len = 0;
                memcpy(&msg_len, buf_read + current_offset, sizeof(int));

                if (msg_len <= 0) {
                    LOGW("_gather_diag_data: Invalid msg_len (%d) for sub %d/%d", msg_len, i + 1, num_data);
                    current_offset += sizeof(int);
                    continue;
                }
                if (current_offset + sizeof(int) + msg_len > read_len) {
                    LOGW("_gather_diag_data: Packet ended reading payload for sub %d/%d (len %d)", i + 1, num_data, msg_len);
                    break;
                }
                if (send_buf_offset + msg_len > BUFFER_SIZE) {
                    LOGE("_gather_diag_data: buf_send overflow! Dropping sub %d/%d", i + 1, num_data);
                    break;
                }

                memcpy(buf_send + send_buf_offset, buf_read + current_offset + sizeof(int), msg_len);
                send_buf_offset += msg_len;
                current_offset += sizeof(int) + msg_len;
            }


            if (send_buf_offset > 0) {
                char buf_head[12] = {};
                short head_offset = 0;
                uint16_t fifo_msg_type = 1;
                uint16_t fifo_msg_len = (send_buf_offset > UINT16_MAX - 8) ? UINT16_MAX : (uint16_t)(send_buf_offset + 8);
                if (send_buf_offset + 8 > UINT16_MAX) {
                    LOGW("_gather_diag_data: Combined length %d exceeds uint16_t limit!", send_buf_offset + 8);
                }
                unsigned long long ts = get_posix_timestamp();
                uint64_t ts_le = htole64(ts);

                buf_head[head_offset++] = (fifo_msg_type & 0xFF);
                buf_head[head_offset++] = ((fifo_msg_type >> 8) & 0xFF);
                buf_head[head_offset++] = (fifo_msg_len & 0xFF);
                buf_head[head_offset++] = ((fifo_msg_len >> 8) & 0xFF);
                memcpy(buf_head + head_offset, &ts_le, sizeof(ts_le));
                head_offset += sizeof(ts_le);


                if (jvm->AttachCurrentThread(&env, NULL) == 0) {
                    attached = true;
                    jbyteArray headerArray = env->NewByteArray(head_offset);
                    jbyteArray dataArray = env->NewByteArray(send_buf_offset);
                    if (headerArray && dataArray) {
                        env->SetByteArrayRegion(headerArray, 0, head_offset, (const jbyte *) buf_head);
                        env->SetByteArrayRegion(dataArray, 0, send_buf_offset, (const jbyte *) buf_send);
                        env->CallVoidMethod(obj, mid, headerArray, dataArray);
                    } else { LOGE("_gather_diag_data: Failed to create JNI byte arrays"); }
                    if (headerArray) env->DeleteLocalRef(headerArray);
                    if (dataArray) env->DeleteLocalRef(dataArray);
                    jvm->DetachCurrentThread();
                    attached = false;
                } else { LOGE("_gather_diag_data: Failed to attach JNI thread for callback"); }

            } else if (num_data > 0) {
                LOGW("_gather_diag_data: Processed USER_SPACE_DATA_TYPE packet with %d subpackets but sent 0 bytes to Java.", num_data);
            }

        } else {
            LOGW("_gather_diag_data: Received non-USER_SPACE_DATA_TYPE packet (len=%d, type=0x%X). Skipping.", read_len, current_packet_type);
        }

    }

    exit_thread_detach:
    if (attached) { jvm->DetachCurrentThread(); }
    exit_thread:
    LOGD("_gather_diag_data: Exiting thread.");
    worker_thread_result = (must_stop) ? 0 : -1;
    worker_thread_finished = true;
}


/*
 * JNI вспомогательная функция для создания объекта Java Integer из C++ int.
 */
jobject NewInteger(JNIEnv* env, int value) {
    jclass integerClass = env->FindClass("java/lang/Integer");
    if (!integerClass) return nullptr;
    jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
    if (!integerConstructor) {
        env->DeleteLocalRef(integerClass);
        return nullptr;
    }
    jobject result = env->NewObject(integerClass, integerConstructor, static_cast<jint>(value));
    env->DeleteLocalRef(integerClass);
    return result;
}

/*
 * JNI вспомогательная функция для создания массива объектов Java для возврата ошибки (код -1 и сообщение).
 */
jobjectArray CreateErrorReturnArray(JNIEnv *env, const char* errorMessage) {
    jobjectArray retArr = nullptr;
    jclass objClass = env->FindClass("java/lang/Object");
    if (!objClass) {
        LOGE("CreateErrorReturnArray: Failed to find Object class");
        return nullptr;
    }
    retArr = (jobjectArray)env->NewObjectArray(2, objClass, NULL);
    env->DeleteLocalRef(objClass);

    if (!retArr) {
        LOGE("CreateErrorReturnArray: Failed to create return array");
        return nullptr;
    }
    jobject jRetCode = NewInteger(env, -1);
    jstring jMsg = env->NewStringUTF(errorMessage);

    if (jRetCode) env->SetObjectArrayElement(retArr, 0, jRetCode);
    if (jMsg) env->SetObjectArrayElement(retArr, 1, jMsg);

    if (jRetCode) env->DeleteLocalRef(jRetCode);
    if (jMsg) env->DeleteLocalRef(jMsg);
    return retArr;
}

/*
 * JNI функция: инициализирует сессию чтения Diag логов, открывает устройство, настраивает его и запускает поток сбора данных.
 */
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_login_DiagRevealerControl_readDiag(
        JNIEnv *env,
        jobject obj, jstring diagConfigFilePath) {

    must_stop = false;
    worker_thread_finished = false;
    worker_thread_result = -1;

    int local_fd = -1;
    std::stringstream result_msg_stream;
    int final_ret_code = 0;

    const char * diagConfigPath = nullptr;
    binaryBuffer buf_write = {nullptr, 0};
    jobject myobj_global = nullptr;
    std::thread t1, t2;
    pthread_t t1h = 0, t2h = 0;

    jobjectArray retobjarr = (jobjectArray)env->NewObjectArray(2, env->FindClass("java/lang/Object"), NULL);
    if (!retobjarr) {
        LOGE("readDiag: Failed to create return array at start");
        return nullptr;
    }

    diagConfigPath = env->GetStringUTFChars( diagConfigFilePath , NULL );
    if (!diagConfigPath) {
        LOGE("readDiag: GetStringUTFChars failed for diagConfigFilePath");
        return CreateErrorReturnArray(env, "Failed to get config file path");
    }

    local_fd = open("/dev/diag", O_RDWR | O_LARGEFILE | O_NOCTTY);
    if (local_fd < 0) {
        final_ret_code = -1;
        result_msg_stream << "open /dev/diag failed: " << strerror(errno);
        LOGE("%s", result_msg_stream.str().c_str());
        env->ReleaseStringUTFChars(diagConfigFilePath, diagConfigPath);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }
    fd_atomic.store(local_fd);
    LOGD("readDiag: open /dev/diag OK (blocking mode), fd = %d", local_fd);

    remote_dev = 0;
    int ioctl_ret = ioctl(local_fd, DIAG_IOCTL_REMOTE_DEV, &remote_dev);
    if (ioctl_ret < 0) {
        LOGW("readDiag: ioctl DIAG_IOCTL_REMOTE_DEV failed: %s. Assuming MSM.", strerror(errno));
        remote_dev = 0;
    } else {
        LOGD("readDiag: ioctl DIAG_IOCTL_REMOTE_DEV OK: remote_dev = %u", remote_dev);
    }

    if (!_diag_switch_logging(local_fd, MEMORY_DEVICE_MODE)) {
        final_ret_code = -1;
        result_msg_stream << "Failed to switch diag logging mode";
        LOGE("%s", result_msg_stream.str().c_str());
        env->ReleaseStringUTFChars(diagConfigFilePath, diagConfigPath);
        close(local_fd); fd_atomic.store(-1);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }
    LOGD("readDiag: Switched logging mode OK.");

    buf_write = read_diag_cfg( diagConfigPath );
    env->ReleaseStringUTFChars(diagConfigFilePath, diagConfigPath);
    diagConfigPath = nullptr;

    if (buf_write.p == NULL || buf_write.len == 0) {
        final_ret_code = -1;
        result_msg_stream << "Reading Diag config file failed";
        LOGE("%s", result_msg_stream.str().c_str());
        close(local_fd); fd_atomic.store(-1);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }

    LOGD("readDiag: Starting write_commands thread...");
    try {
        t1 = std::thread(write_commands, local_fd, &buf_write);
        t1h = t1.native_handle();
    } catch (const std::system_error& e) {
        final_ret_code = -1;
        result_msg_stream << "Failed to create write_commands thread: " << e.what();
        LOGE("%s", result_msg_stream.str().c_str());
        free(buf_write.p);
        close(local_fd); fd_atomic.store(-1);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }


    worker_thread_finished = false;
    unsigned int retries_n = 60;
    unsigned int retry_t_us = 500000;
    while(retries_n > 0 && !worker_thread_finished) {
        usleep(retry_t_us);
        retries_n--;
    }

    if (!worker_thread_finished || worker_thread_result != 0) {
        final_ret_code = -1;
        if (retries_n <= 0 && !worker_thread_finished) {
            LOGE("readDiag: write_commands thread timed out.");
            result_msg_stream << "Writing Diag.cfg timed out. ";
            if(t1.joinable()) t1.detach();
        } else {
            LOGE("readDiag: write_commands thread finished with error (result=%d).", worker_thread_result);
            result_msg_stream << "Writing Diag.cfg failed (thread error). ";
            if(t1.joinable()) t1.join();
        }
        result_msg_stream << "Unable to continue.";
        free(buf_write.p);
        close(local_fd); fd_atomic.store(-1);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }

    if(t1.joinable()) t1.join();
    LOGD("readDiag: write_commands thread finished successfully.");
    free(buf_write.p);
    buf_write.p = nullptr;
    reset_binary();

    JavaVM* jvm = nullptr;
    env->GetJavaVM(&jvm);
    myobj_global = env->NewGlobalRef(obj);
    if (!myobj_global) {
        final_ret_code = -1;
        result_msg_stream << "Failed to create global JNI ref for callback.";
        LOGE("%s", result_msg_stream.str().c_str());
        close(local_fd); fd_atomic.store(-1);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }

    LOGD("readDiag: Starting _gather_diag_data thread...");
    worker_thread_finished = false;
    worker_thread_result = -1;
    try {
        t2 = std::thread(_gather_diag_data, jvm, myobj_global, local_fd);
        t2h = t2.native_handle();
    } catch (const std::system_error& e) {
        final_ret_code = -1;
        result_msg_stream << "Failed to create _gather_diag_data thread: " << e.what();
        LOGE("%s", result_msg_stream.str().c_str());
        env->DeleteGlobalRef(myobj_global);
        close(local_fd); fd_atomic.store(-1);
        return CreateErrorReturnArray(env, result_msg_stream.str().c_str());
    }


    LOGD("readDiag: Waiting for stop command (must_stop)...");
    while(!must_stop) {
        usleep(100000);
    }
    LOGD("readDiag: Stop command received.");
    result_msg_stream << "Stop command received.";

    retries_n = 100;
    retry_t_us = 50000;
    while(retries_n > 0 && !worker_thread_finished) {
        usleep(retry_t_us);
        retries_n--;
    }

    if(!worker_thread_finished) {
        LOGE("readDiag: _gather_diag_data thread timed out.");
        result_msg_stream << " Log reading thread timed out.";
        if(t2.joinable()) t2.detach();
    } else {
        result_msg_stream << " Log reading thread stopped (result=" << worker_thread_result << ").";
        LOGD("readDiag: _gather_diag_data thread finished (result=%d).", worker_thread_result);
        if (t2.joinable()) t2.join();
        if (worker_thread_result != 0) {
            final_ret_code = -1;
        }
    }

    if (myobj_global) {
        env->DeleteGlobalRef(myobj_global);
        myobj_global = nullptr;
    }
    if (fd_atomic.load() >= 0) {
        int current_fd = fd_atomic.exchange(-1);
        if (current_fd >= 0) {
            close(current_fd);
            LOGD("readDiag: Closed diag fd %d.", current_fd);
        }
    }


    jobject jFinalRetCode = NewInteger(env, final_ret_code);
    jstring jFinalMsg = env->NewStringUTF(result_msg_stream.str().c_str());

    if(jFinalRetCode) env->SetObjectArrayElement(retobjarr, 0, jFinalRetCode);
    if(jFinalMsg) env->SetObjectArrayElement(retobjarr, 1, jFinalMsg);

    if(jFinalRetCode) env->DeleteLocalRef(jFinalRetCode);
    if(jFinalMsg) env->DeleteLocalRef(jFinalMsg);

    LOGI("readDiag: Finished. Returning code %d, message: %s", final_ret_code, result_msg_stream.str().c_str());
    return retobjarr;
}

/*
 * JNI функция: сигнализирует потоку сбора данных о необходимости остановки.
 */
extern "C" JNIEXPORT void JNICALL
Java_com_example_login_DiagRevealerControl_stopDiag(
        JNIEnv *env,
        jobject obj) {
    LOGD("stopDiag called from Java.");
    if (!must_stop) {
        must_stop = true;
        int current_fd = fd_atomic.load();
        if (current_fd >= 0) {
            // Здесь можно добавить логику посылки сигнала потоку t2h, если он сохранен
            // pthread_kill(t2h, SIGUSR2);
        }
    }
}

/*
 * JNI функция: обрабатывает порцию сырых данных Diag, выделяет HDLC кадры, декодирует пакеты логов и возвращает результат в виде массива строк JSON.
 */
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_login_DiagRevealerControl_processLogChunk(
        JNIEnv *env,
        jobject obj,
        jbyteArray pktbytes){

    jbyte* payload_buf_ptr = env->GetByteArrayElements( pktbytes, NULL);
    if (!payload_buf_ptr) {
        LOGE("processLogChunk: GetByteArrayElements failed");
        return env->NewObjectArray(0, env->FindClass("java/lang/String"), NULL);
    }
    char* payload_buf = (char*) payload_buf_ptr;
    int buf_length = env->GetArrayLength(pktbytes);

    std::vector<std::string> out_json_data;
    feed_binary(payload_buf, buf_length);

    std::string frame;
    bool crc_correct = false;

    while(get_next_frame(frame, crc_correct)){
        if (!crc_correct) {
            LOGW("processLogChunk: wrong HDLC frame CRC, dropping frame.");
            continue;
        }

        if(is_log_packet(frame.data(), frame.size())) {
            if (frame.size() > 2) {
                std::string json_packet = decode_log_packet(frame.data() + 2, frame.size() - 2, false);
                if (!json_packet.empty()) {
                    out_json_data.push_back(json_packet);
                } else {
                    LOGW("processLogChunk: decode_log_packet returned empty string.");
                }
            } else {
                LOGW("processLogChunk: Log packet frame too short (%zu bytes).", frame.size());
            }
        } else {
            // LOGD("processLogChunk: Skipping non-log packet frame.");
        }
        frame.clear();
    }

    env->ReleaseByteArrayElements(pktbytes, payload_buf_ptr, JNI_ABORT);

    jclass stringClass = env->FindClass("java/lang/String");
    if (!stringClass) return nullptr;
    jobjectArray ret = (jobjectArray)env->NewObjectArray(out_json_data.size(), stringClass, NULL);
    if (!ret) {
        LOGE("processLogChunk: Failed to create return String array");
        env->DeleteLocalRef(stringClass);
        return nullptr;
    }

    for(size_t i = 0; i < out_json_data.size(); i++){
        jstring jstr = env->NewStringUTF(out_json_data[i].c_str());
        if (jstr) {
            env->SetObjectArrayElement(ret, i, jstr);
            env->DeleteLocalRef(jstr);
        } else {
            LOGE("processLogChunk: Failed to create Java string for element %zu", i);
        }
    }
    env->DeleteLocalRef(stringClass);
    return ret;
}

/*
 * JNI функция: возвращает массив пар [Имя типа пакета, ID типа пакета] для всех включенных типов логов.
 */
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_login_DiagRevealerControl_getTypeNames(
        JNIEnv *env,
        jobject obj ){

    int enabled_count = 0;
    for(const auto& type_info : LogPacketTypeID_To_Name) {
        if (type_info.enabled) enabled_count++;
    }

    jclass objClass = env->FindClass("java/lang/Object");
    if (!objClass) return nullptr;
    jobjectArray ret = (jobjectArray)env->NewObjectArray(enabled_count, objClass, NULL);
    if (!ret) {
        env->DeleteLocalRef(objClass);
        return nullptr;
    }

    int current_index = 0;
    for(const auto& type_info : LogPacketTypeID_To_Name) {
        if (!type_info.enabled) continue;

        jobjectArray pairArray = (jobjectArray)env->NewObjectArray(2, objClass, NULL);
        if (!pairArray) {
            LOGE("getTypeNames: Failed to create inner array for index %d", current_index);
            continue;
        }

        jstring jName = env->NewStringUTF(type_info.name);
        jobject jValue = NewInteger(env, type_info.val);

        if (jName && jValue) {
            env->SetObjectArrayElement(pairArray, 0, jName);
            env->SetObjectArrayElement(pairArray, 1, jValue);
            env->SetObjectArrayElement(ret, current_index++, pairArray);
            env->DeleteLocalRef(pairArray);
        } else {
            LOGE("getTypeNames: Failed to create JNI String or Integer for %s", type_info.name);
            env->DeleteLocalRef(pairArray);
        }

        if (jName) env->DeleteLocalRef(jName);
        if (jValue) env->DeleteLocalRef(jValue);
    }

    env->DeleteLocalRef(objClass);
    return ret;
}

/*
 * JNI функция: генерирует и записывает файл конфигурации Diag.cfg на основе списка запрошенных типов сообщений.
 */
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_login_DiagRevealerControl_writeDiagCfg(
        JNIEnv *env,
        jobject obj,
        jobjectArray stringArray,
        jstring filepath){

    std::vector<std::string> reqMsgTypes;
    std::string filePathStr;

    const char* cFilePath = env->GetStringUTFChars(filepath, NULL);
    if (!cFilePath) {
        LOGE("writeDiagCfg: GetStringUTFChars failed for filepath");
        return CreateErrorReturnArray(env, "Failed to get file path string");
    }
    filePathStr = cFilePath;
    env->ReleaseStringUTFChars(filepath, cFilePath);

    int stringCount = env->GetArrayLength(stringArray);
    for (int i=0; i<stringCount; i++) {
        jstring jstr = (jstring) (env->GetObjectArrayElement(stringArray, i));
        if (!jstr) continue;
        const char* cstr = env->GetStringUTFChars(jstr, NULL);
        if (cstr) {
            reqMsgTypes.push_back(cstr);
            env->ReleaseStringUTFChars(jstr, cstr);
        }
        env->DeleteLocalRef(jstr);
    }

    LOGD("writeDiagCfg: Trying to write Diag.cfg to path '%s'", filePathStr.c_str());
    std::pair<int, std::string> result = generate_diag_cfg(filePathStr, reqMsgTypes);
    LOGD("writeDiagCfg: Result code=%d, message='%s'", result.first, result.second.c_str());

    jobjectArray retobjarr = (jobjectArray)env->NewObjectArray(2, env->FindClass("java/lang/Object"), NULL);
    if (!retobjarr) {
        LOGE("writeDiagCfg: Failed to create return array");
        return nullptr;
    }

    jobject jRetCode = NewInteger(env, result.first);
    jstring jResultMsg = env->NewStringUTF(result.second.c_str());

    if (jRetCode) env->SetObjectArrayElement(retobjarr, 0, jRetCode);
    if (jResultMsg) env->SetObjectArrayElement(retobjarr, 1, jResultMsg);

    if (jRetCode) env->DeleteLocalRef(jRetCode);
    if (jResultMsg) env->DeleteLocalRef(jResultMsg);

    return retobjarr;
}

/*
 * JNI функция: выполняет тестовое декодирование предопределенных пакетов с использованием библиотеки epan (Wireshark).
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_login_DiagRevealerControl_wsTest(
        JNIEnv *env,
        jobject /* this */,
        jbyte testnum){

    LOGD("wsTest called with testnum = %d", testnum);
    if(!epan_initialized) {
        LOGD("wsTest: Initializing epan...");
        initialize_epan();
        epan_initialized = true;
        LOGD("wsTest: epan initialized.");
    } else {
        LOGD("wsTest: epan already initialized.");
    }

    std::string resultstr;
    std::string msgType;
    std::string msgDataHex;

    switch(static_cast<unsigned char>(testnum)){
        case 1: msgType = "LTE-RRC_PCCH"; msgDataHex = "4001BF281AEBA00000"; break;
        case 2: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "60c428205aa2fe0090c8506e422419822a3653940c40c0"; break;
        case 3: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "10c424c05aa2fe00a0c850448c466608a8e54a80100a0100"; break;
        case 4: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "c764b108500b1ba01483078a2be62ad0"; break;
        case 5: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "0d801f4544fc60005001000011094e"; break;
        case 6: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "63403AFFFF03FFFC5010F0290C0A8018000C8BF5B15EA0000003F5210E30000247894201400010440060222E56300C60202C000C14CC003C4300B6D830021844A0585760186AF400"; break;
        case 7: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "018000"; break;
        case 8: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "b38111d024541a42a0"; break;
        case 9: msgType = "LTE-RRC_BCCH_DL_SCH"; msgDataHex = "41a1001694e49470"; break;
        default:
            resultstr = "Wireshark decoding test unsupported test number: " + std::to_string(testnum) + "\n";
            return env->NewStringUTF(resultstr.c_str());
    }

    resultstr = "Wireshark decoding test #" + std::to_string(testnum) + "\n";
    resultstr += "Type: \"" + msgType + "\", Data(hex): \"" + msgDataHex + "\"\nResult:\n";
    std::string decoded = decode_msg(msgType, msgDataHex);
    resultstr.append(decoded);

    return env->NewStringUTF(resultstr.c_str());
}