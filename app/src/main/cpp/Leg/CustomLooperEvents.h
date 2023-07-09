//
// Created by Shiva Shankar patel psy on 4/7/21.
//

#ifndef PHOTOFX_CUSTOMLOOPEREVENTS_H
#define PHOTOFX_CUSTOMLOOPEREVENTS_H


#include <android_native_app_glue.h>
enum CustomEventType{IMPORTIMAGE};
typedef struct CustomMsg
{
    CustomEventType eEventType;
    int fd;

}CustomMsg;
class CustomLooperEvents{
public:
    static int readFd,writeFd;
    static struct android_poll_source customPollSource;
    static void init();
    static void addCustomEvent(CustomMsg msg);
};


#endif //PHOTOFX_CUSTOMLOOPEREVENTS_H
