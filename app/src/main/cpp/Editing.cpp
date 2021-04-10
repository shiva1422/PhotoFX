//
// Created by Shiva Shankar patel psy on 2/16/21.
//

#include <string>
#include "Editing.h"
#include "Main.h"
const int PARAMSLOC=5,FILTERTYPELOC=0;
std::string Editor::shadersFolder="Filters";

void Editor::onInputValuesChanged(uint sliderNo, float newInputValue)
{
    ////////make sliderNo (equal) or attahed to a certain shader input param nad newInputValue to that params value;
  //  Loge("Editor:onInputChanged ","SliderNo is %d and value is %f",sliderNo,newInputValue);
    switch (sliderNo)
    {
        case 0://param1;
        {
            params[0]=newInputValue;
        }
        break;
        case 1://param2
        {
            params[1]=newInputValue;
        }
        break;
        case 2:
        {
            params[2]=newInputValue;
        }
        break;
        case 3:
        {
            params[3]=newInputValue;
        }break;
        default:
        {

        }

    }
    //Loge("OnInputChange","sds");
   // process();
  // editableImage->outputHistogram.reset();
   isUpdatedNeeded=true;
}
void Editor::process()
{
  //  Loge("process","sds");
  editableImage=layers->getEditableImages();/////temporary move processing to layers;
    if(isUpdatedNeeded&&editableImage)
    {
        GlobalData::useGlProgram(activeShaderId);

        if(useGLCompute)
        {
            Loge("Editor","using GlCompute");
            if(eActiveShader==SMOOTHEN_SHADER)
            {
                editableImage->smoothen(0,0,0,params[0],0);
                return;
            }
            setShaderInputs();
            computeProcess();///////sometimes slow as work is not diveided to use less more threads and less workgroups
        }
        else
        {
            Loge("Editor","not usingGlCompute");
            vfShaderProcess();
        }
           isUpdatedNeeded= false;
        GlobalData *globalData = (GlobalData *) ((AppContext::getApp())->userData);
        GlobalData::useGlProgram(globalData->UIProgram);
    }

}
void Editor::vfShaderProcess()
{
    editableImage->outputBuffer.makeActive();
    glClearColor(0.0, 0.0, 0.0, 1.0);///check if needed (or move to frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUniform1i(glGetUniformLocation(activeShaderId, "frameBuf"),1);//for any frame buf for to invert inverted images;
    setShaderInputs();//////set shader inputs;
    GLushort indices[6];
    indices[0] = 0, indices[1] = 1, indices[2] = 2, indices[3] = 2, indices[4] = 3, indices[5] = 0;
    float verts[] = {-1, -1, 1, -1, 1, 1, -1, 1};
    float textCoords[8];
    textCoords[0] = 0.0f, textCoords[1] = 0.0f, textCoords[2] = 1.0f, textCoords[3] = 0.0f, textCoords[4] = 1.0f, textCoords[5] = 1.0f, textCoords[6] = 0.0f, textCoords[7] = 1.0f;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) verts);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) textCoords);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(activeShaderId, "image"), 0);
    glBindTexture(GL_TEXTURE_2D, editableImage->getInputTexId());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *) indices);

    //
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Graphics::printGlError("1fxShader");
    FrameBuffer::setToDefault();



}
void Editor::computeProcess()
{
    //TO decide how work shoulld be decide use workGroupsize and counts
    Graphics::printGlError("computeProcess error1");
    //setShaderInputs();//moved to process();any error move back here;
    glBindImageTexture(0, editableImage->getInputTexId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindImageTexture(1,editableImage->getOutputTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);//////should be moved to setshaderInputs?
    glDispatchCompute(editableImage->workGroupSizeX,editableImage->workGroupSizeY,1);
    //   printGlError("computing");
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindImageTexture(1,0, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
  //  editableImage->showHistogramValues();
  Loge("computeProcess","the dispacthc count is %d",editableImage->getImageWidth()*editableImage->getImageHeight());
    ///////glBindImaageTexture to 0;
}
void Editor::setActiveSubOption(uint ActiveSubOption)
{
    this->subOptionActive=ActiveSubOption;
    setActiveFilter();
    manageShaders();
    if(optionActive==1)
    {//equalize should work on cliking the suboption so no need for sliders
        isUpdatedNeeded=true;
      editableImage->inputHistogram.reset();//just setting isCalculateToFalse in enoug;
        //editableImage->resetHistogram();

    }
    editableImage->outputHistogram.reset();

}
void Editor::setActiveOption(uint ActiveOption)
{
    this->optionActive=ActiveOption;
   // manageShaders();//create shaders only when suboption is selected;


}
void Editor::setOptions(ImageViewStack *optionsMenu, ImageViewStack *subOptionsMenu)
{

}
void Editor::setShaderInputs()
{//no error checking handled
    glUniform1i(FILTERTYPELOC,(int)EactiveFilter);
    switch(EactiveFilter) {
        case LIGHT://should match filterType in shaders
        {
            glUniform1f(PARAMSLOC, params[0]);
        }break;
        case SATURATION:
        {
            glUniform1f(PARAMSLOC, params[0]);
        }break;
        case HUE:
        {
            glUniform1f(PARAMSLOC, params[0]);
        }break;
        case GAMMA://log
        {
            glUniform1f(PARAMSLOC, params[0]);
        }break;
        case CONTRAST://contrast streching
        {

            for(int i=0;i<4;i++)
            {
                glUniform1f(PARAMSLOC+i,params[i]);//can be seetWIthout loop 4f;
            }
        }break;
        case HISTOGRAM:
        {
            editableImage->equalize(subOptionActive);
            if(editableImage->bEqualized)
            {
                //output histogram now contains equalized values; before drawing should be computed
                //if not equalized outpu would be noise so
            }
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,editableImage->outputHistogram.binsBuffer);
            glUniform1i(FILTERTYPELOC,subOptionActive);
        }break;
        case HSI:
        {

            glUniform1fv(PARAMSLOC,4,params);
            glUniform1i(FILTERTYPELOC,subOptionActive);//0 from hsi1 and 1 for groma hsi2;

        }
        break;
        case BLUR:
        {
            glUniform1i(FILTERTYPELOC,subOptionActive);//not needed for now;
            glUniform1f(PARAMSLOC,params[0]);
            glUniform1f(PARAMSLOC+1,params[1]);
        }break;
        case SHARPEN:
        {
            glUniform1i(FILTERTYPELOC,subOptionActive);
            glUniform1f(PARAMSLOC,params[0]);
            glUniform1f(PARAMSLOC+1,params[1]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,editableImage->laplaceBuffer);
            if(subOptionActive==1||subOptionActive==3)//check cases in shader these require two iterations;
                editableImage->computeLaplace();
        }
        default:
        {

        }
    }
}

void Editor::setActiveFilter()
{
    ///need to compile new shader if not same as previous;
    switch(optionActive)
    {
        case 0:
        {
            switch (subOptionActive)
            {
                case 0:
                {
                    EactiveFilter=LIGHT;//also set the shader here./////is all suboption have same shader make it if instead switch for that option
                    eActiveShader=ENHANCE_SHADER;
                }break;
                case 1:
                {
                    EactiveFilter=SATURATION;
                    eActiveShader=ENHANCE_SHADER;

                }break;
                case 2:
                {
                    EactiveFilter=HUE;
                    eActiveShader=ENHANCE_SHADER;
                }break;
                case 3:
                {
                    EactiveFilter=GAMMA;
                    eActiveShader=ENHANCE_SHADER;
                }
                break;
                case 4:
                {
                    EactiveFilter=CONTRAST;
                    eActiveShader=ENHANCE_SHADER;
                }break;
                case 5:
                {
                    EactiveFilter=HISTOGRAM;
                    eActiveShader=ENHANCE_SHADER;/////but computeShader;/////remmovet his
                }break;

            }
        }
        break;
        case 1:
        {
            EactiveFilter=HISTOGRAM;
            eActiveShader=EQ_SHADER;
        }break;
        case 2:
        {
            EactiveFilter=HSI;
            eActiveShader=HSI_SHADER;
        }break;
        case 3:
        {
            EactiveFilter=BLUR;
            eActiveShader=BLUR_SHADER;

        }
        break;
        case 4:
        {
            EactiveFilter=SHARPEN;
            eActiveShader=SHARPEN_SHADER;
        }break;
        case 5:
        {
            EactiveFilter=SMOOTHEN;
            eActiveShader=SMOOTHEN_SHADER;
        }

    }
}

void Editor::toggleProcessingType()
{

        useGLCompute=!useGLCompute;
        if(useGLCompute)
        {
            Loge("Editor","usingGlCompute");
        } else
            Loge("Editor","Not usingGlCompute");


    manageShaders();/////need Right management;
}
void Editor::manageShaders()
{
    //////delete previous unused program;also try removing this class and and in editing context create a method for converting option to shader paths and create shader program using Graphics;
    std::string vertexSource=shadersFolder +"/" + "vertex.glsl";
    std::string fragmentSource=shadersFolder + "/";
    if(useGLCompute)
    {
        fragmentSource=shadersFolder + "/computeShaders/";
    }
//////initially shader is invalid
    Loge("ShaderManager::createShaderPro","%d option",optionActive);
    switch (eActiveShader)
    {
        case ENHANCE_SHADER:
        {
            fragmentSource+="enhance.glsl";
        }
            break;
        case EQ_SHADER:
        {
            fragmentSource += "auto.glsl";
        }
        break;
        case HSI_SHADER:
        {
            fragmentSource += "hsi.glsl";
        }break;
        case BLUR_SHADER:
        {
            fragmentSource+="blur.glsl";
        }
        break;
        case SHARPEN_SHADER:
        {
            fragmentSource+="sharpen.glsl";
        }break;
        case SMOOTHEN_SHADER:
        {
            fragmentSource+="smoothen.glsl";
        }break;
        default:
        {
            Loge("ShaderMangage:createShaderProgram","Invalid Option for creating Shader");
            return ;///need not return zero instead keep active shader program
        }

    }
    /////////////need not compile vertexShader EveryTime as it is same for all(if not deleted) :
      Loge("the shaderloca ","%s ,%s",vertexSource.c_str(),fragmentSource.c_str());
    glDeleteShader(activeShaderId);////should do only if there is change
    if(!useGLCompute)
    activeShaderId= Shader::createShaderProgram(AppContext::getApp(),vertexSource.c_str(),fragmentSource.c_str());
    else
    {
        activeShaderId=Shader::createComputeProgram(AppContext::getApp(),fragmentSource.c_str());
    }
    if(activeShaderId==0)
    {
        Loge("ERROR:EditingonOptionSet","could not create shader program");
    }
}
void Editor::addEditableImage(EditableImage *editableImage)
{
    if(this->layers)//later find layer and add to that layer appropriately;
    {
        this->layers->addEditableImage(editableImage);
    }
}
void Editor::addLayer(Layer *layer)
{
    this->layers=layer;//for now only one;
}
void Editor::draw()
{
    if(layers)
    {
        layers->draw();
    }
}

