#ifndef HAL_UART_DMA_H_

#define HAL_UART_DMA_H_

#include <stdint.h>
#include <msp430.h>
/*
	UART_RX 9.5
	UART_TX 9.4
	RTS P1.4
	CTS P1.3
	ACLK P11.0
	SHUTDOWN P8.2
*/


#define BT_PORT_OUT 			P9OUT
#define BT_PORT_SEL 			P9SEL
#define BT_PORT_DIR 			P9DIR
#define BT_PORT_REN 			P9REN
#define BT_HCI_TX_PIN			BIT4
#define BT_HCI_RX_PIN			BIT5

/*
#define BT_FLOW_CTRL_PORT_OUT 	P1OUT
#define BT_FLOW_CTRL_SEL		P1SEL
#define BT_FLOW_CTRL_REN		P1REN
#define BT_FLOW_CTRL_DIR		P1DIR
*/


#define BT_CTS_PORT_OUT			P1OUT
#define BT_CTS_PORT_DIR			P1DIR
#define BT_CTS_PORT_REN			P1REN
#define BT_CTS_PORT_SEL			P1SEL


#define BT_RTS_PORT_OUT			P1OUT
#define BT_RTS_PORT_DIR			P1DIR
#define BT_RTS_PORT_REN			P1REN
#define BT_RTS_PORT_SEL			P1SEL

#define BT_CTS_PIN				BIT3 //(P1.3) Input.
#define BT_RTS_PIN				BIT4 //(P1.4) Ouput.

#define BT_SHUTDOWN_PORT_OUT	P8OUT
#define BT_SHUTDOWN_PORT_SEL	P8SEL
#define BT_SHUTDOWN_PORT_DIR	P8DIR
#define BT_SHUTDOWN_PORT_REN	P8REN
#define BT_SHUTDOWN_PIN			BIT2

#define BT_CLOCK_PORT_OUT		P11OUT
#define BT_CLOCK_PORT_SEL		P11SEL
#define BT_CLOCK_PORT_DIR		P11DIR
#define BT_CLOCK_PORT_REN		P11REN
#define BT_CLOCK_PIN			BIT0


#define enable_uart_tx_interrupt() \
	UCA2IE |= UCTXIE

#define disable_uart_tx_interrupt() \
	UCA2IE &= ~UCTXIE


#define set_uart_tx_ifg() \
	UCA2IFG |= UCTXIFG

#define enable_uart_rx_interrupt() \
	UCA2IE |= UCRXIE

#define disable_uart_rx_interrupt() \
	UCA2IE &= ~UCRXIE

#define hal_uart_enable_rx() \
	BT_RTS_PORT_OUT &= ~BT_RTS_PIN
	//BT_FLOW_CTRL_PORT_OUT &= ~BT_RTS_PIN //Low --> Send


#define hal_uart_disable_rx() \
	BT_RTS_PORT_OUT |= BT_RTS_PIN
	//BT_FLOW_CTRL_PORT_OUT |= BT_RTS_PIN //High --> Stop.
	

void hal_uart_dma_init(void); //Complete.
int hal_uart_dma_set_baud(uint32_t baud);
void hal_set_cts_interrupt(void);
void hal_uart_dma_shutdown(void);
int hal_uart_dma_send_data(uint8_t *buff,uint16_t len);
void hal_uart_enable_rcv();
void set_read_blk_size(uint16_t blksize);

#endif
