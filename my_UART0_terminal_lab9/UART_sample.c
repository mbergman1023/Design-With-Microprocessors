#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define RED_MASK 0x02;
#define BLUE_MASK 0x04;
#define GREEN_MASK 0x08;
//
//! 'R’ Red LED turned on; the input key also
//! printed on the serial terminal
//! ‘r’ Red LED turned off; the input key also 
//! printed on the serial terminal
//! ‘B’ Blue LED turned on; the input key also 
//! printed on the serial terminal
//! ‘b’ Blue LED turned off; the input key also 
//! printed on the serial terminal
//! ‘G’ Green LED turned on; the input key also 
//! printed on the serial terminal
//! ‘g’ Green LED turned off; the input key also 
//! printed on the serial terminal
//! Any other keys No effect on LED status; error message 
//! “Invalid Input” printed on the serial 
//! terminal
//

void UART_Init(){
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));//config uart settings
	
	IntEnable(INT_UART0); //enable the UART interrupt
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
}

void PortF_Init(){
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3); //enable pin for LED PF123

}

int leds = 0x00;
char currChar;
char invalid[] = "\n\rInvalid Input\n\r";

void fitfo(){

	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		currChar = UARTCharGetNonBlocking(UART0_BASE);

		switch(currChar){
			case 'R' :	leds |= RED_MASK; UARTCharPut(UART0_BASE, currChar); break;//echo character
			case 'r' : 	leds &= ~RED_MASK; UARTCharPut(UART0_BASE, currChar); break;
			case 'B' :	leds |= BLUE_MASK; UARTCharPut(UART0_BASE, currChar); break;
			case 'b' : 	leds &= ~BLUE_MASK; UARTCharPut(UART0_BASE, currChar); break;
			case 'G' :	leds |= GREEN_MASK; UARTCharPut(UART0_BASE, currChar); break;
			case 'g' : 	leds &= ~GREEN_MASK; UARTCharPut(UART0_BASE, currChar); break;
			default :	
				for(int i = 0; i < sizeof(invalid); i++){
					UARTCharPut(UART0_BASE, invalid[i]);
				}
			break;
		}
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3, leds); //Turn on LEDs
	}
}

void UARTIntHandler(void)
{
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status

    UARTIntClear(UART0_BASE, ui32Status); //clear the interrupts
		
		fitfo();
    
}

int main(void) {
	
		SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);//50MHz

		UART_Init();
		PortF_Init();
	
    IntMasterEnable(); //enable processor interrupts
    
		char text[] = "\n\rEnter Text:\n\r";
	
		for(int i = 0; i < sizeof(text); i++){
			UARTCharPut(UART0_BASE, text[i]);
		}
    
    while(1)
		{
		}
}
