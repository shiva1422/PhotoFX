//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#ifndef PHOTOFX_COMMONS_H
#define PHOTOFX_COMMONS_H
#include "EGL/egl.h"
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif
#include "android/log.h"
#include "stdlib.h"
#include"android/log.h"
#include "android_native_app_glue.h"
#define Loge(...)((void)__android_log_print(ANDROID_LOG_ERROR,__VA_ARGS__))
#define Logi(...)((void)__android_log_print(ANDROID_LOG_INFO,__VA_ARGS__))
enum TouchAction{ACTION_DOWN,ACTION_POINTER_DOWN,ACTION_MOVE,ACTION_POINTER_UP,ACTION_UP};
enum status{STATUS_OK,STATUS_KO,STATUS_EXTRA};
class Bitmap{
public:
    int width,height,stride;
    uint8_t * pixels=NULL;
};
typedef struct DisplayParams{
    uint32_t screenWidth,screenHeight,densityDpi,deviceStableDensity,screenStride;
    float density,scaledDensity,xdpi,ydpi;
}DisplayParams;
class AppContext{
public:
    int32_t width,height;
    bool glInitStatus=false,windowInitStatus=false,appFirstOpen=true;
    static android_app* app;//should be set before anything ;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;
    static DisplayParams displayParams;
    static android_app *getApp(){return app;}
};
#endif //PHOTOFX_COMMONS_H
