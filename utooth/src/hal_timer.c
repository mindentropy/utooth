#include "hal_timer.h"

void init_timer_A0() {

//	TA0CTL &= TAIFG; //Clear the interrupt pending flag.
	TA0CCR0 = 0x1000;
	TA0CCTL0 = CCIE;
	TA0CTL = TASSEL_1|MC_1|ID_3|TAIE|TACLR; //ACLK=32768Hz,/8 Divider. 
									//Up mode and Interrupt Enable.
}

#ifdef __GNUC__
__attribute__((interrupt(TIMER0_A1_VECTOR)))
#endif
void TIMER0_A1_ISR(void) {
	switch(TA0IV) {
		case 0x00:
			//No interrupt pending
			break;
		case 0x02:
			//CC1
			break;
		case 0x04:
			//CC2
			break;
		case 0x06:
			//CC3
			break;
		case 0x08:
			//CC4
			break;
		case 0x0A:
			//CC5
			break;
		case 0x0C:
			//CC6
			break;
		case 0x0E:
			blink_led2();
			break;
	}
}
