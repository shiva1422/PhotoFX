//
// Created by shivaaz on 7/9/23.
//

#ifndef PHOTOFX_PHOTOFXAPP_H
#define PHOTOFX_PHOTOFXAPP_H

#include <KSApp/KSApplication.h>

class PhotoFXApp : public KSApplication {

public:

    PhotoFXApp() = delete;

    PhotoFXApp(android_app *app);

    bool onInterceptMotionEvent(const ks::MotionEvent &me) override;

protected:

    void onCreate() override;

    void onStart() override;

    void onResume() override;

    void onPause() override;

    void onStop() override;

    void onDestroy() override;

    void onWindowInit() override;

    void onWindowResized() override;

    void onWindowRedrawNeeded() override;

    void onWindowTermination() override;

    void onContentRectChanged() override;

    void onLowMemory() override;

    void onFocusLost() override;

    void onFocusGained() override;

    void onConfigChanged() override;

    void onSaveState() override;

    void onInputChange() override;

    void runTasks() override;

    void resizeUI();

protected:

    virtual void onDraw() override;

};

#endif //PHOTOFX_PHOTOFXAPP_H
