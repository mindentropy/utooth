#include "hci.h"
#include "l2cap.h"
#include "hal_board.h"
#include "msp430.h"
#include "list.h"
#include "rfcomm.h"


extern struct cq rx_q;
extern struct cq tx_q;
extern struct l2cap_info l2capinfo;
extern struct connection_info conn_info[];

uint8_t l2cap_sig_id;
uint16_t l2cap_cur_cid;

uint8_t credit_cnt = MAX_CREDIT_CNT;

struct l2cap_conn l2cap_conn_pool[L2CAP_CONN_POOL_SIZE];
struct list_head l2cap_empty_pool;

uint8_t gen_l2cap_sig_id() {
	if(l2cap_sig_id == MIN_L2CAP_SIG_ID)
		l2cap_sig_id = 1;
	else
		l2cap_sig_id++;

	return l2cap_sig_id;
}

uint16_t gen_l2cap_channel_id() {
	if(l2cap_cur_cid == MAX_L2CAP_CID)
		l2cap_cur_cid = MIN_L2CAP_CID;
	else
		l2cap_cur_cid++;

	return l2cap_cur_cid;
}

struct l2cap_conn * get_l2cap_conn_from_channel_id(
								struct connection_info *conn_info,
								uint16_t conn_handle,
								uint16_t channel_id) 
								{
	int conn_index = 0;
	struct list_head *node;
	struct l2cap_conn *conn;

	conn_index = get_index_from_connection_handle(conn_info,conn_handle);

	if(conn_index == -1)
		return NULL;
	
	node = conn_info[conn_index].l2cap_conn_pool.next;

	while(node != NULL) {
		conn = (struct l2cap_conn *)node;

		if(conn->channel_id == channel_id)
			return conn;
		
		node = node->next;
	}

	return NULL;
}

void l2cap_init() {

	init_pool(l2cap_conn_pool,
		sizeof(struct l2cap_conn),
		L2CAP_CONN_POOL_SIZE,
		&l2cap_empty_pool);

	//l2cap_cur_cid = MIN_L2CAP_CID-1;
	l2cap_cur_cid = MIN_L2CAP_CID;
}

void get_options(uint8_t *buff,uint8_t config_size,struct l2cap_info *l2cap_info) {
	char tmpbuff[20];

	while(config_size) {
		switch(get_l2cap_param8_le(buff,CONFIG_TYPE_OFFSET)) {
			case MTU_OPTION_TYPE:
				sprintf(tmpbuff,"len:%x,mtu:%u\n",get_l2cap_param8_le(buff,CONFIG_LEN_OFFSET),
										get_l2cap_param16_le(buff,CONFIG_OPTION_DATA_OFFSET));
				halUsbSendStr(tmpbuff);
				l2cap_info->conf_opt.option_bits |= MTU_OPTION_BIT;
				l2cap_info->conf_opt.mtu = get_l2cap_param16_le(buff,CONFIG_OPTION_DATA_OFFSET);
				break;
			case FLUSH_TIMEOUT_OPTION_TYPE:
				sprintf(tmpbuff,"len:%x,fto:%u\n",get_l2cap_param8_le(buff,CONFIG_LEN_OFFSET),
										get_l2cap_param16_le(buff,CONFIG_OPTION_DATA_OFFSET));
				halUsbSendStr(tmpbuff);
				l2cap_info->conf_opt.option_bits |= FLUSH_TIMEOUT_OPTION_BIT;
				l2cap_info->conf_opt.flush_timeout = get_l2cap_param16_le(buff,CONFIG_OPTION_DATA_OFFSET);
				break;
			case QoS_OPTION_TYPE:
				l2cap_info->conf_opt.option_bits |= QOS_OPTION_BIT;
				break;
			case RETRANSMISSION_AND_FLOW_CONTROL_OPTION_TYPE:
				l2cap_info->conf_opt.option_bits |= RETRANSMISSION_FLOW_CONTROL_OPTION_BIT;
				break;
		}
		config_size -=  ((CONFIG_OPTION_HEADER_SIZE) + get_l2cap_param8_le(buff,CONFIG_LEN_OFFSET));
		buff += ((CONFIG_OPTION_HEADER_SIZE) + get_l2cap_param8_le(buff,CONFIG_LEN_OFFSET));
	}
}

/*
void process_rfcomm_data_pkt(uint16_t conn_handle,
					uint16_t channel_id,
					uint16_t l2cap_len,
					uint8_t *l2cap_pkt_buff) {
	struct l2cap_conn *l2cap_conn;

	l2cap_conn = get_l2cap_conn_from_channel_id(conn_info,
									conn_handle,
									channel_id);

	
	if(l2cap_conn->l2cap_info
									
}
*/

void process_connection_request(uint16_t conn_handle,
							uint16_t channelid,
							uint16_t dcid,
							PSM_TYPE psm
							) {
	int conn_index = 0;
	struct l2cap_conn *data;
	char tmpbuff[10];

	conn_index = get_index_from_connection_handle(conn_info,conn_handle);

	if(conn_index == -1)
		return;

	data = remove_from_pool_head(&l2cap_empty_pool);

	if(data == NULL)
		return;


	add_to_pool_head(&(conn_info[conn_index].l2cap_conn_pool),
						data);
	

	data->channel_id = channelid;

	sprintf(tmpbuff,"chid %x\n",channelid);
	halUsbSendStr(tmpbuff);

	(data->l2cap_info).l2cap_state = WAIT_CONNECT_RSP; 
	(data->l2cap_info).dcid = dcid;
	(data->l2cap_info).psm_type = psm;


	return;
}

