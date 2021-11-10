#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"

#define RED_MASK 0x02;


//*****************************************************************************
//
//! if you want to sample an input signal at 1000 Hz, how would you implement it
//
//*****************************************************************************

uint32_t ui32ADC0Value;
volatile uint32_t ui32InputVoltage;

//**
void Timer0A_Init(unsigned long period)
{   
	//
  // Enable Peripheral Clocks 
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // peripheral driver pg 483
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER0_BASE, TIMER_A, period -1);      //reload value
	TimerControlTrigger(TIMER0_BASE,TIMER_A,true); // enable trigger for ADC 
  TimerEnable(TIMER0_BASE, TIMER_A);      // enable timer0A
}

//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
		// acknowledge flag for Timer0A timeout
		TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void PortF_Init(){

		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF1 for GPIOOutput
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

}
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

//ADC0 initializaiton
void ADC0_Init(void)
{
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	//activate the clock of ADC0
		SysCtlDelay(2);	//insert a few cycles after enabling the peripheral to allow the clock to be fully activated.
		
		ADCSequenceDisable(ADC0_BASE, 3); //disable ADC0 before the configuration is complete
		//**
		ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 1); // will use ADC0, SS3, timer0A-trigger, priority 1
		//**
		ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END); // Task 2
	
		IntPrioritySet(INT_ADC0SS3, 0x00);  	 // configure ADC0 SS3 interrupt priority as 0
		IntEnable(INT_ADC0SS3);    				// enable interrupt 33 in NVIC (ADC0 SS3)// data sheet 103
		ADCIntEnableEx(ADC0_BASE, ADC_INT_SS3);      // arm interrupt of ADC0 SS3
	
		ADCSequenceEnable(ADC0_BASE, 3); //enable ADC0
}
		
//interrupt handler
void ADC0_Handler(void)
{
		ADCIntClear(ADC0_BASE, 3);
		ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value);
		ui32InputVoltage = ui32ADC0Value * 0.000806;
	
		GPIO_PORTF_DATA_R ^= RED_MASK;
}

int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // configure the system clock to be 40MHz
		PortE_Init();
		PortF_Init();
		Timer0A_Init(SysCtlClockGet()/1000); // 1kHz sample rate
		ADC0_Init();
		IntMasterEnable();       		// globally enable interrupt
		GPIO_PORTF_DATA_R |= RED_MASK; //used to measure sampling frequency
		
		while(1)
		{	
		}
}
