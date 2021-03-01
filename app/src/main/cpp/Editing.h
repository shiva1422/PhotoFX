//
// Created by Shiva Shankar patel psy on 2/16/21.
//

#ifndef PHOTOFX_EDITING_H
#define PHOTOFX_EDITING_H

#include <string>
#include "stdlib.h"
#include "Graphics.h"
#include "EditableImage.h"
#include "FilterProperties.h"
enum EActiveFilter{LIGHT=0,SATURATION,HUE};//should mathc in setActiveFilter and Shaders
enum EditOptions{HSI=0};
enum EditSubOption{H=0,S,I};
class ImageViewStack;
///create editingContext for each image if needed;
class Editor{
private:
    uint optionActive=0,subOptionActive=0,noOfSliders=0.0;
    GLuint activeShaderId=0;
    float sliderValues[4]={0.0f,0.0f,0.0f,0.0f};
    float inputValue=0.0f;
    EActiveFilter EactiveFilter;
public:
    EditableImage *editableImage=nullptr;///first//make private just for tes
    bool isUpdatedNeeded=false;
public:
    Editor()
    {

    }
    void setOptions(ImageViewStack *optionsMenu,ImageViewStack *subOptionsMenu);
    void setActiveOption(uint ActiveOption);//when option changes change the shader accordingly also reset sliders.
    void setActiveSubOption(uint ActiveSubOption);
    void onInputValuesChanged(uint sliderNo,float newInputValue);///sliderNo and counter are not yet complete in sliderSet
    void setShaderInputs();
    void setActiveFilter();
    void process();

};
class Layer{
    //Layer has similar functionality of ImageGroup with its own editing fearures; add its own draw functions to draw layers
};

class ShaderManager{//remove class and move to EditingCOntext;
private:
   static std::string shadersFolder;

public:
   static GLuint createShaderProgram(uint option);

};
#endif //PHOTOFX_EDITING_H
