//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#include <cstring>
#include "EditableImage.h"
#include "Main.h"
EditableImage::EditableImage(float startX, float startY, float Width, float height, Bitmap *image):ImageView(startX,startY,width,height,image)
{/////set buffer to 0 or noise is draw or draw input to ouput firsttime.
    outputBuffer.setDims(image->width, image->height);
    outputBuffer.configureColorBuffer();
    //outputBuffer.configureDepthBuffer();////optional remove if not needed
    outputBuffer.configure();
    outputTexId=outputBuffer.getTexId();
    inputTexId=texId;//two lines so that by default super draw method draw ouput;based on need we can change them to draw ouptu input or both.
    texId=outputTexId;//since these texId all beolong to single object exchnaging them is no problem (also input output are same size;
    createHistogramTexture();//////should not be in consrtucted only create and delelte when rquired;
}
void EditableImage::initHistogramBuffer()
{
    static bool isBufferInitialized=false;
    if(!isBufferInitialized)
    {
        glGenBuffers(1,&histogramBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,256*sizeof(int32_t),(void * )0,GL_DYNAMIC_COPY);
        int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
        if(bindata)
        {
            Loge("HistogramEqualization","success buffer;.;;;;;;;;;;;;;");
            memset(bindata,0,256*sizeof(int32_t));
            for(int i=0;i<255;i++)
                Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        Graphics::printGlError("HistoGramEqualization");
        isBufferInitialized=true;
    }
    //to read or write ssbo
    //step1
    GLuint blockIndex=0;
    blockIndex=glGetProgramResourceIndex(GlobalData::getProgramId(),GL_SHADER_STORAGE_BLOCK,"bins");///may not be in curren program;
    //step 2 coonec block to ssbo tell shader on which binding oint it can find ssbo (2);
    GLuint bindingPoint=2;//same as in shader;
   // glBindBufferBase(GL_SHADER_STORAGE_BUFFER,bindingPoint,ssboId);
    //glUniformBlockBinding()
}
void EditableImage::computeHistogram()
{
    //just for printing;
    static bool isBufferInitialized=false;
    if(!isBufferInitialized)
    {
        glGenBuffers(1,&histogramBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,256*sizeof(int32_t),(void * )0,GL_DYNAMIC_COPY);
        int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
        if(bindata)
        {
            Loge("HistogramEqualization","success buffer;.;;;;;;;;;;;;;");
            memset(bindata,0,256*sizeof(int32_t));
            for(int i=0;i<255;i++)
                Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        Graphics::printGlError("HistoGramEqualization");
        isBufferInitialized=true;
    }
    if(isBufferInitialized)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
        int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
        if(bindata)
        {
             memset(bindata,0,256*sizeof(int32_t));
            for(int i=0;i<255;i++)
                Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
        } else
        {
            Loge("computeHis","could not map");
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }


}
void EditableImage::showHistogramValues()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
    int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    if(bindata)
    {
       // memset(bindata,0,256*sizeof(int32_t));
        for(int i=0;i<255;i++)
            Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
    } else
    {
        Loge("computeHis","could not map");
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
};
void EditableImage::drawHistogram()
{
   /*float lineVertss[]={0.0,1.0};//////////////draw from UBO bound to histogram buffer that is written in compute shader
    GlobalData::setDefaultGlProgram();
    glUniform1i(0,2);//drawType
    glUniform1i(4,image->width*image->height);//param3;
    GLuint binsUniformIndex=glGetUniformBlockIndex(GlobalData::getProgramId(),"binsData");
    if(binsUniformIndex==GL_INVALID_INDEX)
    {
        Loge("drawHistogra","invalid Index");
    }
    glUniformBlockBinding(GlobalData::getProgramId(),binsUniformIndex,2);
    glBindBuffer(GL_UNIFORM_BUFFER,histogramBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER,binsUniformIndex,histogramBuffer);////checkc layout packing form st430 only for ssbos
    Graphics::printGlError("histogram drawing");
   //glBindImageTexture(0, histogramTexId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I);///////texture should be set before this image variable not supported by vertexshader

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,lineVertss);
    glDrawArraysInstanced(GL_POINTS,0,1,256);
    glBindBufferBase(GL_UNIFORM_BUFFER,binsUniformIndex,0);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    Graphics::printGlError("drawHistogram");*////Make above work;

  //draw using normal methd
  int32_t  totalPixelsFromHistogra=0;
        GlobalData::setDefaultGlProgram();
        glUniform1i(0,3);
        float lineVerts[256*4];
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
         int32_t  *bins=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256,GL_MAP_READ_BIT);
    if(bins)
    {
        for(int i=0;i<256;i++)
        {//too many cal no problem for once
            lineVerts[4*i]=-1.0+i*2.0/256.0;
            lineVerts[4*i+1]=0.0;
            lineVerts[4*i+2]=lineVerts[4*i];
            lineVerts[4*i+3]=100*float(bins[i])/float(image->width*image->height);
            totalPixelsFromHistogra+=bins[i];
        }
        Loge("Histogram values","readto draw totalPix from histogram %d and totalPixels of Images is %d (just to check if they match=>accurate histogram)",totalPixelsFromHistogra,image->width*image->height);
    } else
    {
        Loge("resetHistogram","cannot mapBuffer");
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,lineVerts);
    glDrawArrays(GL_LINES,0,256*2);


}
void EditableImage::createHistogramTexture()
{
    if(glIsBuffer(histogramBuffer))
    {glDeleteBuffers(1,&histogramBuffer);}
    glGenBuffers(1, &histogramBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,histogramBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, 256*sizeof(int32_t),(void *)0 ,GL_DYNAMIC_COPY);
    if(Graphics::printGlError("Histogram::Texture")==GL_OUT_OF_MEMORY)
        return;
    if(glIsTexture(histogramTexId))
        glDeleteTextures(1,&histogramTexId);
    glGenTextures(1, &histogramTexId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, histogramTexId);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, 256,1);//wiki commonmistakes//use glTexImage for mutable textures.//glpixelstore for way to read(pack)and write(unpack) image using this fun.
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,256,1,GL_RGBA,GL_UNSIGNED_BYTE,0);
    Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *),glTextStorage");
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR)
    {
        Loge("HistogramTexture","created");
        resetHistogram();

    }
}
void EditableImage::resetHistogram()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
    int32_t  *bins=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256,GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    if(bins)
    {
        memset(bins,0,256*sizeof(int32_t));
        Loge("resetHistogram","success");
    } else
    {
        Loge("resetHistogram","cannot mapBuffer");
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
}