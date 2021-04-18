//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#include <android_native_app_glue.h>
#include "Commons.h"
#include "Main.h"
#include "JavaCalls.h"
#include "Graphics.h"
void onCmd(android_app* app, int32_t cmd)
{

    //  IO("command handling");
    switch(cmd)
    {
        case APP_CMD_SAVE_STATE:
            // the OS asked us to save the state of the app
            break;
        case APP_CMD_INIT_WINDOW:

        {
            hideSystemUI(app);
            //  CallbacksLog("the window h is %d and the window w is %d",ANativeWindow_getHeight(app->window),ANativeWindow_getWidth(app->window));
            AppContext  *appContext=((PhotoApp *)app->userData)->appContext;
            if(appContext->appFirstOpen)
            {
                if (Graphics::init_display(appContext) == STATUS_OK)
                {
                    Logi("WINDOW INIT","graphics initialized succesfully");
                    appContext->glInitStatus = true;
                    appContext->windowInitStatus = true;
                    appContext->appFirstOpen=false;
                }
            }
            else
            {
                if(Graphics::onAppReopen(appContext)==STATUS_OK)
                {
                    appContext->windowInitStatus= true;
                };
            }


        }
            break;
        case APP_CMD_TERM_WINDOW: {
            // clean up the window because it is being hidden/closed
            AppContext  *appContext=((PhotoApp *)app->userData)->appContext;
            appContext->windowInitStatus=false;


        }
            break;
        case APP_CMD_LOST_FOCUS:
            // if the app lost focus, avoid unnecessary processing
            //(like monitoring the accelerometer)
            break;
        case APP_CMD_GAINED_FOCUS:
            // bring back a certain functionality, like monitoring the accelerometer
            break;
    }
}
