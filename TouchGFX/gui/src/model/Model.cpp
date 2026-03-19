#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#ifndef SIMULATOR
#include"main.h"
#endif

extern"C"
{
 extern ADC_HandleTypeDef hadc1;
 extern uint16_t ADC_value;


}
Model::Model() : modelListener(0),ADC_VAL(50)
{

}

void Model::tick()
{
#ifndef SIMULATOR
//HAL_ADC_Start(&hadc1);
//HAL_ADC_PollForConversion(&hadc1,1);
//HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_RESET);
//HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
//HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
//ADC_value=HAL_ADC_GetValue(&hadc1);
//HAL_ADC_Stop(&hadc1);
//ADC_VAL=map(value,0,4095,0,100);
modelListener->incrementText_rpm(ADC_value);
#endif


}
