/*
 * RTC.c
 *
 *  Created on: 2017. aug. 24.
 *      Author: szmik_000
 */


#include "RTC.h"

/*********************************************************************************************************************
-------------------------------------------------- Variables --------------------------------------------------
 *********************************************************************************************************************/

RTC_time_struct time = {0,0,0};
RTC_date_struct date = {2017,11,13,9,45}; //the dafult value
EE_Variable_TypeDef var_year, var_month, var_day, var_hour, var_minute;


/*********************************************************************************************************************
-------------------------------------------------- Local Functions --------------------------------------------------
 *********************************************************************************************************************/

/*************************************************************
 * Reads the values from EEPROM and stores it in the date struct
 *************************************************************/
static void ReadDateFromEEPROM(){
	uint16_t readValue;

	//These are magic numbers that I found in the Flash memory.
	//Using the random numbers in flash is only needed when you want to change the date to the default value upon reset
	EE_Read(&var_year, &readValue);
	if(readValue!=0) date.year = readValue;

	EE_Read(&var_month, &readValue);
	if(readValue!=0) date.month = readValue;

	EE_Read(&var_day, &readValue);
	if(readValue!=0) date.day = readValue;

	EE_Read(&var_hour, &readValue);
	if(readValue!=0) date.hour = readValue;

	EE_Read(&var_minute, &readValue);
	if(readValue!=0) date.minute = readValue;
}


/*************************************************************
 * Updates the date values in EEPROM
 *************************************************************/
static void UpdateDateInEEPROM(){

	//It will only write a value if it has changed
	EE_Write(&var_year, date.year);
	EE_Write(&var_month, date.month);
	EE_Write(&var_day, date.day);
	EE_Write(&var_hour, date.hour);
	EE_Write(&var_minute, date.minute);

}


/*************************************************************
 * Initializes the date in eeprom
 *************************************************************/
static void InitDate(){
	//Variables in the emulated eeprom memory (flash)
	EE_DeclareVariable(&var_year);
	EE_DeclareVariable(&var_month);
	EE_DeclareVariable(&var_day);
	EE_DeclareVariable(&var_hour);
	EE_DeclareVariable(&var_minute);

	//If there were already saved values in EEPROM then load them in
	ReadDateFromEEPROM();

	//Updates the values in memory
	UpdateDateInEEPROM();
	SetGPIO(MCULED3_PORT, MCULED3_PIN, 1);
}

/*************************************************************
 * Returns the amount of days each month has
 * Calculates with leap years too
 *************************************************************/
static uint8_t DaysOfMonth(uint8_t month, uint16_t year){
	switch(month){
		case 1: 	return 31; 	//January
		case 2:{				//February
			if(year%4 == 0)
			    {
			        if( year%100 == 0)
			        {
			            // year is divisible by 400, hence the year is a leap year
			            if ( year%400 == 0)
			                return 29;
			            else
			                return 28;
			        }
			        else
			            return 29;
			    }
			    else
			    	return 28;
		}
		case 3: 	return 31;	//March
		case 4: 	return 30;	//April
		case 5: 	return 31;	//May
		case 6: 	return 30;	//June
		case 7: 	return 31;	//July
		case 8: 	return 31;	//August
		case 9: 	return 30;	//September
		case 10: 	return 31;	//October
		case 11: 	return 30;	//November
		case 12: 	return 31;	//December
		default:	return 0;
	}
}



/*********************************************************************************************************************
-------------------------------------------------- Functions --------------------------------------------------
 *********************************************************************************************************************/

/*************************************************************
 * Initialize RTC
 *************************************************************/
void InitRTC(){

	InitDate();

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO); //<---Enable ULFRCO as LFACLK
	CMU_ClockEnable(cmuClock_RTC, true);

	RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;
	RTC_Init(&rtcInit);
	RTC_CompareSet(0, RTC_IT_INTERVAL);

	NVIC_EnableIRQ(RTC_IRQn);
	RTC_IntEnable(RTC_IEN_COMP0);
	RTC_Enable(true);
}


/*************************************************************
 * Handles RTC interrupts
 *************************************************************/
void RTC_IRQHandler(){
	time.seconds++;

	if(time.seconds>=60){
		time.seconds = 0;
		time.minutes++;

		if(time.minutes >= 60){
			time.minutes = 0;
			time.hours++;

			if(time.hours>=24){
				time.hours=0;
			}
		}

		//Date struct
		date.minute++;

		if(date.minute>=60){
			date.minute = 0;
			date.hour++;

			if(date.hour>=24){
				date.hour=0;
				date.day++;

				if(date.day >= (DaysOfMonth(date.month, date.year)+1)){
					date.day = 1;
					date.month++;

					if(date.month>=13){
						date.month = 1;
						date.year++;
					}
				}
			}
		}

	}
	UpdateDateInEEPROM(); //Note: It can't write to flash while in EM3

	event = RTC_IT_EVENT;
	RTC_IntClear(RTC_IntGet());
}

/*************************************************************
 * Returns the specified time
 * t: can be second, minute or hour
 *************************************************************/
int getTimeRTC(time_enum t){
	switch(t){
		case SECOND: return time.seconds; break;
		case MINUTE: return time.minutes; break;
		case HOUR:	 return time.hours; break;
		default: return -1;
	}
	return -1;
}


/*************************************************************
 * Returns the time struct which contains the seconds,
 * minutes and hours too
 *************************************************************/
RTC_time_struct getTimeStructRTC(){
	return time;
}

/*************************************************************
 * Returns true if the given seconds has elapsed
 *************************************************************/
unsigned didElapseGivenSeconds(int seconds, RTC_time_struct previous_time ){
	unsigned long current_time_s 	= time.hours*24*60 + time.minutes*60 + time.seconds;
	unsigned long previous_time_s	= previous_time.hours*24*60 + previous_time.minutes*60 + previous_time.seconds;

	if(current_time_s >= (previous_time_s+seconds))return true;
	else return false;
}


/*************************************************************
 * Sends out the date using the RFduino
 *************************************************************/
void SendDate(){
	char tempc[20];
	snprintf(tempc,20, "%d/%d/%d %d:%d\n",date.year,date.month,date.day,date.hour,date.minute);
	send_string(tempc);
}
