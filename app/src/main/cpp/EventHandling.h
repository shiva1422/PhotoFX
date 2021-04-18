//
// Created by Shiva Shankar patel psy on 2/17/21.
//

#ifndef PHOTOFX_EVENTHANDLING_H
#define PHOTOFX_EVENTHANDLING_H
#include "Commons.h"
/*
 * IMPROVE
 * Make touch and clicklisteners as inner class View;
 */
bool touchFunct(float touchX,float touchY,TouchAction touchAction);
class View;
class ImageViewStack;
class OnTouchListener{//can be inner class of view;


protected:
   static android_app *app;
    int32_t previousPointerId=INT32_MAX;//max here means not a valid pointer;
    typedef bool (*OnTouch)(float x,float y,TouchAction touchAction);//func nam Ontouch;
    OnTouch touchFunc=&touchFunct;//=&OnTouchListener::defaultOnTouch;


public:
    //virtual bool defaultOnTouch(float touchX,float touchY,TouchAction touchAction)=0;
  /*  void setTouchFunction(bool (*funcAddr)(float, float, TouchAction))
    {
        touchFunc=&touchFunct;
      //  touchFunc=&onTouch;
    }*/
    static void setApp(android_app *app);
    int32_t getPreviousPointerId(){return previousPointerId;}
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction){return true;}//multiple onTOuch can be used(overloading)
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view);

};
class OnClickListener: public OnTouchListener{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction){return true;}//multiple onTOuch can be used(overloading)
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view);
    virtual bool onClick(View *view) = 0;


};
class ViewTouchListener: public OnTouchListener{

public:
 /*   virtual bool defaultOnTouch(float touchX,float touchY,TouchAction touchAction)
    {
        Loge("eventHandling","the touch is from my ViewtouchListener %f and %f",touchX,touchY);
        onTouch(touchX,touchY,touchAction);
        (*touchFunc)(touchX,touchY,ACTION_DOWN);
        return true;
    }*/
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction)
    {
        return false;
    }
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view);


};

class ImageViewStackClickListener: public OnTouchListener
{
private:
    int maxAllowedMove=10;//pixles to consider as move as click;
    float initialTouchX=0,initialTouchY=0;
    float totalMoveDisX=0.0f,totalMoveDisY=0.0f,previousTouchX=0.0,previousTouchY=0.0f;
   // using OnTouchListener::onTouch;
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;///should not be accesed by derive class make private using delete or using using;
    virtual bool onClick(float clickX, float clickY, ImageViewStack *view);
};
class SliderTouchListener : public OnTouchListener{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;

};
/*class ImageViewTouchListener: public OnTouchListener
{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction) override ;
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;

};*/

class FilesTouchListener : public OnClickListener{
public:
    virtual bool onClick(View *view) override ;


};

class ToggleProcessingTypeTouchListener : public OnTouchListener{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;

};

class ToggleHistogramTL : public OnTouchListener{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;

};


#endif //PHOTOFX_EVENTHANDLING_H
