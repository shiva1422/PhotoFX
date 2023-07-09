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
//HueRange
HueRange::HueRange()
{
    rangeLow=new Triangle();
    rangeHigh=new Triangle();
    rangeLow->setBackgroundColor(0.3,0.3,0.3,1.0);
    rangeHigh->setBackgroundColor(1.0,1.0,1.0,1.0);
    setOnTouchListener(new HueRangeTouchListener());
}
HueRange ::~HueRange()
{
    delete rangeLow;
    delete rangeHigh;
}
void HueRange::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    if(rangeHigh)
    {
        rangeHigh->setBounds(centerX()+upperLimit-height/2.0,startY,height,height);
    }
    if(rangeLow)
    {
        rangeLow->setBounds(centerX()-lowerLimit-height/2.0,startY,height,height);
    }
}
void HueRange::draw()
{
   // clearRect(1.0,0.0,0.0,1.0);
    rangeLow->draw();
    rangeHigh->draw();
}
void HueRange::calculateLowerLimit()
{
    lowerLimit=(rangeLow->centerX()-centerX())*360/(centerX()-startX);//-360 to 0
}
void HueRange::calculateUpperLimit()
{
    upperLimit=360.0-(endX()-rangeHigh->centerX())*360.0/(endX()-centerX());

}
float HueRange::rangeToDistance(float limitValue)
{
    return (centerX()-startX)*limitValue/360.0;
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
        {
            return false;
        }break;
        case ACTION_MOVE:
        {
            if(pointerId==previousPointerId)
            {
                slider->setPointerLoc(touchX,touchY);
            }
            else
                return false;

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
//Hue Range
bool HueRange::onDispatchTouchToChildViews(float touchX, float touchY, int32 pointerId, TouchAction touchAction,float moveX)
{
    switch(touchAction)
    {
        case ACTION_DOWN:
        {
            if(touchX<=centerX())
            {
                rangeLow->setStartX(touchX-rangeLow->getWidth()/2.0);
                calculateLowerLimit();
                if((upperLimit-lowerLimit)>360.0)
                {
                    lowerLimit=upperLimit-360.0;
                    rangeLow->setStartX(centerX()+rangeToDistance(lowerLimit)-rangeLow->getWidth()/2.0);

                }
                activePointer=0;//for monitoring which pointer moved;
            }
            else if(touchX>=centerX())
            {
                rangeHigh->setStartX(touchX-rangeHigh->getWidth()/2.0);
                calculateUpperLimit();
                if((upperLimit-lowerLimit)>360.0)
                {
                    upperLimit=360.0+lowerLimit;
                    rangeHigh->setStartX(centerX()+rangeToDistance(upperLimit)-rangeLow->getWidth()/2.0);
                }

                activePointer=1;
            }
            else return false;
        }break;
        case ACTION_MOVE:
        {
            if(activePointer==0)//left pointer;or rangeLow;
            {

                //if(rangeLow->getStartX()+moveX)
                rangeLow->moveHorizontalByDistance(moveX);
                calculateLowerLimit();
                if((upperLimit-lowerLimit)>360.0)
                {
                    lowerLimit=upperLimit-360.0;
                    rangeLow->setStartX(centerX()+rangeToDistance(lowerLimit)-rangeLow->getWidth()/2.0);

                }
                if(rangeLow->centerX()>centerX())
                {
                    rangeLow->setStartX(centerX()-rangeLow->getWidth()/2.0);
                    calculateLowerLimit();
                }
               // lowerLimit=(rangeLow->centerX()-centerX())*360/(centerX()-startX);//-360 to 0
                Loge("the hue range lower limiet","is %f",lowerLimit);
            }
            else
            {
                rangeHigh->moveHorizontalByDistance(moveX);
                calculateUpperLimit();
                if((upperLimit-lowerLimit)>360.0)
                {
                    upperLimit=360.0+lowerLimit;
                    rangeHigh->setStartX(centerX()+rangeToDistance(upperLimit)-rangeLow->getWidth()/2.0);
                }
               if(rangeHigh->centerX()<centerX())
                {
                    rangeHigh->setStartX(centerX()-rangeHigh->getWidth()/2.0);
                      calculateUpperLimit();
                }
                Loge("the hue range upper limiet","is %f",upperLimit);

            }
        }break;
    }
    return true;
}
bool HueRangeTouchListener::onTouch(float touchX, float touchY, int32 pointerId, TouchAction touchAction, View *view)
{
    HueRange *hueRange=(HueRange *)view;
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            if(!hueRange->isPointInside(touchX,touchY))//should not happen but happens check the view
            {
                return false;
            }
            previousPointerId=pointerId;
            previousTouchY=touchY;
            previousTouchX=touchX;
            return hueRange->onDispatchTouchToChildViews(touchX,touchY,pointerId,touchAction,0);

        }break;
        case ACTION_POINTER_DOWN:
        {
            return false;
        }
            break;
        case ACTION_MOVE:
        {
            if(previousPointerId==pointerId)
            {
                float moveX=touchX-previousTouchX;
                hueRange->onDispatchTouchToChildViews(touchX,touchY,pointerId,touchAction,moveX);
                previousTouchX=touchX;
                previousTouchY=touchY;
            }
            else return false;
        }break;
        case ACTION_POINTER_UP:
        {
            if(pointerId==previousPointerId)
                previousPointerId==INT32_MAX;
            else
                return false;
        }break;
        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
                previousPointerId==INT32_MAX;
            else return false;
        }break;

    }
    return true;
}