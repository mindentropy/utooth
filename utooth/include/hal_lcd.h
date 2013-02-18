#ifndef LCD_H_

#define LCD_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include <hal_usb.h>

/*
	UCB2
	====

	UCB2CLK		--> P9.3
	UCB2SOMI 	--> P9.2
	UCB2SIMO 	--> P9.1
*/

#define LCD_ROW 110
#define LCD_COL	138

#define LCD_COL_BLK (LCD_COL>>3)

#define LCD_COL_MAX_ADDR 0x20
#define BITS_PER_PIXEL	2

#define LCD_BKLT_PORT_DIR	P8DIR
#define LCD_BKLT_PORT_OUT	P8OUT
#define LCD_BKLT_PORT_SEL	P8SEL

#define LCD_BKLT_PIN	BIT3

/* All UCB */
#define LCD_PORT_DIR	P9DIR
#define LCD_PORT_OUT	P9OUT
#define LCD_PORT_IN		P9IN
#define LCD_PORT_SEL	P9SEL

#define LCD_SPI_SEL		P9SEL


#define LCD_MOSI_PIN	BIT1
#define LCD_MISO_PIN	BIT2
#define LCD_CLK_PIN		BIT3 //Setting for UCB2CLK SPI.
#define LCD_CS_PIN		BIT6
#define LCD_RST_PIN		BIT7


#define LCD_SPI_CTRL0	UCB2CTL0
#define LCD_SPI_CTRL1	UCB2CTL1
#define LCD_IE_REG		UCB2IE
#define LCD_IFG_REG		UCB2IFG
#define LCD_IV_REG		UCB2IV
#define LCD_RX_BUF		UCB2RXBUF
#define LCD_TX_BUF		UCB2TXBUF

#define LCD_SPI_STAT	UCB2STAT

#define LCD_BR0			UCB2BR0
#define LCD_BR1			UCB2BR1

//Switch MOSI to I/O function.
//Switch MISO to peripheral function.
#define lcd_switch_spi2c_input()	\
	LCD_SPI_SEL |= (LCD_MISO_PIN);	\
	LCD_SPI_SEL &= ~(LCD_MOSI_PIN)

//Switch MOSI to output direction.
//Switch MISO to input direction.
#define lcd_switch_spi2c_output()	\
	LCD_SPI_SEL |= (LCD_MOSI_PIN);	\
	LCD_SPI_SEL &= ~(LCD_MISO_PIN)

#define lcd_bk_light_on() 	\
	LCD_BKLT_PORT_OUT |= LCD_BKLT_PIN

#define lcd_bk_light_off()	\
	LCD_BKLT_PORT_OUT &= ~LCD_BKLT_PIN

#define lcd_clear_reset()	\
	LCD_PORT_OUT |= LCD_RST_PIN

#define lcd_reset()	\
	LCD_PORT_OUT &= ~LCD_RST_PIN

#define lcd_spi_reset() \
	LCD_SPI_CTRL1 |= UCSWRST

#define lcd_spi_remove_reset()	\
	LCD_SPI_CTRL1 &= ~UCSWRST

#define lcd_set_spi_3pin() \
	LCD_SPI_CTRL0 |= (UCMODE_0)

#define lcd_set_spi_master()	\
	LCD_SPI_CTRL0 |= (UCMST)

#define lcd_set_spi_aclk()	\
	LCD_SPI_CTRL1 |= (UCSSEL__ACLK)

#define lcd_set_spi_smclk() \
	LCD_SPI_CTRL1 |= (UCSSEL__SMCLK)

#define lcd_set_spi_async()	\
	LCD_SPI_CTRL0 &= ~(UCSYNC)

#define lcd_set_spi_sync()	\
	LCD_SPI_CTRL0 |= (UCSYNC)

#define lcd_set_cs_high()	\
	LCD_PORT_OUT |= LCD_CS_PIN

#define lcd_set_cs_low()	\
	LCD_PORT_OUT &= ~LCD_CS_PIN

//Inactive high.
#define lcd_set_clk_pl_high()	\
	LCD_SPI_CTRL0 |= (UCCKPL)

//Inactive low.
#define lcd_set_clk_pl_low()	\
	LCD_SPI_CTRL0 &= ~(UCCKPL)

#define lcd_set_msb_first()	\
	LCD_SPI_CTRL0 |= (UCMSB)

#define lcd_set_lsb_first()	\
	LCD_SPI_CTRL0 &= ~(UCMSB)

#define lcd_set_prescaler(br0,br1)	\
	LCD_BR0 = br0;	\
	LCD_BR1	= br1

#define lcd_clr_tx_ifg() \
	LCD_IFG_REG &= ~UCTXIFG

#define lcd_set_tx_ifg()	\
	LCD_IFG_REG |= UCTXIFG

#define lcd_clr_rx_ifg()	\
	LCD_IFG_REG &= ~UCRXIFG

#define lcd_set_rx_ifg()	\
	LCD_IFG_REG |= UCRXIFG

#define lcd_spi_tx_intr_enable()	\
	LCD_IE_REG |= UCTXIE

#define lcd_spi_tx_intr_disable()	\
	LCD_IE_REG &= ~UCTXIE

#define lcd_spi_rx_intr_enable()	\
	LCD_IE_REG |= UCRXIE

#define lcd_spi_rx_intr_disable()	\
	LCD_IE_REG &= ~UCRXIE

#define is_tx_buf_empty()	\
	(LCD_IFG_REG & UCTXIFG)

#define is_rx_buff_empty()	\
	(LCD_IFG_REG & UCRXIFG)

#define is_lcd_spi_busy()	\
	(LCD_SPI_STAT & UCBUSY)

#define get_lcd_address(x,y) \
	((y<<5) + (x>>3))

enum pixel_grayscale {
	PIXEL_OFF = 0x00,
	PIXEL_LIGHT,
	PIXEL_DARK,
	PIXEL_ON
};


void lcd_bk_light_init();
void lcd_init();
void lcd_set_contrast(uint8_t contrast_level);
void lcd_set_pixel(uint8_t x,uint8_t y,uint16_t grayscale);
void lcd_clear_screen();

#endif
