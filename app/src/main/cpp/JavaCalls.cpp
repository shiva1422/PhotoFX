//
// Created by Shiva Shankar patel psy on 12/7/20.
//
#include <android_native_app_glue.h>
#include "JavaCalls.h"
#include "Commons.h"
#include "android/bitmap.h"
#include "UI.h"
JavaVM* JavaCalls::vm=nullptr;
JNIEnv* JavaCalls::env= nullptr;
jclass JavaCalls::cls=nullptr;
android_app* JavaCalls::app= nullptr;
status saveImage()
{
    android_app *app=AppContext::getApp();
    JavaVM* vm=app->activity->vm;
    JNIEnv *env ;
    vm->AttachCurrentThread(&env,NULL);
    if(env==NULL)
    {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }
    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }
    jmethodID mid = env->GetMethodID(cls, "saveImage", "()V");
    if (mid == 0)
    {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    env->CallVoidMethod(app->activity->clazz,mid);
    vm->DetachCurrentThread();
    return STATUS_OK;


}
status setUiShaderId()
{
    android_app *app=AppContext::getApp();
    JavaVM* vm=app->activity->vm;
    JNIEnv *env ;
    vm->AttachCurrentThread(&env,NULL);
    if(env==NULL)
    {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }
    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }
    jmethodID mid = env->GetMethodID(cls, "setUiShaderProgramId", "(I)V");
    if (mid == 0) {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    env->CallVoidMethod(app->activity->clazz,mid,AppContext::UIProgram);
    vm->DetachCurrentThread();
    return STATUS_OK;
}

