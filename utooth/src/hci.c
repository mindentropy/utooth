#include "hal_board.h"
#include "hci.h"
#include "hci_cmds.h"
#include "bt_control_init.h"
#include "hal_uart_dma.h"
#include "hal_buttons.h"
#include "hal_usb.h"
#include "cq.h"
#include "bt_params.h"
#include "l2cap.h"

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>


/***********************************************************************************
 *  NOTES on HCI OPCODE
 *  ===================
 * 
 *  TI MSP430 is little endian. Hence 
 * 
 *  Eg:
 *  --
 *  uint16_t num = 0xff0f;  // 0xff MSB and 0x0f LSB
 *  uint8_t *ch = &num;
 *  printf("%u %u",*(ch),*(ch+1));
 *  
 *  Output:
 *  ------
 *  0x0f and 0xff.
 *  Lower byte or the LSB is stored in lower part of the memory
 *  and the upper byte or the MSB is stored in the higher part of the memory.
 * 
 *  OPCODE == OCF + OGF
 *  OCF == 10 bits.
 *  OGF == 6 bits.
 *
 *  In case of HCI OPCODE the opcode is in big endian format.
 *  Hence OCF is LSB and OGF is MSB.
 *  To make the OCF LSB in little endian format or it to opcode without shift.
 *  To make the OGF MSB in little endian format shift it by 10 bits(the width of OCF).
 *  i.e. OPCODE = (OGF << 10) | OCF;
 *  
 *  To represent in the char buffer format set LSB(OCF) in ch[0] and MSB(OGF) in ch[1].
 * 
 **************************************************************************************/

uint8_t hci_cmd_buff[HCI_CMD_HEADER_LEN + HCI_CMD_PAYLOAD_SIZE];
uint8_t hci_evt_buff[HCI_EVT_HEADER_LEN + HCI_EVT_PAYLOAD_SIZE];

extern struct cq rx_q;
extern struct cq tx_q;

enum event_state evt_state = NONE;
struct buff_len_info buff_len_info;
struct local_version_info local_version_info;
struct remote_bd_info inq_result_remote_bd_info[MAX_INQUIRY_RESULTS];

bdaddr_t local_bdaddr;
uint8_t bt_supported_cmds[BT_SUPPORTED_CMDS_OCTET_SIZE];
volatile uint8_t connected = 0;

uint16_t connection_handle = 0;

volatile uint8_t inquiry_results = 0;

struct connection_info conn_info[MAX_CONNECTIONS];

uint8_t l2cap_pkt_buff[L2CAP_BUFF_SIZE];
uint16_t l2cap_buff_index = 0;

uint16_t l2cap_len = 0;

SYSTEM_STATUS sys_stat;

int8_t get_index_from_connection_info(
				struct connection_info *conn_info,
				bdaddr_t bdaddr) {

	int8_t conn_index = 0;

	for(conn_index = 0;conn_index<MAX_CONNECTIONS;conn_index++) 
		if(bdaddr_is_equal(conn_info[conn_index].remote_bd_info.bdaddr,bdaddr)) 
			return conn_index;

	return -1;
}

int8_t get_index_from_connection_handle(
				struct connection_info *conn_info,
				uint16_t conn_handle) {

	int8_t conn_index = 0;

	for(conn_index = 0;conn_index<MAX_CONNECTIONS;conn_index++)
		if((conn_info[conn_index].connection_handle) == conn_handle)
			return conn_index;


	return -1;
	
}

int8_t get_empty_conn_info(struct connection_info *conn_info) {
	int8_t conn_index = 0;

	for(conn_index = 0;conn_index<MAX_CONNECTIONS;conn_index++)
		if((conn_info[conn_index].conn_status) == DISCONNECTED) 
			return conn_index;

	return -1;
}

int8_t get_index_from_inquiry_list(
				struct remote_bd_info *inq_result,
				bdaddr_t bdaddr) {
	int8_t index = 0;

	for(index = 0;index<MAX_INQUIRY_RESULTS;index++) 
		if(bdaddr_is_equal(inq_result_remote_bd_info[index].bdaddr,bdaddr))
			return index;
	

	return -1;
}

void hci_send_packet_type(enum hci_packet_type hci_pkt_type) {
	uint8_t ch = hci_pkt_type;
	uint16_t i = 0,payload_len = 1;

	hci_send_data_chk(i,&ch,payload_len);
}

void send_nop() {
	uint8_t hci_test_buff[] = {0x01,0x00,0x00,0x0f};

	hal_uart_dma_send_data(hci_test_buff,4);
}

void send_hci_acl_header(uint16_t connhandle,
					uint8_t pbflag, /* Packet boundary flag */
					uint8_t bcflag, /* Broadcast flag */
					uint16_t total_data_len
					) {

	uint16_t payload_len = 2,i = 0;

	hci_send_packet_type(HCI_ACL_DAT_PKT);

	set_acl_pb_flag(connhandle,pbflag);
	set_acl_bc_flag(connhandle,bcflag);

	hci_send_data_chk(i,(uint8_t *)&connhandle,payload_len);

	hci_send_data_chk(i,(uint8_t *)&total_data_len,payload_len);
}



