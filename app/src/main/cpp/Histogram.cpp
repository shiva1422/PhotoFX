//
// Created by Shiva Shankar patel psy on 3/24/21.
//

#include <cstring>
#include "Histogram.h"
#include "EditableImage.h"
#include "Main.h"
GLuint Histogram::programId=0;
Histogram::Histogram() ////Destructor
{

        if(glIsBuffer(binsBuffer))
            glDeleteBuffers(1,&binsBuffer);

        glGenBuffers(1,&binsBuffer); ///if Fails?
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,binsBuffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,binsSize*sizeof(int32_t),(void *)0,GL_DYNAMIC_COPY);//360 sho that hue can also be done else 256;
        if(glIsTexture(binsTexId))
            glDeleteTextures(1,&binsTexId);

        glGenTextures(1,&binsTexId); //fails?
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,binsTexId);
        glTexStorage2D(GL_TEXTURE_2D, 1,GL_R32I, binsSize,1);//wiki commonmistakes//use glTexImage for mutable textures.//glpixelstore for way to read(pack)and write(unpack) image using this fun.
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,binsSize,1,GL_RGBA,GL_UNSIGNED_BYTE,0);//check if this need to be changed for both binbuffers;
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
        glBindTexture(GL_TEXTURE_2D,0);
        ////check any effect of texture formats in sampler in shaders;
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR)
    {
        Loge("HistogramTexture","created");
    }
    reset();
    if(programId==0)
    {
        programId=Shader::createComputeProgram(AppContext::getApp(),"Filters/computeShaders/histogram.glsl");
    }

}
void Histogram::reset()
{

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffer);
        void *bins = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, binsSize * sizeof(int32_t),
                                      GL_MAP_WRITE_BIT);
        if (bins)
        {
            memset(bins, 0, binsSize * sizeof(int32_t));
            Loge("resetHistogram", "success");
        }
        else
        {
            Loge("resetHistogram", "cannot map buffer");
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    memset(outBins,0,binsSize*sizeof(int32_t));
    bCalculated=false;
}
void Histogram::compute(int histogramFor)
{
    //isHistorgramCalcuated should be set is success
    eHistogramType=(EHistogramType)histogramFor;
    int tempBinsSize=256;
    if(histogramFor == 4)
    {
        tempBinsSize=360;
    }

    if(programId)
    {
       glCompute(tempBinsSize) ;
    }
    else
        {
        cpuCompute(tempBinsSize);
        }


eHistogramType=R;
}
void Histogram::glCompute(int tempBinSize)
{
    GlobalData::useGlProgram(programId);////this may cause error in Editing check
    glUniform1i(0,eHistogramType);////may need some mod
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,binsBuffer);
    glBindImageTexture(0, ownerTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,binsBuffer);
    glDispatchCompute(ownerImage->getImageWidth(),ownerImage->getImageHeight(),1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    if(Graphics::printGlError("histogramCompute")==GL_NO_ERROR)
    {
        Loge("Histogram COmpute","SUCCESS");
        bCalculated=true;
    }
    else Loge("Histogram Compute","ERROR(warning)");
    GlobalData::usePreviousProgram();
}
void Histogram::cpuCompute(int tempBinSize)
{
    Loge("HistogramCOmpute","using cpuCompute");
}
void Histogram::equalize(EditableImage *ownerImage)
{
    //this is static method check if input histogram is calculated befor this
    int tempBinsSize=256;///check based on histogramFor
    if(ownerImage)
    {
        ownerImage->bEqualized = false;
        Loge("Compute Histogram", "started");
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ownerImage->inputHistogram.binsBuffer);
        int32_t *binsBefore = (int32_t *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
                                                           tempBinsSize * sizeof(int32_t),
                                                           GL_MAP_READ_BIT);
        if (binsBefore)
        {
            int32_t *eqVals = ownerImage->equalizedValues;
            float imageH = (float) ownerImage->getImageHeight();
            float imageW = (float) ownerImage->getImageWidth();
            if (imageH != 0 && imageW != 0) {
                double tempBins[tempBinsSize];
                for (int i = 0; i < tempBinsSize; i++) {
                    tempBins[i] = (float) (binsBefore[i]);
                    tempBins[i] /= (imageH * imageW);
                }
                for (int i = 1; i < tempBinsSize; i++) {
                    Loge(" bincal", "before at %d is %lf", i, tempBins[i]);
                    tempBins[i] += tempBins[i - 1];
                    Loge("bincal", "after %lf", tempBins[i]);
                    eqVals[i] = (int) (tempBins[i] * (float) tempBinsSize);
                    //   Loge("tests","loop id %d",i);
                }
                ownerImage->bEqualized = true;
            } else {
                Loge("COmputeHistogram error", "imagesize found 0 could not compute");
            }
        }
        else
        {
            Loge("computeHistogram Error", "cant map buffer");
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        Loge("Histogram Compute", "success");
    }
    else
    {
        Loge("Histogram:","Couldnt compute as owner Image not Set");
    }
}
void Histogram::printValues()
{
    int temBinsSize=256;
    if(eHistogramType == H)//active suboption that should be same as inshadre;
    {
        temBinsSize=360;//for hud also check for saturation;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffer);
    int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,temBinsSize*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    if(bindata)
    {
        // memset(bindata,0,256*sizeof(int32_t));
        for(int i=0;i<temBinsSize;i++)
            Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
    } else
    {
        Loge("computeHis","could not map");
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
}
void Histogram::draw()
{
  /*  if(activeHistogram==0)
        drawInput();
    else if(activeHistogram==1)
        drawOutput();

    Loge("histogChec in main","the sdfsdf %d",activeHistogram);*/

    int tempBinsSize=256;
    if(eHistogramType == H)
    {
        tempBinsSize=360;
    }
    int32_t  totalPixelsFromHistogra=0;
    GlobalData::setDefaultGlProgram();
    glUniform1i(0,3);
    float lineVerts[tempBinsSize*4];//////need to set unifrom based on tempBinsSize for drawing
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffer);
    int32_t  *bins=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,tempBinsSize*sizeof(int32_t),GL_MAP_READ_BIT);
    if(bins)
    {
        for(int i=0;i<tempBinsSize;i++)
        {//too many cal no problem for once
            lineVerts[4*i]=-1.0+i*2.0/tempBinsSize;
            lineVerts[4*i+1]=0.0;
            lineVerts[4*i+2]=lineVerts[4*i];
           /// lineVerts[4*i+3]=20*float(bins[i])/float(ownerImage->getWidth()*ownerImage->getHeight());///if denom 0 also in drawoutpu
            totalPixelsFromHistogra+=bins[i];
        }
        //Loge("Histogram values","readto draw totalPix from histogram %d and totalPixels of Images is %d (just to check if they match=>accurate histogram)",totalPixelsFromHistogra,ownerImage->getImageWidth()*ownerImage->getImageHeight());
    } else
    {
        Loge("resetHistogram","cannot mapBuffer");
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,lineVerts);
    glDrawArrays(GL_LINES,0,tempBinsSize*2);

}

