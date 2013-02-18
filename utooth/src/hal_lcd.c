#include "hal_lcd.h"


/*
  LCD Communication
  =================

  Send an control register to give the type of instruction. The
  register holds the opcode for a particular instruction. This is
  done by specifying the index register using the IR instruction.

  Then send the instruction parameters. The instruction will read
  the control register to decode the instruction type and apply the
  parameters.

For eg:
Start oscillation.

Reg no -> R00 --> Start Oscillation. 
R/W -> 0
RS	-> 1


Device ID --> 01110 ID RS R/W
			  01110 1  1   0	--> 0x76


Index Register -> R/W -> 0 RS -> 0
01110 1 0 0

*/

/*
unsigned char lcd_init_script[] = {
    0x74, 0x00, 0x00, 0x76, 0x00, 0x01,            // R00 start oscillation
    0x74, 0x00, 0x01, 0x76, 0x00, 0x0D,            // R01 driver output control
    0x74, 0x00, 0x02, 0x76, 0x00, 0x4C,            // R02 LCD - driving waveform control
    0x74, 0x00, 0x03, 0x76, 0x12, 0x14,            // R03 Power control
    0x74, 0x00, 0x04, 0x76, 0x04, 0x66,            // R04 Contrast control
    0x74, 0x00, 0x05, 0x76, 0x00, 0x10,            // R05 Entry mode
    0x74, 0x00, 0x06, 0x76, 0x00, 0x00,            // R06 RAM data write mask
    0x74, 0x00, 0x07, 0x76, 0x00, 0x15,            // R07 Display control
    0x74, 0x00, 0x08, 0x76, 0x00, 0x03,            // R08 Cursor Control
    0x74, 0x00, 0x09, 0x76, 0x00, 0x00,            // R09 RAM data write mask
    0x74, 0x00, 0x0A, 0x76, 0x00, 0x15,            // R0A
    0x74, 0x00, 0x0B, 0x76, 0x00, 0x03,            // R0B Horizontal Cursor Position
    0x74, 0x00, 0x0C, 0x76, 0x00, 0x03,            // R0C Vertical Cursor Position
    0x74, 0x00, 0x0D, 0x76, 0x00, 0x00,            // R0D
    0x74, 0x00, 0x0E, 0x76, 0x00, 0x15,            // R0E
    0x74, 0x00, 0x0F, 0x76, 0x00, 0x03,            // R0F
    0x74, 0x00, 0x10, 0x76, 0x00, 0x15,            // R0E
    0x74, 0x00, 0x11, 0x76, 0x00, 0x03,            // R0F
};*/


uint8_t lcd_init_script[] = {
	0x74,0x00,0x00,0x76,0x00,0x01,  //R00 - Start Oscillation
	0x74,0x00,0x01,0x76,0x00,0x0D,	//R01 - Driver output control
	0x74,0x00,0x02,0x76,0x00,0x4C,	//R02 - LCD Driving Waveform control
	0x74,0x00,0x03,0x76,0x12,0x14,	//R03 - Power control
	0x74,0x00,0x04,0x76,0x04,0x66,	//R04 - Contrast control
	0x74,0x00,0x05,0x76,0x00,0x10,	//R05 - Entry mode --> Addr increased automatically.
	0x74,0x00,0x06,0x76,0x00,0x00,	//R06 - Data write mask.!!!
	0x74,0x00,0x07,0x76,0x00,0x15,	//R07 - Display control
	0x74,0x00,0x08,0x76,0x00,0x03,	//R08 - Cursor control
	0x74,0x00,0x09,0x76,0x00,0x00,	//R09 - Write data mask
	0x74,0x00,0x0A,0x76,0x00,0x15,	//R0A - NOOP
	0x74,0x00,0x0B,0x76,0x00,0x03,	//R0B - Horizontal Cursor Position
	0x74,0x00,0x0C,0x76,0x00,0x03,	//R0C - Vertical Cursor Position
};

uint8_t ram_addr_set[] 		= {0x74,0x00,0x11,0x76,0x00,0x00};

uint8_t ram_data_write[] 	= {0x74,0x00,0x12,0x76,0xFF,0xFF};

uint8_t ram_data_read[] 	= {0x74,0x00,0x12,0x77,0x00,0x00};

uint16_t lcd_table_addr = 0;


