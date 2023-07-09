//
// Created by Shiva Shankar patel psy on 12/7/20.
//

#ifndef PHOTOFX_JAVACALLS_H
#define PHOTOFX_JAVACALLS_H
#include "jni.h"
#include "android/log.h"
#include "Commons.h"
#define JniLog(...)((void)__android_log_print(ANDROID_LOG_ERROR,"JNILOG",__VA_ARGS__))
class ImageViewStack;
class ImageView;
class JavaCalls{
    static android_app * app;
    static jclass cls;
    static JavaVM *vm;
    static JNIEnv *env;
    static status attachThreadAndFindClass();
public:
    static status importImageFromAssets(const char*,Bitmap *pixaMap);
    static status setImageViewTexture(ImageView *imageView,const char *assetLoc);
    static status setImageViewStackTexture(ImageViewStack *,int viewNo,const char* assetLoc);
};
void getDisplayParams(android_app *app, DisplayParams *displayParams);
status getPhoto(android_app* app,Bitmap *bitmap,int imageId);
status importImage(Bitmap *bitmap,int fd);
status hideSystemUI(android_app *app);
status changeVolume(android_app *app,bool increase);
status openFileExplorer();
status setUiShaderId();
status saveImage();
#endif //PHOTOFX_JAVACALLS_H
