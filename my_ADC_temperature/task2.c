#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"


//*****************************************************************************
//
//!
//! Modify the project you implemented in Task 1. Instead of sampling signal from 
//!the internal temperature sensor, sample signal from the analog input port Ain0 using 
//!ADC0 SS3 in the modified implementation.
//
//*****************************************************************************

uint32_t ui32ADC0Value;

volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;
volatile uint32_t ui32InputVoltage;

//**
void PortE_Init(){

	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	SysCtlDelay(2);
	
	GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTE_CR_R |= 0X08; //allow changes to PE3
	
	GPIO_PORTE_DIR_R &= ~0X08;// make PE3 input
	GPIO_PORTE_AFSEL_R |= 0X08;
	GPIO_PORTE_DEN_R |= ~0X08;// diable digital
	GPIO_PORTE_AMSEL_R |= 0X08;// enable analog
}
//**

//ADC0 initializaiton
void ADC0_Init(void)
{
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	//activate the clock of ADC0
		SysCtlDelay(2);	//insert a few cycles after enabling the peripheral to allow the clock to be fully activated.
		
		ADCSequenceDisable(ADC0_BASE, 3); //disable ADC0 before the configuration is complete

		ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	//ADC0 SS3 Step 0, sample from internal temperature sensor, completion of this step will set RIS, last sample of the sequence
	//**
		ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END); // Task 2
	//**
		IntPrioritySet(INT_ADC0SS3, 0x00);  	 // configure ADC0 SS3 interrupt priority as 0
		IntEnable(INT_ADC0SS3);    				// enable interrupt 33 in NVIC (ADC0 SS3)// data sheet 103
		ADCIntEnableEx(ADC0_BASE, ADC_INT_SS3);      // arm interrupt of ADC0 SS3
	
		ADCSequenceEnable(ADC0_BASE, 3); //enable ADC0
}
		
//interrupt handler
void ADC0_Handler(void)
{
		ADCIntClear(ADC0_BASE, 3);
		ADCProcessorTrigger(ADC0_BASE, 3);
		ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value);
		//ui32TempValueC = (1475 - ((2475 * ui32ADC0Value)) / 4096)/10;
		//ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
		ui32InputVoltage = ui32ADC0Value * 0.000806;
	
}

int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // configure the system clock to be 40MHz
		PortE_Init();
		ADC0_Init();
		IntMasterEnable();       		// globally enable interrupt
		ADCProcessorTrigger(ADC0_BASE, 3);

		while(1)
		{	
		}
}
