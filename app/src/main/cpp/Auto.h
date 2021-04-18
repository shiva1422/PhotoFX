//
// Created by Shiva Shankar patel psy on 4/14/21.
//

#ifndef PHOTOFX_AUTO_H
#define PHOTOFX_AUTO_H
/*
 * cleanup bins buffer on change of option;
 */
enum EAutoType{AUTO_RED=0,AUTO_GREEN,AUTO_BLUE,
               AUTO_SATURATION,AUTO_INTENSITY,
               AUTO_CYAN,AUTO_MAGENTA,AUTO_YELLOW,//first 8 need equalization;
               AUTO_CGB,AUTO_RMB,AUTO_RGY,
               AUTO_RBG,AUTO_GBR,AUTO_GRB,AUTO_BRG,AUTO_BGR,
               AUTO_NONE
                };
class EditableImage;
class Auto {
private:
    static EAutoType autoType;
public:
    static void setAutoType(int autoType);
    static void apply(EditableImage *editableImage, EAutoType eAutoType);
};


#endif //PHOTOFX_AUTO_H
