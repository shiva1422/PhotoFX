//
// Created by Shiva Shankar patel psy on 2/17/21.
//

#include "Commons.h"
#include "EventHandling.h"
#include "UI.h"
#include "Main.h"
#include "JavaCalls.h"

android_app * OnTouchListener::app=nullptr;


bool touchFunct(float touchX, float touchY, TouchAction touchAction)
{
   // Loge("EVentH","in touch function external");
    return true;
}

bool ToggleHistogramTL::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    if(touchAction==ACTION_DOWN)
    {
        PhotoApp *globalData=(PhotoApp *)app->userData;
        globalData->activeHistogram=!globalData->activeHistogram;
    }
    return true;
}
bool ToggleProcessingTypeTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    if(touchAction==ACTION_DOWN)
    {
        PhotoApp *globalData=(PhotoApp *)app->userData;
        globalData->toggleProcessingType();
    }
    return true;
}
bool FilesTouchListener::onClick(View *view)
{
    // view->setBounds(0,0,Graphics::displayParams.screenWidth,view->getHeight());
    Loge("filesTouch","Uishae id in c is %d",AppContext::UIProgram);
    setUiShaderId();
    openFileExplorer();///////get error handle
    Loge("FIleToouchListern OnClick","done");
    return true;

}


bool ImageViewStackClickListener::onClick(float clickX, float clickY, ImageViewStack *view)
{
    view->setActiveViewNo(view->getViewNoAtLoc(clickX, clickY));
    return true;
}
bool ImageViewStackClickListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    // Loge("EVEnthand","the view is %d",touchDownView);


    switch(touchAction)
    {
        case ACTION_DOWN:
        {
            initialTouchX=touchX;
            initialTouchY=touchY;
            previousPointerId=pointerId;
            previousTouchX=touchX;
            previousTouchY=touchY;
            totalMoveDisX=0;
            totalMoveDisY=0;

        }
        break;
        case ACTION_MOVE:
        {
            if(previousPointerId==pointerId)//no need check as already done before calling this meth
            {
                float tempMovey=touchY-previousTouchY;
                if(tempMovey<0){tempMovey*=-1;}
                float tempMoveX=touchX-previousTouchX;
                if(tempMoveX<0){tempMoveX*=-1;}
                totalMoveDisX+=tempMoveX;
                totalMoveDisY+=tempMovey;
                previousTouchX=touchX;
                previousTouchY=touchY;
            }
        }break;
        case ACTION_POINTER_UP:
        {
            if(pointerId==previousPointerId)//cancel Touch;
            {

                previousPointerId=INT32_MAX;

            }
        }
        break;
        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
            {
                if(totalMoveDisX<=maxAllowedMove&&totalMoveDisY<=maxAllowedMove)
                {
                    previousPointerId=INT32_MAX;
                    return onClick(initialTouchX,initialTouchY,(ImageViewStack *)view);
                }


            }

        }
            break;
    }


    return true;
}


bool ViewTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction,View *view)
{
   // (*touchFunc)(touchX,touchY,ACTION_DOWN);
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            previousPointerId=pointerId;
        }break;
        case ACTION_POINTER_DOWN:
        {}break;
        case ACTION_MOVE:
        {
            if(pointerId==previousPointerId)
            {

            }
        }break;
        case ACTION_POINTER_UP:
        {
            if(pointerId=previousPointerId)
            {
                previousPointerId=INT32_MAX;
            }
        }break;
        case ACTION_UP:
        {
            if(pointerId=previousPointerId)
            {
                previousPointerId=INT32_MAX;
            }
        }break;


    }

    return true;
}
bool ViewGroupTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    ViewGroup *viewGroup=(ViewGroup *)view;
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            previousPointerId=pointerId;

        }
        break;

    }
    return true;
}
bool OnClickListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            previousPointerId=pointerId;

        }break;
        case ACTION_POINTER_DOWN:
        {
            previousPointerId=INT32_MAX;//calcel the action;
        }break;
        case ACTION_MOVE:
        {
           if(previousPointerId==pointerId)
           {
               if(!view->isPointInside(touchX,touchY))
               {
                   previousPointerId=INT32_MAX;//cancel the touch as moved out
               }
           }
        }break;
        case ACTION_POINTER_UP:
        {

        }
            break;
        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
            {
                return onClick(view);
            }
        }break;
        default:
        {}

    }
    return true;
}

bool OnTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction,View *view)
{
    (*touchFunc)(touchX,touchY,ACTION_DOWN);

    return true;
}
void OnTouchListener::setApp(android_app *app) {OnTouchListener::app=app;}
