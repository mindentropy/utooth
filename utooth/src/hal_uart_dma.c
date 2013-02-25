#include "hal_board.h"
#include "hal_uart_dma.h"
#include "hal_usb.h"
#include "cq.h"
#include "hci.h"
#include "hal_led.h"

#include <stdio.h>
#include <string.h>

struct cq tx_q;
struct cq rx_q;

volatile uint16_t readblksize;
extern enum event_state evt_state;

uint8_t printflag = 0;
uint8_t printflag1 = 0;

extern uint16_t tmptxcnt;
extern uint16_t tmprxcnt;

extern uint8_t dbg_print_flg;

/*
 *  Note on HAL CTS/RTS Handshaking.
 *  ===============================
 * 
 *  MSP430            BT CC256x
 *  ------			  ---------
 *  RTS(P1.4) --------> CTS
 *  CTS(P1.3) <-------- RTS
 *
 *	
 *	When the CC256x is ready to accept data the RTS of CC256x is set to low. 
 *	When the UART Rx buffer of the CC256x crosses the threshold it sets the 
 *	RTS high. This is the signal for the MSP430 to stop sending data. NOTE that
 *	the RTS pin will be pulled low by the CC256x when it boots up.
 *
 *	When the MSP430 needs to receive data it sets the RTS pin low. The CC256x
 *	will send data until the RTS pin goes high again. When it goes high CC256x 
 *	will stop sending data and will wait until the RTS pin goes low again.
 */


/* Note: gaun HAL */
void delay_one_sec() {
	uint16_t i = 0;

	for(i = 0;i<1000;i++) 
		__delay_cycles(16000);
}


void hal_uart_dma_init(void) {
	
	/* BT Tx and Rx settings */
	BT_PORT_SEL |= BT_HCI_RX_PIN | BT_HCI_TX_PIN; //Set the pin to secondary function.
	BT_PORT_DIR |= BT_HCI_TX_PIN;	// Set the transfer port to out.
	BT_PORT_DIR &= ~BT_HCI_RX_PIN;  // Set the receive port to in.

	/* BT CTS and RTS settings */

	/* Request to send BT_RTS_PIN == P1.4 */
	BT_RTS_PORT_SEL &= ~(BT_RTS_PIN); 	// Set to I/O port == 0
	BT_RTS_PORT_DIR |= BT_RTS_PIN;		// Set direction to output.
	BT_RTS_PORT_OUT |= BT_RTS_PIN; 	// Set to high. STOP.

	/* Clear to send BT_CTS_PIN == P1.3 */
	BT_CTS_PORT_SEL &= ~(BT_CTS_PIN);	// Set to I/O port == 0 
	BT_CTS_PORT_DIR &= ~(BT_CTS_PIN); 	// Set direction to input 


	/* Set shutdown settings */
	BT_SHUTDOWN_PORT_SEL &= ~(BT_SHUTDOWN_PIN); // Set to I/O port == 0
	BT_SHUTDOWN_PORT_DIR |= BT_SHUTDOWN_PIN;	// Set direction to output.
	BT_SHUTDOWN_PORT_OUT |= BT_SHUTDOWN_PIN;	// Active low.

	/* Set clock settings */
	BT_CLOCK_PORT_SEL |= BT_CLOCK_PIN; 	// Set to peripheral function i.e ACLK == 32kHz
	BT_CLOCK_PORT_DIR |= BT_CLOCK_PIN;	// Set the pin to output.

	/* Wait for BT power up */
	delay_one_sec();

	/* Set UCA2 UART Settings */
	UCA2CTL1 |= UCSWRST; // Hold UART in reset 
	UCA2CTL0 = UCMODE_0;

	UCA2CTL0 &= ~UC7BIT;
	UCA2CTL1 |= UCSSEL__SMCLK;

	hal_set_cts_interrupt(); //Set the hal cts interrupt for flow control.

	UCA2CTL1 &= ~UCSWRST; // Remove UART reset

	/* Enable Rx interrupt */
	cq_init(&tx_q,BUFFSIZE);
	cq_init(&rx_q,BUFFSIZE);
	hal_uart_dma_set_baud(115200);


	enable_uart_rx_interrupt();
}

/* 
Clock setup to 16,000,000 Hz

Baudrate: 115200
UCBRx : 16000000/115200 == 138
UCBRFx : 0
UCBRSx : 7
*/

/* 
	All values taken from the table from the user guide. Pg 760 
*/

int hal_uart_dma_set_baud(uint32_t baud) {
	
	int retval = 0;

	/* Put UART in reset */
	UCA2CTL1 |= UCSWRST;
	
	switch(baud) {
		case 115200:
			//UCA2BRW = 138;
			UCA2BRW = 8;
			UCA2MCTL = UCBRS_0|UCBRF_11|UCOS16;
			break;
		default:
			retval = -1;
			break;
	}
	

	/*Remove UART from reset */
	UCA2CTL1 &= ~UCSWRST;

	return retval;
}

/* Clear to send sent from the Peripheral to CPU */
void hal_set_cts_interrupt(void) {
	P1IFG = 0; // Clear the PxIFG(Interrupt flag) to request an interrupt
	P1IV  = 0; // Clear the interrupt vector
	P1IES &= ~BT_CTS_PIN; // Enable low to high transition.
	P1IE |= BT_CTS_PIN; // Interrupt enable for the pin.
}

