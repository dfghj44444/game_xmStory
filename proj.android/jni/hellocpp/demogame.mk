LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := main.cpp\
                   ../../../Classes/AppDelegate.cpp\
                   ../../../Classes/HelloWorldScene.cpp\
                   ../../../Classes/GameScene.cpp\
                   ../../../Classes/InputWidget.cpp\
				   ../../../Classes/sequick.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes


LOCAL_PRELINK_MODULE := false



LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_SHARED_LIBRARIES += seQuick-prebuilt

LOCAL_LDLIBS := -llog



#include $(PREBUILT_SHARED_LIBRARY)


include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)

#$(call import-module,cocossequick)