void 
process_l2cap_pkt(uint16_t conn_handle,
				uint8_t *l2cap_pkt_buff) {

	uint8_t cmdid = 0,cmdcode = 0,*tmp = NULL;
	char tmpbuff[40];
	uint16_t channel_id;
	uint16_t l2cap_len,signal_len;
	uint16_t cmd_len = 0;
	uint16_t i = 0;
	uint16_t scid = 0,dcid = 0x0130,infotype = 0;

	//uint32_t extended_feature_mask = 0;
	struct l2cap_conn *conn;

	/*for(i = 0;i<acl_data_len;i++) {
		sprintf(tmpbuff,"%02x ",read8_le());
		halUsbSendStr(tmpbuff);
	}*/
	
	/*sprintf(tmpbuff,"connhandle:%x\n",conn_handle);
	halUsbSendStr(tmpbuff);*/

	l2cap_len = get_l2cap_len(l2cap_pkt_buff); //Read L2CAP len.
	channel_id = get_l2cap_channel_id(l2cap_pkt_buff) ; //Read L2CAP channelid.
/*	sprintf(tmpbuff,"l2cap len:%u\n",l2cap_len);
	halUsbSendStr(tmpbuff);

	sprintf(tmpbuff,"l2cap channelid:%x\n",channel_id);
	halUsbSendStr(tmpbuff);*/

	switch(channel_id) { 
		case CHID_NULL_ID:
			halUsbSendStr("Nullid\n");
			break;
		case CHID_SIGNALING_CHANNEL:
			//halUsbSendStr("Signaling channel\n");
			// Read code.
			sprintf(tmpbuff,"cmdcode:%x\n",cmdcode = get_l2cap_cmd_code(l2cap_pkt_buff));
			halUsbSendStr(tmpbuff);
			switch(cmdcode) {  //Read command code.
				case SIG_COMMAND_REJECT:
					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);
					
					switch(get_l2cap_cmd_rej_reason(l2cap_pkt_buff)) {
						case REASON_COMMAND_NOT_UNDERSTOOD:
							break;
						case REASON_SIGNAL_MTU_EXCEEDED:
							break;
						case REASON_INVALID_CID_IN_REQUEST:
							break;
					}

					break;
				case SIG_CONNECTION_REQUEST:
					halUsbSendStr("Connection Request\n");

					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);

					sprintf(tmpbuff,"slen:%u,psm:%x,scid:%x\n",
							signal_len,
							get_l2cap_cmd_conn_req_psm(l2cap_pkt_buff),
							scid = get_l2cap_cmd_conn_req_scid(l2cap_pkt_buff));

					halUsbSendStr(tmpbuff);

					halUsbSendChar('\n');
					
					dcid = gen_l2cap_channel_id();

					process_connection_request(
										get_acl_conn_handle(conn_handle),
										dcid,
										scid,
										get_l2cap_cmd_conn_req_psm(l2cap_pkt_buff)
										);

					l2cap_send_connection_response(
						conn_handle,
						get_l2cap_cmd_id(l2cap_pkt_buff),
						dcid,
						get_l2cap_cmd_conn_req_scid(l2cap_pkt_buff),
						l2cap_pkt_buff
						);
						
					
					l2cap_config_request(
							get_acl_conn_handle(conn_handle),
							scid,
							l2cap_pkt_buff
						);
		
							
					break;
				case SIG_CONNECTION_RESPONSE:
					halUsbSendStr("Connection response\n");
					
					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);					
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);

					sprintf(tmpbuff,"did:%x,sid:%x,res:%x,stat:%x\n",
							get_l2cap_cmd_conn_resp_dcid(l2cap_pkt_buff),
							get_l2cap_cmd_conn_resp_scid(l2cap_pkt_buff),
							get_l2cap_cmd_conn_resp_result(l2cap_pkt_buff),
							get_l2cap_cmd_conn_resp_status(l2cap_pkt_buff)
							);
					halUsbSendStr(tmpbuff);
					
					conn = get_l2cap_conn_from_channel_id(conn_info,
								get_acl_conn_handle(conn_handle),
								get_l2cap_cmd_conn_resp_scid(l2cap_pkt_buff)
								);
					
					if(conn != NULL) {
						if((get_l2cap_cmd_conn_resp_result(l2cap_pkt_buff)) == 0) {
								(conn->l2cap_info).l2cap_state = CONFIG;
								(conn->l2cap_info).dcid = get_l2cap_cmd_conn_resp_dcid(l2cap_pkt_buff);

								//Sending a configuration request.
								halUsbSendStr("\nSend Conf Req\n");
								l2cap_config_request(
										get_acl_conn_handle(conn_handle),
										get_l2cap_cmd_conn_resp_dcid(l2cap_pkt_buff),
										l2cap_pkt_buff
										);
						}
					}
					halUsbSendChar('\n');

					break;
				case SIG_CONFIGURATION_REQUEST:
					halUsbSendStr("Configuration req\n");
					
					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);
					
					sprintf(tmpbuff,"signal_len:%x\n",signal_len);
					halUsbSendStr(tmpbuff);

					sprintf(tmpbuff,"dcid:%x,flags:%x\n",
							dcid = get_l2cap_cmd_conf_req_dcid(l2cap_pkt_buff),
								get_l2cap_cmd_conf_resp_flags(l2cap_pkt_buff));
					halUsbSendStr(tmpbuff);

					//Get total configuration size.
					conn = get_l2cap_conn_from_channel_id(
												conn_info,
												get_acl_conn_handle(conn_handle),
												dcid
												);

					get_options(
							l2cap_pkt_buff + L2CAP_CFRAME_CMD_CONF_REQ_CONFIG_OPT_OFFSET,
							signal_len - CTRL_SIG_CMD_DCID_SIZE - CTRL_SIG_CMD_FLAGS_SIZE,
							&(conn->l2cap_info)
						);
					
					halUsbSendChar('\n');

					halUsbSendStr("SendingConfResp\n");
					/*
						TODO: 
						Accept or reject parameters based on configuration rules.
						Currently returning success for everything.
					*/
					
					cmd_len = CTRL_SIG_CMD_CONF_RESP_PAYLOAD_SIZE;
					//cmd_len = CTRL_SIG_CMD_SCID_SIZE + CTRL_SIG_CMD_FLAGS_SIZE + CTRL_SIG_CMD_RESULT_SIZE;
					set_l2cap_signal_cmd_conf_resp_result(l2cap_pkt_buff,CONFIG_SUCCESS);
					i = 0;
					tmp = l2cap_pkt_buff + L2CAP_CFRAME_CMD_CONF_RESP_CONFIG_OPT_OFFSET;
					//Check for the configuration options set. Based on this build the configuration response.
					if((conn->l2cap_info).conf_opt.option_bits & MTU_OPTION_BIT) {
						if((conn->l2cap_info).conf_opt.mtu != L2CAP_PAYLOAD_MTU) {
							set_l2cap_signal_cmd_conf_resp_result(l2cap_pkt_buff,
											FAILURE_UNACCEPTABLE_PARAMETERS);
							set_l2cap_signal_mtu_option(tmp,L2CAP_PAYLOAD_MTU); //Set the acceptable parameter.
							tmp += TOTAL_OPTION_LEN(MTU_OPTION_LEN); //Increase the offset by the total option len.
							cmd_len += TOTAL_OPTION_LEN(MTU_OPTION_LEN);
						}

						if((conn->l2cap_info).conf_opt.option_bits & FLUSH_TIMEOUT_OPTION_BIT) {
							if((conn->l2cap_info).conf_opt.flush_timeout != L2CAP_FLUSH_TIMEOUT) {
								set_l2cap_signal_cmd_conf_resp_result(l2cap_pkt_buff,
												FAILURE_UNACCEPTABLE_PARAMETERS);
								set_l2cap_signal_flush_timeout_option(tmp,L2CAP_FLUSH_TIMEOUT); //Set the acceptable parameter.
								tmp += TOTAL_OPTION_LEN(FLUSH_TIMEOUT_OPTION_LEN); //Increase the offset by the total option len.
								cmd_len += TOTAL_OPTION_LEN(FLUSH_TIMEOUT_OPTION_LEN);
							}
						}
					}

					set_l2cap_sig_cmd_header(l2cap_pkt_buff,
											channel_id,
											SIG_CONFIGURATION_RESPONSE,
											cmdid,
											cmd_len);

					//set_l2cap_signal_cmd_conf_resp_scid(l2cap_pkt_buff,dcid);

					//TODO:Send the destination channelid 
					//i.e. the endpoint receiving the response. BUG below!!
					set_l2cap_signal_cmd_conf_resp_scid(l2cap_pkt_buff,(conn->l2cap_info).dcid); 
																				
					set_l2cap_signal_cmd_conf_resp_flags(l2cap_pkt_buff,L2CAP_CONF_NOCONTINUE_FLAG);
					
					send_hci_acl_header(conn_handle,
										PB_FIRST_AUTO_FLUSH_PKT,
										H2C_NO_BROADCAST,
										(L2CAP_HEADER_SIZE + 
										CTRL_SIG_HEADER_SIZE + 
										cmd_len));

					hci_send_data_chk(i,
									l2cap_pkt_buff,
									(L2CAP_HEADER_SIZE + 
									CTRL_SIG_HEADER_SIZE + 
									cmd_len));

					halUsbSendChar('\n');


					break;
				case SIG_CONFIGURATION_RESPONSE:
					halUsbSendStr("Configuration Response\n");

					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);
					
					sprintf(tmpbuff,"signal_len:%x\n",signal_len);
					halUsbSendStr(tmpbuff);

					sprintf(tmpbuff,"scid:%x,flags:%x,res:%x\n",
								scid = get_l2cap_cmd_conf_resp_scid(l2cap_pkt_buff),
								get_l2cap_cmd_conf_resp_flags(l2cap_pkt_buff),
								get_l2cap_cmd_conf_resp_result(l2cap_pkt_buff));
					halUsbSendStr(tmpbuff);

					break;
				case SIG_DISCONNECTION_REQUEST:
					halUsbSendStr("DisconnReq\n");

					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);

					sprintf(tmpbuff,"signal_len:%x\n",signal_len);
					halUsbSendStr(tmpbuff);

					sprintf(tmpbuff,"dcid:%x,scid:%x\n",
								dcid = get_l2cap_cmd_disconn_req_dcid(l2cap_pkt_buff),
								scid = get_l2cap_cmd_disconn_req_scid(l2cap_pkt_buff));
					halUsbSendStr(tmpbuff);

					halUsbSendStr("DisconnResp\n");

					cmd_len = CTRL_SIG_CMD_DISCONN_RESP_PAYLOAD_SIZE;
					set_l2cap_sig_cmd_header(l2cap_pkt_buff,
												channel_id,
												SIG_DISCONNECTION_RESPONSE,
												cmdid,
												cmd_len);
					set_l2cap_signal_cmd_disconn_req_resp_dcid(l2cap_pkt_buff,dcid);
					set_l2cap_signal_cmd_disconn_req_resp_scid(l2cap_pkt_buff,scid);

					send_hci_acl_header(conn_handle,
										PB_FIRST_AUTO_FLUSH_PKT,
										H2C_NO_BROADCAST,
										(L2CAP_HEADER_SIZE + 
											CTRL_SIG_HEADER_SIZE + 
											cmd_len));
					hci_send_data_chk(i,
									l2cap_pkt_buff,
									(L2CAP_HEADER_SIZE + 
										CTRL_SIG_HEADER_SIZE + 
										cmd_len));

					halUsbSendChar('\n');

					break;
				case SIG_DISCONNECTION_RESPONSE:
					//dcid: Channel endpoint on the device sending the response.
					//scid: Channel endpoint on the device receiving the response.
					
					break;
				case SIG_ECHO_REQUEST:
					cmdid = get_l2cap_cmd_id(l2cap_pkt_buff);
					signal_len = get_l2cap_cmd_len(l2cap_pkt_buff);

	/*				halUsbSendStr("Echo request\n"); */
					sprintf(tmpbuff,"id:%x\n",cmdid);
					halUsbSendStr(tmpbuff);

					sprintf(tmpbuff,"signal len:%u\n", signal_len);
					halUsbSendStr(tmpbuff);
	
	
					for(i = 0;i<signal_len;i++)
						halUsbSendChar(l2cap_pkt_buff[L2CAP_CFRAME_DATA_OFFSET + i]);
					
					halUsbSendChar('\n');

					halUsbSendStr("Pong\n");
					sprintf(tmpbuff,"chid:%u,cmdid:%u\n",channel_id,cmdid);
					halUsbSendStr(tmpbuff);
					
					cmd_len = 0;
					set_l2cap_sig_cmd_header(l2cap_pkt_buff,
												channel_id,
												SIG_ECHO_RESPONSE,
												cmdid,
												cmd_len);
					
					sprintf(tmpbuff,"connhandle:%x\n",conn_handle);
					halUsbSendStr(tmpbuff);

					//L2CAP header size + Command size
					send_hci_acl_header(conn_handle,
								PB_FIRST_AUTO_FLUSH_PKT,
								H2C_NO_BROADCAST,
								(L2CAP_HEADER_SIZE+CTRL_SIG_HEADER_SIZE+cmd_len)); 

					hci_send_data_chk(i,
								l2cap_pkt_buff,
								(L2CAP_HEADER_SIZE+CTRL_SIG_HEADER_SIZE+cmd_len));
					halUsbSendChar('\n');

					break;
				case SIG_ECHO_RESPONSE:
					halUsbSendStr("Echo response\n");
					//Call the pong call back here.
