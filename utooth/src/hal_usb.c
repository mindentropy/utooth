/** 
 * @file  hal_usb.c
 * 
 * Copyright 2008 Texas Instruments, Inc.
***************************************************************************/
#include  <msp430x54x.h>
#include "hal_usb.h"
#include <string.h>

#ifdef USE_IRQ_RX
static char halUsbReceiveBuffer[255];
static unsigned char bufferSize=0;
#endif

/**********************************************************************//**
 * @brief  Initializes the serial communications peripheral and GPIO ports 
 *         to communicate with the TUSB3410.
 * 
 * @param  none
 * 
 * @return none
 **************************************************************************/
void halUsbInit(void)
{

#ifdef USE_IRQ_RX
	volatile unsigned char i;
	for (i = 0;i < 255; i++){
	    halUsbReceiveBuffer[i]='\0';
	}
	bufferSize = 0;
#endif
	  
	USB_PORT_SEL |= USB_PIN_RXD + USB_PIN_TXD;
	USB_PORT_DIR |= USB_PIN_TXD;
	USB_PORT_DIR &= ~USB_PIN_RXD;
		  
	UCA1CTL1 |= UCSWRST;                          //Reset State                      
	UCA1CTL0 = UCMODE_0;	//Set to UART mode.
		  
	UCA1CTL0 &= ~UC7BIT;                      // 8bit char
	UCA1CTL1 |= UCSSEL__SMCLK; 				//SMCLK

//TODO:Gaun commented below.
	UCA1BRW = 34;							//16Mhz/460800
//	UCA1BRW = 138;								//16Mhz/115200
//  UCA1BR0 = 109;								//8Mhz/9600
 
/*  UCA1BR0 = 16;                             // 8Mhz/57600=138
  UCA1BR1 = 1;*/
//  UCA1MCTL = 0xE;

//TODO:Gaun commented below.
//  UCA1MCTL = (2<<1);
	UCA1MCTL = UCBRS_6;
  	UCA1CTL1 &= ~UCSWRST;  
  // UCA1IE |= UCRXIE;
  // __bis_SR_register(GIE);                   // Enable Interrupts
}

/**********************************************************************//**
 * @brief  Disables the serial communications peripheral and clears the GPIO
 *         settings used to communicate with the TUSB3410.
 * 
 * @param  none
 * 
 * @return none
 **************************************************************************/
void halUsbShutDown(void)
{
  UCA1IE &= ~UCRXIE;
  UCA1CTL1 = UCSWRST;                          //Reset State                         
  USB_PORT_SEL &= ~( USB_PIN_RXD + USB_PIN_TXD );
  USB_PORT_DIR |= USB_PIN_TXD;
  USB_PORT_DIR |= USB_PIN_RXD;
  USB_PORT_OUT &= ~(USB_PIN_TXD + USB_PIN_RXD);
}

/**********************************************************************//**
 * @brief  Sends a character over UART to the TUSB3410.
 * 
 * @param  character The character to be sent. 
 * 
 * @return none
 **************************************************************************/
void halUsbSendChar(char character)
{
  while (!(UCA1IFG & UCTXIFG));  
  UCA1TXBUF = character;
}

char halUsbRecvChar(){
    while (!(UCA1IFG & UCRXIFG));  
    return UCA1RXBUF;
}

/**********************************************************************//**
 * @brief  Sends a string of characters to the TUSB3410
 * 
 * @param  string[] The array of characters to be transmit to the TUSB3410.
 * 
 * @param  length   The length of the string.
 * 
 * @return none
 **************************************************************************/
void halUsbSendString(char string[], unsigned char length)
{
  volatile unsigned char i;
  for (i=0; i < length; i++)
    halUsbSendChar(string[i]);  
}

void halUsbSendStr(char string[])
{
	volatile unsigned char i;
	unsigned char len = strlen(string);

	for(i = 0;i<len;i++) {
		halUsbSendChar(string[i]);
	}
}

#ifdef USE_IRQ_RX
/************************************************************************/
interrupt(USCI_A1_VECTOR) USCI_A1_ISR (void)
{
  halUsbReceiveBuffer[bufferSize++] = UCA1RXBUF;
   __bic_SR_register_on_exit(LPM3_bits);   
}
#endif

// provide putchar used by printf
int putchar(int c){
    halUsbSendChar(c);
    return 1;
}
