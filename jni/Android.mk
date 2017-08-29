LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_PATH := F:\Prj\OpenCV\OpenCV-android-sdk
LOCAL_MODULE := libopencv_java
LOCAL_SRC_FILES := $(OPENCV_PATH)\sdk\native\libs\armeabi-v7a\libopencv_java3.so 
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

include $(OPENCV_PATH)\sdk\native\jni\OpenCV.mk

LOCAL_MODULE    := locateString
LOCAL_SRC_FILES := locateString.cpp \
				   array.cpp \
				   common.c

LOCAL_LDLIBS    += -lm -llog -landroid -ljnigraphics -lGLESv1_CM -lGLESv2 -lEGL 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := liblept
EXTERN_PATH := G:\tesseract\android-tesseract-ocr-master\extern
LOCAL_SRC_FILES := $(EXTERN_PATH)\armeabi-v7a\liblept.so 
include $(PREBUILT_SHARED_LIBRARY)
LOCAL_SHARED_LIBRARIES := prebuilt/liblept

include $(CLEAR_VARS)
LOCAL_MODULE    := libtess
LOCAL_SRC_FILES := $(EXTERN_PATH)\armeabi-v7a\libtess.so 
include $(PREBUILT_SHARED_LIBRARY)
LOCAL_SHARED_LIBRARIES := prebuilt/libtess
