#ifndef __ANDROID_UTILS_H
#define __ANDROID_UTILS_H

#include <cstdlib>
#include <android/log.h>

#define LOG_TAG "Native"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);

inline jint throwWUPException(JNIEnv *env, const char * message)
{
    jclass exClass = env->FindClass(
            "com/github/diegofps/android/andwup/WUPException");
    
    if (exClass == NULL) {
        LOGE("ClassDef not found");
        return (jint)-1;
    }

    return env->ThrowNew( exClass, message );
}

#endif