//TODO: Set the below function to have variable args.
void send_hci_cmd(uint8_t ogf,uint16_t ocf,uint8_t argcnt,...) {
	uint8_t i = 0, payload = 0,len = 0;
	uint8_t pbuff[20],*buffarg;
	//uint8_t tmpbuff[20];

	//uint8_t tmpbuff[] = "here\n";
	va_list cmd_list;

	while(cq_is_full(&tx_q))
		;

	hci_send_packet_type(HCI_CMD_PKT);

	switch(ogf) {
		case OGF_HCI_CONTROLLER_BASEBAND:
			switch(ocf) {
				case OCF_RESET:
					hci_state_init();
					payload = HCI_CMD_HEADER_LEN;
					create_hci_cmd_packet(ocf,
								ogf,
								0,
								NULL);

					break;
				case OCF_WRITE_LOCAL_NAME:
					len = strlen(BT_NAME);
					payload = HCI_CMD_HEADER_LEN + len;
					strcpy((char *)pbuff,BT_NAME);
					create_hci_cmd_packet(ocf,
								ogf,
								len,
								pbuff);
									
					break;
				case OCF_READ_LOCAL_NAME:
					payload = HCI_CMD_HEADER_LEN;
					create_hci_cmd_packet(ocf,
								ogf,
								0,
								NULL);
					break;
				case OCF_WRITE_SCAN_ENABLE:
					payload = HCI_CMD_HEADER_LEN + 1;
					pbuff[0] = SCAN_TYPE;

					create_hci_cmd_packet(ocf,
										ogf,
										1,
										pbuff);
					break;
				case OCF_READ_SCAN_ENABLE:
					payload = HCI_CMD_HEADER_LEN;
					create_hci_cmd_packet(ocf,ogf,0,NULL);
					break;
			}
			break;
		case OGF_HCI_INFO_PARAM:
			payload = HCI_CMD_HEADER_LEN;
			switch(ocf) {
				case OCF_READ_LOCAL_VERSION_INFO:
				case OCF_READ_LOCAL_SUPPORTED_CMDS:
				case OCF_READ_LOCAL_SUPPORTED_FEATURES:
				case OCF_READ_LOCAL_EXTENDED_FEATURES:
				case OCF_READ_BUFFER_SIZE:
				case OCF_READ_BD_ADDR:
					create_hci_cmd_packet(ocf,
						ogf,
						0,
						NULL);
					break;
			}
			break;
		case OGF_HCI_LINK_CONTROL:
			switch(ocf) {
				case OCF_ACCEPT_CONN_REQ:
					payload = HCI_CMD_HEADER_LEN + BDADDR_SIZE + 1;
					va_start(cmd_list,argcnt);
					buffarg = va_arg(cmd_list,uint16_t);
					for(i = 0;i<BDADDR_SIZE;i++) {
						pbuff[i] = buffarg[i];
					}
					pbuff[6] = ROLE_SLAVE;
					create_hci_cmd_packet(ocf,
										ogf,
										BDADDR_SIZE+1,
										pbuff);
					va_end(cmd_list);
					break;
/*				case OCF_SET_CONN_ENCRYPTION:
					len = CONNECTION_ID_SIZE + ENCRYPT_ENABLE_SIZE;
					payload = HCI_CMD_HEADER_LEN + len;
					pbuff[0] = (connection_handle & 0xff);
					pbuff[1] = (connection_handle >> 8);
					pbuff[2] = LINK_LEVEL_ENCRYPT_ENABLE;

					create_hci_cmd_packet(ocf,
									ogf,
									len,
									pbuff);

					
					break;*/
			}
			break;
	}

/*	for(i = 0;i<payload;i++)
		halUsbSendChar(hci_cmd_buff[i]);*/
	/*i = 0;
	while(i != payload)
		i += hal_uart_dma_send_data(hci_cmd_buff+i,payload-i);*/
	hci_send_data_chk(i,hci_cmd_buff,payload);

}


void create_hci_cmd_packet(uint16_t OCF,
						uint8_t OGF,
						uint8_t payload_len,
						uint8_t *payload) {
	uint16_t opcode = 0;
	opcode = SET_OPCODE(OGF,OCF);
	
	hci_cmd_buff[HCI_CMD_OPCODE_OFFSET] = *((char *)&opcode);
	hci_cmd_buff[HCI_CMD_OPCODE_OFFSET+1] = *(((char *)&opcode)+1);

	hci_cmd_buff[HCI_CMD_PAYLOAD_LEN_OFFSET] = payload_len;

	if(payload_len)
		memcpy(hci_cmd_buff+HCI_CMD_PAYLOAD_OFFSET,payload,payload_len);
}


