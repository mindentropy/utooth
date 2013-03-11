#ifndef L2CAP_H_

#define L2CAP_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "hal_usb.h"
#include "hci.h"
#include "rfcomm.h"
#include "cq.h"

#define L2CAP_CONN_POOL_SIZE 5


#define MIN_L2CAP_CID	0x0040
#define MAX_L2CAP_CID	0xFFFF

#define MIN_L2CAP_SIG_ID 0x00000

#define L2CAP_CONF_NOCONTINUE_FLAG 	0x0000
#define L2CAP_CONF_CONTINUE_FLAG 	0x0001

#define L2CAP_LEN_SIZE			2
#define L2CAP_CHANNEL_ID_SIZE	2

#define L2CAP_LENGTH_OFFSET				0
#define L2CAP_CHANNEL_ID_OFFSET			(L2CAP_LEN_SIZE + L2CAP_LENGTH_OFFSET)
#define L2CAP_BFRAME_PAYLOAD_OFFSET		(L2CAP_CHANNEL_ID_OFFSET + L2CAP_CHANNEL_ID_SIZE)
#define L2CAP_PSM_OFFSET				4
#define L2CAP_CONTROL_OFFSET			4
//#define L2CAP_CONTROL_OFFSET			((L2CAP_CHANNEL_ID_OFFSET) + (L2CAP_CHANNEL_ID_SIZE))

#define L2CAP_HEADER_SIZE ((L2CAP_LEN_SIZE) + (L2CAP_CHANNEL_ID_SIZE))

#define L2CAP_CFRAME_HEADER_SIZE ((L2CAP_LEN_SIZE) + (L2CAP_CHANNEL_ID_SIZE))

#define L2CAP_IFRAME_SDU_LEN_OFFSET		6
#define L2CAP_SFRAME_FCS_OFFSET			6
#define L2CAP_IFRAME_FCS_OFFSET			65533

#define CTRL_SIG_CMD_CODE_SIZE		1
#define CTRL_SIG_CMD_ID_SIZE		1
#define CTRL_SIG_CMD_LEN_SIZE		2
#define CTRL_SIG_CMD_SCID_SIZE		2
#define CTRL_SIG_CMD_DCID_SIZE		2
#define CTRL_SIG_CMD_RESULT_SIZE	2
#define	CTRL_SIG_CMD_STATUS_SIZE	2
#define CTRL_SIG_CMD_PSM_SIZE		2
#define CTRL_SIG_CMD_INFOTYPE_SIZE	2
#define CTRL_SIG_CMD_FLAGS_SIZE		2
#define CTRL_SIG_CMD_REASON_SIZE	2

#define CTRL_SIG_INFO_CONNECTIONLESS_MTU_SIZE 		2
#define CTRL_SIG_INFO_EXTENDED_FEATURES_MASK_SIZE	4

#define CTRL_SIG_CMD_CODE_OFFSET	0

#define CTRL_SIG_CMD_ID_OFFSET	 \
				((CTRL_SIG_CMD_CODE_OFFSET) + (CTRL_SIG_CMD_CODE_SIZE))

#define CTRL_SIG_CMD_LEN_OFFSET	 \
				((CTRL_SIG_CMD_ID_OFFSET) + (CTRL_SIG_CMD_ID_SIZE))

#define CTRL_SIG_CMD_DATA_OFFSET \
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_REJ_REASON_OFFSET	\
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_REJ_DATA_OFFSET	\
				((CTRL_SIG_CMD_REJ_REASON_OFFSET) + (CTRL_SIG_CMD_REASON_SIZE))

#define CTRL_SIG_CMD_PSM_OFFSET	 \
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_CONN_RESP_DCID_OFFSET \
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_CONN_RESP_SCID_OFFSET \
				((CTRL_SIG_CMD_CONN_RESP_DCID_OFFSET) + (CTRL_SIG_CMD_DCID_SIZE))

#define CTRL_SIG_CMD_INF_REQ_RESP_INFOTYPE_OFFSET \
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_CONN_REQ_SCID_OFFSET \
				((CTRL_SIG_CMD_PSM_OFFSET) + (CTRL_SIG_CMD_PSM_SIZE))

