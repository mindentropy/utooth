#ifndef BT_PARAMS_H_

#define BT_PARAMS_H_

#define BT_NAME "TRINITY"
#define SCAN_TYPE	INQUIRY_PAGE_SCAN_ENABLE


#define MAX_INQUIRY_RESULTS			10

//TIME N*1.28 seconds. Range 0x01 - 0x30
#define MAX_INQ_LEN 				0x5
#define NUM_OF_RESPONSES			20

//Range 0x00 to 0xB540
#define CONNECTION_ACCEPT_TIMEOUT 	0x1FA0

#define MAX_CONNECTIONS				2

#endif