static inline void process_payload(uint16_t opcode,uint8_t payload_len) {
	char tmpbuff[20];
	//char str[10] = "Here\n";
	uint8_t i = 0;
	char ch,flag = 0;
		
	switch(GET_OGF(opcode)) {
		case OGF_HCI_CONTROLLER_BASEBAND:
			switch(GET_OCF(opcode)) {
				case OCF_RESET:
					//halUsbSendString(str,strlen(str));
					break;
				case OCF_WRITE_LOCAL_NAME:
					//halUsbSendChar('W');
					break;
				case OCF_READ_LOCAL_NAME:
					for(i = 0;i<payload_len;i++) {

						ch = read8_le();

						if(ch == 0x00) 
							flag = 1;
						
						if(!flag) {
							halUsbSendChar(ch);
						}
					}
					break;
				case OCF_WRITE_SCAN_ENABLE:
					break;
				case OCF_READ_SCAN_ENABLE:
					for(i = 0;i<payload_len;i++) {
						read8_le();
						/*halUsbSendStr("RSE\n");
						halUsbSendChar(cq_del(&rx_q));*/
					}
					break;
			}
			break;
		case OGF_HCI_INFO_PARAM:
			switch(GET_OCF(opcode)) {
				case OCF_READ_BUFFER_SIZE:
					buff_len_info.hc_acl_data_pkt_len = read16_le();
					buff_len_info.hc_synch_data_pkt_len = read8_le();
					buff_len_info.hc_total_num_acl_data_pkts = read16_le();
					buff_len_info.hc_total_num_synch_data_pkts = read16_le();
					
					i+=payload_len;
					break;
				case OCF_READ_LOCAL_VERSION_INFO:
					local_version_info.hci_version = read8_le();
					local_version_info.hci_rev = read16_le();
					local_version_info.lmp_ver = read8_le();
					local_version_info.manu_id = read16_le();
					local_version_info.lmp_subver = read16_le();
					i += payload_len;
	
					break;
				case OCF_READ_BD_ADDR:
					read_bd_addr(local_bdaddr);
					sprintf(tmpbuff,"\n%02x:%02x:%02x:%02x:%02x:%02x\n",
										local_bdaddr[5],
										local_bdaddr[4],
										local_bdaddr[3],
										local_bdaddr[2],
										local_bdaddr[1],
										local_bdaddr[0]);

					halUsbSendStr(tmpbuff);

					i += payload_len;
					break;
				case OCF_READ_LOCAL_SUPPORTED_CMDS:
					for(i = 0;i<payload_len;i++) 
						bt_supported_cmds[i] = read8_le();
					
					break;
				default:
					break;
			}
		 	break;
	}

	//Drain out the payload. Overflow tested here.
	for(;i<payload_len;i++) {
		halUsbSendChar(read8_le());
	}
} 

void process_inquiry_result_event(uint8_t is_rssi) {
	uint8_t tmp = 0,reset_flag = 0;
	uint8_t num_of_responses = tmp = read8_le();
	uint8_t i = 0;
	char tmpbuff[20];
	

	if(inquiry_results == MAX_INQUIRY_RESULTS) {
		halUsbSendStr("Max responses reached\n");
		cq_reset(&rx_q);
		return;
	}

	if((inquiry_results + num_of_responses) > MAX_INQUIRY_RESULTS) {
		num_of_responses =  MAX_INQUIRY_RESULTS;
		halUsbSendStr("Reaching max responses\n");
		reset_flag = 1;
		//TODO: Reset the queue after processing.
	} else {
		num_of_responses += inquiry_results; //Increase num_of_responses 
											//by the inquiry_results offset.
	}

	for(i = inquiry_results;i<num_of_responses;i++) {
		read_bd_addr(inq_result_remote_bd_info[i].bdaddr);

		sprintf(tmpbuff,"\n%02x:%02x:%02x:%02x:%02x:%02x\n",
					inq_result_remote_bd_info[i].bdaddr[5],
					inq_result_remote_bd_info[i].bdaddr[4],
					inq_result_remote_bd_info[i].bdaddr[3],
					inq_result_remote_bd_info[i].bdaddr[2],
					inq_result_remote_bd_info[i].bdaddr[1],
					inq_result_remote_bd_info[i].bdaddr[0]
					);

		halUsbSendStr(tmpbuff);
	}
	
	for(i = inquiry_results;i<num_of_responses;i++) {
		inq_result_remote_bd_info[i].page_scan_repetition_mode = read8_le();
		sprintf(tmpbuff,"PSRM:%x\n",inq_result_remote_bd_info[i].page_scan_repetition_mode);
		halUsbSendStr(tmpbuff);
	}
					
	for(i = inquiry_results;i<num_of_responses;i++) {
		inq_result_remote_bd_info[i].reserved1 = read8_le();
		sprintf(tmpbuff,"R1:%x\n",inq_result_remote_bd_info[i].reserved1);
		halUsbSendStr(tmpbuff);
	}

	if(!is_rssi) {
		for(i = inquiry_results;i<num_of_responses;i++) {
			inq_result_remote_bd_info[i].reserved2 = read8_le();
			sprintf(tmpbuff,"R2:%x\n",inq_result_remote_bd_info[i].reserved2);
			halUsbSendStr(tmpbuff);
		}
	}
	
	//x<<16|x<<8|x
	for(i = inquiry_results;i<num_of_responses;i++) {
		read24_le_arr(inq_result_remote_bd_info[i].dev_class);
		sprintf(tmpbuff,"%02x%02x%02x\n",inq_result_remote_bd_info[i].dev_class[2],
										inq_result_remote_bd_info[i].dev_class[1],
										inq_result_remote_bd_info[i].dev_class[0]
										);
		halUsbSendStr(tmpbuff);
	}


	for(i = inquiry_results;i<num_of_responses;i++) {
		//Parameter values are Little endian i.e. LSB is sent first.
		inq_result_remote_bd_info[i].clk_offset = read16_le();
		sprintf(tmpbuff,
			"clock offset: %x\n",inq_result_remote_bd_info[i].clk_offset);
		halUsbSendStr(tmpbuff);
	}

	if(is_rssi) {
		for(i = inquiry_results;i<num_of_responses;i++) {
			inq_result_remote_bd_info[i].rssi = read8_le();

			sprintf(tmpbuff,
				"rssi: %ddBm\n",inq_result_remote_bd_info[i].rssi);
			halUsbSendStr(tmpbuff);
		}
	}

	inquiry_results += tmp;

	if(reset_flag) {
		halUsbSendStr("Resetting queue\n");
		cq_reset(&rx_q);
	}
}