#define CTRL_SIG_CMD_CONN_RESP_RESULT_OFFSET \
				((CTRL_SIG_CMD_CONN_RESP_SCID_OFFSET) + (CTRL_SIG_CMD_SCID_SIZE))

#define CTRL_SIG_CMD_CONN_RESP_STATUS_OFFSET \
				((CTRL_SIG_CMD_CONN_RESP_RESULT_OFFSET) + (CTRL_SIG_CMD_RESULT_SIZE))

#define CTRL_SIG_CMD_INF_RESP_RESULT_OFFSET \
				((CTRL_SIG_CMD_INF_REQ_RESP_INFOTYPE_OFFSET) + (CTRL_SIG_CMD_INFOTYPE_SIZE))

#define CTRL_SIG_CMD_INF_RESP_DATA_OFFSET \
				((CTRL_SIG_CMD_INF_RESP_RESULT_OFFSET) + (CTRL_SIG_CMD_RESULT_SIZE))

#define CTRL_SIG_CMD_CONF_REQ_DCID_OFFSET	\
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_CONF_REQ_RESP_FLAGS_OFFSET	\
			((CTRL_SIG_CMD_CONF_REQ_DCID_OFFSET) + (CTRL_SIG_CMD_DCID_SIZE))

#define CTRL_SIG_CMD_CONF_REQ_CONF_OPT_OFFSET \
			((CTRL_SIG_CMD_CONF_REQ_RESP_FLAGS_OFFSET) + (CTRL_SIG_CMD_FLAGS_SIZE))

#define CTRL_SIG_CMD_CONF_RESP_SCID_OFFSET	\
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_CONF_RESP_FLAGS_OFFSET	\
				((CTRL_SIG_CMD_CONF_RESP_SCID_OFFSET) + (CTRL_SIG_CMD_SCID_SIZE))

#define CTRL_SIG_CMD_CONF_RESP_RESULT_OFFSET	\
				((CTRL_SIG_CMD_CONF_RESP_FLAGS_OFFSET) + (CTRL_SIG_CMD_FLAGS_SIZE))

#define CTRL_SIG_CMD_CONF_RESP_CONFIG_OFFSET	\
				((CTRL_SIG_CMD_CONF_RESP_RESULT_OFFSET) + (CTRL_SIG_CMD_RESULT_SIZE))

#define CTRL_SIG_CMD_DISCONN_REQ_RESP_DCID_OFFSET	\
				((CTRL_SIG_CMD_LEN_OFFSET) + (CTRL_SIG_CMD_LEN_SIZE))

#define CTRL_SIG_CMD_DISCONN_REQ_RESP_SCID_OFFSET	\
				((CTRL_SIG_CMD_DISCONN_REQ_RESP_DCID_OFFSET	) + (CTRL_SIG_CMD_DCID_SIZE))

#define L2CAP_CFRAME_CMD_CODE_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CODE_OFFSET))
#define L2CAP_CFRAME_CMD_ID_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_ID_OFFSET))	
#define L2CAP_CFRAME_CMD_LEN_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_LEN_OFFSET))

	
#define L2CAP_CFRAME_DATA_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_DATA_OFFSET))
#define L2CAP_CFRAME_INF_REQ_RESP_INFOTYPE_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_INF_REQ_RESP_INFOTYPE_OFFSET))

#define L2CAP_CFRAME_CMD_INF_RESP_RESULT_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_INF_RESP_RESULT_OFFSET))

#define L2CAP_CFRAME_CMD_INF_RESP_DATA_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_INF_RESP_DATA_OFFSET))

#define L2CAP_CFRAME_CMD_PSM_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_PSM_OFFSET))

#define L2CAP_CFRAME_CMD_CONN_REQ_SCID_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONN_REQ_SCID_OFFSET))

#define L2CAP_CFRAME_CMD_CONN_RESP_DCID_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONN_RESP_DCID_OFFSET))

#define L2CAP_CFRAME_CMD_CONN_RESP_SCID_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONN_RESP_SCID_OFFSET))

#define L2CAP_CFRAME_CMD_CONN_RESP_RESULT_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONN_RESP_RESULT_OFFSET))

