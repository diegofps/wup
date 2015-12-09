LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := andwup
LOCAL_SRC_FILES := andwup.cpp

include $(BUILD_SHARED_LIBRARY)
