
#pragma once

#include <stdint.h>

void waitAboutOneSecond(void);

// single byte read
uint8_t flash_read_byte(uint32_t addr);

// argument order matches memcpy
void flash_read_block(uint8_t *buffer, uint32_t addr,  uint16_t len);

