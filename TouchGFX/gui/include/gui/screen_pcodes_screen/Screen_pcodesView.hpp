#ifndef SCREEN_PCODESVIEW_HPP
#define SCREEN_PCODESVIEW_HPP

#include <gui_generated/screen_pcodes_screen/Screen_pcodesViewBase.hpp>
#include <gui/screen_pcodes_screen/Screen_pcodesPresenter.hpp>

class Screen_pcodesView : public Screen_pcodesViewBase
{
public:
    Screen_pcodesView();
    virtual ~Screen_pcodesView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();
	void Pcode_screen();

	protected:
	   int Processounter = 0;
};

#endif // SCREEN_PCODESVIEW_HPP
