LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS     = -Wall -O3 -fexceptions
LOCAL_LDLIBS    := -llog
LOCAL_MODULE    := awup
LOCAL_SRC_FILES := models/wisard.cpp

include $(BUILD_SHARED_LIBRARY)
