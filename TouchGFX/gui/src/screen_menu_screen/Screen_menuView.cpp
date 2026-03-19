#include <gui/screen_menu_screen/Screen_menuView.hpp>
#include "Screen.h"

Screen_menuView::Screen_menuView()
{

}

void Screen_menuView::setupScreen()
{
    Screen_menuViewBase::setupScreen();
}

void Screen_menuView::tearDownScreen()
{
    Screen_menuViewBase::tearDownScreen();
}

void Screen_menuView::handleTickEvent()
{

		if(++Processounter >= 1u)
		{
			Processounter = 0;
			SelectScreen();
		}
}

void Screen_menuView::SelectScreen()
{
//	static uint8_t screenStatus = GetScreenStatus();

	if( GetArrowStatus() == eBrightnessSelectState )
	{
		brightness_arrow_img.setVisible(true);
		brightness_arrow_img.invalidate();
		pcodes_arrow_img.setVisible(false);
		pcodes_arrow_img.invalidate();
		exit_arrow_img.setVisible(false);
		exit_arrow_img.invalidate();

		ChangeScreen();
	}

	else if( GetArrowStatus() == ePCodeSelectState )
	{
		brightness_arrow_img.setVisible(false);
		brightness_arrow_img.invalidate();
		pcodes_arrow_img.setVisible(true);
		pcodes_arrow_img.invalidate();
		exit_arrow_img.setVisible(false);
		exit_arrow_img.invalidate();

		ChangeScreen();
	}

	else if( GetArrowStatus() == eExitState )
	{
		brightness_arrow_img.setVisible(false);
		brightness_arrow_img.invalidate();
		pcodes_arrow_img.setVisible(false);
		pcodes_arrow_img.invalidate();
		exit_arrow_img.setVisible(true);
		exit_arrow_img.invalidate();

		ChangeScreen();
	}

}

void Screen_menuView::ChangeScreen()
{
	//static uint8_t screenStatus = GetScreenStatus();

	if( GetScreenStatus() == eBrightnessScreen && GetArrowStatus() == eBrightnessSelectState )
	{
		change_to_brightnessScreen();
	}

	else if( GetScreenStatus() == ePCodeScreen && GetArrowStatus() == ePCodeSelectState)
	{
		change_to_pcodesScreen();
	}

	else if( GetScreenStatus() == eHomeScreen && GetArrowStatus() == eExitState )
	{
		//exit_flag = FALSE;
		//setExitFlagStatus(0);
		exit_from_menu();
	}

}
