//
// Created by Shiva Shankar patel psy on 2/17/21.
//

#include "Commons.h"
#include "EventHandling.h"
#include "UI.h"
bool touchFunct(float touchX, float touchY, TouchAction touchAction)
{
    Loge("EVentH","in touch function external");
    return true;
}
bool SliderTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction) {return true;}
bool SliderTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    SliderSet *slider=(SliderSet *)view;
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            previousPointerId=pointerId;
            slider->setPointerLoc(touchX,touchY);

        }break;
        case ACTION_POINTER_DOWN:
        {}break;
        case ACTION_MOVE:
        {
            if(pointerId==previousPointerId)
            {
                slider->setPointerLoc(touchX,touchY);
            }

        }break;
        case ACTION_POINTER_UP:
        {
            if(pointerId==previousPointerId)
            {
                previousPointerId=INT32_MAX;
            }
        }
        break;
        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
            {
                previousPointerId=INT32_MAX;
            }

        }break;
         default:
        {}

    }
    return true;
}
bool ImageViewStackTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    ImageViewStack *imageViewStack=(ImageViewStack *)view;
    static uint touchDownView;

   // Loge("EVEnthand","the view is %d",touchDownView);


    switch(touchAction)
    {
        case ACTION_DOWN:
        {
            touchDownView=imageViewStack->getViewNoAtLoc(touchX,touchY);
            previousPointerId=pointerId;

        }
            break;
        case ACTION_POINTER_DOWN:
        {
            touchDownView=imageViewStack->getViewNoAtLoc(touchX,touchY);
            previousPointerId=pointerId;
        }
            break;

        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
            {
                if(imageViewStack->getViewNoAtLoc(touchX,touchY)==touchDownView)
                {
                    imageViewStack->activeView=touchDownView;
                   // Loge("eve","the acitveView is %d",imageViewStack->activeView);
                }
                previousPointerId=INT32_MAX;

            }

        }
            break;
    }

    return true;
}
bool ImageViewStackTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction)
{

    return true;
}

bool ViewTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction,View *view)
{
    (*touchFunc)(touchX,touchY,ACTION_DOWN);

    return true;
}

bool OnTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction,View *view)
{
    (*touchFunc)(touchX,touchY,ACTION_DOWN);

    return true;
}