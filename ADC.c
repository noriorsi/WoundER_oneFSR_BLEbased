
#include "ADC.h"


/**************************************************
 *  Initializes the ADC module
**************************************************/
void InitADC(){

	CMU_ClockEnable(cmuClock_ADC0, true);
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	init.ovsRateSel = adcOvsRateSel2;
	init.lpfMode = adcLPFilterBypass;
	init.warmUpMode = adcWarmupNormal;
	init.timebase = ADC_TimebaseCalc(0);
	init.prescale = ADC_PrescaleCalc(7000000, 0);
	init.tailgate = 0;

	ADC_Init(ADC0, &init);
}


/**************************************************
 *  Makes an ADC scan on CH4 = PD4 and returns the value
**************************************************/
// the analog reading from the FSR resistor divider
uint32_t GetADCvalue(void) {

	ADC_InitScan_TypeDef scanInit = ADC_INITSCAN_DEFAULT;
	scanInit.reference = adcRefVDD;
	scanInit.input     = ADC_SCANCTRL_INPUTMASK_CH4;
	ADC_InitScan(ADC0, &scanInit);
	ADC_Start(ADC0, adcStartScan);

	while (ADC0->STATUS & ADC_STATUS_SCANACT) ;
	//Get ADC result (FSR adc result)
	return ADC_DataScanGet(ADC0);

}

// the analog reading converted to voltage - not used jet
/*
double ADC_to_Voltage_forBattery(uint32_t ADCvalue){
	//Calculate with the voltage divider resistors
	return (ADCvalue / ADC_MAX_VALUE * VMCU * (RESISTOR1+RESISTOR2)/RESISTOR2);
}*/

double ADC_to_Voltage(uint32_t ADCvalue){
	//Calculate with the voltage divider resistors
	return (ADCvalue / ADC_MAX_VALUE * VMCU );
}

double Voltage_to_force(double volt){
	volt *= 1000.0;			 // now its in mV
	double fsrResistance;	 //ohm
	double fsrConductance;
	double fsrForce;		 // Newton
	double vmcu = 3300.0;	 // now its in mV
	fsrResistance = vmcu - volt;
	fsrResistance *= 10000.0;// 10K resistor in ohm
	fsrResistance /= volt;
	fsrConductance = 1000000.0;
	fsrConductance /= fsrResistance;

		if (fsrConductance<=1000.0){
					fsrForce = fsrConductance/80.0; //first figure from datasheet
				}else{
					fsrForce = fsrConductance - 1000.0;
					fsrForce /= 30.0; 				//second figure from datasheet
				}
		return fsrForce;
}


/*double BatteryVoltageMeasurement(){
	SetGPIO(BAT_MEAS_EN_PORT, BAT_MEAS_EN_PIN, 1);
	uint32_t adc_temp = GetADCvalue();
	SetGPIO(BAT_MEAS_EN_PORT, BAT_MEAS_EN_PIN, 0);

	return ADC_to_Voltage(adc_temp);
}*/

/**************************************************
 *  Truncates the 32 bit unsigned to a 16 bit unsigned
**************************************************/
/*
uint16_t Convert32to16bit(uint32_t source){
	return (source>>16);
}*/