#define L2CAP_CFRAME_CMD_CONN_RESP_STATUS_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONN_RESP_STATUS_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_REQ_DCID_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_REQ_DCID_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_REQ_FLAGS_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_REQ_RESP_FLAGS_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_REQ_RESP_FLAGS_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_REQ_RESP_FLAGS_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_REQ_CONFIG_OPT_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_REQ_CONF_OPT_OFFSET))



#define L2CAP_CFRAME_CMD_CONF_RESP_SCID_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_RESP_SCID_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_RESP_FLAGS_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_RESP_FLAGS_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_RESP_RESULT_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_RESP_RESULT_OFFSET))

#define L2CAP_CFRAME_CMD_CONF_RESP_CONFIG_OPT_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_CONF_RESP_CONFIG_OFFSET))

#define L2CAP_CFRAME_CMD_DISCONN_REQ_RESP_DCID_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_DISCONN_REQ_RESP_DCID_OFFSET))

#define L2CAP_CFRAME_CMD_DISCONN_REQ_RESP_SCID_OFFSET \
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_DISCONN_REQ_RESP_SCID_OFFSET))

#define L2CAP_CFRAME_CMD_REJ_REASON_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_REJ_REASON_OFFSET))

#define L2CAP_CFRAME_CMD_REJ_DATA_OFFSET	\
				((L2CAP_CFRAME_HEADER_SIZE) + (CTRL_SIG_CMD_REJ_DATA_OFFSET))

#define L2CAP_CFRAME_CMD_DEST_CID_OFFSET	4
#define L2CAP_CFRAME_CMD_SRC_CID_OFFSET		6

#define L2CAP_FLUSH_TIMEOUT		0xFFFF //Default. Infinite retransmissions.

#define L2CAP_PAYLOAD_MIN_MTU	48
#define L2CAP_PAYLOAD_MTU		341 //TODO: Set to a packet type. DH5 PKT set as of now.
#define L2CAP_BUFF_SIZE	((L2CAP_PAYLOAD_MTU)+(L2CAP_LEN_SIZE)+(L2CAP_CHANNEL_ID_SIZE))

#define CTRL_SIG_HEADER_SIZE ((CTRL_SIG_CMD_CODE_SIZE) + \
								(CTRL_SIG_CMD_ID_SIZE) + \
								(CTRL_SIG_CMD_LEN_SIZE))


/* Fixed payload sizes */
#define CTRL_SIG_CMD_REJ_PAYLOAD_SIZE ((CTRL_SIG_REASON_SIZE))
#define CTRL_SIG_CMD_CONN_REQ_PAYLOAD_SIZE \
					((CTRL_SIG_CMD_PSM_SIZE) + \
					(CTRL_SIG_CMD_SCID_SIZE))

#define CTRL_SIG_CMD_CONN_RESP_PAYLOAD_SIZE	((CTRL_SIG_CMD_DCID_SIZE) + \
									(CTRL_SIG_CMD_SCID_SIZE) +	\
									(CTRL_SIG_CMD_RESULT_SIZE) + \
									(CTRL_SIG_CMD_STATUS_SIZE))

#define CTRL_SIG_CMD_CONF_REQ_PAYLOAD_SIZE 	((CTRL_SIG_CMD_DCID_SIZE) + \
									(CTRL_SIG_CMD_FLAGS_SIZE))

#define CTRL_SIG_CMD_CONF_RESP_PAYLOAD_SIZE	((CTRL_SIG_CMD_SCID_SIZE) +	\
									(CTRL_SIG_CMD_FLAGS_SIZE) + \
									(CTRL_SIG_CMD_RESULT_SIZE))

#define CTRL_SIG_CMD_DISCONN_REQ_PAYLOAD_SIZE ((CTRL_SIG_CMD_DCID_SIZE) + \
								(CTRL_SIG_CMD_SCID_SIZE))

#define CTRL_SIG_CMD_DISCONN_RESP_PAYLOAD_SIZE	((CTRL_SIG_CMD_DCID_SIZE) + \
										(CTRL_SIG_CMD_SCID_SIZE))

#define CTRL_SIG_CMD_ECHO_REQ_PAYLOAD_SIZE	0
#define CTRL_SIG_CMD_ECHO_RESP_PAYLOAD_SIZE	0

#define CTRL_SIG_CMD_INFO_REQ_PAYLOAD_SIZE	(CTRL_SIG_CMD_INFOTYPE_SIZE)

