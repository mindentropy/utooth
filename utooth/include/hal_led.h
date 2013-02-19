#ifndef HAL_LED_H_

#define HAL_LED_H_

#define LED_PORT_DIR      P1DIR
#define LED_PORT_OUT      P1OUT
#define LED_1             BIT0
#define LED_2             BIT1


#define blink_led1() \
	LED_PORT_OUT ^= LED_1

#define led1_on() \
	LED_PORT_OUT |= LED_1

#define led1_off() \
	LED_PORT_OUT &= ~LED_1

#define blink_led2() \
	LED_PORT_OUT ^= LED_2

#define led2_on() \
	LED_PORT_OUT |= LED_2

#define led2_off() \
	LED_PORT_OUT &= ~LED_2


#endif

