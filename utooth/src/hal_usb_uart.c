#include "hal_usb_uart.h"
#include "cq.h"
#include <string.h>

struct cq uartcq;

/* Setting to default clocks*/
void usb_uart_init() {
	
	cq_init(&uartcq);

	/* Port pin settings */
	USB_PORT_SEL |= (USB_TXD_PIN|USB_RXD_PIN); //Set to secondary functions.
	USB_PORT_DIR |= (USB_TXD_PIN); //TXD output.
	USB_PORT_DIR &= ~(USB_RXD_PIN); //RXD input.
	USB_PORT_OUT &= ~(USB_RXD_PIN);
	USB_PORT_REN |= (USB_RXD_PIN);

	/* UART settings */
	UCA1CTL1 |= UCSWRST; //Set UART to reset
	UCA1CTL0 = (UCMODE_0); // Set to  UART mode.
	UCA1CTL0 &= ~UC7BIT;
	UCA1CTL1 |= UCSSEL__SMCLK; //Set UART to SMCLK.
	UCA1BR0 = 9;	//1048576Hz 115200 Baud UCBRx
	UCA1BR1 = 0;

	UCA1MCTL = (1<<1); //UCBRSx
	
//	UCA1IFG |= UCTXIFG; //Set the Tx Buf as empty.
	UCA1IFG &= ~UCRXIFG;

	UCA1CTL1 &= ~UCSWRST;
	UCA1IE |= (UCRXIE);
}

int8_t uart_send_str(const char *str) {
	uint8_t i = 0;
	uint8_t len = strlen(str);

	if(len > cq_freesize(&uartcq)) {
		return -1;
	}

	cq_add(&uartcq,str[i++]);
	enable_uart_tx_intr();

	for(;i<len;i++) 
		cq_add(&uartcq,str[i]);
	

	enable_uart_tx_intr();

	return 0;
}

int8_t uart_send_char(const char ch) {
	if(cq_is_full(&uartcq))
		return -1;
	
	cq_add(&uartcq,ch);
	enable_uart_tx_intr();

	return 0;
}

char usb_uart_recv_char() {
	while(!(UCA1IFG & UCRXIFG)) {
		;
	}

	return UCA1RXBUF;
}


#ifdef __GNUC__
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
#ifdef __IAR_SYSTESM_ICC__
#pragma vector=USCI_A1_VECTOR
__interrupt
#endif 
void USCI_A1_ISR(void)
{
	//char ch;
	switch(UCA1IV) {
		case 0x00:
			break;
		case 0x02:	//UCRXIFG
			//ch = UCA1RXBUF;
			break;
		case 0x04:	//UCTXIFG
			if(cq_is_empty(&uartcq)) {
				disable_uart_tx_intr();
			} else {
				UCA1TXBUF = cq_del(&uartcq);
			}
			break;
		default:
			break;
	}
}