#define CTRL_SIG_CMD_INFO_RESP_PAYLOAD_SIZE	((CTRL_SIG_CMD_INFOTYPE_SIZE) + \
									(CTRL_SIG_CMD_RESULT_SIZE))


#define set_l2cap_param8_le(buff,index,val)	\
	((buff[index]) = (val))

#define set_l2cap_param16_le(buff,index,val)	\
	((buff[index]) = ((val) & 0xFF));	\
	((buff[(index) + 1]) = ((val) >> 8))

#define set_l2cap_param32_le(buff,index,val) \
	(buff[index] = ((val) & 0xFF)); \
	((buff[(index) + 1]) = ((val >> 8) & 0xFF)); \
	((buff[(index) + 2]) = ((val >> 16) & 0xFF)); \
	((buff[(index) + 3]) = ((val >> 24) & 0xFF)) 

#define get_l2cap_param8_le(buff,index) \
	((buff[index]))

#define get_l2cap_param16_le(buff,index) \
	((buff[index]) | (buff[index+1] << 8 ))

#define get_l2cap_param24_le(buff,index) \
	((buff[index]) | (buff[index+1] << 8) | (buff[index+2] << 16))

#define get_l2cap_param32_le(buff,index) \
	((buff[index]) | (buff[index+1] << 8) | \
		(buff[index+2] << 16) | (buff[index+3] << 24))

#define set_l2cap_len(buff,len) \
	set_l2cap_param16_le(buff,L2CAP_LENGTH_OFFSET,len)

#define get_l2cap_len(buff) \
	read16_buff_le(buff,L2CAP_LENGTH_OFFSET)

#define get_l2cap_bframe_size(buff)	\
	((L2CAP_HEADER_SIZE) + (get_l2cap_len(buff)))

#define set_l2cap_channel_id(buff,chid) \
	set_l2cap_param16_le(buff,L2CAP_CHANNEL_ID_OFFSET,chid)

#define get_l2cap_channel_id(buff) \
	read16_buff_le(buff,L2CAP_CHANNEL_ID_OFFSET)

#define get_l2cap_ctrl(buff) \
	read16_buff_le(buff,4)

#define get_l2cap_cmd_id(buff) \
	read8_buff_le(buff,L2CAP_CFRAME_CMD_ID_OFFSET)

#define get_l2cap_cmd_code(buff) \
	read8_buff_le(buff,L2CAP_CFRAME_CMD_CODE_OFFSET)

#define get_l2cap_cmd_len(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_LEN_OFFSET)

#define get_l2cap_cmd_rej_reason(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_REJ_REASON_OFFSET)

#define get_l2cap_cmd_conn_resp_dcid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_DCID_OFFSET)

#define get_l2cap_cmd_conn_resp_scid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_SCID_OFFSET)
	
#define get_l2cap_cmd_conn_resp_result(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_RESULT_OFFSET)

#define get_l2cap_cmd_conn_resp_status(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_STATUS_OFFSET)

#define get_l2cap_cmd_conn_req_psm(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_PSM_OFFSET)

#define get_l2cap_cmd_conn_req_scid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONN_REQ_SCID_OFFSET)

#define get_l2cap_cmd_conf_req_dcid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONF_REQ_DCID_OFFSET)

#define get_l2cap_cmd_conf_req_flags(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONF_REQ_RESP_FLAGS_OFFSET)

#define get_l2cap_cmd_conf_resp_scid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONF_RESP_SCID_OFFSET)

#define get_l2cap_cmd_conf_resp_flags(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONF_REQ_RESP_FLAGS_OFFSET)

#define get_l2cap_cmd_conf_resp_result(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_CONF_RESP_RESULT_OFFSET)

#define get_l2cap_cmd_disconn_req_dcid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_DISCONN_REQ_RESP_DCID_OFFSET)

#define get_l2cap_cmd_disconn_req_scid(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_CMD_DISCONN_REQ_RESP_SCID_OFFSET)

#define get_l2cap_cmd_info_req_infotype(buff) \
	read16_buff_le(buff,L2CAP_CFRAME_INF_REQ_RESP_INFOTYPE_OFFSET)