void reset_inquiry_results() {
	inquiry_results = 0;
}

void reject_connection_request(bdaddr_t bdaddr) {
	uint8_t pbuff[BDADDR_SIZE + 1],payload = 0,i = 0;

	hci_send_packet_type(HCI_CMD_PKT);
	payload = HCI_CMD_HEADER_LEN+(BDADDR_SIZE+1);
	
	bdaddr_cpy(bdaddr,pbuff);
	
	pbuff[6] = CONNECTION_LIMIT_EXCEEDED;
	create_hci_cmd_packet(OCF_REJECT_CONN_REQ,
					OGF_HCI_LINK_CONTROL,
					BDADDR_SIZE + 1,
					pbuff);


	hci_send_data_chk(i,hci_cmd_buff,payload);
	
	return;
}

void process_connection_request_event() {
	char tmpbuff[20];
	char *ptr;
	int8_t conn_index = 0;
	bdaddr_t tmpbdaddr;

	halUsbSendStr("connreqevt\n");
	conn_index = get_empty_conn_info(conn_info);

	if(conn_index != -1) {
		read_bd_addr(conn_info[conn_index].remote_bd_info.bdaddr);
		
		sprintf(tmpbuff,"%02x:%02x:%02x:%02x:%02x:%02x\n",
									conn_info[conn_index].remote_bd_info.bdaddr[5],
									conn_info[conn_index].remote_bd_info.bdaddr[4],
									conn_info[conn_index].remote_bd_info.bdaddr[3],
									conn_info[conn_index].remote_bd_info.bdaddr[2],
									conn_info[conn_index].remote_bd_info.bdaddr[1],
									conn_info[conn_index].remote_bd_info.bdaddr[0]);
		halUsbSendStr(tmpbuff);
	
		ptr = (char *)( &(conn_info[conn_index].dev_class) );
		read24_le_arr(ptr);
		sprintf(tmpbuff,"%02x%02x%02x\n",ptr[2],
										ptr[1],
										ptr[0]
										);
		halUsbSendStr(tmpbuff);

		conn_info[conn_index].link_type = read8_le();
		sprintf(tmpbuff,"link type %x\n\n",conn_info[conn_index].link_type);
		halUsbSendStr(tmpbuff);

		halUsbSendStr("AcceptConnReqSent\n");
		send_hci_cmd(OGF_HCI_LINK_CONTROL,
			OCF_ACCEPT_CONN_REQ,
			1,
			conn_info[conn_index].remote_bd_info.bdaddr);

		conn_info[conn_index].conn_status = CONNECTING;
		get_remote_name_request(conn_info[conn_index].remote_bd_info.bdaddr);

	} else {
		halUsbSendStr("Rejectingconn\n");
		read_bd_addr(tmpbdaddr);
		reject_connection_request(tmpbdaddr);
		cq_reset(&rx_q);
	}
}

void process_disconnection_complete_event() {
	char tmpbuff[20];
	uint8_t index = 0;
	uint16_t val = 0;

	sprintf(tmpbuff,"Status :%x\n",read8_le());
	halUsbSendStr(tmpbuff);

	index = get_index_from_connection_handle(conn_info,val = read16_le());

	sprintf(tmpbuff,"connection handle : %x\n",val);
	halUsbSendStr(tmpbuff);

	sprintf(tmpbuff,"reason : %x\n",read8_le());
	halUsbSendStr(tmpbuff);

	conn_info[index].conn_status = DISCONNECTED;

}

