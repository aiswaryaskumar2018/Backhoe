#include <gui/screen_brightness_screen/Screen_brightnessView.hpp>
#include "Screen.h"
#include "Brightness.h"


uint8_t BrightnessScreenEnterStatus = FALSE;


Screen_brightnessView::Screen_brightnessView()
{

}

void Screen_brightnessView::setupScreen()
{
    Screen_brightnessViewBase::setupScreen();
    BrightnessScreenEnterStatus = TRUE;
    brightness_value();
    TftBrightNessProcess();
}

void Screen_brightnessView::tearDownScreen()
{
    Screen_brightnessViewBase::tearDownScreen();
}

void Screen_brightnessView::handleTickEvent()
{
	if(++Processounter >= 30u)
	{
		Processounter = 0;
		brightness_value();
		TftBrightNessProcess();
	}
}


void Screen_brightnessView::brightness_value()
{
	uint8_t value;

		value = GetBrightnessBarStatus();

		if( GetScreenStatus() == eBrightnessScreen )
		{
			switch(value)
			{
				case eBrightnessLevel_1_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(false);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(false);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(false);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(false);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(false);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(false);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();
				break;
				case eBrightnessLevel_2_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(false);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(false);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(false);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(false);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(false);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();
				break;
				case eBrightnessLevel_3_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(false);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(false);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(false);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(false);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();
				break;
				case eBrightnessLevel_4_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(false);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(false);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(false);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				case eBrightnessLevel_5_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(true);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(false);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(false);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				case eBrightnessLevel_6_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(true);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(true);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(false);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				case eBrightnessLevel_7_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(true);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(true);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(true);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(false);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				case eBrightnessLevel_8_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(true);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(true);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(true);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(true);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(false);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				case eBrightnessLevel_9_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(true);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(true);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(true);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(true);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(true);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(false);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				case eBrightnessLevel_10_Bars:
					brightness_fill_bar_1.setVisible(true);
					brightness_fill_bar_1.invalidate();
					brightness_fill_bar_2.setVisible(true);
					brightness_fill_bar_2.invalidate();
					brightness_fill_bar_3.setVisible(true);
					brightness_fill_bar_3.invalidate();
					brightness_fill_bar_4.setVisible(true);
					brightness_fill_bar_4.invalidate();
					brightness_fill_bar_5.setVisible(true);
					brightness_fill_bar_5.invalidate();
					brightness_fill_bar_6.setVisible(true);
					brightness_fill_bar_6.invalidate();
					brightness_fill_bar_7.setVisible(true);
					brightness_fill_bar_7.invalidate();
					brightness_fill_bar_8.setVisible(true);
					brightness_fill_bar_8.invalidate();
					brightness_fill_bar_9.setVisible(true);
					brightness_fill_bar_9.invalidate();
					brightness_fill_bar_10.setVisible(true);
					brightness_fill_bar_10.invalidate();

					Unicode::snprintf(brightness_percent_valueBuffer, BRIGHTNESS_PERCENT_VALUE_SIZE, "%u", (value*10));
					brightness_percent_value.invalidate();

				break;
				default:
					/* NA */
				break;

		    }
		}

		if( GetScreenStatus() == eHomeScreen )
		{
			exit_from_Brightness();
		}

}

uint8_t GetBrightnessScreenEnterStatus(void)
{
	return  BrightnessScreenEnterStatus;
}

void SetBrightnessScreenEnterStatus(uint8_t Status)
{
	BrightnessScreenEnterStatus = Status;
}

