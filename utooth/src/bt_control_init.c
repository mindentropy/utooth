#include "bt_control_init.h"
#include "hal_usb.h"
#include "hci.h"
#include "hal_compat.h"

#include <stdio.h>
#include <stdint.h>


extern const uint8_t cc256x_init_script[];
extern const uint32_t cc256x_init_script_size;


uint32_t init_script_offset = 0;

int bt_control_next_cmd(uint8_t *hci_cmd_buff){

	uint8_t payload_len = 0;

    if (init_script_offset >= cc256x_init_script_size) {
        return 0;
    }
    
    init_script_offset++;     //Skip over the command.

#if defined(__GNUC__) && (__MSP430X__ > 0)
    
    // workaround: use flash_read_block with 32-bit integer 
	// and assume init script starts at 0x10000
    uint32_t script_start_addr = 0x10000;
    flash_read_block(hci_cmd_buff, 
					script_start_addr + init_script_offset, 
					3);  // cmd header
    init_script_offset += 3;
    payload_len = hci_cmd_buff[HCI_CMD_PAYLOAD_LEN_OFFSET];
    flash_read_block(hci_cmd_buff+HCI_CMD_PAYLOAD_OFFSET,
					script_start_addr + init_script_offset,
					payload_len);  // cmd payload

#else

    // use memcpy with pointer
    uint8_t * init_script_ptr = (uint8_t*) &cc256x_init_script[0];
    memcpy(hci_cmd_buff[0], init_script_ptr + init_script_offset, 3);  // cmd header
    init_script_offset += 3;
    payload_len = hci_cmd_buffer[2];
    memcpy(hci_cmd_buff[3], init_script_ptr + init_script_offset, payload_len);  // cmd payload

#endif

    // support for cc256x power commands and ehcill 
   // bt_control_cc256x_update_command(hci_cmd_buff);

    init_script_offset += payload_len;

    return 1; 
}

