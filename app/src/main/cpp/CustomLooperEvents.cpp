//
// Created by Shiva Shankar patel psy on 4/7/21.
//

#include <unistd.h>
#include <errno.h>
#include <cstring>
#include "CustomLooperEvents.h"
#include "Commons.h"
#include "Main.h"

int CustomLooperEvents::writeFd,CustomLooperEvents::readFd;
 android_poll_source CustomLooperEvents::customPollSource;
void customPollProcess(struct android_app* app, struct android_poll_source* source)
 {
    Loge("Custom poll pRoces","input recieved on Custom Event");
    CustomMsg msg;
     if (read(CustomLooperEvents::readFd, &msg, sizeof(msg)) == sizeof(msg))
     {
         switch (msg.eEventType)
         {
             case IMPORTIMAGE:
             {
                PhotoApp *globalData= static_cast<PhotoApp *>(app->userData);
                globalData->onImportImage(msg.fd);
             }break;

         }
     }
     else
     {
         Loge("CustomPollProcess","cannot obtain msg");
     }
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
void CustomLooperEvents::addCustomEvent(CustomMsg msg)
{

    if (write(writeFd, &msg, sizeof(msg)) != sizeof(msg))
    {
        Loge("addCustomEvent","Failure writing android_app cmd: %s\n", strerror(errno));
        return;
    }
    Loge("CustomEvent","added");
}