void lcd_bk_light_init() {
	LCD_BKLT_PORT_SEL &= ~LCD_BKLT_PIN;	// I/O function
	LCD_BKLT_PORT_DIR |= LCD_BKLT_PIN;	// Output direction.
}


/* The LCD clk is set to SPI CLK (UCB2CLK) */
/* The transfer modes are set to I2C SCL and SDA */
static void lcd_send_data(uint8_t *cmd) { //Send a 3 byte lcd command.
	//Set CS low
	uint8_t i = 0;
	lcd_set_cs_low();
	
	for(i = 0;i<6;i++) {
		while(!is_tx_buf_empty()) //Wait until tx buf is empty.
			;

		LCD_TX_BUF = cmd[i];

		if(i == 2) {

			while(is_lcd_spi_busy())
				;

			lcd_set_cs_high();
			lcd_set_cs_low();
		}

	}
	
	while(is_lcd_spi_busy()) //Wait until SPI is not busy.
		;

	lcd_set_cs_high();
}

static void lcd_bringup() {
	lcd_send_data(lcd_init_script); //R00 Start oscillation.
	
	__delay_cycles(250000);

	lcd_init_script[18+5] |= BIT3; //Power supply inhibited.
	lcd_init_script[18+5] &= ~BIT0; //Standby cancel.

	lcd_send_data(lcd_init_script + 18);
}

/* Contrast control is R04 register */
void lcd_set_contrast(uint8_t contrast_level) {
	/* 
	 *	According the table I am satisfying the limits 
	 * 	with 1/7 drive bias ratio and Vlref == 16.5V
	 */


	if(contrast_level < 70 ) 
		contrast_level = 70;

	lcd_init_script[24+5] =  contrast_level;

	lcd_send_data(lcd_init_script + 24);
}


void lcd_set_address(uint16_t addr) {
	//uint16_t temp = 0;

	ram_addr_set[4] = addr >> 8;
	ram_addr_set[5] = addr & 0xFF;
	
	lcd_send_data(ram_addr_set);

//	temp = addr >> 5 ; //Get the row. Divide by 32 or 20h.
	
}

void draw_block(uint16_t addr,uint16_t value) {
	lcd_set_address(addr);
	
	ram_data_write[4] = value >> 8;
	ram_data_write[5] = value & 0xFF;

	lcd_send_data(ram_data_write);
}


void lcd_clear_screen() {
	uint8_t i = 0;
	uint8_t j = 0;

	//Set the write command value to 0
	ram_data_write[4] = 0xFF;
	ram_data_write[5] = 0xFF;

	//For every row.
	for(i = 0;i<LCD_ROW;i++) {
		lcd_set_address(i*LCD_COL_MAX_ADDR);
		for(j = 0;j<LCD_COL_BLK;j++) {
			lcd_send_data(ram_data_write);
		}
	}
}

/*
 * Note: 
 * ---- 
 * Writing and reading a pixel happens in blocks of 16 bits with
 * 2 bits per pixel. Hence row is x * 32 for the row and y / 8 for the
 * particular column. To set the particular pixel go to that particular
 * 2 bits and set the value.
 * For eg:
 * ------
 * Row 1 and column is 12
 * Then row is 1 * 32 == 32 (First row) and column is 12 / 8 == 1 the second
 * pixel block.
 *
 * -|--------> x
 *  |
 *	|
 *	|
 *  v
 *  y
 *
 */
void lcd_set_pixel(uint8_t x,uint8_t y,uint16_t grayscale) {
	uint16_t address = 0;
	
	if(x < LCD_COL && y < LCD_ROW) {
		address = (y << 5); //Go to the particular row.
		address += (x >> 3); //Go to the particular column.

		draw_block(address,grayscale);
	}
}


/*
 * Procedure for read
 * ------------------
 * Set the address -> Do a dummy read. (CGRAM-> Read data latch) ->
 * Read data (Read data latch -> DB15-0).
 * Refer page 45 of the data sheet for the reading of dummy bytes.
 */
