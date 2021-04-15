//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#include <cstring>
#include "EditableImage.h"
#include "Main.h"
#include "gpgpu.h"
EditableImage::EditableImage(Bitmap *bitmap, const View &boundsToFitIn) : EditableImage(0.0f,0.0f,bitmap->width,bitmap->height,bitmap,true)
{
    Loge("Constructed","EditableImage");
    setBoundsDeviceIndependent(this->startX, this->startY,this->width,this->height);
    fitToCentre(boundsToFitIn);
}
EditableImage::EditableImage(float startX, float startY, float width, float height, Bitmap *image,bool visible):ImageView(startX,startY,width,height,image)
{/////set buffer to 0 or noise is draw or draw input to ouput firsttime.

    outputBuffer.setDims(image->width, image->height);
    outputBuffer.configureColorBuffer();
    //outputBuffer.configureDepthBuffer();////optional remove if not needed
    outputBuffer.configure();
    outputTexId=outputBuffer.getTexId();
    inputTexId=texId;//two lines so that by default super draw method draw ouput;based on need we can change them to draw ouptu input or both.
    texId=outputTexId;//since these texId all beolong to single object exchnaging them is no problem (also input output are same size;
   // createHistogramTextures();//////should not be in consrtucted only create and delelte when rquired;
   inputHistogram.setOwnerTexture(inputTexId);
   outputHistogram.setOwnerTexture(outputTexId);
   inputHistogram.setOwnerImage(this);
   outputHistogram.setOwnerImage(this);
   activeHistogram=4;//>3just so that histogram are invisible by defalut;
   toggleHistogramView();//initially histograms are invisible
   inputHistogram.setBackgroundColor(1.0,0.0,0.0,1.0);
   outputHistogram.setBackgroundColor(0.0f,0.0f,1.0f,1.0);
   createLaplaceBuffer();/////check if needed in contructor or just using while Sharpening;
   workGroupSizeX=getImageWidth();
   copyInputToOuput();
  // workGroupSizeY=getImageHeight();
}
void EditableImage::equalize(int histogramFor)
{
    eHistogramType=(EHistogramType)histogramFor;
    if(!inputHistogram.isCalculated())
        inputHistogram.compute(histogramFor);
    Histogram::equalize(this);
    if(bEqualized)
    {
        outputHistogram.updateBuffer(equalizedValues);//now outputHistogra contains eq values so before drawing call compute histogram on outputHistogram;anyways the bCalculated is false
    }
    else
    {
        Loge("Equalize","Failed");
    }
}
void EditableImage::copyInputToOuput()
{
   //using vertex and fragment shaders
    GLuint copyProgram=Shader::createComputeProgram(AppContext::getApp(),"Filters/computeShaders/copy.glsl");
    if(!copyProgram)
    {
        Loge("CopyInput to output failes","could not create program");
        return;
    }
    GlobalData::useGlProgram(copyProgram);
    glBindImageTexture(0,getInputTexId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(1,getOutputTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
    glDispatchCompute(getImageWidth(),getImageHeight(),1);
    glDeleteProgram(copyProgram);
    GlobalData::usePreviousProgram();

}
void EditableImage::copyOutputToInput()
{
    //using vertex and fragment shaders
    GLuint copyProgram=Shader::createComputeProgram(AppContext::getApp(),"Filters/computeShaders/copy.glsl");
    if(!copyProgram)
    {
        Loge("CopyInput to output failes","could not create program");
        return;
    }
    GlobalData::useGlProgram(copyProgram);
    glBindImageTexture(1,getInputTexId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(0,getOutputTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
    glDispatchCompute(getImageWidth(),getImageHeight(),1);
    glDeleteProgram(copyProgram);
    GlobalData::usePreviousProgram();

}
void EditableImage::toggleHistogramView()
{
    activeHistogram++;
    if(activeHistogram>3)
    {
        activeHistogram=0;
    }
    if(activeHistogram==0)
    {
        inputHistogram.setVisibility(false);
        outputHistogram.setVisibility(false);
    }
    else if(activeHistogram==1)
    {
        inputHistogram.setVisibility(true);
        outputHistogram.setVisibility(false);
    }
    else if(activeHistogram==2)
    {
        inputHistogram.setVisibility(false);
        outputHistogram.setVisibility(true);
    }
    else if(activeHistogram==3)
    {
        inputHistogram.setVisibility(true);
        outputHistogram.setVisibility(true);
    }

}
void EditableImage::onSaveEdit()
{
    //clear input histogram and output histog if histogram is active;
}
void EditableImage::draw()
{
  //  printVerts();
   // Loge("bounds","%f,%f,%f,%f",startX,startY,width,height);
    ImageView::draw();//draw active image either input or output by default output
   // Graphics::printGlError("editableImage draw");
   // inputHistogram.draw();
   if(inputHistogram.getVisibility())
   {
       inputHistogram.draw();
   }
   if(outputHistogram.getVisibility())
   {
       if(!outputHistogram.isCalculated())
       {
           outputHistogram.reset();
           outputHistogram.compute(eHistogramType);
       }
       outputHistogram.draw();
   }
}
void EditableImage::drawInput()
{
    texId=inputTexId;
    Loge("EditableImage Input","draw");
    draw();
}
void EditableImage::createLaplaceBuffer()
{
    int32_t RGBA[]={0,0,0,0};
    glGenBuffers(1,&laplaceBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,laplaceBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,7*sizeof(int32_t),RGBA ,GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
}
void EditableImage::computeLaplace()
{
    //SHould be using right program
    int32_t minMax[]={0, 0, 0, 0, 0, 0, 0};
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,laplaceBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,7*sizeof(int32_t), minMax);
    glBindImageTexture(0,getInputTexId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindImageTexture(1,getOutputTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);//////should be moved to setshaderInputs?
    glDispatchCompute(workGroupSizeX,workGroupSizeY,1);
    //   printGlError("computing");
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    minMax[6]=1;//last one just to check if min is calulate
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,6*sizeof(int32_t),sizeof(int32_t),&minMax[6]);//minumum computed so A=1;
    int32_t  *rgba=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,7*sizeof(int32_t),GL_MAP_READ_BIT);
    if(rgba)
    {

            Loge("the Laplacian minimums","R %d G %d B %d,A %d max value %d ,%d, %d",rgba[0],rgba[1],rgba[2],rgba[6],rgba[3],rgba[4],rgba[5]);

    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //  editableImage->showHistogramValues();
    Graphics::printGlError("computeLaplace");
}
void EditableImage::onDestroy()
{
    //clear buffers ,texture,hisograms
}
/*  Editing,Lookup in shader for particular function for the uniformLocation,bindingIndex,etc;
 * 1.Smoothing:
 *
*/
void EditableImage::smoothen(float centreX, float centreY, float innerRadius, float outerRadius, float strength)
{
    Loge("Smoothen:","Blurring");
    //glUniform1i(FILTERTYPELOC,subOptionActive);//not needed for now;
    glUniform1f(0,outerRadius);
    //glUniform1f(PARAMSLOC+1,params[1]);
    glBindImageTexture(0, getInputTexId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindImageTexture(1,getOutputTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);//////should be moved to setshaderInputs?
    glDispatchCompute(1+getImageWidth()/16,1+getImageHeight()/8,1);
    //   printGlError("computing");
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindImageTexture(1,0, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
    //  editableImage->showHistogramValues();
    Loge("computeProcess","the dispacthc count is %d",getImageWidth()/16*getImageHeight()/16);
   // glFinish();//glFlush
}
kforceinline void EditableImage::bindTexturesToShaderImages()
{
    //should be using the right program
    glBindImageTexture(0, getInputTexId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(1,getOutputTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
}
kforceinline void EditableImage::unbindTexturesToShaderImages()
{
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(1,0, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
}