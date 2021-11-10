#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"

#define 	GREEN_MASK 		0x08
//*****************************************************************************
//
//!
//! Modify the code to make the green LED (connected with PF3) flash. Also try to 
//! make the LED flash 5 times slower. 
//
//*****************************************************************************


void
PortFunctionInit(void)
{
//
		volatile uint32_t ui32Loop;   
	// Enable the GPIO port that is used for the on-board LED.
    //
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;

    //
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    //
    ui32Loop = SYSCTL_RCGC2_R;

    //
    // Enable the GPIO pin for the red LED (PF1).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //	red = 0x02 blue = 0x04 green = 0x08
    //	white = 0x0E -> 2+4+8= 14
   																	
    GPIO_PORTF_DIR_R |= GREEN_MASK; // 0x08 = 00001000 
    GPIO_PORTF_DEN_R |= GREEN_MASK;

}


int main(void)
{
	
		//initialize the GPIO ports	
		PortFunctionInit();
	
    // Turn on the LED.
    GPIO_PORTF_DATA_R |= GREEN_MASK;

    
    //
    // Loop forever.
    //
    while(1)
    {
        // Delay for a bit.
	SysCtlDelay(10000000);	// Original delay: 2000000

        // Toggle the LED.
        GPIO_PORTF_DATA_R ^= GREEN_MASK; // bitwise xor to change only specified bits
    }
}