#define set_l2cap_sframe_control(buff,control) \
	set_l2cap_param16_le(buff,L2CAP_CONTROL_OFFSET,control)

#define set_l2cap_signal_cmd_code(buff,code) \
	set_l2cap_param8_le(buff,L2CAP_CFRAME_CMD_CODE_OFFSET,code)

#define set_l2cap_signal_cmd_id(buff,id)	\
	set_l2cap_param8_le(buff,L2CAP_CFRAME_CMD_ID_OFFSET,id)

#define set_l2cap_signal_cmd_len(buff,len)	\
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_LEN_OFFSET,len)

#define get_l2cap_data_size_from_acl_len(acl_data_len) \
	((acl_data_len) - (L2CAP_HEADER_SIZE))

#define get_cmd_data_size_from_acl_len(acl_data_len) \
	((acl_data_len) - (L2CAP_HEADER_SIZE) - (CTRL_SIG_HEADER_SIZE))

#define get_cmd_data_size_from_l2cap_len(l2cap_len) \
	((l2cap_len) - (CTRL_SIG_HEADER_SIZE))

#define set_l2cap_signal_cmd_conn_resp_scid(buff,scid) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_SCID_OFFSET,scid)

#define set_l2cap_signal_cmd_conn_resp_dcid(buff,dcid) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_DCID_OFFSET,dcid)

#define set_l2cap_signal_cmd_conn_resp_result(buff,res) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_RESULT_OFFSET,res)

#define set_l2cap_signal_cmd_conn_resp_status(buff,stat) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONN_RESP_STATUS_OFFSET,stat)
	
#define set_l2cap_signal_cmd_info_req_resp_infotype(buff,infotype)	\
	set_l2cap_param16_le(buff,L2CAP_CFRAME_INF_REQ_RESP_INFOTYPE_OFFSET,infotype)

#define set_l2cap_signal_cmd_info_resp_result(buff,result) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_INF_RESP_RESULT_OFFSET,result)

#define set_l2cap_signal_cmd_info_resp_data(buff,data) \
	set_l2cap_param32_le(buff,L2CAP_CFRAME_CMD_INF_RESP_DATA_OFFSET,data)

#define set_l2cap_signal_cmd_info_resp_data_mtu(buff,data) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_INF_RESP_DATA_OFFSET,data)

#define set_l2cap_signal_cmd_conf_req_dcid(buff,dcid) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONF_REQ_DCID_OFFSET,dcid)
	
#define set_l2cap_signal_cmd_conf_req_flags(buff,flags) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONF_REQ_FLAGS_OFFSET,flags)

#define set_l2cap_signal_cmd_conf_resp_scid(buff,scid) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONF_RESP_SCID_OFFSET,scid)

#define set_l2cap_signal_cmd_conf_resp_flags(buff,flags) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONF_RESP_FLAGS_OFFSET,flags)

#define set_l2cap_signal_cmd_conf_resp_result(buff,result) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONF_RESP_RESULT_OFFSET,result)
	
#define set_l2cap_signal_cmd_conf_resp_config(buff,config) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONF_RESP_CONFIG_OPT_OFFSET,config)

#define set_l2cap_signal_cmd_disconn_req_resp_dcid(buff,dcid)	\
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_DISCONN_REQ_RESP_DCID_OFFSET,dcid)

#define set_l2cap_signal_cmd_disconn_req_resp_scid(buff,scid) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_DISCONN_REQ_RESP_SCID_OFFSET,scid)

#define set_l2cap_sig_cmd_header(buff,chid,cmdcode,cmdid,cmdlen) \
	set_l2cap_channel_id(buff,chid);	\
	set_l2cap_len(buff,((CTRL_SIG_HEADER_SIZE) + cmdlen));	\
	set_l2cap_signal_cmd_code(buff,cmdcode);	\
	set_l2cap_signal_cmd_id(buff,cmdid);	\
	set_l2cap_signal_cmd_len(buff,cmdlen)
	
#define set_l2cap_signal_cmd_conn_req_PSM(buff,PSM) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_PSM_OFFSET,PSM)

#define set_l2cap_signal_cmd_conn_req_scid(buff,scid) \
	set_l2cap_param16_le(buff,L2CAP_CFRAME_CMD_CONN_REQ_SCID_OFFSET,scid)

