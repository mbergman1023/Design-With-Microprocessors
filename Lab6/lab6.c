//*****************************************************************************
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This file was automatically generated by the Tiva C Series PinMux Utility
// Version: 1.0.4
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "lab6.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"

#define BLUE_LED 0X04;
#define RED_LED 0X02;

//*****************************************************************************
void
PortFunctionInit(void)
{
    //
    // Enable Peripheral Clocks 
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF2 for GPIOOutput
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    //
    //First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

    //
    //Now modify the configuration of the pins that we unlocked.
    //
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);

    //
    // Enable pin PF4 for GPIOInput
    //
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    //
    // Enable pin PF1 for GPIOOutput
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
		
		GPIO_PORTF_PUR_R |= 0x11; //ENABLE PULL UP RESISTOR FOR PF0&PF4
		
}

/*

System Requirements
The system has two input switches (SW1 and SW2) and two output LEDs (blue and 
red LEDs). Overall functionality of this system is described in the following rules.
1) If SW1 is pressed, the red LED will be toggled every half second (the blue LED will 
be off).

2) If SW1 is not pressed, the blue LED will be toggled every half second (the red LED
will be off).

3) Whenever SW2 is pressed (no matter SW1 is pressed or not), both LEDs will be off. 

SW2->PF0
SW1->PF4

REDLED->PF1
BLUELED->PF2

*/

int main(void){
	
	PortFunctionInit();//INITIALIZE
	
	uint32_t freq = SysCtlClockGet();	// GET OPERATING FREQ
	uint32_t toggleRed = RED_LED; 		//set toggle values to high
	uint32_t toggleBlue = BLUE_LED;
		
	while(1){
		
		for(int i = 0; i < 100; i++){
			
			SysCtlDelay(freq/600); 			// 100 5ms delays
			
			if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==GPIO_PIN_0){ //IF SW2 IS NOT PRESSED NEGATIVE LOGIC
				
				if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==GPIO_PIN_4)// if sw1 is not pressed negative logic
					
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1, toggleBlue);//use toggle value to turn blue led on/off
					
				else		//if sw1 is pressed
				
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2, toggleRed);//use toggle value to turn red led on/off
								
			}
			else  //if sw2 is pressed 
				
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0x00);
				
		}
		
		toggleRed^=RED_LED;
		toggleBlue^=BLUE_LED;	// toggle both values to stay consistent with time and switch			
	
	}
}