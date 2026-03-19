#ifndef SCREEN_MENUVIEW_HPP
#define SCREEN_MENUVIEW_HPP

#include <gui_generated/screen_menu_screen/Screen_menuViewBase.hpp>
#include <gui/screen_menu_screen/Screen_menuPresenter.hpp>

class Screen_menuView : public Screen_menuViewBase
{
public:
    Screen_menuView();
    virtual ~Screen_menuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();
    void SelectScreen(void);
    void ChangeScreen(void);

protected:
   int Processounter = 0;
};

#endif // SCREEN_MENUVIEW_HPP