//					l2capinfo.l2cap_pong_cb(&l2capinfo,0);
					break;
				case SIG_INFO_REQUEST:
					halUsbSendStr("Information Request\n");
					sprintf(tmpbuff,"infotype:%x\n",
								infotype = get_l2cap_cmd_info_req_infotype(l2cap_pkt_buff));
					halUsbSendStr(tmpbuff);


					/* Send information response */
					cmd_len = CTRL_SIG_CMD_INFO_RESP_PAYLOAD_SIZE;

					if(infotype == 2) {
						set_l2cap_signal_cmd_info_resp_result(l2cap_pkt_buff,INFO_NOT_SUPPORTED);
					} else if(infotype == 1){
						set_l2cap_signal_cmd_info_resp_result(l2cap_pkt_buff,INFO_SUCCESS);
						cmd_len+=CTRL_SIG_INFO_CONNECTIONLESS_MTU_SIZE;
						set_l2cap_signal_cmd_info_resp_data_mtu(l2cap_pkt_buff,L2CAP_PAYLOAD_MTU);
					}

					set_l2cap_sig_cmd_header(l2cap_pkt_buff,
												channel_id,
												SIG_INFO_RESPONSE,
												cmdid,
												cmd_len);

					set_l2cap_signal_cmd_info_req_resp_infotype(l2cap_pkt_buff,infotype);
					/*extended_feature_mask = (FLOW_CONTROL_MODE | RETRANSMISSION_MODE);
					set_l2cap_signal_cmd_info_resp_data(l2cap_pkt_buff,
													extended_feature_mask);*/

					send_hci_acl_header(conn_handle,
								PB_FIRST_AUTO_FLUSH_PKT,
								H2C_NO_BROADCAST,
								(L2CAP_HEADER_SIZE+CTRL_SIG_HEADER_SIZE+cmd_len));

					hci_send_data_chk(i,
								l2cap_pkt_buff,
								(L2CAP_HEADER_SIZE+CTRL_SIG_HEADER_SIZE+cmd_len));

					halUsbSendChar('\n');
					break;
				case SIG_INFO_RESPONSE:
					break;
				default:
					break;
			}
			break;
		case CHID_CONNECTIONLESS_RECEPTION_CHANNEL:
			halUsbSendStr("connectionlessrecepchannel\n");
			break;
		default:
			/* 	
				NOTE: PSM value is associated with the channel id. Hence to know the 
				protocol traverse the connection pool and get the l2cap_info psm value.
				This not true for the connectionless reception channel. Hence a PSM 
				value is provided in the packet itself
			*/
			
			conn = get_l2cap_conn_from_channel_id(
								conn_info,
								get_acl_conn_handle(conn_handle),
								channel_id
								);

			if(conn == NULL) {
				halUsbSendStr("conn NULL\n");
				return;
			}
			