//TODO: Handle error conditions.
void process_connection_complete_event() {
	char tmpbuff[20];
	uint8_t errorcode;
	bdaddr_t remote_bdaddr;
	int8_t conn_index = 0;
	

	halUsbSendStr("conncompleteevt\n");
	errorcode = read8_le();
	sprintf(tmpbuff,"Status : %x\n",errorcode);
	halUsbSendStr(tmpbuff);

	connection_handle = read16_le();
	sprintf(tmpbuff,"Conn handle : %x\n",connection_handle);
	halUsbSendStr(tmpbuff);

	read_bd_addr(remote_bdaddr);

	conn_index = get_index_from_connection_info(conn_info,
										remote_bdaddr);

	sprintf(tmpbuff,"conn_index:%d\n",(int)conn_index);
	halUsbSendStr(tmpbuff);

	conn_info[conn_index].conn_status = CONNECTED;
	conn_info[conn_index].conn_err = errorcode;
	conn_info[conn_index].connection_handle = connection_handle;

	
/*	sprintf(tmpbuff,"\n%02x:%02x:%02x:%02x:%02x:%02x\n",
				read8_le(),
				read8_le(),
				read8_le(),
				read8_le(),
				read8_le(),
				read8_le()
				);

	halUsbSendStr(tmpbuff);*/

	conn_info[conn_index].link_type = read8_le();
	sprintf(tmpbuff,"link type %x\n",conn_info[conn_index].link_type);
	halUsbSendStr(tmpbuff);
	
	conn_info[conn_index].encrypt_enable = read8_le();
	sprintf(tmpbuff,"encryption enabled? %x\n",
				conn_info[conn_index].encrypt_enable);
	halUsbSendStr(tmpbuff);

	while(!cq_is_empty(&rx_q)) {
		halUsbSendChar(read8_le());
	}

	//Send a l2cap connection request.
	l2cap_connect_request(conn_info[conn_index].remote_bd_info.bdaddr,
						l2cap_pkt_buff,
						PSM_RFCOMM);

}

void process_num_of_completed_pkts() {
	//char /*tmpbuff[20],*/i;
	uint8_t num_handles;
	//uint16_t tmp;

	num_handles = read8_le();
/*	sprintf(tmpbuff,"numofhandles:%x\n",num_handles);
	halUsbSendStr(tmpbuff);*/
	
	cq_discard(&rx_q,num_handles<<1);
/*	for(i = 0;i<num_handles;i++) {
		tmp = read16_le();*/
		//sprintf(tmpbuff,"handle:%x\n",read16_le());
		//halUsbSendStr(tmpbuff);
//	}
	
	cq_discard(&rx_q,num_handles<<1);
/*	for(i = 0;i<num_handles;i++) {
		tmp = read16_le();*/
	/*	sprintf(tmpbuff,"completed_packets:%x\n",read16_le());
		halUsbSendStr(tmpbuff);*/
//	}
}


void process_remote_name_req() {
	bdaddr_t bdaddr;
	char tmpbuff[20];
	uint8_t  i = 0,status = 0;
	int16_t index = 0,conn_index = 0;
	char ch;

	status = read8_le();

/*	sprintf(tmpbuff,"Status: %d\n",status);
	halUsbSendStr(tmpbuff);*/


	if(status != HCI_CMD_SUCCESS) {
		//Reset queue and exit.
		cq_reset(&rx_q);
		return;
	}

	index = get_index_from_inquiry_list(
							inq_result_remote_bd_info,
							bdaddr);

	read_bd_addr(bdaddr);

	//If the connection is already present.
	conn_index = get_index_from_connection_info(conn_info,bdaddr);

	sprintf(tmpbuff,"\n%02x:%02x:%02x:%02x:%02x:%02x\n",
					bdaddr[5],
					bdaddr[4],
					bdaddr[3],
					bdaddr[2],
					bdaddr[1],
					bdaddr[0]
				);
	halUsbSendStr(tmpbuff);

	while(((ch = read8_le()) != 0x00)) {
		halUsbSendChar(ch);

		if(index != -1) //Update if index is present.
			inq_result_remote_bd_info[index].remote_bd_name[i] = ch;

		if(conn_index != -1) //Update if conn_index is present.
			conn_info[conn_index].remote_bd_info.remote_bd_name[i] = ch;

		i++;
	}

	while(!cq_is_empty(&rx_q))
		read8_le();

	halUsbSendStr("\n\n");

	return;
}

void process_role_change() {
	char tmpbuff[20];
	bdaddr_t bdaddr;

	halUsbSendStr("rolechange\n");
	sprintf(tmpbuff,"status:%u\n",read8_le());
	halUsbSendStr(tmpbuff);

	read_bd_addr(bdaddr);
	sprintf(tmpbuff,"%02x:%02x:%02x:%02x:%02x:%02x\n",
					bdaddr[5],
					bdaddr[4],
					bdaddr[3],
					bdaddr[2],
					bdaddr[1],
					bdaddr[0]
				);
	halUsbSendStr(tmpbuff);

	sprintf(tmpbuff,"new role:%u\n",read8_le());
	halUsbSendStr(tmpbuff);

	return;
}

