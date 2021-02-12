//
// Created by Shiva Shankar patel psy on 11/30/20.
//
#include <android_native_app_glue.h>
#include "Commons.h"
#include "Graphics.h"
#include "Touch.h"
#include "Callbacks.h"
#include "Main.h"
#include "JavaCalls.h"
#include "OffScreenRender.h"
void android_main(android_app *app)
{
    app->onAppCmd = onCmd;
    app->onInputEvent = onInput;
    AppContext appContext;
    DisplayParams displayParams;
    GlobalData globalData;
    getDisplayParams(app,&displayParams);
    View::setDisplayParams(displayParams);
    globalData.displayParams=&displayParams;///set App Global data
    Graphics::displayParams=displayParams;
    globalData.appContext=&appContext;
    app->userData=(void *)&globalData;
    appContext.app=app;
    int32_t eventId, events, fdesc;
    android_poll_source *source;


    while (true)
    {
        while ((eventId = ALooper_pollAll(0, &fdesc, &events, (void **) &source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(app, source);
                if(appContext.glInitStatus)
                    break;
            }

            }
     if(appContext.glInitStatus)
         break;
    }




    //OPENGL AND WINDOW SHOULD BE INITIALIZED BY NOW;

    globalData.UIProgram=Shader::createShaderProgram(app,"UIProgram/vertexShader.glsl","UIProgram/fragmentShader.glsl");
    glUseProgram(globalData.UIProgram);
    InitializeUI();
    //Bitmap defaultKalaImage;//move most of the setup things to seperate function;
  //  getPhoto(app,&defaultKalaI,3);
  getPhoto(app,&ImageView::defaultImage,12121);


    Bitmap InputImage;
    getPhoto(app,&InputImage,0);
    View frameBounds(0,displayParams.screenHeight*15/100,displayParams.screenWidth,displayParams.screenHeight*60/100);
    frameBounds.setBackgroundColor(0.0,0.0,0.0,1.0);
    ImageView MainImageView(0,displayParams.screenHeight*20/100,InputImage.width,InputImage.height,&InputImage);
    MainImageView.setBoundsDeviceIndependent(MainImageView.getStartX(),MainImageView.getStartY(),InputImage.width,InputImage.height);
    MainImageView.fitToCentre(frameBounds);
    globalData.contentView=&MainImageView;
    ImageViewStack optionsStack(6,ImageView::defaultImage.width,ImageView::defaultImage.height);
    ImageViewStack subOptionsStack(6,ImageView::defaultImage.width,ImageView::defaultImage.height);
    optionsStack.setBounds(0,displayParams.screenHeight*93/100,displayParams.screenWidth,displayParams.screenHeight*7.5/100);
    subOptionsStack.setBounds(0,displayParams.screenHeight*85/100,displayParams.screenWidth,optionsStack.getHeight());
    ImageView TestImage(100,100,50,50);
    SliderSet sliderSet;
    sliderSet.setBounds(0,displayParams.screenHeight*75/100,displayParams.screenWidth,displayParams.screenHeight*2/100);
    ImageView fbImage(0,0,displayParams.screenWidth,displayParams.screenHeight);

    // MainImageView.setBoundsDeviceIndependent(0,displayParams.screenHeight*20/100,InputImage.width,InputImage.height);




FrameBuffer outputImageBuf;

//
fbImage.setTextureId(outputImageBuf.getTexId());






    while(true)
    {
        while ((eventId = ALooper_pollAll(0, &fdesc, &events, (void **) &source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(app, source);
                glClearColor(0.0,0.0,0.0,1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                frameBounds.draw();
                float x=(rand()%displayParams.screenWidth);
                float y=(rand()%displayParams.screenHeight);

              //  MainImageView.setBounds(x,y,MainImageView.getWidth(),MainImageView.getHeight());
                //MainImageView.draw();
               // globalData.contentView->draw();
                globalData.contentView->draw();
                MainImageView.draw();
                optionsStack.draw();
                subOptionsStack.draw();
                sliderSet.draw();
                FrameBuffer::setToDefault();
                fbImage.draw();
                outputImageBuf.makeActive();

                if(eglSwapBuffers(appContext.eglDisplay, appContext.eglSurface) == EGL_FALSE)
                {
                    Loge("Main:draw","swap buffers failure");
                    Graphics::printGlError("EGLSWAP DRAW LOOP");

                };

            }

        }
        if(app->destroyed)
        {
            break;
        }

    }

}