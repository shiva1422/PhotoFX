//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#ifndef PHOTOFX_EDITABLEIMAGE_H
#define PHOTOFX_EDITABLEIMAGE_H


#include "OffScreenRender.h"
#include "UI.h"
#include "Histogram.h"
////try to keep the editable image to only rgb and single alpha for whole image and add aplha only when required(save memory).

class EditableImage : public ImageView{ //the layer class consists of EditableImages
private:
    GLuint outputTexId=0,inputTexId=0;//imageView texId is set to whatever we want to draw inputTexId is for storing default texId
    uint imageToDraw=0;//1-to draw input ,0 to draw ouput ,2for both;
    int activeHistogram=0;//just to set the visibilty of histograms
    EHistogramType eHistogramType=R;

public:
    /////////User Ssbos instead of fbos(arm doc compute shaders)ShaderImages
    int32_t threadSize[3]={16,8,1};//same as local workgroup size in shaders;
    int32_t workGroupSizeX=1,workGroupSizeY=1;//in constructor set WorkGroupSizex to width also private?
    GLuint laplaceBuffer=0;//to store minimum value of laplace during sharpening
    bool bEqualized=false;
    Histogram inputHistogram,outputHistogram;
    int32_t equalizedValues[256];//size should be equal to binssize in histogram
    FrameBuffer outputBuffer;//stores output of image above after editing;//default const create id so just set dims and configure;
    ~EditableImage()
    {
        Loge("Editable","Destructor");//delete outputbuffer frameBuffer
    }
     EditableImage(Bitmap *bitmap,const View &boundsToFitIn);
  //  EditableImage(ImageView *inputImage){}
    EditableImage(float startX,float startY,float Width,float height,Bitmap *image,bool Visible);
    GLuint getActiveTexId(){return texId;}//could be input or outputtexid
    GLuint getInputTexId(){return inputTexId;}
    GLuint getOutputTexId(){return outputTexId;}
    virtual void draw() override;
    void drawInput() ;
    void drawOuput() ;
    int getImageWidth(){ return image.width;}
    int getImageHeight(){return image.height;}

    //editing
    void copyInputToOuput();
    void copyOutputToInput();
    kforceinline void bindTexturesToShaderImages();
    kforceinline void unbindTexturesToShaderImages();

    void smoothen(float centreX,float centreY,float innerRadius,float outerRadius,float strength);
    void equalize(int histogramFor);
    void createLaplaceBuffer();/////For sharpening;
    void toggleHistogramView();
    void computeLaplace();


    void onSaveEdit();
    void onDestroy();
};


#endif //PHOTOFX_EDITABLEIMAGE_H
