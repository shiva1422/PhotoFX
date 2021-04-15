//
// Created by Shiva Shankar patel psy on 4/14/21.
//

#ifndef PHOTOFX_AUTO_H
#define PHOTOFX_AUTO_H
enum AutoType{AUTO_RED=0,AUTO_GREEN,AUTO_BLUE,
              AUTO_CYAN,AUTO_MAGENTA,AUTO_YELLOW,
              AUTO_SATURATION,AUTO_INTENSITY,
              AUTO_RBG,AUTO_GBR,AUTO_GRB,AUTO_BRG,AUTO_BGR
                };
class EditableImage;
class Auto {
public:
    static void apply(EditableImage *editableImage,AutoType eAutoType);
};


#endif //PHOTOFX_AUTO_H
