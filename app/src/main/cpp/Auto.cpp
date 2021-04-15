//
// Created by Shiva Shankar patel psy on 4/14/21.
//

#include "Auto.h"
#include "gpgpu.h"
#include "EditableImage.h"

void Auto::apply(EditableImage *editableImage,AutoType eAutoType)
{
    //auto program should be active; which is done in editor or layer

    editableImage->equalize(eAutoType);
    if(editableImage->bEqualized)
    {
        /*output histogram now contains equalized values; before drawing should be computed
         if not equalized outpu would be noise so*/

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,editableImage->outputHistogram.binsBuffer);
        glUniform1i(0,eAutoType);
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