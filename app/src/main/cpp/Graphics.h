//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#ifndef PHOTOFX_GRAPHICS_H
#define PHOTOFX_GRAPHICS_H
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
#include "Commons.h"

#define GraphicsLog(...)((void)__android_log_print(ANDROID_LOG_ERROR,"GRAPHICS LOG:",__VA_ARGS__))

class Shader{
private:
    char *source=NULL;
public:
    GLuint id=0;
    bool getSource(android_app *app,const char *fileName);
    GLuint compile(GLenum shaderType);
    GLuint loadAndCompileShader(android_app *app,const char *fileName,GLenum shaderType)
    {
        bool gotSource=getSource(app,fileName);
        if(gotSource)
        {
            id = compile(shaderType);//// id =0 error;

        }
        return id;

    }
    void deleteSource()
    {
        if(source)
        {
            free(source);
            source=NULL;
        }
    }
    static GLuint createShaderProgram(android_app *app,const char *vertexShader,const char *fragmentShader);
    static GLuint linkShaders(GLuint vertexShaderId,GLuint fragmentShaderId); ////// if return 0 => error;
    static GLuint createComputeProgram(android_app *app,const char *computeShaderFile);
};
class Graphics{
public:

    static DisplayParams displayParams;
    static EGLConfig config;
    static status init_display(AppContext* appContext);
    static status onAppReopen(AppContext* appContext);
    static void destroyGL(AppContext *appContext);
    static GLenum printGlError(const char *tag);
};
#endif //PHOTOFX_GRAPHICS_H
