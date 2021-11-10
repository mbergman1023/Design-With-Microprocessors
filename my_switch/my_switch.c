#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

#define 	RED_MASK 		0x02
//*****************************************************************************
//
//!
//!  Modify the C program in the project my_switch created by yourself in Section C. In the 
//! modified system, when SW2 (connected with PF0) is pressed, the red LED (connected with 
//! PF1) is turned off, otherwise the red LED is on. 
//
//*****************************************************************************


void
PortFunctionInit(void)
{

		volatile uint32_t ui32Loop;   
	
		// Enable the clock of the GPIO port that is used for the on-board LED and switch.
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;

    //
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    //
    ui32Loop = SYSCTL_RCGC2_R;
	
		//red = 0x02, blue = 0x04, green = 0x08
	  // white = 0x0E -> 2+4+8 = 14

		// Unlock GPIO Port F
		GPIO_PORTF_LOCK_R = 0x4C4F434B;   
		GPIO_PORTF_CR_R |= 0x01;           // allow changes to PF0

    // Set the direction of PF2&PF1 (blue and red LED) as output
    GPIO_PORTF_DIR_R |= 0x06;
	
		// Set the direction of PF0 (SW2) as input by clearing the bit
    GPIO_PORTF_DIR_R &= ~0x01;
	
    // Enable PF2,1 and PF0 for digital function.
    GPIO_PORTF_DEN_R |= 0x07;
	
		//Enable pull-up on PF0
		GPIO_PORTF_PUR_R |= 0x01; 

}


int main(void)
{
	
		//initialize the GPIO ports	
		PortFunctionInit();
	
    //
    // Loop forever.
    //
    while(1)
    {

        if((GPIO_PORTF_DATA_R&0x01)==0x01) //SW2 is not pressed
				{
						// Turn on the LED.
						GPIO_PORTF_DATA_R |= RED_MASK; // bitwise or 0x02
				}
				else
				{
						// Turn off the LED.
						GPIO_PORTF_DATA_R &= ~RED_MASK; // 0x02 negated
				}
    }
}