/*
	 						RFCOMM PACKET FORMAT 
 
	+------------+--------------+-------------+------------+-------------+
	|   Address  |  Control     | Length Ind  |    Info    |     FCS     |
	+------------+--------------+-------------+------------+-------------+
	|  1 octet   |  1 octet     |1 or 2 octets| Unspecified| 1 octet     |
	+------------+--------------+-------------+------------+-------------+

*/

			/* Process RFCOMM packet here */
			switch((conn->l2cap_info).psm_type) { //L2CAP protcol service multiplexer.
				case PSM_RFCOMM:
				//	halUsbSendStr("RFCOMM pkt\n");

					tmp = get_l2cap_bframe_payload_buff(l2cap_pkt_buff);

					switch(get_rfcomm_control_field(tmp)) {
						case SABM:
							halUsbSendStr("SABM\n");

							/*
							 *	The SABM command shall be used to place the addressed station 
							 *	in the Asynchronous Balanced Mode (ABM) where all control fields 
							 *	shall be one octet in length. The station shall confirm acceptance 
							 *	of the SABM command by transmission of a UA response at the first 
							 *	opportunity. Upon acceptance of this command, the DLC send and 
							 *	receive state variables shall be set to zero.
							 */

							if(verify_fcs(tmp,FCS_SIZE_SABM,get_rfcomm_fcs(tmp)) == FCS_FAIL) 
								halUsbSendStr("FCS not ok\n");

							sprintf(tmpbuff,"chaddr:%x\n",get_rfcomm_server_ch_addr(tmp));
							halUsbSendStr(tmpbuff);

							//Send UA
							create_ua_pkt(get_rfcomm_server_ch_addr(tmp),tmp);
							create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

							send_hci_acl_header(get_acl_conn_handle(conn_handle),
											PB_FIRST_AUTO_FLUSH_PKT,
											H2C_NO_BROADCAST,
											get_l2cap_bframe_size(l2cap_pkt_buff));

							hci_send_data_chk(i,
								l2cap_pkt_buff,
								get_l2cap_bframe_size(l2cap_pkt_buff));

							break;
						case UA:
							if(verify_fcs(tmp,FCS_SIZE_UA,get_rfcomm_fcs(tmp)) == FCS_FAIL) 
								halUsbSendStr("FCS not ok\n");

							halUsbSendStr("UA\n");
							break;
						case DM:
					 		/*
					 		 *	The DM response shall be used to report a status where the 
							 *	station is logically disconnected from the data link. 
							 *	When in disconnected mode no commands are accepted until the disconnected 
					 		 *	mode is terminated by the receipt of a SABM command. If a DISC command 
							 *	is received while in disconnected mode a DM response should be sent.
					 		 */

							if(verify_fcs(tmp,FCS_SIZE_DM,get_rfcomm_fcs(tmp)) == FCS_FAIL) 
								halUsbSendStr("FCS not ok\n");

							halUsbSendStr("DM\n");
							break;
						case DISC:
							
							/*
							 *	The DISC command shall be used to terminate an 
							 *	operational or initialization mode previously set 
							 *	by a command. It shall be used to inform one station 
							 *	that the other station is suspending operation and 
							 *	that the station should assume a logically disconnected 
							 *	mode. Prior to actioning the command, the receiving station 
							 *	shall confirm the acceptance of the DISC command by the 
							 *	transmission of a UA response. DISC command sent at DLCI 0 
							 *	have the same meaning as the Multiplexer Close Down command 
							 *	(see subclause 5.4.6.3.3). See also subclause 5.8.2 for more 
							 *	information about the Close-down procedure.
							 */


							if(verify_fcs(tmp,FCS_SIZE_DISC,get_rfcomm_fcs(tmp)) == FCS_FAIL)
								halUsbSendStr("FCS not ok\n");

							halUsbSendStr("DISC\n");

							//Send a UA pkt.

							create_ua_pkt(get_rfcomm_server_ch_addr(tmp),tmp);
							create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

							send_hci_acl_header(get_acl_conn_handle(conn_handle),
											PB_FIRST_AUTO_FLUSH_PKT,
											H2C_NO_BROADCAST,
											get_l2cap_bframe_size(l2cap_pkt_buff));

							hci_send_data_chk(i,
								l2cap_pkt_buff,
								get_l2cap_bframe_size(l2cap_pkt_buff));
							

							break;
						case UIH:
						//	halUsbSendStr("UIH\n");

							/* For UIH calculate FCS on address and control fields only */
							if(verify_fcs(tmp,FCS_SIZE_UIH,get_rfcomm_fcs(tmp)) == FCS_FAIL) 
								halUsbSendStr("FCS not ok\n");
							

							//Check if the dlci is 0.
							if((get_rfcomm_server_ch_addr(tmp)) != MSG_DLCI) {
							//	halUsbSendStr("DATA\n");

								if(get_control_field_poll_final_bit(tmp)) {
									sprintf(tmpbuff,"credits:%u\n",get_rfcomm_credits(tmp));
									halUsbSendStr(tmpbuff);
								} else {
									
									/* Dump the data */
									/*dump_pkt(tmp,
										get_rfcomm_payload_offset(tmp),
										get_rfcomm_payload_len(tmp));*/

									if(credit_cnt == MAX_CREDIT_CNT) {
										create_credit_pkt(tmp,
											get_rfcomm_server_ch_addr(tmp),
											0,
											MSG_CMD,
											MAX_CREDIT_CNT);
	
										create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

	
										send_hci_acl_header(get_acl_conn_handle(conn_handle),
														PB_FIRST_AUTO_FLUSH_PKT,
														H2C_NO_BROADCAST,
														get_l2cap_bframe_size(l2cap_pkt_buff));
	
										hci_send_data_chk(i,
											l2cap_pkt_buff,
											get_l2cap_bframe_size(l2cap_pkt_buff)); 
	
										credit_cnt = 0;
									}
									credit_cnt++;
								}
							} else if((get_rfcomm_server_ch_addr(tmp)) == MSG_DLCI) {
								switch(get_rfcomm_msg_type(tmp)) {
									case PN:
										halUsbSendStr("PN\n");

										for(i = 0;i<8;i++) {
											(conn->l2cap_info).rfcomm_conf_opt.config[i] = 
													get_rfcomm_msg_payload_uih_param(tmp,i);
										}
										
										if(is_valid_pn((conn->l2cap_info).rfcomm_conf_opt.config)) {
											halUsbSendStr("valid pn\n");
										}
										
										/* Respond with a PN response value of 14 (0xE) */
										set_rfcomm_msg_credit_conf_pkt(tmp,0xE);
										
										create_uih_pkt(tmp,
											get_rfcomm_server_ch_addr(tmp),
											get_rfcomm_payload_len(tmp),
											0,
											MSG_RESP);

										create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

										send_hci_acl_header(get_acl_conn_handle(conn_handle),
														PB_FIRST_AUTO_FLUSH_PKT,
														H2C_NO_BROADCAST,
														get_l2cap_bframe_size(l2cap_pkt_buff));

										hci_send_data_chk(i,
											l2cap_pkt_buff,
											get_l2cap_bframe_size(l2cap_pkt_buff));
										
										break;
									case NSC:
										halUsbSendStr("NSC\n");
										break;
									case FCon:
										halUsbSendStr("FCon\n");
										break;
									case FCoff:
										halUsbSendStr("FCoff\n");
										break;
									case MSC:
										halUsbSendStr("MSC\n");

										if(get_rfcomm_msg_type_cr(tmp) == MSG_RESP) {
											halUsbSendStr("Resp msg\n");
											return;
										}

										if(get_rfcomm_msg_type_cr(tmp) == MSG_CMD){
											halUsbSendStr("CMD msg\n");
										}

										
										create_uih_pkt(tmp,
											get_rfcomm_server_ch_addr(tmp),
											get_rfcomm_payload_len(tmp),
											0,
											MSG_RESP);
													
													
										create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

										send_hci_acl_header(get_acl_conn_handle(conn_handle),
														PB_FIRST_AUTO_FLUSH_PKT,
														H2C_NO_BROADCAST,
														get_l2cap_bframe_size(l2cap_pkt_buff));

										hci_send_data_chk(i,
											l2cap_pkt_buff,
											get_l2cap_bframe_size(l2cap_pkt_buff));
										
										//TODO:Create an MSC pkt with the encapsulating UIH pkt.
										create_msc_pkt(tmp,
											get_rfcomm_server_ch_addr(tmp),
											get_rfcomm_payload_len(tmp),
											0,
											MSG_CMD,
											(MSC_CTRL_SIG_EA_MASK
											| MSC_CTRL_SIG_RTC_MASK
											| MSC_CTRL_SIG_RTR_MASK
											| MSC_CTRL_SIG_DV_MASK));

										/*create_uih_pkt(tmp,
											get_rfcomm_server_ch_addr(tmp),
											get_rfcomm_payload_len(tmp),
											0,
											MSG_CMD);*/
													
										sprintf(tmpbuff,"signal: %x\n",
											get_rfcomm_msg_msc_ctrl_sig_conf_pkt(tmp));

										halUsbSendStr(tmpbuff);
													
										create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

										send_hci_acl_header(get_acl_conn_handle(conn_handle),
														PB_FIRST_AUTO_FLUSH_PKT,
														H2C_NO_BROADCAST,
														get_l2cap_bframe_size(l2cap_pkt_buff));

										hci_send_data_chk(i,
											l2cap_pkt_buff,
											get_l2cap_bframe_size(l2cap_pkt_buff));
										

										break;
									case RPN:
										halUsbSendStr("RPN\n");
										
										
										set_rfcomm_msg_pm_lsb_conf_pkt(tmp,
											PM_BR_MASK|PM_DB_MASK|PM_SB_MASK|PM_P_MASK|PM_PT_MASK|
												PM_XON_MASK|PM_XOFF_MASK);

										set_rfcomm_msg_pm_msb_conf_pkt(tmp,
											PM_XI_MASK|PM_XO_MASK|PM_RTRI_MASK|PM_RTRO_MASK|
											PM_RTCI_MASK|PM_RTCO_MASK);

										create_uih_pkt(tmp,
											get_rfcomm_server_ch_addr(tmp),
											get_rfcomm_payload_len(tmp),
											0,
											MSG_RESP);
													
													
										create_l2cap_bframe_rfcomm_pkt(l2cap_pkt_buff,conn);

										send_hci_acl_header(get_acl_conn_handle(conn_handle),
														PB_FIRST_AUTO_FLUSH_PKT,
														H2C_NO_BROADCAST,
														get_l2cap_bframe_size(l2cap_pkt_buff));

										hci_send_data_chk(i,
											l2cap_pkt_buff,
											get_l2cap_bframe_size(l2cap_pkt_buff));
												
										break;
									case TEST:
										halUsbSendStr("TEST\n");
										sprintf(tmpbuff,"p/f:%x\n",get_control_field_poll_final_bit(tmp));
										halUsbSendStr(tmpbuff);
										//dump_rfcomm_pkt(tmp);
										break;
									case RLS:
										halUsbSendStr("RLS\n");
										
										if(get_rfcomm_msg_rls_oe_status(tmp)) 
											halUsbSendStr("oe\n");
										
										
										if(get_rfcomm_msg_rls_pe_status(tmp)) 
											halUsbSendStr("pe\n");
										

										if(get_rfcomm_msg_rls_pe_status(tmp)) 
											halUsbSendStr("fe\n");


										break;
									default:
	
										break;
								}
							}
							break;
					}
					break;
				default:
					break;
			}

			break;
	}
}

