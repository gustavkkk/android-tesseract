#ifndef __COMMON_H__
#define __COMMON_H__
#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
//
static double now_ms(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0*res.tv_sec + (double)res.tv_nsec/1e6;
}
// ----------------------- LOG PRINT -----------------------//
#define OCRLog(...)				__android_log_print(ANDROID_LOG_DEBUG, "OCRJNI", __VA_ARGS__)
#define OCRErr(...)				__android_log_print(ANDROID_LOG_ERROR, "OCRJNI", __VA_ARGS__)
#define OCRDbg(...)				__android_log_print(ANDROID_LOG_DEBUG, "OCRJNI", __VA_ARGS__)
#define OCRSpeed(...)			__android_log_print(ANDROID_LOG_ERROR, "OCRJNI_TIME", __VA_ARGS__)
#define OCRChkInfo(...)			__android_log_print(ANDROID_LOG_ERROR, "OCRJNI_INFO", __VA_ARGS__)
#define SAFE_FREE(v)	if(v) { free(v);v=NULL;}
#define OCRTRACE			__android_log_print(ANDROID_LOG_ERROR, "OCRJNI_TRACE", "%s:%d", __FUNCTION__, __LINE__);

#ifdef __cplusplus
extern "C" {
#endif

int bgr565_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
int rgb565_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
int bgr8888_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
int rgb8888_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
int rgb565_to_yuv420(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
int bgr565_to_yuv420(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
int rgb8888_to_yuv420(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height);
void yuv420interlaced_to_yuv420(char * yuv420sp, int srcWidth, int srcHeight, char * yuv420);
void yuv420_to_yuv420sp(char* yuv420, int srcWidth, int srcHeight, char* tmp);
int rgb565_to_rgb8888(char* src, char* dest, int width, int height);
int bgr565_to_rgb8888(char* src, char* dest, int width, int height);
int bgr8888_to_rgb8888(char* src, char* dest, int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* __COMMON_H__ */
