//
// Created by Shiva Shankar patel psy on 4/15/21.
//

#include "RecyclerView.h"
#include "Shapes.h"

RecyclerView::RecyclerView() :View()
{
    onTouchListener=new RecyclerTouchListener();
}
RecyclerView::RecyclerView(int32 numViews):RecyclerView()
{
    if(views)
    {
        for(int i=0;i<this->numViews;i++)
        {
            delete[] views[i];
            views[i]= nullptr;
        }
        delete [] views;
        views=nullptr;
    }

    this->numViews=numViews;
    views=new View*[numViews];
    for(int i=0;i<numViews;i++)
    {
        views[i]= nullptr;
    }
    if(views)
    {
        lastViewIndex=numViews-1;
        onInit();
    }
}
void RecyclerView::onInit()
{
    //default aac
}
RecyclerView::~RecyclerView()
{
    if(views)
    {
        for(int i=0;i<this->numViews;i++)
        {
            delete[] views[i];
            views[i]= nullptr;

        }
        delete[] views;
        views=nullptr;
    }


}

void RecyclerView::setBounds(View *view)
{
    View::setBounds(view);
}

void RecyclerView::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    viewWidth=(width-(numViews)*viewGap)/(numViews-1);// /0 if numView=1;////crashes;
    setViewsBounds(startX,startY,width,height);//this functin not needed, inline

}
void RecyclerView::setViewsBounds(float startX, float startY, float width, float height)
{
    for(int i=0;i<numViews;i++)
    {
        if(views[i])
        {
            views[i]->setBounds(startX+(i+1)*viewGap+i*viewWidth,startY,viewWidth,height);
        }
    }
}
void RecyclerView::onRecycle(bool right)
{
    if(right)
    {
        if(getListLastIndex() ==maxLastListIndex)//listLastIndex==maxLastListIn
            return;
        Loge("ONRecyle","right");
        views[startViewIndex]->setStartX(views[lastViewIndex]->endX()+viewGap);
        lastViewIndex=startViewIndex;
        startViewIndex+=1;
        if(startViewIndex==numViews)
        {
            startViewIndex=0;
        }
        listStartIndex++;

    }
    else
    {
        Loge("ONRecyle","left startIndex ,lastIndex Before %d,%d",startViewIndex,lastViewIndex);
        views[lastViewIndex]->setEndX(views[startViewIndex]->getStartX()-viewGap);
        startViewIndex=lastViewIndex;
        lastViewIndex-=1;
        if(lastViewIndex<0)
        {
            lastViewIndex=numViews-1;
        }
        listStartIndex--;
        Loge("ONRecyle","left startIndex ,lastIndex after %d,%d",startViewIndex,lastViewIndex);
    }


}
void RecyclerView::onMove(float moveDistanceX)/////careful startViewIndexAndListStartIndex;
{
    if((listStartIndex==0&&moveDistanceX>0))//no moving right if first view is at start;
    {
        if(views[startViewIndex]->getStartX()>=(startX+viewGap-moveDistanceX))
        {
            return;
        }
    }
    else if(getListLastIndex()==maxLastListIndex&&moveDistanceX<0)//no move left if lastView has no more needed updates;
    {
        if(views[lastViewIndex]->endX()<=endX()-viewGap-moveDistanceX)
        return;
    }

    for(int i=0;i<numViews;i++)
    {
        if(views[i])
        {
            views[i]->moveHorizontalByDistance(moveDistanceX);
        }
    }
    if(views[startViewIndex]->getStartX()<=(startX-viewGap-viewWidth))
    //if(totalMoveDisX >=(viewGap + viewWidth) || totalMoveDisX <=-(viewGap + viewWidth))
    {
        onRecycle(true);//right;
    }
    if(views[startViewIndex]->getStartX()>0&&listStartIndex!=0)
    {
        onRecycle(false);
    }


}

void RecyclerView::draw()
{
    View::draw();
    for(int i=0;i<numViews;i++)
    {//doest draw from start index
        if(views[i])
        {
            views[i]->clearRect();
            views[i]->setBackgroundColor(0.2*i,1.0,0.1*i,1.0);
            views[i]->draw();


        }
    }

}


///touch
bool RecyclerTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    RecyclerView *recyclerView=(RecyclerView*)view;
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            previousPointerId=pointerId;
            previousTouchX=touchX;
            return true;

        }break;
        case ACTION_POINTER_DOWN:
        {}break;
        case ACTION_MOVE:
        {
            if(pointerId==previousPointerId)
            {
                recyclerView->onMove(touchX-previousTouchX);
            }
            previousTouchX=touchX;
            return true;

        }break;
        case ACTION_POINTER_UP:
        {
            if(pointerId==previousPointerId)
            {
                previousPointerId=INT32_MAX;
                return false;
            }
        }
            break;
        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
            {
                previousPointerId=INT32_MAX;
                return false;
            }
        }break;
        default:
        {

        }

    }
    return false;
}