//
// Created by Shiva Shankar patel psy on 4/15/21.
//

#include "RecyclerView.h"
#include "Shapes.h"

RecyclerView::RecyclerView() :View()
{
    setOnTouchListener(new RecyclerTouchListener());
}
RecyclerView::RecyclerView(int32 numViewsToDraw): RecyclerView()
{
    if(views)
    {
        for(int i=0;i<this->numViews;i++)//instead of loop delete as array
        {
            delete views[i];
            views[i]= nullptr;
        }
        delete [] views;
        views=nullptr;
    }

    this->numViews=numViewsToDraw;
    views=new View*[numViewsToDraw];
    for(int i=0; i < numViewsToDraw; i++)
    {
        views[i]= nullptr;
    }
    if(views)
    {
        lastViewIndex= numViewsToDraw - 1;
        maxLastListIndex=lastViewIndex;//should be based on the outside databased(for example max number of options-1)//should change in subclass;(using other construcntor;
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
            delete views[i];
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
    viewWidth=(width-(numViews)*viewGap)/(numViews-1);                                                          /// /0 if numView=1;////crashes;
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
    Loge("recycler","the activeList Index is %d",activeListIndex);
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
{                                                                                                        ////   use scissorTest for not drawing inside views out of the recyler view bounds
    View::draw();
    int activeViewOffset=INT32_MAX;
    if(activeListIndex>=listStartIndex&&activeListIndex<=getListLastIndex())                                //highlight the bacground of active index in
    {
        activeViewOffset=activeListIndex-listStartIndex;
    }
    int viewIndex=0;
    for(int i=0;i<numViews;i++)
    {//doest draw from start index
        viewIndex=startViewIndex+i;
        if(viewIndex>=numViews)
        {
            viewIndex-=numViews;
        }
        if(views[viewIndex])
        {
            if(!(i==activeViewOffset))
                views[viewIndex]->clearRect(0.0,0.0,0.0,1.0);
            else
                views[viewIndex]->clearRect(1.0,1.0,1.0,1.0);                                 //background color to highlight of the activeListIndex;
            views[viewIndex]->setBackgroundColor(0.2*i,1.0,0.1*i,1.0);
            views[viewIndex]->draw();
        }
    }

}


///touch

int32 RecyclerView::setActiveListIndex(float x, float y)
{
   float xDisBetStartViewAndLoc=x-views[startViewIndex]->getStartX();
   int32 previousActiveListIndex=activeListIndex;
   int32 tempActiveViewIndex=INT32_MAX;
           if(xDisBetStartViewAndLoc>=0)
           {
               tempActiveViewIndex= (xDisBetStartViewAndLoc) / (viewGap + viewWidth);///////needs improve as view gaps here also acount for (which should not be )/also this is not actual index but an offset from startView;
               activeListIndex=getListIndexAtOffset(tempActiveViewIndex);
           }
           else
           {
               activeListIndex=INT32_MAX;
           }
           Loge("Recylcer View ", "The active view is %d", activeListIndex);
           if(previousActiveListIndex!=activeListIndex)
               reportChangeOfActiveListIndex();
    return activeListIndex;
}


//touch Listener;
bool RecyclerTouchListener::onTouch(float touchX, float touchY, int pointerId, TouchAction touchAction, View *view)
{
    RecyclerView *recyclerView=(RecyclerView*)view;
    switch (touchAction)
    {
        case ACTION_DOWN:
        {
            previousPointerId=pointerId;
            previousTouchX=touchX;
            initialTouchX=touchX;
            totalMoveDisX=0.0f;
            return true;

        }break;
        case ACTION_POINTER_DOWN:
        {
            return false;
        }break;
        case ACTION_MOVE:
        {
            if(pointerId==previousPointerId)
            {
                float tempMoveDisX=touchX-previousTouchX;
                recyclerView->onMove(tempMoveDisX);
                if(tempMoveDisX<0)
                {
                    tempMoveDisX*=-1;
                }
                totalMoveDisX+=tempMoveDisX;
                previousTouchX=touchX;
                return true;
            }
            else return false;


        }break;
        case ACTION_POINTER_UP://actually this is not needed as pointerDown is not counted for;
        {
            if(pointerId==previousPointerId)
            {
                if(totalMoveDisX<=10)//allowed 10 pixel for click
                {
                    recyclerView->setActiveListIndex(touchX, touchY);
                }
                previousPointerId=INT32_MAX;
            }
            else return false;
        }
            break;
        case ACTION_UP:
        {
            if(pointerId==previousPointerId)
            {
                if(totalMoveDisX<=10)//allowed 10 pixel move for click
                {
                    recyclerView->setActiveListIndex(touchX, touchY);
                }
                previousPointerId=INT32_MAX;
            }
            else return false;
        }break;
        default:
        {

        }

    }
    return true;
}