//
// Created by Shiva Shankar patel psy on 3/1/21.
//
///////for implementing jni method(java meths in c)
#include <jni.h>
#include "android/log.h"
#include "JniMethods.h"
#include "Commons.h"
#include "Graphics.h"
#include "customLooperEvents.h"
///////This file is included in main.cpp in the end for correct linking purposes ignoore syncNow;dont add to cmakelists.
extern "C"
JNIEXPORT jint JNICALL
Java_com_kalasoft_photofx_ChooserRenderer_createGlProgram(JNIEnv *env, jobject thiz,jstring vertex_shader_path,jstring fragment_shader_path)
{
    Loge("JNIMETH","CRESTING PROGRAM");
  //  int program=0;av
  android_app *app=AppContext::getApp();
    if(!app)
  {
        /*
        * app is not running in ndk so there is no instance of app yet
        */
      Loge("JNIMETHOD create GLPROGRAM ERROR","cannot not proceed as there is no app instance (not running ndk app)");
  }
    return Shader::createShaderProgram(app,"JavaShaders/vertexShader.glsl","JavaShaders/fragmentShader.glsl");;
    // TODO: implement createGlProgram()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_kalasoft_photofx_EditorActivity_onImageImport(JNIEnv *env, jobject thiz,jint fd) {
    // TODO: implement onImageImport()
    android_app *app=AppContext::getApp();
    if(app)
    {
        CustomMsg customMsg;
        customMsg.eEventType=IMPORTIMAGE;
        customMsg.fd=fd;
        CustomLooperEvents::addCustomEvent(customMsg);
        Loge("CusotmEvent nativemethod","add Event import;");
    }
    else
        {
        Loge("OnImportImage ","could not complete native method as app instance doesnot exist yet");
        }
}