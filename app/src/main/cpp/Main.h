//
// Created by Shiva Shankar patel psy on 12/7/20.
//

#ifndef PHOTOFX_MAIN_H
#define PHOTOFX_MAIN_H

#include "Commons.h"
#include "UI.h"

typedef struct GlobalData
{
    AppContext *appContext;
    DisplayParams *displayParams;
    View *contentView=NULL;//can be View &contentView;
    GLuint UIProgram;
    GLuint frameBufId=0;
}GlobalData;
#endif //PHOTOFX_MAIN_H
