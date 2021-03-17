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
#include "ImageProcessing.h"
#include "Editing.h"
#include "UI.h"


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
    PhotoFX::app=app;
    OnTouchListener::setApp(app);
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

    GlobalData::UIProgram=Shader::createShaderProgram(app,"UIProgram/vertexShader.glsl","UIProgram/fragmentShader.glsl");
    GlobalData::useGlProgram(GlobalData::UIProgram);
    AppContext::UIProgram=globalData.UIProgram;
    InitializeUI();
    //Bitmap defaultKalaImage;//move most of the setup things to seperate function;
  //  getPhoto(app,&defaultKalaI,3);
  getPhoto(app,&ImageView::defaultImage,12121);

ViewGroup viewGroup;
    Bitmap InputImage;
    getPhoto(app,&InputImage,0);
    View frameBounds(0,displayParams.screenHeight*15/100,displayParams.screenWidth,displayParams.screenHeight*60/100);
    frameBounds.setBackgroundColor(0.0,0.0,0.0,1.0);

    //globalData.contentView=&MainImageView;
    ImageViewStack optionsStack(6,ImageView::defaultImage.width,ImageView::defaultImage.height);
    ImageViewStack subOptionsStack(6,ImageView::defaultImage.width,ImageView::defaultImage.height);
    optionsStack.setBounds(0,displayParams.screenHeight*93/100,displayParams.screenWidth,displayParams.screenHeight*7.5/100);
    subOptionsStack.setBounds(0,displayParams.screenHeight*85/100,displayParams.screenWidth,optionsStack.getHeight());
    subOptionsStack.setNoViewsVisible(5);
    ImageView TestImage(100,100,50,50);
    SliderSet sliderSet[4];
    for(int i=0;i<4;i++)
    {
        sliderSet[i].setBounds(0,5+(i+1)*displayParams.screenHeight*2/100+displayParams.screenHeight*70/100,displayParams.screenWidth,displayParams.screenHeight*2/100);

    }
    ImageView fbImage(0,0,displayParams.screenWidth,displayParams.screenHeight);
    ImageView fileExplorer(0,0,100,100);
    fileExplorer.setOnTouchListener(new FilesTouchListener());


    // MainImageView.setBoundsDeviceIndependent(0,displayParams.screenHeight*20/100,InputImage.width,InputImage.height);


////outputImage.outputBuffer=outImgFrameBuf;
//outputImage.setBounds(&MainImageView);
//outputImage.setTextureId(outImgFrameBuf.getTexId());
////globalData.editor->editableImage=&outputImage;
EditableImage MainImageView(0,displayParams.screenHeight*20/100,InputImage.width,InputImage.height,&InputImage);
MainImageView.setBoundsDeviceIndependent(MainImageView.getStartX(),MainImageView.getStartY(),InputImage.width,InputImage.height);
MainImageView.fitToCentre(frameBounds);
globalData.frameBufId=MainImageView.outputBuffer.getId();


//
ImageView test(100,100,500,500,&InputImage);
PhotoFX photoFx;

photoFx.inputTexId=MainImageView.getInputTexId();
photoFx.inputImage=&InputImage;

viewGroup.addView(&optionsStack);
viewGroup.addView(&subOptionsStack);
//viewGroup.addView(&MainImageView);
viewGroup.setBounds(0,0,displayParams.screenWidth,displayParams.screenHeight);
for(int i=0;i<4;i++)
viewGroup.addView(&sliderSet[i]);
viewGroup.addView(&MainImageView);
viewGroup.addView(&fileExplorer);
globalData.contentView=&viewGroup;

Editor editor;
editor.setOptions(&optionsStack, &subOptionsStack);
globalData.setEditingContext(&editor);
globalData.setMenu(&subOptionsStack,SUBOPTIONS_MENU);
globalData.setMenu(&optionsStack,OPTIONS_MENU);
globalData.addInputComponent(&sliderSet[0],R_INPUT);
editor.editableImage=&MainImageView;


/*class myListener: public OnTouchListener{
public:
    virtual bool defaultOnTouch(float touchX,float touchY,TouchAction touchAction)
    {
        Loge("eventHandling","the touch is from my My Listener in Main %f and %f",touchX,touchY);
        onTouch(touchX,touchY,touchAction);
        (*touchFunc)(touchX,touchY,ACTION_DOWN);
        return true;
    }
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction)
    {

        return false;
    }
};
outputImage.setOnTouchListener(new myListener());*/

    while(true)
    {
        //UILogE("OUTERLOOP");
        while ((eventId = ALooper_pollAll(0, &fdesc, &events, (void **) &source)) >= 0)
        {
            UILogE("INNERLOOP");
            if (source)
            {
                UILogE("EVENT THERE PROCESSING");
                source->process(app, source);
                editor.process();
                MainImageView.compute();
                GlobalData::useGlProgram(GlobalData::UIProgram);
                glUniform1i(glGetUniformLocation(globalData.UIProgram,"param3"),1);//active stackView;
                glClearColor(0.0,0.0,0.0,1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                globalData.contentView->draw();
                glUniform1i(glGetUniformLocation(globalData.UIProgram,"frameBuf"),(int)0);
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

#include "JniMethods.cpp"