void send_conn_oriented_l2cap_pkt(
				uint8_t *l2cap_pkt_buff,
				uint16_t length,
				uint16_t channelid) {
	
}


void create_l2cap_bframe_rfcomm_pkt(
				uint8_t *l2cap_pkt_buff,
				struct l2cap_conn *conn) {
	
	set_l2cap_channel_id(l2cap_pkt_buff,(conn->l2cap_info).dcid);
	set_l2cap_len(l2cap_pkt_buff,
		get_rfcomm_pkt_size(get_l2cap_bframe_payload_buff(l2cap_pkt_buff)));
}


void create_l2cap_ua_rfcomm_pkt(
					uint8_t *l2cap_pkt_buff,
					struct l2cap_conn *conn
					) {

}

void l2cap_ping(bdaddr_t bdaddr,
				struct l2cap_info *l2cap_info,
				uint8_t *l2cap_pkt_buff,
				void (*l2cap_pong_cb)(struct l2cap_info *l2cap_info,uint8_t argcnt,...),
				uint8_t argcnt,
				...) {

	int index = 0;
	//uint16_t connhandle = 0;
	uint16_t payload = 44;
	//char tmpbuff[20];

	index = get_index_from_connection_info(conn_info,bdaddr);

	if(index == -1) 
		return;

	set_l2cap_len(l2cap_pkt_buff,CTRL_SIG_HEADER_SIZE + payload); // MTU == 48
	set_l2cap_channel_id(l2cap_pkt_buff,CHID_SIGNALING_CHANNEL);

	set_l2cap_signal_cmd_code(l2cap_pkt_buff,SIG_ECHO_REQUEST);
	set_l2cap_signal_cmd_id(l2cap_pkt_buff,gen_l2cap_sig_id());
	set_l2cap_signal_cmd_len(l2cap_pkt_buff,payload);	// Payload == 44.


	send_hci_acl_header(conn_info[index].connection_handle,
						PB_FIRST_AUTO_FLUSH_PKT,
						H2C_NO_BROADCAST,
						(L2CAP_HEADER_SIZE+CTRL_SIG_HEADER_SIZE+payload));
	

	memset(l2cap_pkt_buff+L2CAP_HEADER_SIZE+CTRL_SIG_HEADER_SIZE,'T',44);
	hci_send_data_chk(index,
					l2cap_pkt_buff,
					(L2CAP_HEADER_SIZE + CTRL_SIG_HEADER_SIZE + payload));

	return;
}

