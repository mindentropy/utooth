#ifndef USB_UART_H_

#define USB_UART_H_

#include <msp430.h>
#include <stdint.h>

#define USB_PORT_DIR	P5OUT
#define USB_PORT_OUT	P5DIR
#define USB_PORT_SEL	P5SEL
#define USB_PORT_REN	P5REN
#define USB_TXD_PIN		BIT6
#define USB_RXD_PIN		BIT7


#define disable_uart_tx_intr() \
		UCA1IE &= ~UCTXIE; \
		UCA1IFG |= UCTXIFG;

#define enable_uart_tx_intr() \
		UCA1IFG |= UCTXIFG; \
		UCA1IE |= UCTXIE;
	
void usb_uart_init();
void usb_uart_send_char(char ch);
void usb_uart_send_str(const char *str);
char usb_uart_recv_char();
int8_t uart_send_str(const char *str);
int8_t uart_send_char(const char ch);


#endif
