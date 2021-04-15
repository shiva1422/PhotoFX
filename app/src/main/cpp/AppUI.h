//
// Created by Shiva Shankar patel psy on 4/8/21.
//

#ifndef PHOTOFX_APPUI_H
#define PHOTOFX_APPUI_H


#include "UI.h"

class AppUI{

public:
    View frameBounds;//for main image in editor
    View topSection,optionsSection,subOptionsSection,slidersSection;
    static View& getFrameBounds();
    void init();
};
///Click and Touch Handlers Below;
class SaveButtonClickListener : public OnClickListener{
public:
    virtual bool onClick(View *view) override;
};


#endif //PHOTOFX_APPUI_H