#define get_l2cap_bframe_payload_buff(buff)	\
	((buff) + (L2CAP_BFRAME_PAYLOAD_OFFSET))


#define CONFIG_CODE_SIZE					1
#define CONF_OPT_LEN_SIZE					1

#define CONFIG_OPTION_HEADER_SIZE			((CONFIG_CODE_SIZE) + (CONF_OPT_LEN_SIZE))

#define MTU_OPTION_LEN 						2
#define FLUSH_TIMEOUT_OPTION_LEN 			2
#define QoS_OPTION_LEN						22
#define RETRANSMISSION_FLOW_CONTROL_LEN		9


#define CONFIG_TYPE_OFFSET				0
#define CONFIG_LEN_OFFSET				((CONFIG_TYPE_OFFSET) + (CONFIG_CODE_SIZE))
#define CONFIG_OPTION_DATA_OFFSET		((CONFIG_LEN_OFFSET) + (CONF_OPT_LEN_SIZE))

#define TOTAL_OPTION_LEN(OPTION_LEN) \
	((CONFIG_OPTION_HEADER_SIZE) + (OPTION_LEN))



typedef enum option_type {
	MTU_OPTION_TYPE = 0x01,
	FLUSH_TIMEOUT_OPTION_TYPE,
	QoS_OPTION_TYPE,
	RETRANSMISSION_AND_FLOW_CONTROL_OPTION_TYPE,
} OPTION_TYPE;

#define set_l2cap_signal_mtu_option(buff,mtu) \
	set_l2cap_param8_le(buff,CONFIG_TYPE_OFFSET,MTU_OPTION_TYPE);	\
	set_l2cap_param8_le(buff,CONFIG_LEN_OFFSET,MTU_OPTION_LEN);	\
	set_l2cap_param16_le(buff,CONFIG_OPTION_DATA_OFFSET,mtu)

#define set_l2cap_signal_flush_timeout_option(buff,flushtimeout) \
	set_l2cap_param8_le(buff,CONFIG_TYPE_OFFSET,FLUSH_TIMEOUT_OPTION_TYPE);	\
	set_l2cap_param8_le(buff,CONFIG_LEN_OFFSET,FLUSH_TIMEOUT_OPTION_LEN); \
	set_l2cap_param16_le(buff,CONFIG_OPTION_DATA_OFFSET,flushtimeout)

#define FRAME_TYPE_MASK 	0x1
#define TX_SEQ_MASK			0x7E
#define REQ_SEQ_MASK		0x3F00
#define SAR_MASK			0xC000
#define SUPERVISORY_MASK	0x0C
#define RETRANSMISSION_DISABLE_MARK 0x80

#define get_frame_type(ctrl)	\
	((ctrl) & (FRAME_TYPE_MASK))

#define get_tx_seq(ctrl)	\
	(((ctrl) & (TX_SEQ_MASK)) >> 1)

#define get_rx_seq(ctrl)	\
	(((ctrl) & (RX_SEQ_MASK)) >> 8)

#define get_sar(ctrl) \
	(((ctrl) & (SAR_MASK)) >> 14)

#define get_supervisory(ctrl) \
	(((ctrl) & (0x0C)) >> 2)

#define get_retransmission_disable(ctrl) \
	(((ctrl) & (RETRANSMISSION_DISABLE_MARK)) >> 7)
/*
#define set_l2cap_retransmission_and_flowcontrol_option\
	(buff,mode,txwinsize,maxtransmit,retrans_timeout,mon_timeout_lsb_mon_timeout_msb,
*/

typedef enum channel_id_type {
	CHID_NULL_ID = 0x0000,
	CHID_SIGNALING_CHANNEL,
	CHID_CONNECTIONLESS_RECEPTION_CHANNEL
} CHANNEL_ID_TYPE;

/* Control packet command codes */
typedef enum control_packet_signals {
	SIG_RESERVED = 0x00,
	SIG_COMMAND_REJECT,
	SIG_CONNECTION_REQUEST,
	SIG_CONNECTION_RESPONSE,
	SIG_CONFIGURATION_REQUEST,
	SIG_CONFIGURATION_RESPONSE,
	SIG_DISCONNECTION_REQUEST,
	SIG_DISCONNECTION_RESPONSE,
	SIG_ECHO_REQUEST,
	SIG_ECHO_RESPONSE,
	SIG_INFO_REQUEST,
	SIG_INFO_RESPONSE
} CONTROL_PACKET_SIGNALS;