/*
	Connection request:
	-------------------
	When a connection is requested the channel id is generated and set in the 
	structure. This should become the source cid(scid). The connection is 
	requested for a particular destination bdaddr. Hence the l2cap_conn is taken
	from a pool. 
	
	 +-------------+                  +-------------+
	 |        scid1|----------------->|dcid1        |
	 |             |                  |             |
	 |             |                  |             |
	 |        scid2|----------------->|dcid2        |
	 +-------------+                  +-------------+

	a) Request for a connection. Channel id is generated for that particular connection.
	b) If another connection is requested for the same bdaddr another l2cap_conn is 
	requested from the pool and a different channel id is generated to identify 
	the end point.


NOTE:
----
	Connection handle is used to represent the connection between 2 devices. A single
	connection handle can have multiple channel ids at the source representing different
	channel endpoints.
*/
void l2cap_connect_request(bdaddr_t bdaddr,
					uint8_t *l2cap_pkt_buff,
					PSM_TYPE psm_type) {
	struct l2cap_conn *data;
	int8_t index = 0;

	uint16_t payload = CTRL_SIG_CMD_CONN_REQ_PAYLOAD_SIZE;

	data = remove_from_pool_head(&l2cap_empty_pool);

	if(data == NULL) 
		return;

	
	index = get_index_from_connection_info(conn_info,bdaddr);
	
	if(index == -1)
		return;

	add_to_pool_head(&(conn_info[index].l2cap_conn_pool),
						data);

	(data->l2cap_info).l2cap_state = WAIT_CONNECT_RSP;
	data->channel_id = gen_l2cap_channel_id();

	set_l2cap_len(l2cap_pkt_buff,CTRL_SIG_HEADER_SIZE + payload);
	set_l2cap_channel_id(l2cap_pkt_buff,CHID_SIGNALING_CHANNEL);

	set_l2cap_signal_cmd_code(l2cap_pkt_buff,SIG_CONNECTION_REQUEST);
	set_l2cap_signal_cmd_id(l2cap_pkt_buff,gen_l2cap_sig_id());
	set_l2cap_signal_cmd_len(l2cap_pkt_buff,payload);
	set_l2cap_signal_cmd_conn_req_PSM(l2cap_pkt_buff,psm_type);
	set_l2cap_signal_cmd_conn_req_scid(l2cap_pkt_buff,data->channel_id);

	send_hci_acl_header(conn_info[index].connection_handle,
						PB_FIRST_AUTO_FLUSH_PKT,
						H2C_NO_BROADCAST,
						(L2CAP_HEADER_SIZE + CTRL_SIG_HEADER_SIZE + payload));

	
	hci_send_data_chk(index,
					l2cap_pkt_buff,
					(L2CAP_HEADER_SIZE + CTRL_SIG_HEADER_SIZE + payload));
}

