//
// Created by pspr1 on 4/18/2021.
//

#include "SliderSet.h"
#include "PhotoApp.h"

int  SliderSet::sliderCounter=0;

SliderSet::SliderSet()
{
    baseLineView=new Capsule(8);
    pointerView=new Polygon(8);
    setOnTouchListener(new SliderTouchListener());
    Loge("SliderSet()","cconstructed");
}

SliderSet::~SliderSet()
{
    delete baseLineView;
    delete pointerView;
    Loge("SliderSet","destructed");
}
void SliderSet::setPointerLoc(float x, float y)
{
    pointerView->setBounds(x-pointerView->getHeight()/2.0,pointerView->getStartY(),pointerView->getWidth(),pointerView->getHeight());
    if(isPointToTheLeft(pointerView->centerX()))
        pointerView->setBounds(startX-pointerView->getHeight()/2.0,pointerView->getStartY(),pointerView->getWidth(),pointerView->getHeight());
    if(isPointToTheRight(pointerView->centerX()))
        pointerView->setBounds(endX()-pointerView->getHeight()/2.0,pointerView->getStartY(),pointerView->getWidth(),pointerView->getHeight());

    value=(pointerView->centerX()-startX)/width;//0.0 to 1.0
      UILogE("the slider value is %f",value);
}
void SliderSet::setBounds(float startX,float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    pointerView->setBounds(centerX()-height/2.0, startY, height, height);
    baseLineView->setBounds(startX,startY+height/3.0, width, height/3.0);

}

void SliderSet::draw()
{
    //  View::draw();
    if(visible){
        baseLineView->draw();
        pointerView->draw();
    }
}

void SliderSet::reset()
{
    pointerView->setBounds(startX + (width / 2), startY, 50, height);
    value=0.0f;
    UILogE("the slider is reset");

}

///Hue bar

HueBar::HueBar()
{
    delete baseLineView;
    delete pointerView;
    baseLineView=new HueBarShape();
    pointerView=new Triangle();
    Loge("HueBar","constructed");
}

///Touch

bool SliderTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    SliderSet *slider=(SliderSet *)view;
    static float previousSliderValue=0.0;
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
    float scaledSliderValue=(slider->getVaule()*360.0);
    if(previousSliderValue!=scaledSliderValue)
    {
        Loge("SliderTouch","the scaled slider val is %f",scaledSliderValue);
        PhotoApp *globalData = (PhotoApp *) (app->userData);
        globalData->editor->onInputValuesChanged(slider->sliderNo, scaledSliderValue);
        previousSliderValue=scaledSliderValue;
    }
    return true;
}