uint16_t get_pixel_block(uint16_t address) {
	uint8_t i = 0;
	uint8_t rxdata[7];
	uint16_t data = 0;
	//char tmpbuff[10];

	lcd_set_address(address); //Set the address.
	lcd_send_data(ram_data_read); //Send the read command. 
								//Note the 0x77 for RS -> 1 and R/W ->1
	
	lcd_set_cs_low();
	while(!is_tx_buf_empty()) //Wait until tx buf is empty.
		;
	
	LCD_TX_BUF = 0x77; //Send the start byte.

	//Switch the SPI2C for read operation.
	lcd_switch_spi2c_input();
	
	//Read 5 bytes of invalid data and 2 bytes of valid data.
	//Refer pg 45 of the data sheet.
	for(i = 0;i<7;i++) {
		lcd_clr_rx_ifg(); //Clear the rx interrupt flag.
		LCD_TX_BUF = 0x1; //Send a dummy byte for clock.

		while(!is_rx_buff_empty()) //Loop until rxbuff is empty.
				;

		rxdata[i] = LCD_RX_BUF;

	}

	lcd_set_cs_high();
	lcd_switch_spi2c_output();
	
	/* Reset the pins to proper input and output. */
	
	LCD_PORT_DIR |= (LCD_MOSI_PIN|LCD_CLK_PIN); //Make the MOSI pin output.
	LCD_PORT_DIR &= ~(LCD_MISO_PIN); //Make the MISO pin input.

	data = (rxdata[5] << 8);
	data |= rxdata[6] & 0xFF;

	return data;
}

int halLcdReadBlock(unsigned int Address)
{
    int i = 0, Value = 0, ReadData[7];
	//char tmpbuff[10];

    lcd_set_address(Address);
    lcd_send_data(ram_data_read);

    LCD_PORT_OUT &= ~LCD_CS_PIN;          // start transfer CS=0
    UCB2TXBUF = 0x77;                       // Transmit first character 0x77

    while (!(UCB2IFG & UCTXIFG)) ;
    while (UCB2STAT & UCBUSY) ;

    //Read 5 dummies values and 2 valid address data
    LCD_SPI_SEL &= ~LCD_MOSI_PIN;           //Change SPI2C Dir
    LCD_SPI_SEL |= LCD_MISO_PIN;

    for (i = 0; i < 7; i++)
    {
        UCB2IFG &= ~UCRXIFG;
        UCB2TXBUF = 1;                      // load dummy byte 1 for clk
        while (!(UCB2IFG & UCRXIFG)) ;
        ReadData[i] = UCB2RXBUF;

		/*sprintf(tmpbuff,"p: %u\n",ReadData[i]);
		halUsbSendString(tmpbuff,strlen(tmpbuff));*/
    }

    LCD_PORT_OUT |= LCD_CS_PIN;           // Stop Transfer CS = 1

    LCD_SPI_SEL |= LCD_MOSI_PIN;            //Change SPI2C Dir
    LCD_SPI_SEL &= ~LCD_MISO_PIN;
    LCD_PORT_DIR |= LCD_MOSI_PIN + LCD_CLK_PIN;
    LCD_PORT_DIR &= ~LCD_MISO_PIN;

    Value = (ReadData[5] << 8) + ReadData[6];
    return Value;
}


void lcd_init() {
	//uint8_t i = 0;
	
	LCD_PORT_SEL |= (LCD_MOSI_PIN | LCD_MISO_PIN | LCD_CLK_PIN); //Setting the port pin to secondary function i2c and spi.


	LCD_PORT_DIR |= (LCD_RST_PIN | LCD_CS_PIN);

	lcd_reset();
	lcd_set_cs_low();

	__delay_cycles(0x47ff);
	lcd_clear_reset();
	lcd_set_cs_high();

	lcd_spi_reset();
	
	lcd_set_spi_3pin();
	lcd_set_spi_master();
	lcd_set_spi_smclk();
	lcd_set_spi_async();
	lcd_set_msb_first();

	lcd_set_clk_pl_high();

	lcd_set_prescaler(4,0); //Set to 4Mhz. The minimum value in the data sheet.

	lcd_spi_remove_reset();
	lcd_clr_rx_ifg();
	

	lcd_bringup();

	lcd_send_data(lcd_init_script+(1*6));
	lcd_send_data(lcd_init_script+(2*6));
	lcd_send_data(lcd_init_script+(4*6));
	lcd_send_data(lcd_init_script+(5*6));
	lcd_send_data(lcd_init_script+(6*6));
	lcd_send_data(lcd_init_script+(7*6));

}
