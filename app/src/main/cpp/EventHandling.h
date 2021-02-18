//
// Created by Shiva Shankar patel psy on 2/17/21.
//

#ifndef PHOTOFX_EVENTHANDLING_H
#define PHOTOFX_EVENTHANDLING_H
#include "commons.h"
//Touch:
class OnTouchListener;
bool touchFunct(float touchX,float touchY,TouchAction touchAction);
class OnTouchListener{

protected:
    typedef bool (*OnTouch)(float x,float y,TouchAction touchAction);//func nam Ontouch;
    OnTouch touchFunc=&touchFunct;//=&OnTouchListener::defaultOnTouch;

public:
    virtual bool defaultOnTouch(float touchX,float touchY,TouchAction touchAction)=0;
    void setTouchFunction(bool (*funcAddr)(float, float, TouchAction))
    {
        touchFunc=&touchFunct;
      //  touchFunc=&onTouch;
    }
    virtual bool onTouch(float touchX,float touchY,TouchAction touchAction)=0;

};
class ViewTouchListener: public OnTouchListener{

public:
    virtual bool defaultOnTouch(float touchX,float touchY,TouchAction touchAction)
    {
        Loge("eventHandling","the touch is from my ViewtouchListener %f and %f",touchX,touchY);
        onTouch(touchX,touchY,touchAction);
        (*touchFunc)(touchX,touchY,ACTION_DOWN);
        return true;
    }
    virtual bool onTouch(float touchX,float touchY,TouchAction touchAction)
    {

        return false;
    }

};
#endif //PHOTOFX_EVENTHANDLING_H
