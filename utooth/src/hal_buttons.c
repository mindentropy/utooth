#include "hal_buttons.h"
#include <msp430.h>
#include <stdint.h>

void halButtonsInit(unsigned char buttonsMask) {
	BUTTON_PORT_SEL &= ~(buttonsMask); // Set to I/O function.
	BUTTON_PORT_DIR &= ~(buttonsMask);	//Set to input direction.
	BUTTON_PORT_REN |= (buttonsMask);
	BUTTON_PORT_OUT |= (buttonsMask);
}

unsigned char halButtonsPressed(void) {
	uint8_t button_press = 0;

	button_press |= BUTTON_PORT_IN;

	return button_press;
}

void halButtonsInterruptEnable(unsigned char buttonIntEnableMask) {
	BUTTON_PORT_IE |= buttonIntEnableMask;
}

void halButtonsInterruptDisable(unsigned char buttonIntEnableMask) {
	BUTTON_PORT_IE &= ~(buttonIntEnableMask);
}

void halButtonsShutDown(void) {
	
}



