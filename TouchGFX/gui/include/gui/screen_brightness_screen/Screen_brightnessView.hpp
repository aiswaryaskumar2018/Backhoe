#ifndef SCREEN_BRIGHTNESSVIEW_HPP
#define SCREEN_BRIGHTNESSVIEW_HPP

#include <gui_generated/screen_brightness_screen/Screen_brightnessViewBase.hpp>
#include <gui/screen_brightness_screen/Screen_brightnessPresenter.hpp>

class Screen_brightnessView : public Screen_brightnessViewBase
{
public:
    Screen_brightnessView();
    virtual ~Screen_brightnessView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();
    void brightness_value();

    protected:
       int Processounter = 0;
};

#endif // SCREEN_BRIGHTNESSVIEW_HPP
