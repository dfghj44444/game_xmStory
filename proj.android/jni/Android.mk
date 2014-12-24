LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH := $(realpath $(call my-dir))


include $(MY_LOCAL_PATH)/sequick/seQuickMdl.mk

include $(MY_LOCAL_PATH)/hellocpp/demogame.mk