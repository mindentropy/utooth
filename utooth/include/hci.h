#ifndef HCI_H_

#define HCI_H_

#include <stdint.h>
#include <string.h>
#include "hal_uart_dma.h"
#include "list.h"

#define HCI_CMD_PAYLOAD_SIZE 255
#define HCI_CMD_HEADER_LEN	 3 // Opcode(2 bytes) + Total length(1 byte)

#define HCI_CMD_OPCODE_OFFSET	 	0
#define HCI_CMD_PAYLOAD_LEN_OFFSET	2
#define HCI_CMD_PAYLOAD_OFFSET	 	3


#define HCI_EVT_EVENT_CODE_OFFSET 	0
#define HCI_EVT_PARAM_LEN_OFFSET	1
#define HCI_EVT_HEADER_LEN			2
#define HCI_EVT_PAYLOAD_SIZE		255

#define BDADDR_SIZE 					6
#define BT_SUPPORTED_CMDS_OCTET_SIZE 	64
#define OCF_MASK (0x3FF)

#define BDNAME_MAX_LEN		248

#define LAP0 0x9E8B33
#define LAP1 0x9E8B00

#define GIAC	LAP0
#define LIAC	LAP1

#define LINK_LEVEL_ENCRYPT_ENABLE 	0x01
#define LINK_LEVEL_ENCRYPT_DISABLE 	0x00

#define IAC	GIAC

#define GET_OGF(OPCODE)	\
	((OPCODE) >> 10)

#define GET_OCF(OPCODE) \
	(OPCODE & OCF_MASK)


#define SET_OPCODE(OGF,OCF) \
	((OGF<<10)|(OCF))

#define CONNECTION_ID_SIZE		2
#define EVENT_CODE_SIZE			1
#define EVENT_PKT_PARAM_SIZE	1
#define ENCRYPT_ENABLE_SIZE 	1
#define ACL_CONN_HANDLE_SIZE 	2
#define ACL_DATA_LEN_SIZE		2

#define HCI_EVT_HEADER_SIZE ((EVENT_CODE_SIZE) + (EVENT_PKT_PARAM_SIZE))
#define HCI_ACL_HEADER_SIZE ((ACL_CONN_HANDLE_SIZE) + (ACL_DATA_LEN_SIZE))

/* Read when the LSB is sent first */
#define read8_le() \
		(cq_del(&rx_q))

#define read16_le() \
		((cq_del(&rx_q)) | (cq_del(&rx_q)<<8))