void Histogram::drawOutput()
{
    int tempBinsSize=256;
    if(eHistogramType == H)
    {
        tempBinsSize=360;
    }
    int32_t  totalPixelsFromHistogra=0;
    GlobalData::setDefaultGlProgram();
    glUniform1i(0,3);
    float lineVerts[tempBinsSize*4];//////need to set unifrom based on tempBinsSize for drawing
    for(int i=0;i<tempBinsSize;i++)
    {//too many cal no problem for once
        lineVerts[4*i]=-1.0+i*2.0/tempBinsSize;
        lineVerts[4*i+1]=0.0;
        lineVerts[4*i+2]=lineVerts[4*i];
       //// lineVerts[4*i+3]=10*float(outBins[i])/float(ownerImage->getWidth()*ownerImage->getHeight());//temporray comment this line works
        totalPixelsFromHistogra+=outBins[i];
    }
   // Loge("Histogram values","readto draw totalPix from histogram %d and totalPixels of Images is %d (just to check if they match=>accurate histogram)",totalPixelsFromHistogra,ownerImage->getImageWidth()*ownerImage->getImageHeight());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,lineVerts);
    glDrawArrays(GL_LINES,0,tempBinsSize*2);

}
void Histogram::onDestroy()
{
    //clear textures,buffers,delete program;
}
