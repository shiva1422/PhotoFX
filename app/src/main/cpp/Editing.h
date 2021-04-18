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
#include "Layers.h"

enum EActiveShader{AUTO_SHADER,ENHANCE_SHADER,EQ_SHADER,HSI_SHADER,BLUR_SHADER,SHARPEN_SHADER,SMOOTHEN_SHADER};///FOR SHADER LOCATIONNS DIFFERENT FX MIGHT HAVE SAME SHADER(tracking glsl files)
enum EActiveFilter{AUTO_FILTER,LIGHT=0,SATURATION,HUE,GAMMA,CONTRAST,HISTOGRAM,HSI,BLUR,SHARPEN,SMOOTHEN};//should mathc in setActiveFilter and Shaders //BELOW ALSO SAME;
//enum EditOptions{ENHANCE=0,CHROMA,AUTO,GRAY,BLUR,TRANSFORM,THREED};
enum EOptions{OPTION_AUTO,OPTION_HUE,OPTION_NONE=INT32_MAX};
class ImageViewStack;
///create editingContext for each image if needed;
class Editor{
private:
    int optionActive=0,subOptionActive=0,noOfSliders=0.0;
    GLuint activeShaderId=0;
    int imageCount=0;
    float sliderValues[4]={0.0f,0.0f,0.0f,0.0f};
    float params[4]={0.0f,0.0f,0.0f,0.0f};
    EOptions eActiveOption=OPTION_NONE;
    EActiveFilter EactiveFilter;
    EActiveShader eActiveShader=ENHANCE_SHADER,ePreviouShader=ENHANCE_SHADER;//deault;
    bool useGLCompute=true;
    static std::string shadersFolder;
    Layer *layers= NULL;
    void computeProcess();//glCompute
    void vfShaderProcess();//vertex and Fragment shaders;

public:
    EditableImage *editableImage=NULL;///first//make private just for tes
    bool isUpdatedNeeded=false;

    void addLayer(Layer *layer);
    void addEditableImage(EditableImage *editableImage);
    void removeEditableImage(EditableImage *editableImage);

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
    void toggleProcessingType();
    void manageShaders();
    void printTotalImageCount(){Loge("the total images in editor is ","%d",imageCount);}
    void setActiveOptions(EOptions activeOptions);
    void draw();

};


#endif //PHOTOFX_EDITING_H
