#include <msp430.h>
#include <stdint.h>

#include "hal_uart_dma.h"
#include "hal_board.h"
#include "hal_buttons.h"
#include "hal_usb.h"
#include "hal_dbg.h"
#include "hal_led.h"
//#include "hal_lcd.h"
#include "hci.h"
#include "cq.h"
#include "hci_cmds.h"
//#include "hal_timer.h"
#include "l2cap.h"


extern uint8_t hci_cmd_buff[HCI_CMD_HEADER_LEN + HCI_CMD_PAYLOAD_SIZE];
extern struct cq rx_q;
extern struct cq tx_q;

extern uint16_t tmptxcnt;
extern uint16_t tmprxcnt;
extern const uint32_t cc256x_init_script_size;

extern volatile uint8_t connected;
extern uint8_t l2cap_pkt_buff[];

struct l2cap_info l2capinfo;

extern SYSTEM_STATUS sys_stat;

uint8_t dbg_print_flg = 0;

/*
TODO:
	1) Enable UART for bluetooth.
	2) Turn on Bluetooth.
	3) Turn off bluetooth.
*/

extern const uint8_t  cc256x_init_script[];
extern const uint32_t cc256x_init_script_size;


#define wait_until_idle()	\
	while(sys_stat != STAT_IDLE)	\
		;
/*
void setup_switch_s1() {
	BUTTON_PORT_SEL &= ~(BUTTON_S1); // I/O function 
	BUTTON_PORT_DIR &= ~(BUTTON_S1); // Input direction 
	BUTTON_PORT_OUT |= (BUTTON_S1); // Pin pulled up 
	BUTTON_PORT_REN |= (BUTTON_S1); // Pin pull up resistor enabled 
}

void setup_switch_s2() {
	BUTTON_PORT_SEL &= ~(BUTTON_S2); // I/O function 
	BUTTON_PORT_DIR &= ~(BUTTON_S2); // Input direction 
	BUTTON_PORT_OUT |= (BUTTON_S2); // Pin pulled up 
	BUTTON_PORT_REN |= (BUTTON_S2); // Pin pull up resistor enabled 
}
*/


void long_wait() {
	uint8_t i = 0;

	for(i = 0;i<10;i++) {
		__delay_cycles(50000);
		__delay_cycles(50000);
	}
}


static void init_bt_module() {
	uint8_t i = 0;
//	char tmpbuff[20];

	send_hci_cmd(OGF_HCI_CONTROLLER_BASEBAND,OCF_RESET,0);

	for(i = 0;i<20;i++) {
		long_wait();
		tmptxcnt = tmprxcnt = 0;
	}

/*	while((halButtonsPressed()) & BUTTON_S1) //Loop until high. Pulled up.
		;*/

	dbg_print_flg = 1;
	hci_load_service_patch(hci_cmd_buff);
	
	for(i = 0;i<20;i++) {
		long_wait();
	}

/*	sprintf(tmpbuff,"\n%u %u %u\n",tx_q.start,tx_q.end,tx_q.curfreesize);
	halUsbSendString(tmpbuff,strlen(tmpbuff)); */

/*	while((halButtonsPressed()) & BUTTON_S1) //Loop until high. Pulled up.
		;*/

	dbg_print_flg = 0;
	send_hci_cmd(OGF_HCI_INFO_PARAM,OCF_READ_BUFFER_SIZE,0);
	send_hci_cmd(OGF_HCI_INFO_PARAM,OCF_READ_LOCAL_SUPPORTED_CMDS,0);

	send_hci_cmd(OGF_HCI_INFO_PARAM,OCF_READ_LOCAL_VERSION_INFO,0);

	send_hci_cmd(OGF_HCI_CONTROLLER_BASEBAND,
						OCF_WRITE_LOCAL_NAME,0);

	send_hci_cmd(OGF_HCI_CONTROLLER_BASEBAND,
						OCF_WRITE_SCAN_ENABLE,0);

	long_wait();


	send_hci_cmd(OGF_HCI_CONTROLLER_BASEBAND,
							OCF_READ_LOCAL_NAME,0);

	
	send_hci_cmd(OGF_HCI_INFO_PARAM,OCF_READ_BD_ADDR,0);

	set_scan_type(INQ_SCAN_EN_PAGE_SCAN_EN);
	set_inquiry_mode(INQUIRY_RESULT_WITH_RSSI);
	send_hci_cmd(OGF_HCI_CONTROLLER_BASEBAND,
							OCF_WRITE_SCAN_ENABLE,0);
	
	long_wait();
	send_hci_cmd(OGF_HCI_CONTROLLER_BASEBAND,
							OCF_READ_SCAN_ENABLE,0);

/*	while((halButtonsPressed()) & BUTTON_S1) //Loop until high. Pulled up.
		;*/

}