typedef enum disconnection_reason {
	REASON_COMMAND_NOT_UNDERSTOOD =	0x0000,
	REASON_SIGNAL_MTU_EXCEEDED,
	REASON_INVALID_CID_IN_REQUEST
} DISCONNECTION_REASON;

typedef enum connection_response {
	CONNECTION_SUCCESSFUL = 0x0000,
	CONNECTION_PENDING,
	CONNECTION_REFUSED_PSM_NOT_SUPPORTED,
	CONNECTION_REFUSED_SECURITY_BLOCK,
	CONNECTION_REFUSED_NO_RESOURCES_AVAILABLE
} CONNECTION_RESPONSE;

typedef enum connection_response_status {
	NO_FURTHER_INFORMATION_AVAILABLE = 0x0000,
	AUTHENTICATION_PENDING,
	AUTHORIZATION_PENDING
} CONNECTION_RESPONSE_STATUS;

typedef enum configuration_response_result {
	CONFIG_SUCCESS = 0x0000,
	FAILURE_UNACCEPTABLE_PARAMETERS,
	FAILURE_REJECTED,
	FAILURE_UNKNOWN_OPTIONS
} CONFIGURATION_RESPONSE_RESULT;

typedef enum infotype_request_response_def {
	CONNECTIONLESS_MTU = 0x0001,
	EXTENDED_FEATURES_SUPPORTED
} INFORMATION_REQUEST_RESPONSE;

enum information_response_result {
	INFO_SUCCESS= 0x0000,
	INFO_NOT_SUPPORTED = 0x0001
} INFORMATION_RESPONSE_RESULT;

enum QoS_def {
	NO_TRAFFIC = 0x00,
	BEST_EFFORT,
	GUARANTEED
} QoS;

enum retransmission_flow_control_mode {
	BASIC_L2CAP = 0x00, 
	RETRANSMISSION,
	FLOW_CONTROL
} RETRANSMISSION_FLOW_CONTROL_MODE;

typedef enum {
	L2CAP_OPEN,
	L2CAP_CLOSED,
	L2CAP_CONFIG,
	L2CAP_WAIT_CONNECT,
	L2CAP_WAIT_CONNECT_RSP
} L2CAP_STATE;

typedef enum {
	L2CAP_NONE,
	L2CAP_WAIT_DISCONNECT,
	L2CAP_WAIT_CONFIG,
	L2CAP_WAIT_SEND_CONFIG,
	L2CAP_WAIT_CONFIG_REQ_RESP,
	L2CAP_WAIT_CONFIG_RESP,
	L2CAP_WAIT_CONFIG_REQ
} L2CAP_SUBSTATE;

#define set_l2cap_state(l2cap_state,state,l2cap_substate,substate)	\
	((l2cap_state) = (state)),((l2cap_substate) = (substate));


typedef enum frame_type {
	IFRAME = 0x00,
	SFRAME,
}FRAME_TYPE;


typedef enum connect_initiate {
	LOCAL,
	REMOTE,
} L2CAP_CONNECT_INITIATE;

#define	FLOW_CONTROL_MODE  0x00000001
#define	RETRANSMISSION_MODE  ((0x00000001)<< 1)
#define	BI_QoS  ((0x00000001) << 2)

typedef enum PSM {
	PSM_SDP 				= 0x0001,
	PSM_RFCOMM 				= 0x0003,
	PSM_TCS_BIN 			= 0x0005,
	PSM_TTCS_BIN_CORDLESS 	= 0x0007,
	PSM_TBNEP 				= 0x000F,
	PSM_THID_CONTROL 		= 0x0011,
	PSM_THID_INTERRUPT 		= 0x0013,
	PSM_TUPnP 				= 0x0015,
	PSM_TAVCTP 				= 0x0017,
	PSM_TAVDTP 				= 0x0019,
	PSM_TAVCTP_Browsing 	= 0x001B,
	PSM_TUDI_C_PLANE 		= 0x001D,
	PSM_TATT 				= 0x001F,
}PSM_TYPE;




