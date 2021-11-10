#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"

#define PF123 (*((volatile unsigned long*) 0x40025038)) 

volatile unsigned int count = 0x00;

void PortFunctionInit(void)
{
		volatile uint32_t ui32Loop;   
	
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;// Enable the GPIO port that is used for the on-board LED.

    ui32Loop = SYSCTL_RCGC2_R;// Do a dummy read to insert a few cycles after enabling the peripheral.

		GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock GPIO Port F 
		GPIO_PORTF_CR_R |= 0x11;           // allow changes to PF0, PF4

    GPIO_PORTF_DIR_R &= ~0x11;// Set the direction of PF4 (SW1) and PF0 (SW2) as input by clearing the bit
	
		GPIO_PORTF_DIR_R |= 0x0E; //set direction of PF3 PF2 and PF1 as output.
	
    GPIO_PORTF_DEN_R |= 0x1F;// Enable PF4, PF3, PF2, PF1 and PF0 for digital function.
		
		GPIO_PORTF_PUR_R |= 0x11; //Enable pull-up on PF4 and PF0
}

void IntGlobalEnable(void)//Globally enable interrupts
{
    __asm("    cpsie   i\n");
}

void IntGlobalDisable(void)//Globally disable interrupts 
{
    __asm("    cpsid   i\n");
}

void Timer0A_Init(unsigned long period)
{   
	volatile uint32_t ui32Loop; 
	
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; // activate timer0
  ui32Loop = SYSCTL_RCGC1_R;				// Do a dummy read to insert a few cycles after enabling the peripheral.
  TIMER0_CTL_R &= ~0x00000001;     	// disable timer0A during setup
  TIMER0_CFG_R = 0x00000000;       	// configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;      	// configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;       	// reload value
	NVIC_PRI4_R &= ~0xE0000000; 			// configure Timer0A interrupt priority as 0
  NVIC_EN0_R |= 0x00080000;     		// enable interrupt 19 in NVIC (Timer0A)
	TIMER0_IMR_R |= 0x00000001;      	// arm timeout interrupt data sheet 745
  TIMER0_CTL_R |= 0x00000001;      	// enable timer0A
}

void Interrupt_Init(void)
{
  NVIC_EN0_R |= 0x40000000;  		// enable interrupt 30 in NVIC (GPIOF)// DATA SHEET PG 141
	NVIC_PRI7_R &= ~0x00B00000; 	// configure GPIOF interrupt priority as 2 B 1011
	GPIO_PORTF_IM_R |= 0x11;   		// arm interrupt on PF0 and PF4
	GPIO_PORTF_IS_R &= ~0x11;     // PF0 and PF4 are edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11;   	// PF0 and PF4 not both edges trigger DATA SHEET 658
	GPIO_PORTF_IEV_R &= ~0x11;  	// PF0 and PF4 falling edge event
	
}

void Timer0A_Handler(void)//interrupt handler for Timer0A
{
		TIMER0_ICR_R |= 0x00000001; // acknowledge flag for Timer0A
    count+=0x02;// increment count
		count &= 0x0E;
}

void Timer1A_Init()
{		
		volatile uint32_t ui32Loop2; 
	
		SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER1;        //enable clock to Timer Block 1
		ui32Loop2 = SYSCTL_RCGC1_R;				// dummy read
		TIMER1_CTL_R &= ~0x00000001;     //disable Timer before initialization
		TIMER1_CFG_R |= 0x0;            	//32 bit option. data sheet 727
		TIMER1_TAMR_R |= 0x21;           //one-shot mode and down-counter data sheet 729
		TIMER1_TAILR_R = 1600000; 			 //Timer A interval load value register 1 ms
		NVIC_PRI5_R &= ~0x0000D000; 		// configure Timer1A interrupt priority as 1: D 1101 slides   
		NVIC_EN0_R |= 0x00200000;			 // enable interrupt timer1 21 data sheet 103
		TIMER1_IMR_R |= 0x00000001;		// arm timeout interrupt	
		//TIMER1_CTL_R |= 0x01;				 // enable timer 
}

void Timer1A_Handler(void)
{		
		TIMER1_ICR_R |= 0x1;// clear flag
		IntEnable(INT_GPIOF);// re enable port F interrupt also takes a few cycles to complete function
		//NVIC_EN0_R |= 0x40000000;// another option to re enable GPIO port F
		//GPIO_PORTF_IM_R |= 0x11; arm the interrupt
}

void GPIOPortF_Handler(void)
{
	//debounce by disabling interupt, waiting, then re-enabling it
	IntDisable(INT_GPIOF);// disable interrupt.. takes a few clock cycles to complete function
	//NVIC_DIS0_R &=~0x00200000;//0xE000E180 Very weird how this functions
	//used to disable port F by inversely clearing all bits but the ones we want?
	//not supposed to work this way from what the data sheet says but its how its working
	//GPIO_PORTF_IM_R &= ~0x11; // disarm the interrupt
	//NVIC_EN0_R &= ~0x40000000; This doesn't work to actually disable the interrupt using DIS0 instead which acts weirdly

	if(GPIO_PORTF_RIS_R&0x11){//if switches have action
		
		GPIO_PORTF_ICR_R |= 0x11; // acknowledge flags for PF4 & PF0
		
		if(!(GPIO_PORTF_DATA_R&0x10)){//SW1 is pressed
			count += 0x02;//counter incremented by 2 to correspond to LED
			count &= 0x0E;
		}
		if(!(GPIO_PORTF_DATA_R&0x01)){ //SW2 is pressed
			count -= 0x02;//counter decremented by 2 to correspond to LED
			count &= 0x0E;
		}
	}
	TIMER1_TAILR_R = 1600000;// reload timer val to ensure consistent time
	TIMER1_CTL_R |= 0x01;// enable timer 1A
}

int main(void)
{	
		SysCtlClockSet(SYSCTL_SYSDIV_12_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);// set clock to 16MHz
	
		unsigned long period = SysCtlClockGet(); //reload value to Timer0A to generate 1 second delay
		
		PortFunctionInit();//initialize the GPIO ports
    Interrupt_Init();// initialize GPIO F interrupt
		//Timer0A_Init(period);//initialize Timer0A and configure the interrupt
		//Timer0A commented out till we can figure out why debounce isn't working
		Timer1A_Init();//initialize Timer1A
		IntGlobalEnable(); // globally enable interrupt
		
    while(1)
    {
			PF123 = count;//update LEDs
    }
}
