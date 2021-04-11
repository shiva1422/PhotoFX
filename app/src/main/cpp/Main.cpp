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


void android_main(android_app *app)
{
    app->onAppCmd = onCmd;
    app->onInputEvent = onInput;
    AppContext appContext;
    DisplayParams displayParams;
    GlobalData globalData;
    getDisplayParams(app,&AppContext::displayParams);
    displayParams=AppContext::displayParams;
    View::setDisplayParams(displayParams);
    globalData.displayParams=&displayParams;///set App Global data
    Graphics::displayParams=displayParams;
    globalData.appContext=&appContext;
    app->userData=(void *)&globalData;
    appContext.app=app;
    OnTouchListener::setApp(app);

    TextEngine textEngine;
    textEngine.init();
   // textEngine.setTextSize(25,25);

    int32_t eventId, events, fdesc;
    android_poll_source *source;
    globalData.appUI.init();//move to construcor?
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

    GlobalData::UIProgram=Shader::createShaderProgram(app,"UIProgram/vertexShader.glsl","UIProgram/fragmentShader.glsl");
    GlobalData::useGlProgram(GlobalData::UIProgram);
    AppContext::UIProgram=globalData.UIProgram;
    InitializeUI();
    //Bitmap defaultKalaImage;//move most of the setup things to seperate function;
  //  getPhoto(app,&defaultKalaI,3);
  getPhoto(app,&ImageView::defaultImage,12121);

ViewGroup viewGroup;
    globalData.appUI.frameBounds.setBackgroundColor(0.05,0.05,0.05,1.0);


    //globalData.contentView=&MainImageView;
    ImageViewStack optionsStack(6,ImageView::defaultImage.width,ImageView::defaultImage.height);
    ImageViewStack subOptionsStack(11,ImageView::defaultImage.width,ImageView::defaultImage.height);
    optionsStack.setBounds(0,displayParams.screenHeight*95/100,displayParams.screenWidth,displayParams.screenHeight*6/100);
    subOptionsStack.setBounds(0,displayParams.screenHeight*90/100,displayParams.screenWidth,displayParams.screenHeight*4/100);
    subOptionsStack.setNoViewsVisible(11);
    SliderSet sliderSet[4];
    for(int i=0;i<4;i++)
    {
        sliderSet[i].setBounds(0,globalData.appUI.frameBounds.endY()+(i+1)*(displayParams.screenHeight*2/100+10),displayParams.screenWidth,displayParams.screenHeight*2/100);

    }

    Bitmap showFilesImage;
    textEngine.getImageFromString("  openImage",&showFilesImage);
    ImageView fileExplorer(0,0,showFilesImage.width,showFilesImage.height);
    fileExplorer.setBoundsDeviceIndependent(0,0,showFilesImage.width,showFilesImage.height);
    fileExplorer.setTexture(&showFilesImage);
    fileExplorer.setOnTouchListener(new FilesTouchListener());
    ImageView toggleComputeView(displayParams.screenWidth*90/100,0,100,100);
    toggleComputeView.setOnTouchListener(new ToggleProcessingTypeTouchListener());//delete when done;
    ImageView toggleHistogram(displayParams.screenWidth*90/100,150,100,100);
    toggleHistogram.setOnTouchListener(new ToggleHistogramTL());//need clearing check
    Bitmap saveImage;
    textEngine.getImageFromString("save",&saveImage);
    ImageView saveButton(fileExplorer.endX(),0,saveImage.width,saveImage.height);
    saveButton.setTexture(&saveImage);
    saveButton.setOnTouchListener(new SaveButtonHandler());


viewGroup.addView(&optionsStack);
viewGroup.addView(&subOptionsStack);
//viewGroup.addView(&MainImageView);
viewGroup.setBounds(0,0,displayParams.screenWidth,displayParams.screenHeight);
for(int i=0;i<4;i++)
viewGroup.addView(&sliderSet[i]);
viewGroup.addView(&fileExplorer);
viewGroup.addView(&toggleComputeView);
viewGroup.addView(&toggleHistogram);
viewGroup.addView(&saveButton);
globalData.contentView=&viewGroup;

Editor editor;
Layer layer(&globalData.appUI.frameBounds);
layer.setBackgroundColor(1.0,1.0,0.0,1.0);
editor.addLayer(&layer);
editor.setOptions(&optionsStack, &subOptionsStack);
globalData.setEditingContext(&editor);
globalData.setMenu(&subOptionsStack,SUBOPTIONS_MENU);
globalData.setMenu(&optionsStack,OPTIONS_MENU);
globalData.addInputComponent(&sliderSet[0],R_INPUT);

Rect rect,rect2;
rect2.setFillType(FILLTYPE_EDGE);
Polygon circle(4),circle2(4);
circle.Layer::setBounds(&sliderSet[0]);
circle2.Layer::setBounds(&sliderSet[0]);
circle2.setBackgroundColor(1.0,1.0,1.0,1.0);
circle.setBackgroundColor(1.0,0.0,1.0,1.0);
rect.setBounds(0,0,100,100);
rect.setBackgroundColor(1.0,0.0,0.0,1.0);
rect2.setBounds(20,500,1000,20);
rect2.setBackgroundColor(1.0,0.0,1.0,0.0);
Capsule capsule(8);
//capsule.setFillType(FILLTYPE_EDGE);
capsule.setBounds(20,500,900,10);
capsule.setBackgroundColor(1.0,1.0,1.0,1.0);

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
                GlobalData::useGlProgram(GlobalData::UIProgram);
                glUniform1i(glGetUniformLocation(globalData.UIProgram,"param3"),1);//active stackView;
                glClearColor(0.0,0.0,0.0,1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                globalData.appUI.topSection.clearRect();
                globalData.appUI.frameBounds.clearRect();
                globalData.appUI.slidersSection.clearRect();
                globalData.appUI.subOptionsSection.clearRect();
                globalData.appUI.optionsSection.clearRect();
                editor.draw();
                if(globalData.activeHistogram)
                {
                    //MainImageView.toggleHistogramView();//editableImage.toggleHistogramView();
                    globalData.activeHistogram=false;
                }
                globalData.contentView->draw();
              //  globalData.editor->editableImage->draw();
              if(globalData.imageImportNeeded&&appContext.windowInitStatus)/////////do this onWindowInit in callbacks; or here
              {
                  globalData.importImage();
                  globalData.imageImportNeeded= false;
              }
                if(globalData.testImage)
                {
                   // GlobalData::useGlProgram(GlobalData::UIProgram);
                    globalData.testImage->drawInput();
                    Loge("testImage","draw");
                }
                rect.draw();
                rect2.draw();
                capsule.draw();
                static float rot=0.0f;
                rot-=180;
                circle2.setFillType(FILLTYPE_EDGE);
                circle2.draw();
                circle.draw();
                circle.setRotation(rot);
                glUniform1i(glGetUniformLocation(globalData.UIProgram,"frameBuf"),(int)0);
                if(eglSwapBuffers(appContext.eglDisplay, appContext.eglSurface) == EGL_FALSE)
                {
                    Loge("Main:draw","swap buffers failure");
                    Graphics::printGlError("EGLSWAP DRAW LOOP");

                };

                frameTime.end();
                frameTime.getTimeDiff();
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