#define read24_le() \
		((cq_del(&rx_q)) | (cq_del(&rx_q)<<8) | ((cq_del(&rx_q)<<16))

#define read8_peek_le(index) \
		((cq_peek(&rx_q)),index);

#define read16_peek_le(index) \
		((cq_peek(&rx_q,index)) | (cq_peek(&rx_q,(index + 1))<<8))
		
#define read24_peek_le(index) \
		((cq_peek(&rx_q,index)) | \
		(cq_peek(&rx_q,(index + 1))<<8) | \
		(cq_peek(&rx_q,(index + 2))<<16))

#define read8_buff_le(buff,index)	\
		(buff[index])

#define read16_buff_le(buff,index) \
		((buff[index]) | ((buff[index + 1])<<8))

#define read24_buff_le(buff,index) \
		((buff[index]) | ((buff[index + 1])<<8) | ((buff[index + 2])<<16))

#define read24_peek_le_arr(arr,index) \
		(arr[0]) = cq_peek(&rx_q,index); \
		(arr[1]) = cq_peek(&rx_q,index + 1); \
		(arr[2]) = cq_peek(&rx_q,index + 2) 

#define read24_le_arr(arr) \
		(arr[0]) = cq_del(&rx_q); \
		(arr[1]) = cq_del(&rx_q); \
		(arr[2]) = cq_del(&rx_q)

#define read_bd_addr(buff) \
	buff[0] = cq_del(&rx_q); \
	buff[1] = cq_del(&rx_q); \
	buff[2] = cq_del(&rx_q); \
	buff[3] = cq_del(&rx_q); \
	buff[4] = cq_del(&rx_q); \
	buff[5] = cq_del(&rx_q)


#define bdaddr_is_equal(bdaddr1,bdaddr2) \
	( 	((bdaddr1[0]) == (bdaddr2[0])) && \
		((bdaddr1[1]) == (bdaddr2[1])) && \
		((bdaddr1[2]) == (bdaddr2[2])) && \
		((bdaddr1[3]) == (bdaddr2[3])) && \
		((bdaddr1[4]) == (bdaddr2[4])) && \
		((bdaddr1[5]) == (bdaddr2[5]))	)


#define bdaddr_cpy(srcbdaddr,destbdaddr) \
	 	((destbdaddr[0]) = (srcbdaddr[0])); \
		((destbdaddr[1]) = (srcbdaddr[1])); \
		((destbdaddr[2]) = (srcbdaddr[2])); \
		((destbdaddr[3]) = (srcbdaddr[3])); \
		((destbdaddr[4]) = (srcbdaddr[4])); \
		((destbdaddr[5]) = (srcbdaddr[5]))
	

#define write8_buff_le(buff,index,val)	\
	((buff[index]) = (val))

#define write16_buff_le(buff,index,val)	\
	((buff[index]) = ((val) & 0xFF));	\
	((buff[(index) + 1]) = ((val) >> 8))

#define write24_buff_le(buff,index,val) \
	(buff[index] = ((val) & 0xFF)); \
	((buff[(index) + 1]) = ((val >> 8) & 0xFF)); \
	((buff[(index) + 2]) = ((val >> 16) & 0xFF))

#define write32_buff_le(buff,index,val) \
	(buff[index] = ((val) & 0xFF)); \
	((buff[(index) + 1]) = ((val >> 8) & 0xFF)); \
	((buff[(index) + 2]) = ((val >> 16) & 0xFF)); \
	((buff[(index) + 3]) = ((val >> 24) & 0xFF)) 

#define set_hci_param8_le(buff,index,val)	\
	((buff[index]) = (val))

#define set_hci_param16_le(buff,index,val)	\
	((buff[index]) = ((val) & 0xFF));	\
	((buff[(index) + 1]) = ((val) >> 8))

#define set_hci_param32_le(buff,index,val) \
	(buff[index] = ((val) & 0xFF)); \
	((buff[(index) + 1]) = ((val >> 8) & 0xFF)); \
	((buff[(index) + 2]) = ((val >> 16) & 0xFF)); \
	((buff[(index) + 3]) = ((val >> 24) & 0xFF)) 


#define set_l2cap_len(buff,len) \
	set_l2cap_param16_le(buff,L2CAP_LENGTH_OFFSET,len)

#define get_payload_buff(hcicmdbuff) \
	hcicmdbuff + HCI_CMD_PAYLOAD_OFFSET

#define printopcode(opcode) \
	halUsbSendChar(opcode); halUsbSendChar(opcode>>8)

#define get_acl_conn_handle(connhandle) \
	((connhandle) & 0xFFF)

#define get_acl_pb_flag(connhandle) \
	(((connhandle) & 0x3000) >> 12)

#define get_acl_bc_flag(connhandle) \
	(((connhandle) & 0xC000) >> 14)

#define set_acl_pb_flag(connhandle,pbflag) \
	(connhandle = ((get_acl_conn_handle(connhandle)) | (pbflag<<12)))

#define set_acl_bc_flag(connhandle,bcflag) \
	(connhandle = ((get_acl_conn_handle(connhandle)) | (bcflag<<14)))


typedef uint8_t bdaddr_t[BDADDR_SIZE];

enum hci_packet_type {
	HCI_CMD_PKT = 0x01,
	HCI_ACL_DAT_PKT = 0x02,
	HCI_SYNC_DAT_PKT = 0x03,
	HCI_EVT_PKT = 0x04,
};

enum event_state {
	NONE,
	PACKET_TYPE,
	ACL_HEADER,
	ACL_PAYLOAD,
	EVENT_HEADER,
	EVENT_PAYLOAD
};

enum pb_flag {
	PB_FIRST_NON_AUTO_FLUSH_PKT = 0x00,
	PB_CONTINUING_FRAGMENT,
	PB_FIRST_AUTO_FLUSH_PKT,
	PB_RESERVED
};

enum bc_flag_host_to_controller {
	H2C_NO_BROADCAST,
	H2C_ACTIVE_SLAVE_BROADCAST,
	H2C_PARKED_SLAVE_BROADCAST,
	H2C_RESERVED
};

enum bc_flag_controller_to_host {
	C2H_POINT_TO_POINT,
	C2H_PKT_RCV_AS_SLAVE_NOT_PARK_STATE,
	C2H_PKT_RCV_AS_SLAVE_PARK_STATE,
	C2H_RESERVED
};

enum page_scan_repetition_mode {
	PSRM_R0,
	PSRM_R1,
	PSRM_R2,
};

typedef enum {
	DISCONNECTED,
	CONNECTING,
	CONNECTED
} BD_CONN_STATUS;

typedef enum {
	STAT_BUSY,
	STAT_IDLE
}SYSTEM_STATUS;


enum role_switch {
	ROLE_SW_MASTER, 
	ROLE_SW_MASTER_SLAVE
};

typedef enum scan_type_enable {
	NOSCAN,
	INQ_SCAN_EN_PAGE_SCAN_DIS,
	INQ_SCAN_DIS_PAGE_SCAN_EN,
	INQ_SCAN_EN_PAGE_SCAN_EN,
} SCAN_TYPE_ENABLE;

typedef enum inquiry_mode {
	STD_INQUIRY_RESULT_FORMAT,
	INQUIRY_RESULT_WITH_RSSI,
	INQUIRY_RESULT_WITH_RSSI_EXTENDED_INQUIRY_RESULT,
} INQUIRY_MODE;

enum role {
	ROLE_MASTER,
	ROLE_SLAVE
};

#define set_rx_state(rx_evt_state,state) \
	evt_state = state

struct buff_len_info {
	uint16_t hc_acl_data_pkt_len;
	uint8_t hc_synch_data_pkt_len;
	uint16_t hc_total_num_acl_data_pkts;
	uint16_t hc_total_num_synch_data_pkts;
};

struct local_version_info {
	uint8_t hci_version;
	uint16_t hci_rev;
	uint8_t lmp_ver;
	uint16_t manu_id;
	uint16_t lmp_subver;
};


struct remote_bd_info {
	bdaddr_t bdaddr;
	uint8_t page_scan_repetition_mode;
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t dev_class[3];
	uint16_t clk_offset;
	char remote_bd_name[BDNAME_MAX_LEN];
	int8_t rssi;
};

struct connection_info {
//	bdaddr_t remote_bdaddr;
	uint32_t dev_class;
	uint8_t link_type;
	uint16_t connection_handle; //Represents the connection between 2 bt devices.
	uint8_t encrypt_enable;
	BD_CONN_STATUS conn_status;
	uint8_t conn_err;
	uint16_t clk_offset;
	struct remote_bd_info remote_bd_info;
	struct list_head l2cap_conn_pool;
};


#define set_iac(iac,buff) \
	buff[0] = (iac & 0x0000FF); \
	buff[1] = (iac & 0x00FF00)>>8;	\
	buff[2] = (iac >>16)
	

#define hci_send_data_chk(i,buff,payload)	\
	i = 0; \
	while(i < payload) \
		i += hal_uart_dma_send_data(buff+i,payload-i)

#define hci_state_init() \
	set_read_blk_size(1); \
	set_rx_state(evt_state,PACKET_TYPE)


void hci_init();
void hci_send_packet_type(enum hci_packet_type hci_pkt_type);
void hci_send_data(uint8_t *hci_cmd_buff, uint16_t len);
void hci_load_service_patch(uint8_t *hci_cmd_buff);

void create_hci_cmd_packet(uint16_t OCF,
						uint8_t OGF,
						uint8_t payload_len,
						uint8_t *payload);

void hci_rx_pkt_handler();
void send_hci_cmd(uint8_t ogf,uint16_t ocf,uint8_t argcnt,...);
void send_hci_acl_header(uint16_t connhandle,
					uint8_t pbflag,
					uint8_t bcflag,
					uint16_t total_data_len
					);

void hci_reset();


int8_t get_index_from_connection_info(
						struct connection_info *conn_info,
						bdaddr_t bdaddr
						);
int8_t get_index_from_connection_handle(
				struct connection_info *conn_info,
				uint16_t conn_handle);

int8_t get_index_from_inquiry_list(
				struct remote_bd_info *inq_result,
				bdaddr_t bdaddr);

void send_inquiry_request();
void set_scan_type(SCAN_TYPE_ENABLE scan_type);
void set_inquiry_mode(INQUIRY_MODE inq_mode);
void get_remote_name_request(bdaddr_t bdaddr);
void create_connection(bdaddr_t bdaddr);
void process_remote_name_req();

#endif
