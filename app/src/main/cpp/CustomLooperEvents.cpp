//
// Created by Shiva Shankar patel psy on 4/7/21.
//

#include <unistd.h>
#include <errno.h>
#include <cstring>
#include "CustomLooperEvents.h"
#include "Commons.h"
 int CustomLooperEvents::writeFd,CustomLooperEvents::readFd;
 android_poll_source CustomLooperEvents::customPollSource;
void customPollProcess(struct android_app* app, struct android_poll_source* source)
 {
    Loge("Custom poll pRoces","input recieved on Custom Event");
    int cmd;
     if (read(CustomLooperEvents::readFd, &cmd, sizeof(cmd)) == sizeof(cmd))
     {
         Loge("the message is ","the %d",cmd);
     }
 }
 int callback(int fd,int events,void *data)
 {
     Loge("Custom poll pRoces","input recieved on Custom Event");
     return 3;

 }
void CustomLooperEvents::init()
{
     android_app *app=AppContext::getApp();
     if(!app)
     {
         Loge("CustomEvent init","no app instance Yet");
         return;
     }
    int msgpipe[2];
    if (pipe(msgpipe)) {
        Loge("Custom loope eveents","could not create pipe: %s", strerror(errno));
        return ;
    }
    customPollSource.id=LOOPER_ID_USER;
    customPollSource.app=AppContext::getApp();
    customPollSource.process=customPollProcess;
    readFd = msgpipe[0];
    writeFd = msgpipe[1];
    ALooper_addFd(app->looper, readFd, LOOPER_ID_USER, ALOOPER_EVENT_INPUT, NULL,&customPollSource);
    Loge("CustomEvent init","success");
}
void CustomLooperEvents::addCustomEvent()
{
     int cmd=5;
    if (write(writeFd, &cmd, sizeof(cmd)) != sizeof(cmd))
    {
        Loge("addCustomEvent","Failure writing android_app cmd: %s\n", strerror(errno));
        return;
    }
    Loge("CustomEvent","added");
}