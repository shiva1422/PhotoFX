//
// Created by Shiva Shankar patel psy on 12/7/20.
//
#include <android_native_app_glue.h>
#include "JavaCalls.h"
#include "Commons.h"
#include "android/bitmap.h"
status getPhoto(android_app* app,Bitmap *pixaMap,int imageId) {
    JavaVM* vm=app->activity->vm;
    JNIEnv *env ;
    vm->AttachCurrentThread(&env,NULL);
    if(env==NULL) {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }
    //setImmersiveMode(env,vm,app);

    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }
    jmethodID mid = env->GetMethodID(cls, "getBitmap", "(I)Landroid/graphics/Bitmap;");
    if (mid == 0) {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    jobject bitmap=env->CallObjectMethod(app->activity->clazz,mid,imageId);
    if(bitmap!=NULL)
    {
        JniLog("successfully obtained the bitmap");
        AndroidBitmapInfo bitmapInfo;
        if(AndroidBitmap_getInfo(env,bitmap,&bitmapInfo)<0)
        {
            JniLog("coulnd not obtain bitmap info");
            return STATUS_KO;
        }
        if(bitmapInfo.format!=ANDROID_BITMAP_FORMAT_RGBA_8888)
        {
            JniLog("THE BITMAP FORMATNOT NOT RGBA 8888");
            return STATUS_KO;
        }


        /*pixaMap=(PixaMap *)malloc(sizeof(PixaMap));
         if(pixaMap==NULL)
         {
             JNILOG("could not allocate memory for newImage sorry");
             return STATUS_KO;
         }*/
        pixaMap->width=bitmapInfo.width;
        pixaMap->height=bitmapInfo.height;
        pixaMap->stride=bitmapInfo.stride;
        if(AndroidBitmap_lockPixels(env,bitmap,(void **)&pixaMap->pixels)<0)
        {
            JniLog("the bitmap could not be locked");
            return STATUS_KO;
        }
    }

    ///////////////////dont forget to unlock bitmap
    AndroidBitmap_unlockPixels(env,bitmap);
    env->DeleteLocalRef(bitmap);
    vm->DetachCurrentThread();
    JniLog("Image Obtained succesfully");
    return STATUS_OK;
}
status hideSystemUI(android_app *app)
{
    JavaVM* vm=app->activity->vm;
    JNIEnv *env ;
    vm->AttachCurrentThread(&env,NULL);
    if(env==NULL) {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }
    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }
    jmethodID mid = env->GetMethodID(cls, "hideSystemUI", "()V");
    if (mid == 0) {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    env->CallVoidMethod(app->activity->clazz,mid);
    vm->DetachCurrentThread();/////dssdfsdfs
    return STATUS_OK;
}
status changeVolume(android_app *app,bool increase)
{
    JavaVM* vm=app->activity->vm;
    JNIEnv *env ;
    vm->AttachCurrentThread(&env,NULL);
    if(env==NULL) {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }
    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }

    jmethodID mid = env->GetMethodID(cls, "changeVolume", "(Z)V");
    if (mid == 0) {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    jboolean inc;
    if(increase)
    {
        inc=true;
    }
    else
    {inc=false;}
    env->CallVoidMethod(app->activity->clazz,mid,inc);
    vm->DetachCurrentThread();/////dssdfsdfs
    return STATUS_OK;

}

void getDisplayParams(android_app *app, DisplayParams *displayParams)
{
    JavaVM *vm = app->activity->vm;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);
    if (env == NULL)
    {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return;
    }


    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return ;
    }
    jmethodID mid = env->GetMethodID(cls, "getDisplayParams", "()[F");
    if (mid == 0)
    {
        JniLog("error obtaining the method id");
        return ;
    }
    jfloatArray  displayParamsArray=(jfloatArray) env->CallObjectMethod(app->activity->clazz,mid);
    // jsize length=env->GetArrayLength(displayParamsArray);
    jfloat  *params=env->GetFloatArrayElements(displayParamsArray,0);
    /* for(int i=0;i<length;i++)
     {
         IOLOG("the params are %f ",params[i]);
     }*/
    displayParams->screenWidth=params[0];
    displayParams->screenHeight=params[1];
    displayParams->density=params[2];
    displayParams->densityDpi=params[3];
    displayParams->deviceStableDensity=params[4];
    displayParams->scaledDensity=params[5];
    displayParams->xdpi=params[6];
    displayParams->ydpi=params[7];
    env->ReleaseFloatArrayElements(displayParamsArray,params,0);
    vm->DetachCurrentThread();/////dssdfsdfs
}