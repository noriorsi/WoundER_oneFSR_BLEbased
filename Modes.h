/*
 * Modes.h
 *
 *  Created on: 27 Apr 2017
 *      Author: Mikl�s
 */

#ifndef SRC_MODES_H_
#define SRC_MODES_H_

/*************************************************
 * Includes
 **************************************************/

#include "GPIO.h"
#include "SI7021.h"
#include "ADC.h"
#include "RFDuino.h"
#include "EmMode.h"
#include "RTC.h"
#include "EEPROM.h"
#include "Flash.h"
/*************************************************
 * Functions
 **************************************************/

void EnterPowerSaving();
void MotionSensing();
void PowerSavingModeNotification(unsigned mode);
void ContinousMeasurement();
void SendAndSaveDatas(int n, int period);
//void SendAndSaveDatasPeriod(int n, int period);
void readADConly();
void Measure(int n, int period);
double forceNewton(uint32_t f);
double hgmm (uint32_t f);
double quickMeas(uint32_t f);
double forceing(uint32_t f);

/*************************************************
 * Defines
 **************************************************/

#define MODE0		0
#define MODE1		1
#define MODE2		2
#define MODE3		3

#define PARAMETRIC_MEASUREMENT_STORE_SIZE	100

#endif /* SRC_MODES_H_ */
