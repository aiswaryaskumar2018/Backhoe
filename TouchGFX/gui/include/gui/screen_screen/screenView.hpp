#ifndef SCREENVIEW_HPP
#define SCREENVIEW_HPP

#include <gui_generated/screen_screen/screenViewBase.hpp>
#include <gui/screen_screen/screenPresenter.hpp>
//dsk #include "adc.h"
//dsk#include "TransOilPressreure.h"

class screenView : public screenViewBase
{
public:
    screenView();
    virtual ~screenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
  //  virtual void setADC(int val);

       void handleTickEvent();
       void AF_choke();
       void FourWheel_drive();
       void PCD_Lamp();
       void Fast_Vehicle();
       void MIL_lamp();
       void DPF_lamp();
       void Slow_Vehicle();
       void SCRtempering();
       void FilterClogged();
       void Trans_Oil_Temp_bar();
       void Engine_Oil_Bar();
       void TranOilPressure_bar(void);
       void scr_fluid();
       void engine_oil_pressure();
       void tell_tale();

       void notification_label();
       void PcodeDTC_SPN_FMI_label();

       void incrementText_rpm();
       void incrementText_odo();
       void incrementText_trip();
       void incrementText_AP();
       void incrementText_HourMeter();
       void incrementText_speedo_value();
       void incrementText_soot_load();
       void ShowTransOilTemp();
       void showBattvolt(void);
       void OdoSpeedoTextHourDisplay();
       void GUIProcess(void);
       void increment_counter();
       void SelectMenuScreen(void);
       void HomeScreenParameterProcessed(void);
       void PopUpMessageDisplay_label(void);
       void EngineCoolantLevelStatus(void);

protected:
       bool all_drive_flag=0;
       bool headlamp_low_flag=0;
       bool fast_vehicle_flag=0;
       bool mil_lamp_flag=0;
       bool dpf_lamp_flag=0;
       bool slow_vehicle_flag=0;
       bool engine_emission_flag=0;
       bool fluid_flag=0;
       bool scr_fluid_flag=0;
       bool engine_oil_pressure_flag=0;
       bool tell_tale_flag=0;

       int scr_right_flag=0;
       int  engine_oil_bar_flag=0;
       int  upper_bar_flag=0;
       int notification_flag=0;
       bool trip_label_flag=0;

       int counter=0;
       int counter_rpm=0;
       int counter_decimal_odo=0;
       int counter_odo=0;
       int counter_decimal_trip=0;
       int counter_trip=0;
       int counter_decimal_AP=0;
       int counter_AP=0;
       int counter_decimal_fuel_range=0;
       int counter_fuel_range=0;
       int counter_speedo_value=0;
       int counter_soot_load=0;
       int counter_degree_value=0;
       int counter_volt_value=0;



       unsigned int rpm_value_flag;
       unsigned int oil_temp_flag;
       unsigned int adc_input2_flag;
       unsigned int fuel_gauge_flag;
       unsigned int battery_volt_flag;
       unsigned int adc_input1_flag;
       unsigned int oil_pressure_flag;

       int tickCounter=0;
       int Blink_interval=30;

};

extern bool selfCheck;

#endif // SCREENVIEW_HPP