void l2cap_config_request(
					uint16_t conn_handle,
					uint16_t dcid,
					uint8_t *l2cap_pkt_buff
						) {

	uint16_t index = 0;
	uint8_t *tmpbuff;

	set_l2cap_len(l2cap_pkt_buff,CTRL_SIG_HEADER_SIZE + 
									CTRL_SIG_CMD_CONF_REQ_PAYLOAD_SIZE + 
									TOTAL_OPTION_LEN(MTU_OPTION_LEN));
	set_l2cap_channel_id(l2cap_pkt_buff,CHID_SIGNALING_CHANNEL);

	set_l2cap_signal_cmd_code(l2cap_pkt_buff,SIG_CONFIGURATION_REQUEST);
	set_l2cap_signal_cmd_id(l2cap_pkt_buff,gen_l2cap_sig_id());
	set_l2cap_signal_cmd_len(l2cap_pkt_buff,
				CTRL_SIG_CMD_CONF_REQ_PAYLOAD_SIZE + TOTAL_OPTION_LEN(MTU_OPTION_LEN));
	set_l2cap_signal_cmd_conf_req_dcid(l2cap_pkt_buff,dcid);
	
/* 
	***NOTE***:
	When all configuration options cannot fit into a Configuration Request with
	length that does not exceed the receiver's MTUsig, the options shall be
	passed in multiple configuration command packets. If all options fit into the
	receiver's MTUsig, then they shall be sent in a single configuration request
	with the continuation flag set to zero. 
*/

	set_l2cap_signal_cmd_conf_req_flags(l2cap_pkt_buff,
								L2CAP_CONF_NOCONTINUE_FLAG);

	
	tmpbuff = l2cap_pkt_buff + L2CAP_CFRAME_CMD_CONF_REQ_CONFIG_OPT_OFFSET;

	set_l2cap_signal_mtu_option(tmpbuff,L2CAP_PAYLOAD_MTU);
	
	send_hci_acl_header(conn_handle,
						PB_FIRST_AUTO_FLUSH_PKT,
						H2C_NO_BROADCAST,
						(L2CAP_HEADER_SIZE + 
							CTRL_SIG_HEADER_SIZE + 
							(CTRL_SIG_CMD_CONF_REQ_PAYLOAD_SIZE + 
							TOTAL_OPTION_LEN(MTU_OPTION_LEN))));

	
	hci_send_data_chk(index,
					l2cap_pkt_buff,
					(L2CAP_HEADER_SIZE + 
					CTRL_SIG_HEADER_SIZE + 
					(CTRL_SIG_CMD_CONF_REQ_PAYLOAD_SIZE + 
					TOTAL_OPTION_LEN(MTU_OPTION_LEN))));

	
}


