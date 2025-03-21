
package com.example.login

import android.content.Context
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.core.content.edit
import com.topjohnwu.superuser.Shell
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.concurrent.ArrayBlockingQueue

class RootManager(private val context: Context) {

    companion object {
        private const val PREFS_NAME = "RootPrefs"
        private const val KEY_ROOT_MODE = "root_mode"
        private const val DEV_DIAG = "/dev/diag"
        private const val DIAG_CFG_FILE_NAME = "Diag.cfg"

        init {
            Shell.enableVerboseLogging = true
            Shell.setDefaultBuilder(
                Shell.Builder.create()
                    .setFlags(Shell.FLAG_MOUNT_MASTER)
                    .setTimeout(60)
            )
        }
    }

    private val prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    private val logTag = "RootManager"

    private lateinit var diagLogBuffer: ArrayBlockingQueue<DiagDataPacket>
    private lateinit var DRC: DiagRevealerControl

    init {
        diagLogBuffer = ArrayBlockingQueue<DiagDataPacket>(65535, true)
        DRC = DiagRevealerControl(diagLogBuffer, "${context.filesDir}/$DIAG_CFG_FILE_NAME")
    }

    fun isRootModeEnabled() = prefs.getBoolean(KEY_ROOT_MODE, false)

    @RequiresApi(Build.VERSION_CODES.O)
    fun setRootModeEnabled(enabled: Boolean) {
        prefs.edit { putBoolean(KEY_ROOT_MODE, enabled) }
        if (enabled) {
            requestRootAccess()
        } else {
            DRC.stopRevealer()
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    fun requestRootAccess() {
        if (!isDeviceArchitectureSupported()) {
            Log.e(logTag, "Device architecture not supported")
            setRootModeEnabled(false)
            return
        }
        CoroutineScope(Dispatchers.IO).launch {
            Shell.getShell { shell ->
                if (shell.isRoot) {
                    Log.d(logTag, "Root access granted")
                    if (initDiag()) {
                        DRC.runRevealer()
                    } else{
                        Log.e(logTag, "Diag initialization failed")
                        CoroutineScope(Dispatchers.Main).launch {
                            setRootModeEnabled(false)
                        }
                    }
                } else {
                    Log.e(logTag, "Root access denied")
                    CoroutineScope(Dispatchers.Main).launch {
                        setRootModeEnabled(false)
                    }
                }
            }
        }
    }

    fun checkRootAccess(): Boolean = Shell.rootAccess()

    private fun isDeviceArchitectureSupported(): Boolean {
        val supportedAbis = Build.SUPPORTED_ABIS
        Log.d(logTag, "Supported ABIs: ${supportedAbis.joinToString()}")
        return supportedAbis.contains("arm64-v8a") || supportedAbis.contains("armeabi-v7a") || supportedAbis.contains("x86_64")
    }

    private fun initDiag(): Boolean {
        if (!Shell.rootAccess()) {
            Log.e(logTag, "Unable to get root access. Root access is required.")
            return false
        }

        if (!updateSecurityPolicy()) {
            return false
        }

        // Включаем *все* сообщения
        val enabledMessageTypesList = DRC.getKnownMessageTypes()
            .map { (it as Array<*>)[0] as String }
            .toTypedArray()
        val writeConfigResult = DRC.writeNewDiagCfg(enabledMessageTypesList)
        if (!writeConfigResult) {
            Log.e(logTag, "Failed to write initial Diag.cfg")
            return false
        }
        return true
    }

    private fun updateSecurityPolicy(): Boolean {
        val cmds = listOf(
            "setenforce 0",
            "supolicy --live \"allow untrusted_app diag_device chr_file {read write open ioctl}\"",
            "supolicy --live \"allow untrusted_app system_data_file file {read write}\"",
            "supolicy --live \"allow untrusted_app app_data_file file {rename}\"",
            "supolicy --live \"allow init diag_device chr_file {getattr write ioctl}\"",
            "supolicy --live \"allow system_server diag_device chr_file {read write}\""
        )
        for (cmd in cmds) {
            val res = Shell.cmd(cmd).exec()
            if (!res.isSuccess) {
                Log.e(logTag, "Failed to execute SELinux policy command: $cmd. Error: ${res.err.joinToString("")}")
                return false
            }
        }
        Log.d(logTag, "SELinux policies updated successfully")
        return true
    }
}


class DiagDataPacket {
    private var epoch_msec: Long = 0
    private var timestamp: String = ""
    private var data_length: Int = 0
    private var msg_type: Int = 0
    internal lateinit var payload: ByteArray

    constructor(bb: ByteArray, data_bb: ByteArray) {
        var idx = 0
        val low_msg_type = bb[idx++]
        val high_msg_type = bb[idx++]
        msg_type = ((high_msg_type.toInt() and 0xFF) shl 8) or (low_msg_type.toInt() and 0xFF)

        val low_msg_len = bb[idx++]
        val high_msg_len = bb[idx++]
        data_length = ((high_msg_len.toInt() and 0xFF) shl 8) or (low_msg_len.toInt() and 0xFF)
        data_length -= 8

        epoch_msec = ByteBuffer.wrap(bb, idx, 8).order(ByteOrder.LITTLE_ENDIAN).long
        idx += 8
        val formatter = SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS", Locale.US)
        val date = Date(epoch_msec)
        timestamp = formatter.format(date)
        payload = data_bb
    }

    fun getEpochMs(): Long = epoch_msec
    fun getTimestamp(): String = timestamp
    fun getDataLength(): Int = data_length
    fun getPayload(): ByteArray = payload
    fun getMsgType(): Int = msg_type
}

class DiagRevealerControl(
    private val fifo: ArrayBlockingQueue<DiagDataPacket>,
    private val diagConfigFilePath: String
) {

    private var revealerIsRunning = false

    companion object {
        init {
            try {
                System.loadLibrary("diagRevealer")
                Log.d("DiagRevealerControl", "Library loaded successfully")
            } catch (e: UnsatisfiedLinkError) {
                Log.e("DiagRevealerControl", "Failed to load library: ${e.message}", e)
            }
        }
    }

    fun logRevealer(header_buf: ByteArray, data_buf: ByteArray) {
        if (header_buf.size < 12 || data_buf.isEmpty()) {
            Log.w(
                "DiagRevealerControl",
                "Short packet dropped: header ${header_buf.size}; data ${data_buf.size}"
            )
            return
        }

        val diagPkt = DiagDataPacket(header_buf, data_buf)
        fifo.add(diagPkt)

        CoroutineScope(Dispatchers.Main).launch {
            try {
                val decodedPackets = decodePacket(diagPkt.payload)
                for (packet in decodedPackets) {
                    Log.d("DiagRevealerControl", "Decoded Packet: $packet")
                }
            } catch (e: Exception) {
                Log.e("DiagRevealerControl", "Error decoding packet: ${e.message}", e)
            }
        }
    }

    fun getKnownMessageTypes(): Array<Any?> {
        return getTypeNames()
    }

    fun decodePacket(bb: ByteArray): Array<Any?> {
        return processLogChunk(bb)
    }

    fun writeNewDiagCfg(msgTypes: Array<String>): Boolean {
        val resp = writeDiagCfg(msgTypes, diagConfigFilePath)
        return resp[0] as Int == 0
    }

    fun runRevealer() {
        if (revealerIsRunning) return
        _setRevealerRunState(true)
        CoroutineScope(Dispatchers.IO).launch {
            val resp = readDiag(diagConfigFilePath)
            _setRevealerRunState(false)
            if ((resp[0] as Int) != 0) {
                Log.e("DiagRevealerControl", "DIAG thread ended with error: ${resp[1]}")
            } else {
                Log.d("DiagRevealerControl", "DIAG thread ended gracefully: ${resp[1]}")
            }
        }
    }

    fun stopRevealer() {
        if (revealerIsRunning) stopDiag()
    }
    fun getRevealerRunStatus(): Boolean = revealerIsRunning

    private fun _setRevealerRunState(boo: Boolean) {
        revealerIsRunning = boo
    }


    private external fun processLogChunk(bb: ByteArray): Array<Any?>
    private external fun getTypeNames(): Array<Any?>
    private external fun writeDiagCfg(msgTypes: Array<String>, fileName: String): Array<Any?>
    private external fun wsTest(testnum: Byte): String
    private external fun readDiag(diagConfigFilePath: String): Array<Any?>
    private external fun stopDiag()
}