void l2cap_pong_cb(struct l2cap_info *l2cap_info,
					uint8_t argcnt,
					...) {
	halUsbSendStr("In l2cap pong\n");
	return;
}

int main(void) {
	//char tmpbuff[20];
	uint8_t i = 0;
//	uint8_t row = 0, col = 0,contrast_level = 70;

	bdaddr_t testbdaddr;
	WDTCTL = WDTPW | WDTHOLD;
	
 	/*
		host bdaddr: B8:FF:FE:AC:C6:A2
	*/

/*	testbdaddr[0] = 0x11;
	testbdaddr[1] = 0x11;
	testbdaddr[2] = 0x11;
	testbdaddr[3] = 0x11;
	testbdaddr[4] = 0x11;
	testbdaddr[5] = 0x11;*/

	testbdaddr[0] = 0xb2;
	testbdaddr[1] = 0x29;
	testbdaddr[2] = 0x4c;
	testbdaddr[3] =	0x83;
	testbdaddr[4] =	0x15;
	testbdaddr[5] = 0x00;

	
//	unsigned char ch = 32;
	__delay_cycles(50000);

	halBoardInit();
	halBoardStartXT1();
	halBoardSetSystemClock(SYSCLK_16MHZ);

	/* Enable UART */
	halUsbInit();

	ENABLE_CLK_DBG();

	long_wait();

	LED_PORT_DIR |= (LED_1 | LED_2);

	halButtonsInit(BUTTON_S1|BUTTON_S2); 


	sys_stat = STAT_IDLE;
/*	setup_switch_s1();
	setup_switch_s2();*/
	
	_BIS_SR(GIE);


	hal_uart_dma_init();
	__delay_cycles(50000);

	//halTimerInit();
//	hal_uart_enable_rcv();

//TODO: Have a reset which sets the Rx reset to PACKET_TYPE
// and the data to 1.

	init_bt_module();
	
	hci_init();
	l2cap_init();


	/*while((halButtonsPressed()) & BUTTON_S1) //Loop until high. Pulled up.
		;*/
	

/*	sprintf(tmpbuff,"\n%u %u %u\n",tx_q.start,tx_q.end,tx_q.curfreesize);
	halUsbSendString(tmpbuff,strlen(tmpbuff));

	sprintf(tmpbuff,"\n%u %u %u\n",tmprxcnt,tmptxcnt,cc256x_init_script_size);
	halUsbSendString(tmpbuff,strlen(tmpbuff));*/

/*	while(!connected)
		;*/

	send_inquiry_request();


	for(;;) {
		blink_led1();
		
		if(!(halButtonsPressed() & BUTTON_S1)) {
			halUsbSendStr("Create connection\n");
			create_connection(testbdaddr);
			
			wait_until_idle();
			long_wait();
		}


		/*if(!(halButtonsPressed() & BUTTON_S2)) {
			halUsbSendStr("conn_req\n");
			l2cap_connect_request(testbdaddr,
							l2cap_pkt_buff,
							PSM_RFCOMM
							);*/

			/* l2cap_ping(testbdaddr,
				&l2capinfo,
				l2cap_pkt_buff,
				NULL,
				0);*/
		/*	long_wait();
		}*/


		for(i = 0;i<15;i++) {
			
			long_wait();

		}
		
	}

	return 0; //Make compiler happy :).
}
