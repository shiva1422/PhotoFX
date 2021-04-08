//
// Created by Shiva Shankar patel psy on 12/7/20.
//

#ifndef PHOTOFX_JAVACALLS_H
#define PHOTOFX_JAVACALLS_H
#include "jni.h"
#include "android/log.h"
#include "Commons.h"
#define JniLog(...)((void)__android_log_print(ANDROID_LOG_ERROR,"JNILOG",__VA_ARGS__))
void getDisplayParams(android_app *app, DisplayParams *displayParams);
status getPhoto(android_app* app,Bitmap *bitmap,int imageId);
status importImage(Bitmap *bitmap,int fd);
status hideSystemUI(android_app *app);
status changeVolume(android_app *app,bool increase);
status openFileExplorer();
status setUiShaderId();
#endif //PHOTOFX_JAVACALLS_H
