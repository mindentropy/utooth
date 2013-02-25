#include "hal_compat.h"
#include <msp430x54x.h>

// __delay_cycles is limited
void waitAboutOneSecond(void){
    int i;
    for (i=0;i<1000;i++) __delay_cycles(16000);
}

// access far text for MSP430X platform
#if defined(__GNUC__) && (__MSP430X__ > 0)


uint8_t flash_read_byte(uint32_t addr) {
	register uint32_t status_reg,flash;
	uint8_t flash_data;

//		flash_data -> %0
//		status_reg -> %1
//		flash -> %2
//		GIE -> %3
//		addr -> %4
	
	__asm__ __volatile__ (
			"mov r2,%[status_reg]		\n"		//Save the status register.
			"bic %3,r2					\n"		//Disable global interupt in the status register.
			"nop						\n"
			"movx.a %[addr],%[flash]	\n"		//Move the addr to flash (flash is reg. Indirect register addr mode)
			"movx.b @%2,%[flash_data]	\n"		//Move the value in the addr to flash_data.
			"mov %[status_reg],r2	   	\n"		//Restore status register. (=X any operand allowed)
			:[flash_data] "=X"(flash_data),[status_reg]"=r"(status_reg),[flash]"=r"(flash)	//Output operands.
			:"i"(GIE),[addr]"m"(addr)); //Input operands.


	return flash_data;
}


// argument order matches memcpy
void flash_read_block(uint8_t *buffer, uint32_t addr,  uint16_t len){
    while (len){
        *buffer++ = flash_read_byte(addr++);
        len--;
    }
}

#endif