void handle_hci_event(uint8_t evt_code,uint8_t len) {
	char tmpbuff[20];
	uint16_t opcode = 0;
	uint8_t i = 0,tmp = 0;

	switch(evt_code) {
		//TODO:Should go into the state machine.
		case COMMAND_COMPLETE_EVENT:
			tmp = read8_le(); //Read num of hci cmd packets.
			//halUsbSendChar(tmp); //Num of hci cmd packets.
			opcode = read16_le();
			//printopcode(opcode);
			tmp = read8_le(); // Return parameter
			process_payload(opcode,len-4);
			break;
		case INQUIRY_COMPLETE_EVENT:
			sprintf(tmpbuff,"ICE status:%u %u\n",read8_le(),inquiry_results);
			halUsbSendStr(tmpbuff);
			
			//Update the remote bd names after an inquiry is complete.
			for(i = 0;i<inquiry_results;i++) {
				get_remote_name_request(inq_result_remote_bd_info[i].bdaddr);
			}

			sys_stat = STAT_IDLE;
			break;
		case INQUIRY_RESULT_EVENT:
			process_inquiry_result_event(0);
			break;
		case INQUIRY_RESULT_WITH_RSSI_EVENT:
			process_inquiry_result_event(1);
			break;
		case CONNECTION_REQUEST_EVENT:
			process_connection_request_event();
			break;
		case DISCONNECTION_COMPLETE_EVENT:
			process_disconnection_complete_event();
			halUsbSendStr("Disconnection\n");
			break;
		case CONNECTION_COMPLETE_EVENT:
			process_connection_complete_event();
			sys_stat = STAT_IDLE;
			break;
		case COMMAND_STATUS_EVENT:
			//halUsbSendStr("cmdstatusevt\n");
			read8_le(); //Status
			tmp = read8_le(); //Num of hci cmd packets.
			opcode = read16_le(); //cmd opcode
			break;
		case PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT:
			halUsbSendStr("pgscanrepmode\n");
			cq_discard(&rx_q,7); //Discard 7 octets.
			/*for(i = 0;i<(BDADDR_SIZE-1);i++) {
				sprintf(tmpbuff,"%x:",read8_le());
				halUsbSendStr(tmpbuff);
			}
			sprintf(tmpbuff,"%x\n",read8_le());
			halUsbSendStr(tmpbuff);
	
			sprintf(tmpbuff,"pgscanrepmode:%x\n",read8_le());
			halUsbSendStr(tmpbuff);*/
			break;
		case MAX_SLOTS_CHANGE_EVENT:
			//Discard 3 octets.
			cq_discard(&rx_q,3);
			/*sprintf(tmpbuff,"connection handle:%u\n",read16_le());
			halUsbSendStr(tmpbuff);
			sprintf(tmpbuff,"Max slots:%x\n\n",read8_le());
			halUsbSendStr(tmpbuff);*/
			break;
		case NUM_OF_COMPLETED_PACKETS_EVENT:
			process_num_of_completed_pkts();
			while(!cq_is_empty(&rx_q)) {
				read8_le();
				//halUsbSendChar(read8_le());
			}
			break;
		case REMOTE_NAME_REQ_COMPLETE_EVENT:
			process_remote_name_req();
			sys_stat = STAT_IDLE;
			break;
		case ROLE_CHANGE_EVENT:
			process_role_change();
			break;
		default:
			halUsbSendStr("In default\n");
			halUsbSendChar(evt_code);
			while(!cq_is_empty(&rx_q)) {
				halUsbSendChar(read8_le());
			}
			break;
	}
}

