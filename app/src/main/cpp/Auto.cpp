//
// Created by Shiva Shankar patel psy on 4/14/21.
//

#include "Auto.h"
#include "gpgpu.h"
#include "EditableImage.h"
EAutoType Auto::autoType=AUTO_NONE;
void Auto::setAutoType(int autoType)
{
    Auto::autoType= static_cast<EAutoType>(autoType);
}
void Auto::apply(EditableImage *editableImage, EAutoType eAutoType)
{
    //auto program should be active; which is done in editor or layer
   //one more effect use the histogram for one option apply to other lije equalize reds and apply that to blue;
   if((int)Auto::autoType<8)
   {
       editableImage->inputHistogram.reset();///outputhistogrma.reset ,without resetl
       editableImage->equalize((int)Auto::autoType);//only first 8 are equalized;
   }
   Loge("the auto option is ","%d",(int)eAutoType);
    if(editableImage->bEqualized||(int)Auto::autoType>=8)
    {
        /*output histogram now contains equalized values; before drawing should be computed
         if not equalized outpu would be noise so*/

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,editableImage->outputHistogram.binsBuffer);
        glUniform1i(0,autoType);
        editableImage->bindTexturesToShaderImages();
        glDispatchCompute(editableImage->getImageWidth(),editableImage->getImageHeight(),1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
        editableImage->unbindTexturesToShaderImages();//required?
        Loge("Auto::apply","the dispacthc count is %d",editableImage->getImageWidth()*editableImage->getImageHeight());
    }
    else
    {
        Loge("Auto::apply","could not equalize ");
    }

}