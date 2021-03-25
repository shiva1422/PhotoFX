//
// Created by Shiva Shankar patel psy on 3/24/21.
//

#include <cstring>
#include "Histogram.h"
#include "EditableImage.h"
#include "Main.h"

Histogram::Histogram() ////Destructor
{
    for(int i=0;i<2;i++)
    {
        if(glIsBuffer(binsBuffers[i]))
            glDeleteBuffers(1,&binsBuffers[i]);

        glGenBuffers(1,&binsBuffers[i]); ///if Fails?
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,binsBuffers[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,binsSize*sizeof(int32_t),(void *)0,GL_DYNAMIC_COPY);//360 sho that hue can also be done else 256;
        if(Graphics::printGlError("Histogram::CreateTexture")==GL_OUT_OF_MEMORY)
        {
            continue;
        }
        if(glIsTexture(binsTexIds[i]))
            glDeleteTextures(1,&binsTexIds[i]);

        glGenTextures(1,&binsTexIds[i]); //fails?
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,binsTexIds[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1,GL_R32I, binsSize,1);//wiki commonmistakes//use glTexImage for mutable textures.//glpixelstore for way to read(pack)and write(unpack) image using this fun.
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,binsSize,1,GL_RGBA,GL_UNSIGNED_BYTE,0);//check if this need to be changed for both binbuffers;
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
        glBindTexture(GL_TEXTURE_2D,0);
        ////check any effect of texture formats in sampler in shaders;
    }



    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR)
    {
        Loge("HistogramTexture","created");
        reset();

    }

}
void Histogram::reset()
{
    for (int i = 0; i < 2; i++)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffers[i]);
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

    }
    memset(outBins,0,binsSize*sizeof(int32_t));
    bCalculated=false;
}
void Histogram::compute(int histogramFor)
{
    //isHistorgramCalcuated should be set is success
    eHistogramType=(EHistogramType)histogramFor;
    if(ownerImage)
    {

        int tempBinsSize=256;
        if(histogramFor == 4)
        {
            tempBinsSize=360;
        }
        Loge("Compute Histogram","started");
        ///////One Buffer is sufficient for bins Actualy,check cause we are calcuating here both no or might need both int buffers for before and after bins
        int32_t bins[tempBinsSize];
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,binsBuffers[0]);
        int32_t *binsBefore=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,tempBinsSize*sizeof(int32_t),GL_MAP_READ_BIT);
        if(binsBefore)
        {
            memcpy(bins,binsBefore,tempBinsSize*sizeof(int32_t));
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
        /////bins after are actually equalized values not bins;
        int32_t *eqVals=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, tempBinsSize * sizeof(int32_t), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
        if(eqVals)
        {

            float imageH=(float)ownerImage->getImageHeight();
            float imageW=(float)ownerImage->getImageWidth();
            if(imageH!=0&&imageW!=0)
            {
                double tempBins[tempBinsSize];
                for(int i=0;i<tempBinsSize;i++)
                {
                    tempBins[i]=(float)(bins[i]);
                    tempBins[i]/=(imageH*imageW);
                }
                for(int i=1;i<tempBinsSize;i++)
                {
                    Loge(" bincal","before at %d is %lf",i,tempBins[i]);
                    tempBins[i]+=tempBins[i-1];
                    Loge("bincal","after %lf",tempBins[i]);
                    eqVals[i]=(int)(tempBins[i] * (float)tempBinsSize);
                    //   Loge("tests","loop id %d",i);
                }
                bCalculated=true;
            }
            else
            {
                Loge("COmputeHistogram error","imagesize found 0 could not compute");
            }

            ///calculate ouputbin//oubins should be set to 0 check
            int lastIndex=0;
            for(int i=0;i<tempBinsSize;i++)
            {
                for(int k=lastIndex;k<tempBinsSize;k++)
                {
                    if((int)eqVals[k]==i)
                    {
                        outBins[i]+=bins[k];
                    }
                    else
                    {
                        lastIndex=k;
                        break;
                    }
                }
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
        bCalculated=true;

    } else
        {
        Loge("Histogram:","Couldnt compute as owner Image not Set");
     }
eHistogramType=R;
}
void Histogram::printValues()
{
    int temBinsSize=256;
    if(eHistogramType == H)//active suboption that should be same as inshadre;
    {
        temBinsSize=360;//for hud also check for saturation;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffers[0]);
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
    if(activeHistogram==0)
        drawInput();
    else if(activeHistogram==1)
        drawOutput();

    Loge("histogChec in main","the sdfsdf %d",activeHistogram);

}
void Histogram::drawInput()
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
    int tempBinsSize=256;
    if(eHistogramType == H)
    {
        tempBinsSize=360;
    }
    int32_t  totalPixelsFromHistogra=0;
    GlobalData::setDefaultGlProgram();
    glUniform1i(0,3);
    float lineVerts[tempBinsSize*4];//////need to set unifrom based on tempBinsSize for drawing
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsBuffers[0]);
    int32_t  *bins=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,tempBinsSize*sizeof(int32_t),GL_MAP_READ_BIT);
    if(bins)
    {
        for(int i=0;i<tempBinsSize;i++)
        {//too many cal no problem for once
            lineVerts[4*i]=-1.0+i*2.0/tempBinsSize;
            lineVerts[4*i+1]=0.0;
            lineVerts[4*i+2]=lineVerts[4*i];
            lineVerts[4*i+3]=20*float(bins[i])/float(ownerImage->getWidth()*ownerImage->getHeight());///if denom 0 also in drawoutpu
            totalPixelsFromHistogra+=bins[i];
        }
        Loge("Histogram values","readto draw totalPix from histogram %d and totalPixels of Images is %d (just to check if they match=>accurate histogram)",totalPixelsFromHistogra,ownerImage->getImageWidth()*ownerImage->getImageHeight());
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
        lineVerts[4*i+3]=10*float(outBins[i])/float(ownerImage->getWidth()*ownerImage->getHeight());
        totalPixelsFromHistogra+=outBins[i];
    }
    Loge("Histogram values","readto draw totalPix from histogram %d and totalPixels of Images is %d (just to check if they match=>accurate histogram)",totalPixelsFromHistogra,ownerImage->getImageWidth()*ownerImage->getImageHeight());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,lineVerts);
    glDrawArrays(GL_LINES,0,tempBinsSize*2);

}