LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := seQuick-prebuilt
LOCAL_SRC_FILES := libseQuick.so
include $(PREBUILT_SHARED_LIBRARY)