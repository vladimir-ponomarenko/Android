#ifndef UTILS_H_
#define UTILS_H_
#include <android/log.h>
#include "consts.h"
#define  LOG_TAG    "SIGNAL"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define ARRAY_SIZE(array, element_type) (sizeof(array) / sizeof(element_type))

int find_ids (const char *name, IdVector& out_vector);
const char* search_name (const ValueName id_to_name [], int n, int val);
bool typename_exist( const char *name );
std::string hexStr(unsigned char *data, int len);
#endif /* UTILS_H_ */
