//
// Created by Shiva Shankar patel psy on 2/16/21.
//

#include <string>
#include "Editing.h"
#include "Main.h"
const int PARAMSLOC=5,FILTERTYPELOC=0;
std::string ShaderManager::shadersFolder="Filters";
void Editor::onInputValuesChanged(uint sliderNo, float newInputValue)
{
    ////////make sliderNo (equal) or attahed to a certain shader input param nad newInputValue to that params value;
    Loge("Editor:onInputChanged ","SliderNo is %d and value is %f",sliderNo,newInputValue);
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
   isUpdatedNeeded=true;
}
void Editor::process()
{
    Loge("process","sds");
    if(isUpdatedNeeded) {
        srand(time(nullptr));
        static float param = 1.0;
        GlobalData::useGlProgram(activeShaderId);
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
        GlobalData *globalData = (GlobalData *) ((AppContext::getApp())->userData);
        GlobalData::useGlProgram(globalData->UIProgram);
        FrameBuffer::setToDefault();
        //glUniform1i(3,0);
        // Graphics::printGlError("2fxShader");


        param += 5;
        if (param > 360.0)
            param = 0.00;
        isUpdatedNeeded=false;
    }
}
void Editor::setActiveSubOption(uint ActiveSubOption)
{
    this->subOptionActive=ActiveSubOption;
    setActiveFilter();
}
void Editor::setActiveOption(uint ActiveOption)
{
    this->optionActive=ActiveOption;
    activeShaderId=ShaderManager::createShaderProgram(optionActive);
    if(activeShaderId==0)
    {
        Loge("ERROR:EditingonOptionSet","could not create shader program");
    }
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
                glUniform1f(PARAMSLOC+i,params[i]);
            }
        }break;
        default:
        {

        }
    }
}
GLuint ShaderManager::createShaderProgram(uint option)
{//////delete previous unused program;also try removing this class and and in editing context create a method for converting option to shader paths and create shader program using Graphics;
std::string vertexSource=shadersFolder +"/" + "vertex.glsl";
std::string fragmentSource=shadersFolder + "/";
//////initially shader is invalid
Loge("ShaderManager::createShaderPro","%d option",option);
    switch (option)
    {
        case 0:
        {
            fragmentSource+="hsi.glsl";
        }
        break;
        case 1:
        {
            fragmentSource=shadersFolder + "test.glsl";
        }
        break;
        default:
        {
            Loge("ShaderMangage:createShaderProgram","Invalid Option for creating Shader");
        return 0;///need not return zero instead keep active shader program
        }

    }
    /////////////need not compile vertexShader EveryTime as it is same for all(if not deleted) :
  //  Loge("the shaderloca ","%s ,%s",vertexSource.c_str(),fragmentSource.c_str());
    return Shader::createShaderProgram(AppContext::getApp(),vertexSource.c_str(),fragmentSource.c_str());
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
                    EactiveFilter=LIGHT;//also set the shader here.
                    eActiveShader=HSI_SHADER;
                }break;
                case 1:
                {
                    EactiveFilter=SATURATION;
                    eActiveShader=HSI_SHADER;

                }break;
                case 2:
                {
                    EactiveFilter=HUE;
                    eActiveShader=HSI_SHADER;
                }break;
                case 3:
                {
                    EactiveFilter=GAMMA;
                    eActiveShader=HSI_SHADER;
                }
                break;
                case 4:
                {
                    EactiveFilter=CONTRAST;
                    eActiveShader=HSI_SHADER;
                }break;

            }
        }
        break;
    }
}
