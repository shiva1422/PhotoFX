//
// Created by Shiva Shankar patel psy on 4/8/21.
//

#include "AppUI.h"
#include "Main.h"

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


//Touch & Click Listeners
bool SaveButtonClickListener::onClick(View *view)
{

      GlobalData *globalData=(GlobalData *)app->userData;
      globalData->onSaveImage();//returns ?
      return true;

}