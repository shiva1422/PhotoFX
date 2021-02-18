//
// Created by Shiva Shankar patel psy on 2/16/21.
//

#ifndef PHOTOFX_EDITING_H
#define PHOTOFX_EDITING_H
enum EditOptions{HSI=0};
enum EditSubOption{H=0,S,I};
class EditingContext{
private:
    uint optionActive=0,subOptionActive=0,noOfSliders=0.0;
    float sliderValues[4]={0.0f,0.0f,0.0f,0.0f};
};
class Layer{
    //Layer has similar functionality of ImageView stack with its own editing feature;
};
#endif //PHOTOFX_EDITING_H