status openFileExplorer()
{
    android_app *app=AppContext::getApp();
    JavaVM* vm=app->activity->vm;
    JNIEnv *env ;
    vm->AttachCurrentThread(&env,NULL);
    if(env==NULL)
    {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }
    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if(cls==NULL)
    {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }
    jmethodID mid = env->GetMethodID(cls, "openFileExplorer", "()V");
    if (mid == 0)
    {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    env->CallVoidMethod(app->activity->clazz,mid);
    vm->DetachCurrentThread();
    return STATUS_OK;

}

status getPhoto(android_app* app,Bitmap *pixaMap,int imageId)
{
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
status importImage(Bitmap *pixaMap,int fd) {
    android_app *app = AppContext::getApp();
    if (!app) {
        JniLog("importImage jni", "cannot as there is no appInstance yet");
        return STATUS_KO;
    }
    JavaVM *vm = app->activity->vm;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);
    if (env == NULL) {
        JniLog("coulf not attach/obtain current thread/get java environment");
        return STATUS_KO;
    }

    jclass cls = (env)->GetObjectClass(app->activity->clazz);
    if (cls == NULL) {
        JniLog("coulf not get java object class");
        return STATUS_KO;
    }
    jmethodID mid = env->GetMethodID(cls, "importImage", "(I)Landroid/graphics/Bitmap;");
    if (mid == 0) {
        JniLog("error obtaining the method id");
        return STATUS_KO;
    }
    jobject image = env->CallObjectMethod(app->activity->clazz, mid, fd);
    if (image != NULL) {
        JniLog("successfully obtained the bitmap");
        AndroidBitmapInfo bitmapInfo;
        if (AndroidBitmap_getInfo(env, reinterpret_cast<jobject>(image), &bitmapInfo) < 0) {
            JniLog("coulnd not obtain bitmap info");
            return STATUS_KO;
        }
        if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            JniLog("THE BITMAP FORMATNOT NOT RGBA 8888");/////?improve to support others
            return STATUS_KO;
        }
        pixaMap->width = bitmapInfo.width;
        pixaMap->height = bitmapInfo.height;
        pixaMap->stride = bitmapInfo.stride;
        if (AndroidBitmap_lockPixels(env, image, (void **) &pixaMap->pixels) < 0) {
            JniLog("the bitmap could not be locked");
            return STATUS_KO;
        }
        AndroidBitmap_unlockPixels(env, image);/////is unlock necessary ?
        env->DeleteLocalRef(image);
        vm->DetachCurrentThread();
        JniLog("Image Obtained succesfully");
        return STATUS_OK;

    }
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
status JavaCalls::setImageViewTexture(ImageView *imageView, const char *assetLoc)
{
    Bitmap pixaMap;
    if(attachThreadAndFindClass()==STATUS_OK)
    {
        jmethodID mid = env->GetMethodID(cls, "importImageFromAssets", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            JniLog("error obtaining the method id");
            return STATUS_KO;
        }
        jstring fileNameJava=env->NewStringUTF(assetLoc);
        jobject image = env->CallObjectMethod(app->activity->clazz, mid, fileNameJava);
        if (image != NULL)
        {
            JniLog("successfully obtained the bitmap");
            AndroidBitmapInfo bitmapInfo;
            if (AndroidBitmap_getInfo(env, reinterpret_cast<jobject>(image), &bitmapInfo) < 0) {
                JniLog("coulnd not obtain bitmap info");
                return STATUS_KO;
            }
            if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                JniLog("THE BITMAP FORMATNOT NOT RGBA 8888");/////?improve to support others
                return STATUS_KO;
            }
            pixaMap.width = bitmapInfo.width;
            pixaMap.height = bitmapInfo.height;
            pixaMap.stride = bitmapInfo.stride;
            Loge(assetLoc,"image width-%d and height -%d",pixaMap.width,pixaMap.height);
            if (AndroidBitmap_lockPixels(env, image, (void **) &pixaMap.pixels) < 0) {
                JniLog("the bitmap could not be locked");
                return STATUS_KO;
            }
            imageView->setTexture(&pixaMap);
            //setTexture of imageView or do anything with image and then unlock as this might be garbage collectore after return;
            AndroidBitmap_unlockPixels(env, image);/////is unlock necessary ?
            env->DeleteLocalRef(image);
            vm->DetachCurrentThread();
            JniLog("Image Obtained succesfully");
            return STATUS_OK;
        }
    }
    else
    {
        return STATUS_KO;
    }
}
status JavaCalls::setImageViewStackTexture(ImageViewStack *imageViewStack, int viewNo, const char *assetLoc)
{
    Bitmap pixaMap;///clear
    if(attachThreadAndFindClass()==STATUS_OK)
    {
        jmethodID mid = env->GetMethodID(cls, "importImageFromAssets", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            JniLog("error obtaining the method id");
            return STATUS_KO;
        }
        jstring fileNameJava=env->NewStringUTF(assetLoc);
        jobject image = env->CallObjectMethod(app->activity->clazz, mid, fileNameJava);
        if (image != NULL)
        {
            JniLog("successfully obtained the bitmap");
            AndroidBitmapInfo bitmapInfo;
            if (AndroidBitmap_getInfo(env, reinterpret_cast<jobject>(image), &bitmapInfo) < 0) {
                JniLog("coulnd not obtain bitmap info");
                return STATUS_KO;
            }
            if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                JniLog("THE BITMAP FORMATNOT NOT RGBA 8888");/////?improve to support others
                return STATUS_KO;
            }
            pixaMap.width = bitmapInfo.width;
            pixaMap.height = bitmapInfo.height;
            pixaMap.stride = bitmapInfo.stride;
            Loge(assetLoc,"image width-%d and height -%d",pixaMap.width,pixaMap.height);
            if (AndroidBitmap_lockPixels(env, image, (void **) &pixaMap.pixels) < 0) {
                JniLog("the bitmap could not be locked");
                return STATUS_KO;
            }
            imageViewStack->fillTexture(viewNo,&pixaMap);
            //setTexture of imageView or do anything with image and then unlock as this might be garbage collectore after return;
            AndroidBitmap_unlockPixels(env, image);/////is unlock necessary ?
            env->DeleteLocalRef(image);
            vm->DetachCurrentThread();
            JniLog("Image Obtained succesfully");
            return STATUS_OK;
        }
    }
    else
    {
        return STATUS_KO;
    }
}
status JavaCalls::importImageFromAssets(const char * fileName,Bitmap *pixaMap)
{
    if(attachThreadAndFindClass()==STATUS_OK)
    {
        jmethodID mid = env->GetMethodID(cls, "importImageFromAssets", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            JniLog("error obtaining the method id");
            return STATUS_KO;
        }
        jstring fileNameJava=env->NewStringUTF(fileName);
        jobject image = env->CallObjectMethod(app->activity->clazz, mid, fileNameJava);
        if (image != NULL)
        {
            JniLog("successfully obtained the bitmap");
            AndroidBitmapInfo bitmapInfo;
            if (AndroidBitmap_getInfo(env, reinterpret_cast<jobject>(image), &bitmapInfo) < 0) {
                JniLog("coulnd not obtain bitmap info");
                return STATUS_KO;
            }
            if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                JniLog("THE BITMAP FORMATNOT NOT RGBA 8888");/////?improve to support others
                return STATUS_KO;
            }
            pixaMap->width = bitmapInfo.width;
            pixaMap->height = bitmapInfo.height;
            pixaMap->stride = bitmapInfo.stride;
            Loge(fileName,"image width-%d and height -%d",pixaMap->width,pixaMap->height);
            if (AndroidBitmap_lockPixels(env, image, (void **) &pixaMap->pixels) < 0) {
                JniLog("the bitmap could not be locked");
                return STATUS_KO;
            }
            //setTexture of imageView or do anything with image and then unlock as this might be garbage collectore after return;

            AndroidBitmap_unlockPixels(env, image);/////is unlock necessary ?
            env->DeleteLocalRef(image);
            vm->DetachCurrentThread();
            JniLog("Image Obtained succesfully");
            return STATUS_OK;
        }
    }
    else
    {
        return STATUS_KO;
    }
}
status JavaCalls::attachThreadAndFindClass()
{
    if(!app)
    {
        app=AppContext::getApp();
    }
    if(app)
    {
        vm = app->activity->vm;
        vm->AttachCurrentThread(&env, NULL);
        if (env == NULL)
        {
            JniLog("coulf not attach/obtain current thread/get java environment");
            return STATUS_KO;
        }
        cls = (env)->GetObjectClass(app->activity->clazz);
        if(cls==NULL)
        {
            JniLog("coulf not get java object class");
            return STATUS_KO;
        }
        return STATUS_OK ;
    }
    else
    {
        JniLog("attachThreadAndFindClass","could not get app*");
        return STATUS_KO;
    }
}