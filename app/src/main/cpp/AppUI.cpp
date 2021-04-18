//
// Created by Shiva Shankar patel psy on 4/8/21.
//

#include "AppUI.h"
#include "Main.h"
#include "Shapes.h"
#include "Auto.h"

void AppUI::init()
{
   topSection.setBounds(0, 0, Graphics::displayParams.screenWidth, Graphics::displayParams.screenHeight * 5.0 / 100.0) ;
   topSection.setBackgroundColor(0.2, 0.4, 0.2, 1.0);
   frameBounds.setBounds(0, topSection.endY(), Graphics::displayParams.screenWidth, Graphics::displayParams.screenHeight * 70.0 / 100.0);
   slidersSection.setBounds(0,frameBounds.endY(),Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight*11.0/100.0);
   slidersSection.setBackgroundColor(0.1,0.11,0.11,1.0);
   subOptionsSection.setBounds(0,slidersSection.endY(),Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight*7.0/100.0);
   subOptionsSection.setBackgroundColor(0.2,0.2,0.3,1.0);
   optionsSection.setBounds(0,subOptionsSection.endY(),Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight*7.0/100.0);
   optionsSection.setBackgroundColor(0.2,0.2,0.2,1.0);

}
//OptionsSection
Options::Options(int numViews, float bitmapWidth, float bitmapHeight) :ImageViewStack(numViews,bitmapWidth,bitmapHeight)
{
    setOnClickListener(reinterpret_cast<OnClickListener *>(new OptionsClickListener()));
}

bool OptionsClickListener::onClick(float clickX, float clickY, ImageViewStack *stackView)
{
    int previousOption=stackView->getActiveViewNo();
    int optionOnClick=stackView->getViewNoAtLoc(clickX,clickY);
    if(previousOption!=optionOnClick)
    {
        //report to global data changeoFOption;
        stackView->setActiveViewNo(optionOnClick);
        PhotoApp *photoApp= static_cast<PhotoApp *>(AppContext::getApp()->userData);
        if(photoApp)
        {
            photoApp->onOptionChanged(optionOnClick);
        }
    }

    return true;
}

//Sub options section AutoOptionsRecyler view
AutoOptions::AutoOptions(int numViews) : RecyclerView(numViews) {
    onInit();
}
AutoOptions::AutoOptions(int numViews, int maxListCount) : AutoOptions(numViews)
{
    maxLastListIndex=maxListCount-1;
    if(numViews>maxListCount)                                                                           //ideally this shouldnt happen as numView should be less than maxListCount;
    {

        maxLastListIndex=numViews-1;
    }
}
void AutoOptions::onInit()
{
    for(int i=0;i<numViews;i++)
    {
        views[i]=new Capsule(8);
        views[i]->setBackgroundColor(1.0,0.2*i,0.1*i,1.0);
        Loge("autooptions","oniit()");
    }
}
void AutoOptions::reportChangeOfActiveListIndex()
{
    Loge("AutoOptions::", "reporting change in active view %d and list first index %d and list last index %d and active listIndex is %d", activeListIndex, listStartIndex, getListLastIndex(),activeListIndex);
    Auto::setAutoType(activeListIndex);
    PhotoApp *photoApp= static_cast<PhotoApp *>(AppContext::getApp()->userData);
    if(photoApp)
    {
        photoApp->editor->onInputValuesChanged(0,activeListIndex);
    }

}




//Touch & Click Listeners
bool SaveButtonClickListener::onClick(View *view)
{

      PhotoApp *globalData=(PhotoApp *)app->userData;
      globalData->onSaveImage();//returns ?
      return true;

}