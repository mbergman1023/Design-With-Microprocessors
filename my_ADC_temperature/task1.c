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
//!Modify the sample project “ADC_temperature”. Instead of using ADC0, SS1 to 
//!sample signals from the internal temperature sensor, use ADC0, SS3 in the modified 
//!implementation.
//! 
//
//*****************************************************************************

uint32_t ui32ADC0Value;

volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;
volatile uint32_t ui32InputVoltage;

//ADC0 initializaiton
void ADC0_Init(void)
{
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	//activate the clock of ADC0
		SysCtlDelay(2);	//insert a few cycles after enabling the peripheral to allow the clock to be fully activated.
		
		ADCSequenceDisable(ADC0_BASE, 3); //disable ADC0 before the configuration is complete

		ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	//ADC0 SS3 Step 0, sample from internal temperature sensor, completion of this step will set RIS, last sample of the sequence
		//**
		ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);// Task 1
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
		//**
		ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value);
		//**
		ui32TempValueC = (1475 - ((2475 * ui32ADC0Value)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
		ui32InputVoltage = ui32ADC0Value * 0.000806;

}

int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // configure the system clock to be 40MHz

		ADC0_Init();
		IntMasterEnable();       		// globally enable interrupt
		ADCProcessorTrigger(ADC0_BASE, 3);
		
		while(1)
		{	
		}
}
