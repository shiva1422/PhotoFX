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
#include "Editing.h"
#include "UI.h"
#include "Text.h"
#include "Time.h"
#include "gpgpu.h"
#include "CustomLooperEvents.h"
#include "Shapes.h"
#include "Auto.h"
#include "RecyclerView.h"
#include "TextView.h"

void android_main(android_app *app)
{
    app->onAppCmd = onCmd;
    app->onInputEvent = onInput;
    AppContext appContext;
    DisplayParams displayParams;
    PhotoApp photoApp;
    getDisplayParams(app,&AppContext::displayParams);
    displayParams=AppContext::displayParams;
    View::setDisplayParams(displayParams);
    photoApp.displayParams=&displayParams;///set App Global data
    Graphics::displayParams=displayParams;
    photoApp.appContext=&appContext;
    app->userData=(void *)&photoApp;
    appContext.app=app;
    OnTouchListener::setApp(app);

    TextEngine textEngine;
    textEngine.init();
   // textEngine.setTextSize(25,25);

    int32_t eventId, events, fdesc;
    android_poll_source *source;
    CustomLooperEvents customLooperEvents;
    CustomLooperEvents::init();

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
    PhotoApp::UIProgram=Shader::createShaderProgram(app, "UIProgram/vertexShader.glsl", "UIProgram/fragmentShader.glsl");
    PhotoApp::useGlProgram(PhotoApp::UIProgram);
    AppContext::UIProgram=photoApp.UIProgram;
    InitializeUI();
    //Bitmap defaultKalaImage;//move most of the setup things to seperate function;
    //getPhoto(app,&defaultKalaI,3);
    JavaCalls::importImageFromAssets("icons/hue.png",&ImageView::defaultImage);
    ViewGroup viewGroup;
    photoApp.appUI.frameBounds.setBackgroundColor(0.05, 0.05, 0.05, 1.0);
    photoApp.appUI.init();//move to construcor?



    // subOptionsStack.setNoViewsVisible(6);
   // subOptionsStack.ViewGroup::setBounds(&photoApp.appUI.subOptionsSection);
   // subOptionsStack.fitViewsInBounds();


    SliderSet sliderSet[4];
    for(int i=0;i<4;i++)
    {
        sliderSet[i].setBounds(Graphics::displayParams.screenWidth*5.0/100.0, photoApp.appUI.frameBounds.endY() + (i) * (displayParams.screenHeight * 2 / 100 + 10), (float)displayParams.screenWidth - (displayParams.screenWidth * 10.0 / 100), displayParams.screenHeight * 2 / 100);
        sliderSet[i].setBackgroundColor(1.0,0.0,1.0,1.0);
       // sliderSet[i].setVisibility(false);

    }


    ImageView toggleComputeView(displayParams.screenWidth*90/100,400,100,100);
    toggleComputeView.setOnTouchListener(new ToggleProcessingTypeTouchListener());//delete when done;
    ImageView toggleHistogram(displayParams.screenWidth*90/100,500,100,100);
    toggleHistogram.setOnTouchListener(new ToggleHistogramTL());//need clearing check



HueBarShape hueBarShape;
hueBarShape.setBounds(Graphics::displayParams.screenWidth*5.0/100.0, photoApp.appUI.frameBounds.endY() , (float)displayParams.screenWidth - (displayParams.screenWidth * 10.0 / 100), displayParams.screenHeight * 2 / 100);
viewGroup.addView(photoApp.appUI.options);
viewGroup.setBounds(0,0,displayParams.screenWidth,displayParams.screenHeight);
for(int i=0;i<4;i++)
viewGroup.addView(&sliderSet[i]);
viewGroup.addView(&toggleComputeView);
viewGroup.addView(&toggleHistogram);
    photoApp.contentView=&photoApp.appUI.contentView;

Editor editor;
Layer layer(&photoApp.appUI.frameBounds);
layer.setBackgroundColor(0.0,0.0,0.0,1.0);
editor.addLayer(&layer);
//editor.setOptions(&optionsStack, &subOptionsStack);
photoApp.setEditingContext(&editor);
//photoApp.setMenu(&subOptionsStack,SUBOPTIONS_MENU);
photoApp.addInputComponent(&sliderSet[0], R_INPUT);

TextView testTextView;
testTextView.setBounds(0,100,400,400);

TimeDiff frameTime;
    while(true)
    {
       // UILogE("OUTERLOOP");
        while ((eventId = ALooper_pollAll(0, &fdesc, &events, (void **) &source)) >= 0)
        {
           // UILogE("INNERLOOP");
            if (source)
            {
               // UILogE("EVENT THERE PROCESSING");
               frameTime.start();
                source->process(app, source);
                editor.process();
                PhotoApp::useGlProgram(PhotoApp::UIProgram);
                glUniform1i(glGetUniformLocation(photoApp.UIProgram, "param3"), 1);//active stackView;
                glClearColor(0.0,0.0,0.0,1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                photoApp.appUI.topSection.clearRect();
                photoApp.appUI.frameBounds.clearRect();
                photoApp.appUI.slidersSection.clearRect();
                photoApp.appUI.subOptionsSection.clearRect();
                photoApp.appUI.optionsSection.clearRect();
                editor.draw();
                photoApp.appUI.draw();
              //  testTextView.draw();
                if(photoApp.activeHistogram)
                {
                    //MainImageView.toggleHistogramView();//editableImage.toggleHistogramView();
                    photoApp.activeHistogram=false;
                }
                photoApp.contentView->draw();
              //  photoApp.editor->editableImage->draw();
              if(photoApp.imageImportNeeded && appContext.windowInitStatus)/////////do this onWindowInit in callbacks; or here
              {
                  photoApp.importImage();
                  photoApp.imageImportNeeded= false;
              }
                if(photoApp.testImage)
                {
                   // PhotoApp::useGlProgram(PhotoApp::UIProgram);
                    photoApp.testImage->drawInput();
                    Loge("testImage","draw");
                }
                glUniform1i(glGetUniformLocation(photoApp.UIProgram, "frameBuf"), (int)0);
                if(eglSwapBuffers(appContext.eglDisplay, appContext.eglSurface) == EGL_FALSE)
                {
                    Loge("Main:draw","swap buffers failure");
                    Graphics::printGlError("EGLSWAP DRAW LOOP");

                };
              //  editor.printTotalImageCount();
                frameTime.end();
               // frameTime.getTimeDiff();
                //Compute::showGpuCapacity();
            }
        }
        if(app->destroyed)
        {
            break;
        }

    }

}

#include "JniMethods.cpp"