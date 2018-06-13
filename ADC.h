/*
 * ADC.h
 *
 *  Created on: 2017. márc. 31.
 *      Author: MEMS
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

/**************************************************
 *  Includes
**************************************************/

#include "em_adc.h"
#include "em_cmu.h"
#include "GPIO.h"

#define VMCU			3.3
#define	ADC_MAX_VALUE	4096.0
#define VBAT			3.0
//#define RESISTOR1		10.0
//#define RESISTOR2		33.0

#define ADC_BATTERY		1
/**************************************************
 *  Functions
**************************************************/
void InitADC();
uint32_t GetADCvalue(void);
double ADC_to_Voltage(uint32_t ADCvalue);
double Voltage_to_force(double volt);
//double ADC_to_Voltage_forBattery(uint32_t ADCvalue);
//uint16_t Convert32to16bit(uint32_t source);
//double BatteryVoltageMeasurement();
#endif /* SRC_ADC_H_ */
