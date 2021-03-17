//
// Created by Shiva Shankar patel psy on 2/19/21.
//
#include "Main.h"
#include "UI.h"
#include "Commons.h"
GLuint GlobalData::activeProgram=0;
GLuint GlobalData::UIProgram=0;
////header Main.h for this .cpp change later;
void GlobalData::menuItemChanged()
{
    /////instead of setting all options set only that changed using the parameter as the viewthat changed;
    if(editor)
    {
        Loge("GlobalData","menuitem changed");
        editor->setActiveOption(optionMenu->getActiveViewNo());
        editor->setActiveSubOption(subOptionsMenu->getActiveViewNo());
        /////reset all req inputViews(Sliders);
        for(int i=0;i<SliderSet::sliderCounter;i++)
        {
            slider[i].reset();//check !null;
        }
    }
    else
        {
        Loge("GlobalData::muItemChanged Error","Editing Context Not set Yet");
        }

}
void GlobalData::setMenu(ImageViewStack *imageViewStack, EMenuType menuType)
{
    switch(menuType)
    {
        case OPTIONS_MENU:
            optionMenu=imageViewStack;
            break;
        case SUBOPTIONS_MENU:
            subOptionsMenu=imageViewStack;
            break;
        default:
        {}

    }
}
void GlobalData::addInputComponent(SliderSet *sliderSet, EInputType inputType)
{
    switch (inputType)
    {
        case R_INPUT:
        {
            this->slider=sliderSet;

        }break;
        default:
        {}

    }
}