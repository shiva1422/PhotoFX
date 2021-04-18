//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#include <android_native_app_glue.h>
#include "Main.h"
#include "UI.h"
float touchX,touchY;
int32_t pointerIndex,pointerId,pointerCount;
int32_t onInput(android_app* app, AInputEvent* event) {
    PhotoApp *globalData=(PhotoApp *)(app->userData);///check if can get without any conversion
    View *ContentView=globalData->contentView;


    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        pointerIndex =(AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        pointerId=AMotionEvent_getPointerId(event,pointerIndex);
        // TouchLog("the pointer index for all is %d and the pointer id is %d",pointerIndex,pointerId);
        touchX = AMotionEvent_getX(event, pointerIndex);
        touchY = AMotionEvent_getY(event, pointerIndex);
        switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
            {
               if(ContentView)//not req;
               {
                   ContentView->isTouched(touchX,touchY,pointerId,ACTION_DOWN);
               }
            }
                break;
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                {
                    if(ContentView)//not req;
                    {
                        ContentView->isTouched(touchX,touchY,pointerId,ACTION_POINTER_DOWN);
                    }
                }
                break;
             case AMOTION_EVENT_ACTION_MOVE:
                {
                    pointerCount=AMotionEvent_getPointerCount(event);

                    for(int i=0;i<pointerCount;i++)//calling action down for all pointers call only if historyValue and current value differ for particular index
                   {
                    touchX = AMotionEvent_getX(event, i);
                    touchY = AMotionEvent_getY(event, i);
                    pointerId = AMotionEvent_getPointerId(event, i);

                   }
                    if(ContentView)//not req;
                    {
                        ContentView->isTouched(touchX,touchY,pointerId,ACTION_MOVE);
                    }
                }
                break;
            case AMOTION_EVENT_ACTION_POINTER_UP:
                {

                 if(ContentView)//not req;
                 {
                    ContentView->isTouched(touchX,touchY,pointerId,ACTION_POINTER_UP);
                 }
                }
                break;
            case AMOTION_EVENT_ACTION_UP:
                {

                 if(ContentView)//not req;
                 {


                     ContentView->isTouched(touchX,touchY,pointerId,ACTION_UP);
                 }
                }
                break;


        }
        ////////////on back button close or application closed Graphics::destroyGL
        return 1;
    }
    else if((AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY))
    {
        int32_t keyCode=AKeyEvent_getKeyCode(event);
        switch (AKeyEvent_getAction(event))
        {
            case AKEY_EVENT_ACTION_DOWN:
            {
                //IO("keyDown");
                if(keyCode==AKEYCODE_VOLUME_UP)
                {
                    //changeVolume(papp, true);
                }
                else if(keyCode==AKEYCODE_VOLUME_DOWN)
                {
                    // changeVolume(papp,false);
                }
            }
                break;
            case AKEY_EVENT_ACTION_UP:
            {
                //   IO("keyUP");
            }
                break;


        }


    }
    return 1;
}