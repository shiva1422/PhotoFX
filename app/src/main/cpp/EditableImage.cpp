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
    createHistogramTextures();//////should not be in consrtucted only create and delelte when rquired;
}

//just for printing;
void EditableImage::showHistogramValues()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffers[0]);
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
    glBindBuffer(GL_UNIFORM_BUFFER,binsBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER,binsUniformIndex,binsBuffer);////checkc layout packing form st430 only for ssbos
    Graphics::printGlError("histogram drawing");
   //glBindImageTexture(0, binsTexId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I);///////texture should be set before this image variable not supported by vertexshader

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
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffers[0]);
         int32_t  *bins=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_READ_BIT);
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
void EditableImage::createHistogramTextures()   //////Make clear funcrion as well
{
    for(int i=0;i<2;i++)
    {
        if(glIsBuffer(binsBuffers[i]))
            glDeleteBuffers(1,&binsBuffers[i]);

        glGenBuffers(1,&binsBuffers[i]); ///if Fails?
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,binsBuffers[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,256*sizeof(int32_t),(void *)0,GL_DYNAMIC_COPY);
        if(Graphics::printGlError("Histogram::CreateTexture")==GL_OUT_OF_MEMORY)
        {
            continue;
        }
        if(glIsTexture(binsTexIds[i]))
            glDeleteTextures(1,&binsTexIds[i]);

        glGenTextures(1,&binsTexIds[i]); //fails?
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,binsTexIds[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1,GL_R32I, 256,1);//wiki commonmistakes//use glTexImage for mutable textures.//glpixelstore for way to read(pack)and write(unpack) image using this fun.
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,256,1,GL_RGBA,GL_UNSIGNED_BYTE,0);//check if this need to be changed for both binbuffers;
         glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
         glBindTexture(GL_TEXTURE_2D,0);
         ////check any effect of texture formats in sampler in shaders;
    }



    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR)
    {
        Loge("HistogramTexture","created");
        resetHistogram();

    }

}
void EditableImage::resetHistogram() {
    for (int i = 0; i < 2; i++)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffers[i]);
        void *bins = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 256 * sizeof(int32_t),
                                      GL_MAP_WRITE_BIT);
        if (bins)
        {
            memset(bins, 0, 256 * sizeof(int32_t));
            Loge("resetHistogram", "success");
        }
        else
        {
            Loge("resetHistogram", "cannot map buffer");
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    }
    isHistogramCalculated=false;
}
void EditableImage::computeHistogram()
{
    //isHistorgramCalcuated should be set is success
    Loge("Compute Histogram","started");
    ///////One Buffer is sufficient for bins Actualy,check cause we are calcuating here both no or might need both int buffers for before and after bins
    int32_t bins[256];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,binsBuffers[0]);
    int32_t *binsBefore=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_READ_BIT);
    if(binsBefore)
    {
        memcpy(bins,binsBefore,256*sizeof(int32_t));
    }
    else
        {
        Loge("computeHistogram","cant map buffer");
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
            return;
        }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,binsBuffers[1]);
    int32_t *binsAfter=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    if(binsAfter)
    {

        float imageH=(float)getImageHeight();
        float imageW=(float)getImageWidth();
        if(imageH!=0&&imageW!=0)
        {
            double tempBins[256];
            for(int i=0;i<256;i++)
            {
                tempBins[i]=(float)(bins[i]);
                tempBins[i]/=(imageH*imageW);
            }
            for(int i=1;i<256;i++)
            {
                Loge(" bincal","before at %d is %lf",i,tempBins[i]);
                tempBins[i]+=tempBins[i-1];
                Loge("bincal","after %lf",tempBins[i]);
                binsAfter[i]=(int)(tempBins[i]*255.0);
             //   Loge("tests","loop id %d",i);
            }
            isHistogramCalculated=true;
        }
        else
        {
            Loge("COmputeHistogram error","imagesize found 0 could not compute");
        }

    }
    else
    {
        Loge("computeHistogram error","could not map outputBuf");
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
        return;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    Loge("Histogram Compute","success");
    isHistogramCalculated=true;

}