void hci_rx_pkt_handler() {
	static uint8_t evt_code = 0;
	static uint16_t acl_data_len = 0,conn_handle = 0;
	static uint8_t len = 0;
//	uint16_t opcode = 0;
	uint8_t i = 0,tmp = 0;
	char tmpbuff[20];

	switch(evt_state) {
		case PACKET_TYPE:
			switch(tmp = read8_le()) {
				case HCI_ACL_DAT_PKT:
					set_rx_state(evt_state,ACL_HEADER);
					set_read_blk_size(HCI_ACL_HEADER_SIZE);
					break;
				case HCI_SYNC_DAT_PKT:
					halUsbSendStr("Sync packet\n");
					break;
				case HCI_EVT_PKT:
					/* Read the event code */
					set_rx_state(evt_state,EVENT_HEADER);
					//set_read_blk_size(EVENT_CODE_SIZE);
					set_read_blk_size(HCI_EVT_HEADER_SIZE);
					//halUsbSendChar(tmp);
					break;
				default: //Unknown packet type. Reset!!!
					halUsbSendStr("Default!!!!\n");
					sprintf(tmpbuff,"%x\n",tmp);
					halUsbSendStr(tmpbuff);
					set_rx_state(evt_state,PACKET_TYPE);
					set_read_blk_size(EVENT_CODE_SIZE);
					break;
			}
			break;
		case ACL_HEADER:
			conn_handle = read16_le();
/*			sprintf(tmpbuff,
						"handle:%x,pb:%x,bc:%x\n",
						get_acl_conn_handle(conn_handle),
						get_acl_pb_flag(conn_handle),
						get_acl_bc_flag(conn_handle));
			halUsbSendStr(tmpbuff);*/

			acl_data_len = read16_le();

/*			sprintf(tmpbuff,
						"acl_data_len:%u\n",acl_data_len);
			halUsbSendStr(tmpbuff);*/

			set_rx_state(evt_state,ACL_PAYLOAD);
			set_read_blk_size(acl_data_len);
			break;
		case ACL_PAYLOAD:
			i = 0;
//			halUsbSendStr("acl data\n");

			if(get_acl_pb_flag(conn_handle) == PB_FIRST_AUTO_FLUSH_PKT) {
				//If it is a fragment.
				if((get_l2cap_data_size_from_acl_len(acl_data_len) 
								!= (l2cap_len = read16_peek_le(L2CAP_LENGTH_OFFSET)))) {

					l2cap_buff_index = 0;
					l2cap_len += L2CAP_HEADER_SIZE; //Add the header size.
					halUsbSendStr("Pkt Fragment\n");
					//Buffer the pkt 
					for(i = 0;i<acl_data_len;i++)
						l2cap_pkt_buff[l2cap_buff_index++] = read8_le();

				} else {
					//Buffer the pkt 
					for(i = 0;i<acl_data_len;i++)
						l2cap_pkt_buff[l2cap_buff_index++] = read8_le();

					process_l2cap_pkt(conn_handle,l2cap_pkt_buff);

					//TODO:Remove below
					memset(l2cap_pkt_buff,0,sizeof(l2cap_pkt_buff));
					l2cap_buff_index = 0;
				}
			} else if(get_acl_pb_flag(conn_handle) == PB_CONTINUING_FRAGMENT) { 
				//Buffer the pkt.
				for(i = 0;i<acl_data_len;i++)
					l2cap_pkt_buff[l2cap_buff_index++] = read8_le();
				
				if(l2cap_len == l2cap_buff_index) {
					process_l2cap_pkt(conn_handle,l2cap_pkt_buff);

					//TODO:Remove below
					memset(l2cap_pkt_buff,0,sizeof(l2cap_pkt_buff));
					l2cap_len = 0;
					l2cap_buff_index = 0;
				}
			}

/*			if(get_acl_pb_flag(conn_handle) == 0x01) {
				while(!cq_is_empty(&rx_q)) {
					halUsbSendChar(read8_le());
				}
			} else {
				process_l2cap_pkt(conn_handle,acl_data_len);
			}*/
			set_rx_state(evt_state,PACKET_TYPE);
			set_read_blk_size(1);
			break;
		case EVENT_HEADER:
			evt_code = read8_le();
			len = read8_le();

			if(!len) {
				set_rx_state(evt_state,PACKET_TYPE); //Reset
				set_read_blk_size(1);
			} else {
				set_read_blk_size(len);
				set_rx_state(evt_state,EVENT_PAYLOAD);
			}
			break;
		case EVENT_PAYLOAD: //Handle the payload for the particular cmd opcode.
			handle_hci_event(evt_code,len);
			set_rx_state(evt_state,PACKET_TYPE);
			set_read_blk_size(1); //Reset to read packet type.
			break;
		case NONE:
		default:
			halUsbSendStr("Default state\n");
			break;
	}
}

void create_connection(bdaddr_t bdaddr) {
	int8_t index = 0,conn_index = 0;
	uint8_t buff[20];
	uint8_t payload = 0;
	char tmpbuff[20];

	hci_send_packet_type(HCI_CMD_PKT);
	index = get_index_from_inquiry_list(inq_result_remote_bd_info,
								bdaddr);
	
	sprintf(tmpbuff,"index: %d\n",(int)index);
	halUsbSendStr(tmpbuff);
	
	conn_index = get_empty_conn_info(conn_info);

	if(conn_index != -1) {
		bdaddr_cpy(inq_result_remote_bd_info[index].bdaddr,
					conn_info[conn_index].remote_bd_info.bdaddr);
			
		conn_info[conn_index].remote_bd_info.page_scan_repetition_mode = 
							inq_result_remote_bd_info[index].page_scan_repetition_mode;
		
		conn_info[conn_index].remote_bd_info.reserved1 = 
							inq_result_remote_bd_info[index].reserved1;

		conn_info[conn_index].remote_bd_info.reserved2 = 
							inq_result_remote_bd_info[index].reserved2;

		conn_info[conn_index].remote_bd_info.rssi = inq_result_remote_bd_info[index].rssi;

		memcpy(conn_info[conn_index].remote_bd_info.dev_class,
					inq_result_remote_bd_info[index].dev_class,
					sizeof(uint8_t) * 3);

		conn_info[conn_index].remote_bd_info.clk_offset  = inq_result_remote_bd_info[index].clk_offset;

		memcpy(conn_info[conn_index].remote_bd_info.remote_bd_name,
					inq_result_remote_bd_info[index].remote_bd_name,
					BDNAME_MAX_LEN);

	/*	bdaddr_cpy(inq_result_remote_bd_info[index].bdaddr,
					conn_info[conn_index].remote_bdaddr);*/

		conn_info[conn_index].conn_status = CONNECTING;
	}
	
//	conn_info[conn_index].clk_offset = inq_result_remote_bd_info[index].clk_offset;
	bdaddr_cpy(inq_result_remote_bd_info[index].bdaddr,buff);


	set_hci_param16_le(buff,6,0x0008);
	set_hci_param8_le(buff,8,PSRM_R1);
	set_hci_param8_le(buff,9,0);
	set_hci_param16_le(buff,10,conn_info[conn_index].remote_bd_info.clk_offset);
//	set_hci_param16_le(buff,10,inq_result_remote_bd_info[index].clk_offset);
	set_hci_param8_le(buff,12,ROLE_SW_MASTER_SLAVE);
	payload = HCI_CMD_HEADER_LEN + (6 + 2 + 1 + 1 + 2  + 1);
	
	create_hci_cmd_packet(OCF_CREATE_CONNECTION,
						OGF_HCI_LINK_CONTROL,
						(6 + 2 + 1 + 1 + 2  + 1),
						buff);

	hci_send_data_chk(index,hci_cmd_buff,payload);
	
	sys_stat = STAT_BUSY;
	return;
}