typedef enum {
	L2CAP_PKT_OK = 0x00,
	L2CAP_PKT_MISMATCH
}
#ifdef __GNUC__
__attribute__((packed))
#endif
L2CAP_ERR;

#define MTU_OPTION_BIT 							0x01
#define FLUSH_TIMEOUT_OPTION_BIT 				0x02
#define QOS_OPTION_BIT							0x04
#define RETRANSMISSION_FLOW_CONTROL_OPTION_BIT	0x08

struct QoS_Options {
	uint8_t service_type;
	uint32_t token_rate;
	uint32_t token_bucket_size;
	uint32_t peak_bandwidth;
	uint32_t latency;
	uint32_t delay_variation;
};

struct retransmission_flow_control_option {
	uint8_t TxWinSize;
	uint8_t max_transmit;
	uint16_t retransmission_timeout;
	uint16_t monitor_timeout;
	uint16_t max_pdu_size;
};

struct config_options {
	uint8_t option_bits;
	uint16_t mtu;
	uint16_t flush_timeout;
	struct QoS_Options QoS_options;
	struct retransmission_flow_control_option retransmission_flow_control_option;
};


struct l2cap_info {
	uint8_t 	cmd_sigid;
	uint16_t 	dcid; //Remote host channel id.
	
	L2CAP_CONNECT_INITIATE connect_initiate;

	L2CAP_STATE l2cap_state;
	L2CAP_SUBSTATE l2cap_substate;

	void (*l2cap_pong_cb)(struct l2cap_info *l2cap_info,uint8_t argcnt,...);

	struct config_options conf_opt;
//	struct rfcomm_config_options rfcomm_conf_opt;
	
	struct rfcomm_info rfcomm_info;

	PSM_TYPE psm_type;

};

struct l2cap_conn {
	struct list_head list;
	uint16_t local_channel_id; //Represent the channel endpoint. Single channel can
						// have multiple endpoints. Hence its present in a pool.
						// This is my local host channel id.
	
	struct l2cap_info l2cap_info;

};

uint8_t gen_l2cap_sig_id();
uint16_t gen_l2cap_channel_id();

void process_l2cap_pkt(uint16_t conn_handle,uint8_t *l2cap_pkt_buff);


void l2cap_init();
void l2cap_ping(bdaddr_t bdaddr,
				struct l2cap_info *l2cap_info,
				uint8_t *l2cap_pkt_buff,
				void (*l2cap_pong_cb)(struct l2cap_info *l2cap_info,uint8_t argcnt,...),
				uint8_t argcnt,
				...
				);



/* 
	Get the l2cap connection info for the particular channel endpoint. 
	Minimum info required for this is conn_handle(HCI layer). This 
	conn_handle can have multiple endpoints. Hence a channel_id is 
	needed to identify the endpoint.

	NOTE:
	----
	We cannot just get the required channel id using a bdaddr or a 
	connection handle as there can be multiple channel id endpoints
	to a single device. Therefore specifying a channel id is a must.

*/

struct l2cap_conn * 
get_l2cap_conn_from_channel_id(struct connection_info *conn_info,
								uint16_t conn_handle,
								uint16_t local_channel_id);

void l2cap_connect_request(bdaddr_t bdaddr,
						uint8_t *l2cap_pkt_buff,
						PSM_TYPE psm_type);

void l2cap_config_request(uint16_t conn_handle,
						uint16_t channel_id,
						uint8_t *l2cap_pkt_buff);
void l2cap_send_disconnection_request(
					uint16_t conn_handle,
					uint16_t scid,
					uint8_t *l2cap_pkt_buff
					);

void l2cap_send_connection_response(
					uint16_t conn_handle,
					uint8_t cmdid,
					uint16_t dcid,
					uint16_t scid,
					uint8_t *l2cap_pkt_buff
					);

void create_l2cap_bframe_rfcomm_pkt(
				uint8_t *l2cap_pkt_buff,
				struct l2cap_conn *conn);

struct l2cap_conn * process_connection_request(
							uint16_t conn_handle,
							uint16_t channelid, //my channel id.
							uint16_t dcid, //remote device channel id
							PSM_TYPE psm
							);
#endif
