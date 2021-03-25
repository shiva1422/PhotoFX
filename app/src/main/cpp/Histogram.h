//
// Created by Shiva Shankar patel psy on 3/24/21.
//

#ifndef PHOTOFX_HISTOGRAM_H
#define PHOTOFX_HISTOGRAM_H


#include "UI.h"
class EditableImage;
enum EHistogramType{R=0,G,B,I,H,S};//order should match with shader implemented order
class Histogram : public ImageView{
private:
    GLuint binsTexIds[2]={0,0};//think if threee texids needed Combine into 1;
    int32_t binsSize=360;//256 would be enough but for hue eq 360 required;
    int32_t outBins[360];
    int activeHistogram=0;//0 for input 1 for output used for drawing;
    bool bCalculated=false,boutBinsCalculated=false;
    EHistogramType eHistogramType=R;//remember to set according to suboptions
    EditableImage *ownerImage;



public:
    GLuint binsBuffers[2]={0,0};//first one stores input bins but second one stores equalized values,third one store ouput bins,three not need make one buffer holding all;
    Histogram();//rightNow only Default Contructor dont use imageViewConstructor while Creating;
    void setOwnerImage(EditableImage *owner){this->ownerImage=owner;}
    void compute(int histogramType);//for what should be same as in auto shader//replace for what with enum
    void reset();
    void printValues();//same as computeParam
    bool isCalculated(){return bCalculated;}
    virtual void draw() override ;
    void drawOutput();
    void drawInput();
    void setInputHistogramActive(){activeHistogram=0;}
    void setOuputHistogramActive(){activeHistogram=1;}
    void toggleActive()
    {
        activeHistogram+=1;
        if(activeHistogram>=3)
        {activeHistogram=0;}
    }

};


#endif //PHOTOFX_HISTOGRAM_H