void send_inquiry_request() {
	uint8_t payload = 0, i = 0;
	uint8_t pbuff[20];

	hci_send_packet_type(HCI_CMD_PKT);
	payload = HCI_CMD_HEADER_LEN+(3+1+1);
	set_iac(IAC,pbuff);
	pbuff[3] = MAX_INQ_LEN;
	pbuff[4] = NUM_OF_RESPONSES;

	create_hci_cmd_packet(OCF_INQ,OGF_HCI_LINK_CONTROL,(3+1+1),pbuff);
	inquiry_results = 0;
	sys_stat = STAT_BUSY;

	hci_send_data_chk(i,hci_cmd_buff,payload);
}

void set_scan_type(SCAN_TYPE_ENABLE scan_type) {
	uint8_t payload = 0,i = 0;
	uint8_t pbuff[20];

	hci_send_packet_type(HCI_CMD_PKT);
	payload = HCI_CMD_HEADER_LEN + 1;
	pbuff[0] = scan_type;

	create_hci_cmd_packet(OCF_WRITE_SCAN_ENABLE,
						OGF_HCI_CONTROLLER_BASEBAND,
						1,
						pbuff);

	hci_send_data_chk(i,hci_cmd_buff,payload);

	return;
}

void set_inquiry_mode(INQUIRY_MODE inq_mode) {
	uint8_t payload = 0, i = 0;
	uint8_t pbuff[20];

	hci_send_packet_type(HCI_CMD_PKT);
	payload = HCI_CMD_HEADER_LEN + 1;
	pbuff[0] = inq_mode;


	create_hci_cmd_packet(OCF_WRITE_INQUIRY_MODE,
						OGF_HCI_CONTROLLER_BASEBAND,
						1,
						pbuff);

	hci_send_data_chk(i,hci_cmd_buff,payload);

}

void get_remote_name_request(bdaddr_t bdaddr) {
	int8_t /*conn_index = 0,*/ index = 0;
	uint8_t buff[20];
	//char tmpbuff[20];
	uint8_t payload = 0;

	hci_send_packet_type(HCI_CMD_PKT);

	index = get_index_from_inquiry_list(inq_result_remote_bd_info,
								bdaddr);
	
	if(index == -1) 
		return;
	
	bdaddr_cpy(inq_result_remote_bd_info[index].bdaddr,buff);
	set_hci_param8_le(buff,6,inq_result_remote_bd_info[index].page_scan_repetition_mode);
	set_hci_param8_le(buff,7,0x00);
	set_hci_param16_le(buff,8,inq_result_remote_bd_info[index].clk_offset);

	payload = HCI_CMD_HEADER_LEN + (6 + 1 + 1 + 2);

	create_hci_cmd_packet(OCF_REMOTE_NAME_REQ,
							OGF_HCI_LINK_CONTROL,
							(6 + 1 + 1 + 2),
							buff);

	hci_send_data_chk(index,hci_cmd_buff,payload);

	return;
}

void long_wait1() {
	uint8_t i = 0;

	for(i = 0;i<5;i++) {
		__delay_cycles(50000);
		__delay_cycles(50000);
	}
}


void hci_init() {
	uint8_t i = 0;
	memset(l2cap_pkt_buff,0,sizeof(l2cap_pkt_buff));
	for(i = 0;i<MAX_CONNECTIONS;i++) {
		conn_info[i].l2cap_conn_pool.next = NULL;
		conn_info[i].l2cap_conn_pool.prev = NULL;
	}
}

void hci_load_service_patch(uint8_t *hci_cmd_buff) {
	uint16_t payload_len = 0;
	uint16_t i = 0;
	
	while(bt_control_next_cmd(hci_cmd_buff)) {

	/*	while(1) {
			if(!(halButtonsPressed() & BUTTON_S1)) {
				long_wait();
				break;
			}
		}*/


		hci_send_packet_type(HCI_CMD_PKT);

		payload_len = hci_cmd_buff[HCI_CMD_PAYLOAD_LEN_OFFSET];
		payload_len += HCI_CMD_HEADER_LEN;

		i = 0;
		while(i < payload_len) {
			i += hal_uart_dma_send_data(hci_cmd_buff+i,payload_len-i);
		}
	//	hci_send_data_chk(i,hci_cmd_buff,payload_len);

		//blink_led1();
	}
}