void l2cap_send_disconnection_request(
					uint16_t conn_handle,
					uint16_t scid,
					uint8_t *l2cap_pkt_buff
					) {

	struct l2cap_conn *l2cap_conn;
	uint16_t i = 0;


	set_l2cap_sig_cmd_header(l2cap_pkt_buff,
							CHID_SIGNALING_CHANNEL,
							SIG_DISCONNECTION_REQUEST,
							gen_l2cap_sig_id(),
							CTRL_SIG_CMD_DISCONN_REQ_PAYLOAD_SIZE);

	set_l2cap_signal_cmd_len(l2cap_pkt_buff,CTRL_SIG_CMD_DISCONN_REQ_PAYLOAD_SIZE);
	
	//dcid:endpoint of the channel to be disconnected on the dev receiving this request.
	//scid:endpoint of the channel to be disconnected on the dev sending this request.

	l2cap_conn = get_l2cap_conn_from_channel_id(
									conn_info,
									conn_handle,
									scid);
												
	set_l2cap_signal_cmd_disconn_req_resp_dcid(l2cap_pkt_buff,
									l2cap_conn->l2cap_info.dcid);

	
	set_l2cap_signal_cmd_disconn_req_resp_scid(l2cap_pkt_buff,
												scid);

	send_hci_acl_header(conn_handle,
						PB_FIRST_AUTO_FLUSH_PKT,
						H2C_NO_BROADCAST,
						(L2CAP_HEADER_SIZE + 
							CTRL_SIG_HEADER_SIZE + 
							CTRL_SIG_CMD_DISCONN_REQ_PAYLOAD_SIZE));

	
	hci_send_data_chk(i,
					l2cap_pkt_buff,
					(L2CAP_HEADER_SIZE + 
						CTRL_SIG_HEADER_SIZE + 
						CTRL_SIG_CMD_DISCONN_REQ_PAYLOAD_SIZE));

	
	return;
}

void l2cap_send_connection_response(
					uint16_t conn_handle,
					uint8_t cmdid,
					uint16_t dcid,
					uint16_t scid,
					uint8_t *l2cap_pkt_buff
					) {
	
	uint8_t i = 0;

	/* Send connection response */
	set_l2cap_sig_cmd_header(l2cap_pkt_buff,
							CHID_SIGNALING_CHANNEL,
							SIG_CONNECTION_RESPONSE,
							cmdid,
							CTRL_SIG_CMD_CONN_RESP_PAYLOAD_SIZE);
	
	set_l2cap_signal_cmd_conn_resp_dcid(l2cap_pkt_buff,dcid);
	set_l2cap_signal_cmd_conn_resp_scid(l2cap_pkt_buff,scid);
	set_l2cap_signal_cmd_conn_resp_result(l2cap_pkt_buff,CONNECTION_SUCCESSFUL);
	set_l2cap_signal_cmd_conn_resp_status(l2cap_pkt_buff,0x0000);
	
	send_hci_acl_header(conn_handle,
						PB_FIRST_AUTO_FLUSH_PKT,
						H2C_NO_BROADCAST,
						(L2CAP_HEADER_SIZE + 
							CTRL_SIG_HEADER_SIZE + 
							CTRL_SIG_CMD_CONN_RESP_PAYLOAD_SIZE));
	
	hci_send_data_chk(i,
					l2cap_pkt_buff,
					(L2CAP_HEADER_SIZE + 
					CTRL_SIG_HEADER_SIZE + 
					CTRL_SIG_CMD_CONN_RESP_PAYLOAD_SIZE));

	return;
}
/*
void l2cap_send_infotype_request(
					uint16_t conn_handle,
					uint16_t scid,
					uint8_t *l2cap_pkt_buff) {

	struct l2cap_conn *l2cap_conn;
	
	
	set_l2cap_len(l2cap_pkt_buff,CTRL_SIG_HEADER_SIZE + CTRL_SIG_CMD_INFO_REQ_PAYLOAD_SIZE);
	set_l2cap_channel_id(l2cap_pkt_buff,CHID_SIGNALING_CHANNEL);

	set_l2cap_signal_cmd_code(l2cap_pkt_buff,SIG_INFO_REQUEST);
	set_l2cap_signal_cmd_id(l2cap_pkt_buff,gen_l2cap_sig_id());

	set_l2cap_signal_cmd_len(l2cap_pkt_buff,CTRL_SIG_CMD_INFO_REQ_PAYLOAD_SIZE);
	
}*/