void hal_uart_dma_shutdown(void) {
	UCA2IE &= ~(UCTXIE|UCRXIE);
	UCA2CTL1 = UCSWRST; //Put UART in reset.

	BT_PORT_SEL &= ~(BT_HCI_RX_PIN | BT_HCI_TX_PIN); // Clear the port and 
											 		// set to primary function.
	BT_PORT_DIR = (BT_HCI_RX_PIN | BT_HCI_TX_PIN); // Set the port's to input.
	BT_PORT_OUT &= ~(BT_HCI_RX_PIN | BT_HCI_TX_PIN); // Clear the ports.
}

//TODO: Fix bug here regarding start send overflow.
int hal_uart_dma_send_data(uint8_t *buff,uint16_t len) {
	volatile uint16_t i = 0,freesize = 0;

	__disable_interrupt();
	freesize = cq_freesize(&tx_q); //Take a snapshot of the freesize.

	//Do not directly assign the cq_freesize() to len after a check(see prev stmt) 
	//as the curfreesize might increase size which would be greater than len.
	if(freesize < len) {
		len = freesize;
	}

	for(i = 0;i<len;i++) {
		__disable_interrupt();
		cq_add(&tx_q,buff[i]);
		__enable_interrupt();
	}
	
	__enable_interrupt();
	enable_uart_tx_interrupt();	//Enable transfer interrupt.

	return len;
}


void hal_uart_enable_rcv() {
	enable_uart_rx_interrupt();
	hal_uart_enable_rx();
}

inline void set_read_blk_size(uint16_t blksize) {
	char tmpbuff[10];
	readblksize  = blksize;
	
	/*sprintf(tmpbuff,"rbs: %u\n",blksize);
	halUsbSendStr(tmpbuff);*/

	enable_uart_rx_interrupt();
	hal_uart_enable_rx();

	
}


void check() {
	char tmpbuff[20];

	if(tx_q.start < tx_q.end) {
		if((MODSIZE - (tx_q.end-tx_q.start)) != tx_q.curfreesize) {
			if(!printflag) {
				//halUsbSendChar('R');
				sprintf(tmpbuff,"M%u %u %u\n",tx_q.start,tx_q.end,tx_q.curfreesize);
				halUsbSendString(tmpbuff,strlen(tmpbuff));
	//			printflag = 1;
			}
		}
	} else if(tx_q.end < tx_q.start) {
		if(((tx_q.start - tx_q.end)-1) != tx_q.curfreesize) {
			if(!printflag) {
				//halUsbSendChar('R');
				sprintf(tmpbuff,"R%u %u %u\n",tx_q.start,tx_q.end,tx_q.curfreesize);
				halUsbSendString(tmpbuff,strlen(tmpbuff));
	//			printflag = 1;
			}
		}
	}
}


uint8_t flag = 0;

#ifdef __GNUC__
__attribute__((interrupt(USCI_A2_VECTOR)))
#endif
#ifdef __IAR_SYSTEMS_ICC_
#pragma vector=USCI_A2_VECTOR
__interrupt
#endif
void USCI_A2_ISR(void) {
	uint8_t ch;
	char tmpbuff[20];

	switch(UCA2IV) {
		case 0x00:
			break;
		case 0x02:
			led1_on();
			if(cq_is_full(&rx_q)) {
				hal_uart_disable_rx(); 
				disable_uart_rx_interrupt();
				led1_off();
				return;
			}
			
			cq_add(&rx_q,UCA2RXBUF);

			if(cq_used_size(&rx_q) >= readblksize) {
				hal_uart_disable_rx();
				disable_uart_rx_interrupt();
				hci_rx_pkt_handler();
				
				
			} 
			
			
			led1_off();
			break;
		case 0x04:
			//check();
			led2_on();
			if(cq_is_empty(&tx_q)) {
				disable_uart_tx_interrupt();
				//set_uart_tx_ifg();
				led2_off();
				return;
			}
			//halUsbSendChar('C');
			ch = cq_del(&tx_q);
			UCA2TXBUF = ch;

			if(cq_is_empty(&tx_q)) {
				disable_uart_tx_interrupt();
			}

		/*	if(dbg_print_flg)
				halUsbSendChar(ch);*/

			//check();
			
			led2_off();
			break;
		default:
			break;
	}
}


#ifdef __GNUC__
__attribute__((interrupt(PORT1_VECTOR)))
#endif
#ifdef __IAR_SYSTEMS_ICC_
#pragma vector=PORT1_VECTOR
__interrupt
#endif
void PORT1_CTS_ISR(void) 
{
	while(1) {
		blink_led2();

		__delay_cycles(50000);
		__delay_cycles(50000);
		__delay_cycles(50000);
		__delay_cycles(50000);
	}

	
	P1IES |=  BT_CTS_PIN; //1 to 0 transition.
	P1IV = 0; //Clear the interrupt. TODO:Clear CTS IFG only.
	halUsbSendChar('C');
	
	disable_uart_tx_interrupt();
	set_uart_tx_ifg();
	
	P1IFG = 0;
}

