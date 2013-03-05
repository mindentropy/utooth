#ifndef RFCOMM_H_

#define RFCOMM_H_

#include <stdint.h>
#include "hci.h"

#define MAX_CREDIT_CNT 33

#define FCS_PASS	1
#define FCS_FAIL	0

typedef enum rfcomm_frame_type { 
	SABM = 0x2F, //Set Asynchronous balanced mode.
	UA   = 0x63, //Unnumbered Acknowledgement
	DM   = 0x0F, //Disconnected mode response.
	DISC = 0x43, //Disconnect command.
	UIH  = 0xEF	 //Unnumbered information with header check.
} RFCOMM_FRAME_TYPE;

typedef enum rfcomm_frame_msg_type {
	PN		=	0x80,
	PSC 	=	0x40,
	CLD		=	0xC0,
	TEST	=	0x20,
	FCon	=	0xA0,
	FCoff	=	0x60,
	MSC		=	0xE0,
	NSC		=	0x10,
	RPN		=	0x90,
	RLS		=	0x50,
	SNC		=	0xD0,
} RFCOMM_FRAME_MSG_TYPE;


struct rfcomm_config_options {
	uint8_t pn_config[8];
	uint8_t rpn_config[8];
	uint8_t msc_config[3];
};

typedef enum i_conf {
	USE_UIH,
	USE_UI,
	USE_I
} I_CONF;

typedef enum cl_conf {
	CL_TYPE1,
	CL_TYPE2,
	CL_TYPE3,
	CL_TYPE4
} CL_CONF;


typedef enum parity_type {
	ODD_PARITY,
	EVEN_PARITY,
	MARK_PARITY,
	SPACE_PARITY
} PARITY_TYPE;

typedef enum rfcomm_state {
	RFCOMM_NONE,
	RFCOMM_CONNECT,
	RFCOMM_DICONNECT,
	RFCOMM_PN_REQUEST,
	RFCOMM_PN_CONFIRM,
	RFCOMM_TEST_REQUEST,
	RFCOMM_TEST_CONFIRM,
	RFCOMM_FCON_REQUEST,
	RFCOMM_FCON_CONFIRM,
	RFCOMM_FCOFF_REQUEST,
	RFCOMM_FCOFF_CONFIRM,
	RFCOMM_MSC_REQUEST,
	RFCOMM_MSC_CONFIRM,
	RFCOMM_NSC_REQUEST,
	RFCOMM_NSC_CONFIRM,
	RFCOMM_RPN_REQUEST,
	RFCOMM_RPN_CONFIRM,
	RFCOMM_RLS_REQUEST,
	RFCOMM_RLS_CONFIRM,
	RFCOMM_SNC_REQUEST,
	RFCOMM_SNC_CONFIRM
} RFCOMM_STATE;

typedef enum rfcomm_state_transition {
	RFCOMM_ACTIVE,
	RFCOMM_IDLE
} RFCOMM_STATE_TRANSITION;


struct rfcomm_info {
	struct rfcomm_config_options rfcomm_conf_opt;
	uint8_t dlci;
	RFCOMM_STATE rfcomm_state;
	RFCOMM_STATE_TRANSITION rfcomm_state_transition;
};

#define init_rfcomm_state(rfcomm_state)	\
		rfcomm_state = RFCOMM_NONE

#define init_rfcomm_transition(rfcomm_state) \
		rfcomm_state = RFCOMM_IDLE

#define set_rfcomm_state(rfcomm_state,state)	\
		rfcomm_state = state

#define set_rfcomm_transition(rfcomm_transition,transition)	\
		rfcomm_transition = transition

#define get_rfcomm_state(rfcomm_state)	\
		rfcomm_state
	

#define get_rfcomm_transition(rfcomm_state)	\
		rfcomm_state_transition

typedef enum pn_offset_conf {
	DLCI_OFFSET = 0,
	CL_AND_FRAME_OFFSET,
	PRIORITY_OFFSET,
	ACK_TIMER_OFFSET,
	MAX_FRAME_SIZE_LSB_OFFSET,
	MAX_FRAME_SIZE_MSB_OFFSET,
	MAX_RETRANS_OFFSET,
	CREDITS_ISSUED_OFFSET
} PN_OFFSET_CONF;


typedef enum msc_param_offset {
	MSC_ADDRESS_FIELD_OFFSET,
	MSC_CONTROL_SIGNAL_OFFSET,
	MSC_BREAK_SIGNAL_OFFSET	
} MSC_PARAM_OFFSET;

#define MSC_BREAK_SIGNAL_LEN_MASK	0xF0
#define MSC_BREAK_SIGNAL_MASK		0xE

typedef enum rpn_param_offset {
	RPN_ADDRESS_FIELD_OFFSET,
	RPN_BAUD_RATE_FIELD_OFFSET,
	RPN_DATA_START_STOP_PARITY_FIELD_OFFSET,
	RPN_FLOW_CTRL_FIELD_OFFSET,
	RPN_HW_XON_FIELD_OFFSET,
	RPN_HW_XOFF_FIELD_OFFSET,
	RPN_PM_FIELD_LSB_OFFSET,
	RPN_PM_FIELD_MSB_OFFSET
} RPN_PARAM_OFFSET;

typedef enum nsc_param_offset {
	NSC_CMD_TYPE_OFFSET
} NSC_PARAM_OFFSET;

typedef enum baud_rate {
	B2400,
	B4800,
	B7200,
	B9600,
	B19200,
	B38400,
	B57600,
	B115200,
	B230400
} BAUD_RATE;

typedef enum data_bits {
	BITS5,
	BITS6,
	BITS7,
	BITS8
} DATA_BITS;

typedef enum rls_param_offset {
	RLS_OFFSET
} RLS_PARAM_OFFSET;

#define PM_BR_MASK 		(1<<0)
#define PM_DB_MASK 		(1<<1)
#define PM_SB_MASK 		(1<<2)
#define PM_P_MASK 		(1<<3)
#define PM_PT_MASK 		(1<<4)
#define PM_XON_MASK 	(1<<5)
#define PM_XOFF_MASK	(1<<6)

#define PM_XI_MASK		(1<<0)
#define PM_XO_MASK		(1<<1)
#define PM_RTRI_MASK	(1<<2)
#define PM_RTRO_MASK	(1<<3)
#define PM_RTCI_MASK	(1<<4)
#define PM_RTCO_MASK	(1<<5)

#define RLS_OVERRUN_ERROR	(1<<1)
#define RLS_PARITY_ERROR	(1<<2)
#define RLS_FRAMING_ERROR	(1<<3)


#define EA_ENABLE	0
#define EA_DISABLE	1

#define CMD_RESP_ENABLE	1

#define DIR_ENABLE	1

#define RFCOMM_ADDR_SIZE 	1
#define RFCOMM_CTRL_SIZE	1
#define RFCOMM_FCS_SIZE 	1

#define RFCOMM_MSG_TYPE_SIZE 	1
#define RFCOMM_MSG_LEN_SIZE		1

#define RFCOMM_HEADER_SIZE ((RFCOMM_ADDR_SIZE) + (RFCOMM_CTRL_SIZE))

#define RFCOMM_ADDR_OFFSET 		0
#define RFCOMM_CTRL_OFFSET 		((RFCOMM_ADDR_OFFSET) + (RFCOMM_ADDR_SIZE))
#define RFCOMM_LENGTH_OFFSET 	((RFCOMM_CTRL_OFFSET) + (RFCOMM_CTRL_SIZE))

#define EA_ADDR_LEN_MASK	(1<<0)
#define CMD_RESP_MASK		(1<<1)
#define DIR_MASK			(1<<2)

#define MSG_MASK			(0xFC)

#define SERVER_CH_ADDR_MASK	(0xF8)
#define POLL_FINAL_MASK		(1<<4)

#define POLL_FINAL_ENABLE	1
#define POLL_FINAL_DISABLE	0

#define CTRL_MASK			(0xEF)


#define DLCI_ADDR_MIN		0x02
#define	DLCI_ADDR_MAX		0x61

#define NORMAL_HEADER_LEN_SIZE		1
#define EXTENDED_HEADER_LEN_SIZE	2

#define	FCS_SIZE_SABM	3
#define FCS_SIZE_DISC	3
#define FCS_SIZE_UA		3
#define FCS_SIZE_DM		3
#define FCS_SIZE		3
#define FCS_SIZE_UIH	2


#define MSC_CTRL_SIG_EA_MASK	(1<<0)
#define MSC_CTRL_SIG_FC_MASK	(1<<1)
#define MSC_CTRL_SIG_RTC_MASK	(1<<2)
#define MSC_CTRL_SIG_RTR_MASK	(1<<3)
#define MSC_CTRL_SIG_RES1_MASK	(1<<4)
#define MSC_CTRL_SIG_RES2_MASK	(1<<5)
#define MSC_CTRL_SIG_IC_MASK	(1<<6)
#define MSC_CTRL_SIG_DV_MASK	(1<<7)


#define RPN_DATA_MASK			(0x03)
#define RPN_STOP_MASK			(1<<2)
#define RPN_PARITY_BIT_MASK		(1<<3)
#define RPN_PARITY_TYPE_MASK	((0x03) << 4)
#define RPN_FC_MASK				(0x3F)


typedef enum rpn_flow_control {
	RPN_XI_MASK		= (1<<0),
	RPN_XO_MASK		= (1<<1),
	RPN_RTRI_MASK	= (1<<2),
	RPN_RTRO_MASK	= (1<<3),
	RPN_RTCI_MASK	= (1<<4),
	RPN_RTCO_MASK	= (1<<5)
} RPN_FLOW_CONTROL;

#define MSG_CMD		1
#define MSG_RESP	0

#define RFCOMM_CREDIT_OFFSET	0

#define RFCOMM_CREDIT_SIZE		1

#define MSG_DLCI		0

#define RFCOMM_SABM_ADDR	0

/***********************MSG params***********************************/
#define RFCOMM_MAX_FRAME_SIZE	336
/*******************************************************************/

#define get_rfcomm_addr(rfcomm_pkt_buff)	\
	read8_buff_le(rfcomm_pkt_buff,RFCOMM_ADDR_OFFSET)

#define get_rfcomm_ctrl(rfcomm_pkt_buff) \
	read8_buff_le(rfcomm_pkt_buff,RFCOMM_CTRL_OFFSET)

#define get_rfcomm_payload_len_lsb(rfcomm_pkt_buff) \
	read8_buff_le(rfcomm_pkt_buff,RFCOMM_LENGTH_OFFSET)

#define get_rfcomm_payload_len_msb(rfcomm_pkt_buff) \
	read8_buff_le(rfcomm_pkt_buff,RFCOMM_LENGTH_OFFSET+1)

#define get_rfcomm_addr_ea_status(rfcomm_pkt_buff) \
	((get_rfcomm_addr(rfcomm_pkt_buff)) & (EA_ADDR_LEN_MASK))

#define get_rfcomm_cmd_resp_status(rfcomm_pkt_buff)	\
	((get_rfcomm_addr(rfcomm_pkt_buff) & (CMD_RESP_MASK)) >> 1)

#define get_rfcomm_dir_status(rfcomm_pkt_buff) \
	((get_rfcomm_addr(rfcomm_pkt_buff) & (DIR_MASK)) >> 2)

#define get_rfcomm_server_ch_addr(rfcomm_pkt_buff) \
	((get_rfcomm_addr(rfcomm_pkt_buff) & (SERVER_CH_ADDR_MASK)) >> 3)

#define get_rfcomm_control_field(rfcomm_pkt_buff) \
	((get_rfcomm_ctrl(rfcomm_pkt_buff)) & (CTRL_MASK))

#define get_rfcomm_len_ea_status(rfcomm_pkt_buff) \
	((get_rfcomm_payload_len_lsb(rfcomm_pkt_buff)) & (EA_ADDR_LEN_MASK))

#define get_control_field_poll_final_bit(rfcomm_pkt_buff)	\
	((get_rfcomm_ctrl(rfcomm_pkt_buff) & (POLL_FINAL_MASK)) >> 4)

/*#define get_rfcomm_fcs(rfcomm_pkt_buff)	\
	read8_buff_le(rfcomm_pkt_buff,RFCOMM_HEADER_SIZE\
				+ get_rfcomm_payload_len(rfcomm_pkt_buff)\
				+ get_rfcomm_len_size(rfcomm_pkt_buff))*/

#define get_rfcomm_payload_offset(rfcomm_pkt_buff)	\
		((RFCOMM_HEADER_SIZE) \
			+ (get_rfcomm_len_size(rfcomm_pkt_buff)))
			

/* 				 Address Field 				*/
/*
 *  +-------+----+-----+---+----+----+----+----+-----+
 *  |Bit No.| 1  |  2  | 3 | 4  |  5 |  6 |  7 |  8  |
 *  +-------+----+-----+---+----+----+----+----+-----+
 *  |RFCOMM | EA | C/R | D |   Server Channel        |
 *  +-------+----+-----+---+-------------------------+
 *
 */

//EA field is set to 1 for disable.
#define disable_ea_addr_field(rfcomm_pkt_buff) \
	write8_buff_le(rfcomm_pkt_buff,\
		RFCOMM_ADDR_OFFSET,\
		get_rfcomm_addr(rfcomm_pkt_buff) | (EA_DISABLE))

//EA field is set to 0 for enable.
#define enable_ea_addr_field(rfcomm_pkt_buff)	\
	write8_buff_le(rfcomm_pkt_buff,\
		RFCOMM_ADDR_OFFSET,\
		get_rfcomm_addr(rfcomm_pkt_buff) & ~(EA_DISABLE))
	
/*
 *
 * +-------------------+----------------------------------+-------------+
 * | Command/Response  |           Direction              |  C/R value  |
 * +-------------------+----------------------------------+-------------+ 
 * | 	Command        |     Initiator ---> Responder	  |     1	    |
 * |                   |     Responder ---> Initiator     |     0       |
 * +-------------------+----------------------------------+-------------+ 
 * | 	Response       |     Initiator ---> Responder     |     0       |
 * |                   |     Responder ---> Initiator     |     1       |
 * +-------------------+----------------------------------+-------------+
 *
 * An example of a command and response:
 * The 'command' SABM is sent. C/R bit is 1.
 * The 'response' is sent by the responder to the intiator using a UA pkt.
 * C/R bit is set to 1. Note that the C/R bit is set to 1 in this case too.
 * Only if a 'command' is again sent from the responder to the initiator
 * will the C/R bit be set to 0.
 *
 */
#define enable_cr_addr_field(rfcomm_pkt_buff) \
	write8_buff_le(rfcomm_pkt_buff,\
		RFCOMM_ADDR_OFFSET,\
		get_rfcomm_addr(rfcomm_pkt_buff) | (CMD_RESP_ENABLE<<1))

#define disable_cr_addr_field(rfcomm_pkt_buff)	\
	write8_buff_le(rfcomm_pkt_buff,\
		RFCOMM_ADDR_OFFSET,\
		get_rfcomm_addr(rfcomm_pkt_buff) & ~(CMD_RESP_ENABLE<<1))

/*
 * For RFCOMM the initiating device is given the direction bit D=1
 * and conversly D=0 for the other device.
 */
#define enable_dir_addr_field(rfcomm_pkt_buff) \
	write8_buff_le(rfcomm_pkt_buff,\
		RFCOMM_ADDR_OFFSET,	\
		get_rfcomm_addr(rfcomm_pkt_buff) | (DIR_ENABLE<<2))

#define disable_dir_addr_field(rfcomm_pkt_buff)	\
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_ADDR_OFFSET,	\
		get_rfcomm_addr(rfcomm_pkt_buff) & ~(DIR_ENABLE<<2))

#define set_rfcomm_server_channel(rfcomm_pkt_buff,channel)	\
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_ADDR_OFFSET,	\
		get_rfcomm_addr(rfcomm_pkt_buff) | (channel << 3))

#define set_rfcomm_ctrl_field(rfcomm_pkt_buff,ctrl)	\
	write8_buff_le(rfcomm_pkt_buff,RFCOMM_CTRL_OFFSET,ctrl)
		

#define set_rfcomm_poll_final_bit(rfcomm_pkt_buff)	\
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_CTRL_OFFSET,	\
		(get_rfcomm_control_field(rfcomm_pkt_buff) | (POLL_FINAL_MASK)))

#define clear_rfcomm_poll_final_bit(rfcomm_pkt_buff) \
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_CTRL_OFFSET,	\
		(get_rfcomm_control_field(rfcomm_pkt_buff) & ~(POLL_FINAL_MASK)))

//Set the ea bit to 0 for enable.
#define enable_rfcomm_len_ea(rfcomm_pkt_buff)	\
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_LENGTH_OFFSET,	\
		get_rfcomm_payload_len_lsb(rfcomm_pkt_buff) & ~(EA_DISABLE))

//Set the ea bit to 1 for disable.
#define disable_rfcomm_len_ea(rfcomm_pkt_buff)	\
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_LENGTH_OFFSET,	\
		get_rfcomm_payload_len_lsb(rfcomm_pkt_buff) | (EA_DISABLE))

#define set_rfcomm_len_field_lsb(rfcomm_pkt_buff,len) \
	write8_buff_le(rfcomm_pkt_buff,	\
		RFCOMM_LENGTH_OFFSET,	\
		(len<<1))

#define set_rfcomm_len_field_msb(rfcomm_pkt_buff,len)	\
	write8_buff_le(rfcomm_pkt_buff,\
		RFCOMM_LENGTH_OFFSET+1,	\
		len)

/*
#define set_rfcomm_fcs(rfcomm_pkt_buff,fields)	\
	write8_buff_le(rfcomm_pkt_buff,\
				get_rfcomm_payload_len(rfcomm_pkt_buff) + \
				get_rfcomm_len_size(rfcomm_pkt_buff) + \
				RFCOMM_HEADER_SIZE, \
				create_fcs(rfcomm_pkt_buff,fields))
*/

/*
#define get_rfcomm_pkt_size(rfcomm_pkt_buff)	\
		((RFCOMM_HEADER_SIZE) + \
		(get_rfcomm_payload_len(rfcomm_pkt_buff)) +\
		(get_rfcomm_len_size(rfcomm_pkt_buff)) +\
		(RFCOMM_FCS_SIZE))
*/

#define dump_rfcomm_pkt(rfcomm_pkt_buff)	\
	dump_pkt(rfcomm_pkt_buff,0,	\
		(RFCOMM_HEADER_SIZE) + \
		(get_rfcomm_payload_len(rfcomm_pkt_buff) + \
		get_rfcomm_len_size(rfcomm_pkt_buff)+RFCOMM_FCS_SIZE+1))
	
#define dump_rfcomm_payload(rfcomm_pkt_buff)	\
	dump_pkt(rfcomm_pkt_buff,\
		get_rfcomm_payload_offset(rfcomm_pkt_buff),\
		(get_rfcomm_payload_len(rfcomm_pkt_buff)))

#define get_rfcomm_msg_offset(rfcomm_pkt_buff)	\
		(get_rfcomm_payload_offset(rfcomm_pkt_buff))

#define get_rfcomm_msg_type(rfcomm_pkt_buff)	\
	((read8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff))) & (MSG_MASK))

#define get_rfcomm_msg_type_ea(rfcomm_pkt_buff) \
	(read8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff)) & (EA_ADDR_LEN_MASK))

#define get_rfcomm_msg_type_cr(rfcomm_pkt_buff) \
	((read8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff)) & CMD_RESP_MASK) >> 1)

#define set_rfcomm_msg_type(rfcomm_pkt_buff,msg_type)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff),\
		((get_rfcomm_msg_type(rfcomm_pkt_buff) \
		& (EA_ADDR_LEN_MASK|CMD_RESP_MASK)) | msg_type)))

/* EA Mods */
//EA Field is set to 1 for disable.
#define disable_rfcomm_msg_type_ea(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff),\
		(read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_offset(rfcomm_pkt_buff)) | EA_ADDR_LEN_MASK)))
	
#define enable_rfcomm_msg_type_ea(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff),\
		(read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_offset(rfcomm_pkt_buff)) & ~EA_ADDR_LEN_MASK)))

#define enable_rfcomm_msg_type_cr(rfcomm_pkt_buff) \
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff),\
		(read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_offset(rfcomm_pkt_buff)) | CMD_RESP_MASK)))

#define disable_rfcomm_msg_type_cr(rfcomm_pkt_buff) \
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff),\
		(read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_offset(rfcomm_pkt_buff)) & ~CMD_RESP_MASK)))

#define disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff) + 1,\
		(read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_offset(rfcomm_pkt_buff) + 1) | EA_ADDR_LEN_MASK)))

#define get_rfcomm_msg_type_len(rfcomm_pkt_buff)	\
	((read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_offset(rfcomm_pkt_buff) + 1))>>1)

/*
#define set_rfcomm_msg_type_len(rfcomm_pkt_buff,len)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff) + 1,\
		len<<1))*/
			
#define set_rfcomm_msg_type_len(rfcomm_pkt_buff,len)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff) + 1,\
		(read8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff + 1)) \
								& EA_ADDR_LEN_MASK) \
								| (len<<1)))

#define get_rfcomm_msg_type_lsb(rfcomm_pkt_buff) \
	(read8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff))

#define get_rfcomm_msg_type_msb(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		get_rfcomm_msg_offset(rfcomm_pkt_buff) + 1 ))

//TODO: Tentative. Have to check extension field to create the length.
#define get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)	\
		(get_rfcomm_msg_offset(rfcomm_pkt_buff) + 2)

#define get_rfcomm_msg_payload_uih_param(rfcomm_pkt_buff,param_num)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + param_num)))

	
#define get_rfcomm_msg_dlci_conf(config)	\
			(config[DLCI_OFFSET])

#define get_rfcomm_msg_uih_frame_conf(config)	\
			((config[CL_AND_FRAME_OFFSET]) & 0xF)

#define get_rfcomm_msg_credit_conf(config)	\
			(((config[CL_AND_FRAME_OFFSET]) & 0xF0)>>4)

#define get_rfcomm_msg_priority_conf(config)	\
			(config[PRIORITY_OFFSET])

#define get_rfcomm_msg_timer_ack_conf(config)	\
			(config[ACK_TIMER_OFFSET])

#define get_rfcomm_msg_max_frame_size_conf(config)\
			((config[MAX_FRAME_SIZE_MSB_OFFSET]<<8)|	\
				(config[MAX_FRAME_SIZE_LSB_OFFSET]))

#define get_rfcomm_msg_max_frame_retrans_conf(config)	\
			(config[MAX_RETRANS_OFFSET])

#define get_rfcomm_msg_credits_issued_conf(config)	\
			(config[CREDITS_ISSUED_SIZE_OFFSET])

#define set_rfcomm_msg_dlci_conf(config,dlci) \
			((config[DLCI_OFFSET]) = (dlci))

#define set_rfcomm_msg_uih_frame_conf(config,uih)	\
			((config[CL_AND_FRAME_OFFSET]) = \
				((config[CL_AND_FRAME_OFFSET]) & 0xF0)|(uih))

#define set_rfcomm_msg_credit_conf(config,credit)	\
			((config[CL_AND_FRAME_OFFSET]) = \
				((config[CL_AND_FRAME_OFFSET]) & 0x0F)|((credit)<<4))

#define set_rfcomm_msg_priority_conf(config,priority) \
			((config[PRIORITY_OFFSET]) = (priority))

#define set_rfcomm_msg_timer_ack_conf(config,timer) \
			((config[ACK_TIMER_OFFSET]) = (timer))

#define set_rfcomm_msg_max_frame_size_conf(config,frame_size) \
			((config[MAX_FRAME_SIZE_MSB_OFFSET]) = ((frame_size & 0xF0)>>8), \
			(config[MAX_FRAME_SIZE_LSB_OFFSET]) = (frame_size & 0x0F))

#define set_rfcomm_msg_max_frame_retrans_conf(config,retrans_size)	\
			((config[MAX_RETRANS_OFFSET]) = (retrans_size))


#define set_rfcomm_msg_credits_issued_conf(config,credits_issued)	\
			((config[CREDITS_ISSUED_OFFSET]) = (credits_issued))


#define is_valid_pn(config)	\
	(((get_rfcomm_msg_uih_frame_conf(config)) == (USE_UIH)) && \
		((get_rfcomm_msg_timer_ack_conf(config)) == (0)) && \
		((get_rfcomm_msg_max_frame_retrans_conf(config)) == (0)) && \
		((get_rfcomm_msg_credit_conf(config)) == (0xF)))


#define get_rfcomm_msg_dlci_conf_pkt(rfcomm_pkt_buff)	\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + DLCI_OFFSET)))

#define get_rfcomm_msg_uih_frame_conf_pkt(rfcomm_pkt_buff)	\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + CL_AND_FRAME_OFFSET)) & 0x0F)

#define get_rfcomm_msg_credit_conf_pkt(rfcomm_pkt_buff)	\
		((read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + CL_AND_FRAME_OFFSET)) & 0xF0) >> 4)


#define get_rfcomm_msg_priority_conf_pkt(rfcomm_pkt_buff)	\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + PRIORITY_OFFSET)))

#define get_rfcomm_msg_timer_ack_conf_pkt(rfcomm_pkt_buff) \
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + ACK_TIMER_OFFSET)))

#define get_rfcomm_msg_max_frame_size_conf_pkt(rfcomm_pkt_buff)	\
		((read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MAX_FRAME_SIZE_LSB_OFFSET)) |\
			((read8_buff_le(rfcomm_pkt_buff,\
				get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MAX_FRAME_SIZE_MSB_OFFSET))<<8))

#define get_rfcomm_msg_max_frame_retrans_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MAX_RETRANS_OFFSET)))
		

#define get_rfcomm_msg_msc_dlci_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))>>2)

#define get_rfcomm_msg_msc_dlci_ea_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET)) & \
				EA_ADDR_LEN_MASK)

/*#define get_rfcomm_msg_msc_dlci_cr_conf_pkt(rfcomm_pkt_buff)	\
	((read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET)) &	\
			(CMD_RESP_MASK)) >> 1) */

#define get_rfcomm_msg_msc_ctrl_sig_offset(rfcomm_pkt_buff)	\
	(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET)

#define get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_msc_ctrl_sig_offset(rfcomm_pkt_buff))))

#define get_rfcomm_msg_msc_break_sig_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET)))

#define get_rfcomm_msg_msc_ctrl_sig_ea(rfcomm_pkt_buff)	\
	(get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff) & MSC_CTRL_SIG_EA_MASK)

#define get_rfcomm_msg_msc_ctrl_sig_fc(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff) & MSC_CTRL_SIG_FC_MASK) >> 1)

#define get_rfcomm_msg_msc_ctrl_sig_rtc(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff) & MSC_CTRL_SIG_RTC_MASK) >> 2)

#define get_rfcomm_msg_msc_ctrl_sig_rtr(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff) & MSC_CTRL_SIG_RTR_MASK) >> 3)

#define get_rfcomm_msg_msc_ctrl_sig_ic(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff) & MSC_CTRL_SIG_IC_MASK) >> 6)

#define get_rfcomm_msg_msc_ctrl_sig_dv(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_msc_ctrl_sig_conf_pkt(rfcomm_pkt_buff) & MSC_CTRL_SIG_DV_MASK) >> 7)

#define enable_rfcomm_msg_msc_ctrl_sig_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET))& \
				~(EA_ADDR_LEN_MASK))))

#define disable_rfcomm_msg_msc_ctrl_sig_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET))| \
				(EA_ADDR_LEN_MASK))))


#define set_rfcomm_msg_msc_ctrl_sig(rfcomm_pkt_buff,sig)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_CONTROL_SIGNAL_OFFSET))& \
				(EA_ADDR_LEN_MASK))|(sig)))

#define set_rfcomm_msg_msc_dlci_conf_pkt(rfcomm_pkt_buff,dlci)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))& \
				(CMD_RESP_MASK|EA_ADDR_LEN_MASK))|(dlci<<2)))

#define enable_rfcomm_msg_msc_dlci_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))& \
				~(EA_ADDR_LEN_MASK))))

#define enable_rfcomm_msg_msc_dlci_cr(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))| \
				(CMD_RESP_MASK))))

#define disable_rfcomm_msg_msc_dlci_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))| \
				(EA_ADDR_LEN_MASK))))

#define enable_rfcomm_msg_msc_break_sig_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET))& \
				~(EA_ADDR_LEN_MASK))))

#define disable_rfcomm_msg_msc_break_sig_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET))| \
				(EA_ADDR_LEN_MASK))))

#define set_rfcomm_msg_msc_break_signal_conf_pkt(rfcomm_pkt_buff,break_signal)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET))& \
				(EA_ADDR_LEN_MASK|MSC_BREAK_SIGNAL_LEN_MASK))|(break_signal<<1)))
		
#define set_rfcomm_msg_msc_break_signal_len_conf_pkt(rfcomm_pkt_buff,signal_len)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_BREAK_SIGNAL_OFFSET))& \
				(EA_ADDR_LEN_MASK|MSC_BREAK_SIGNAL_MASK))|(signal_len<<4)))

/*
#define enable_rfcomm_msg_msc_dlci_cr_conf_pkt(rfcomm_pkt_buff) \
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))| \
				(CMD_RESP_MASK))))
	

#define disable_rfcomm_msg_msc_dlci_cr_conf_pkt(rfcomm_pkt_buff) \
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MSC_ADDRESS_FIELD_OFFSET))& \
				~(CMD_RESP_MASK))))
*/


#define set_rfcomm_msg_dlci_conf_pkt(rfcomm_pkt_buff,dlci)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + DLCI_OFFSET),\
		dlci))

#define set_rfcomm_msg_uih_credit_frame_conf_pkt(rfcomm_pkt_buff,uih_credit)	\
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+CL_AND_FRAME_OFFSET),\
			(uih_credit)))

#define set_rfcomm_msg_uih_frame_conf_pkt(rfcomm_pkt_buff,uih)	\
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+CL_AND_FRAME_OFFSET),\
			((read8_buff_le(rfcomm_pkt_buff,\
			get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+CL_AND_FRAME_OFFSET) & 0xF0)|uih)))

#define set_rfcomm_msg_credit_conf_pkt(rfcomm_pkt_buff,credit)	\
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+CL_AND_FRAME_OFFSET),\
			(read8_buff_le(rfcomm_pkt_buff,\
				get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+CL_AND_FRAME_OFFSET) & 0x0F)|\
												(credit<<4)))


#define set_rfcomm_msg_priority_conf_pkt(rfcomm_pkt_buff,priority) \
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+PRIORITY_OFFSET),\
			(priority)))

#define set_rfcomm_msg_timer_ack_conf_pkt(rfcomm_pkt_buff,timer)	\
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)+ACK_TIMER_OFFSET),\
			(timer)))

#define set_rfcomm_msg_max_frame_size_conf_lsb_pkt(rfcomm_pkt_buff,framesize) \
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MAX_FRAME_SIZE_LSB_OFFSET),\
			(framesize & 0xFF)))

#define set_rfcomm_msg_max_frame_size_conf_msb_pkt(rfcomm_pkt_buff,framesize) \
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MAX_FRAME_SIZE_MSB_OFFSET),\
			(framesize) >> 8))

#define set_rfcomm_msg_max_frame_size_conf_pkt(rfcomm_pkt_buff,framesize) \
		set_rfcomm_msg_max_frame_size_conf_lsb_pkt(rfcomm_pkt_buff,framesize);\
		set_rfcomm_msg_max_frame_size_conf_msb_pkt(rfcomm_pkt_buff,framesize)


#define set_rfcomm_msg_max_frame_retrans_conf_pkt(rfcomm_pkt_buff,retrans_size)	\
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + MAX_RETRANS_OFFSET),\
			(retrans_size)))


#define set_rfcomm_msg_credits_issued_conf_pkt(rfcomm_pkt_buff,credits_issued)	\
		(write8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + CREDITS_ISSUED_OFFSET),\
			(credits_issued)))

#define get_rfcomm_msg_credits_issued_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + CREDITS_ISSUED_OFFSET)))


#define get_rfcomm_msg_rpn_dlci_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET))>>2)

#define get_rfcomm_msg_rpn_dlci_ea_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET)) & \
				EA_ADDR_LEN_MASK)

#define set_rfcomm_msg_rpn_dlci_conf_pkt(rfcomm_pkt_buff,dlci)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET))& \
				(CMD_RESP_MASK|EA_ADDR_LEN_MASK))|(dlci<<2)))

#define enable_rfcomm_msg_rpn_dlci_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET))& \
				~(EA_ADDR_LEN_MASK))))

#define disable_rfcomm_msg_rpn_dlci_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET))| \
				(EA_ADDR_LEN_MASK))))

#define enable_rfcomm_msg_rpn_dlci_cr(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_ADDRESS_FIELD_OFFSET))| \
				(CMD_RESP_MASK))))

#define get_rfcomm_msg_rpn_baud_rate_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_BAUD_RATE_FIELD_OFFSET)))

#define set_rfcomm_msg_rpn_baud_rate_conf_pkt(rfcomm_pkt_buff,baudrate)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_BAUD_RATE_FIELD_OFFSET),\
		baudrate))

#define get_rfcomm_msg_rpn_data_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
		+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET))	& RPN_DATA_MASK)

#define set_rfcomm_msg_rpn_data_conf_pkt(rfcomm_pkt_buff,data_bits)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET),\
		((read8_buff_le(rfcomm_pkt_buff,(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)\
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & (~RPN_DATA_MASK)) | (data_bits))\
			))

#define get_rfcomm_msg_rpn_stop_bit_conf_pkt(rfcomm_pkt_buff)	\
	((read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
		+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & RPN_STOP_MASK) >> 2)

#define set_rfcomm_msg_rpn_stop_bit_conf_pkt(rfcomm_pkt_buff,stop_bit)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET),\
		((read8_buff_le(rfcomm_pkt_buff,(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)\
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & (~RPN_STOP_MASK)) | (stop_bit<<2))\
			))

#define get_rfcomm_msg_rpn_parity_bit_conf_pkt(rfcomm_pkt_buff)	\
	((read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & RPN_PARITY_BIT_MASK) >> 3)

#define set_rfcomm_msg_rpn_parity_bit_conf_pkt(rfcomm_pkt_buff,parity_bit)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET),\
		((read8_buff_le(rfcomm_pkt_buff,(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)\
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & (~RPN_PARITY_BIT_MASK)) | (parity_bit<<3))\
			))

#define get_rfcomm_msg_rpn_parity_type_conf_pkt(rfcomm_pkt_buff) \
	((read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & RPN_PARITY_TYPE_MASK) >> 4)

#define set_rfcomm_msg_rpn_parity_type_conf_pkt(rfcomm_pkt_buff,parity_type)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET),\
		((read8_buff_le(rfcomm_pkt_buff,(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)\
			+ RPN_DATA_START_STOP_PARITY_FIELD_OFFSET)) & (~RPN_PARITY_TYPE_MASK)) | (parity_type<<4))\
			))

#define get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff) \
	(read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_FLOW_CTRL_FIELD_OFFSET)))

#define set_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff,flow_control) \
	write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_FLOW_CTRL_FIELD_OFFSET),\
		(flow_control))


#define get_rfcomm_msg_rpn_xi_conf_pkt(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff)) \
											& RPN_XI_MASK)

#define get_rfcomm_msg_rpn_xo_conf_pkt(rfcomm_pkt_buff)	\
	(((get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff)) \
											& RPN_XO_MASK)>>1)

#define get_rfcomm_msg_rpn_rtri_conf_pkt(rfcomm_pkt_buff)	\
	(((get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff)) \
											& RPN_RTRI_MASK)>>2)

#define get_rfcomm_msg_rpn_rtro_conf_pkt(rfcomm_pkt_buff)	\
	(((get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff)) \
											& RPN_RTRO_MASK)>>3)

#define get_rfcomm_msg_rpn_rtci_conf_pkt(rfcomm_pkt_buff)	\
	(((get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff)) \
											& RPN_RTCI_MASK)>>4)

#define get_rfcomm_msg_rpn_rtco_conf_pkt(rfcomm_pkt_buff)	\
	(((get_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff)) \
											& RPN_RTCO_MASK)>>5)

#define get_rfcomm_msg_rpn_hw_xon_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_HW_XON_FIELD_OFFSET)))

#define set_rfcomm_msg_rpn_hw_xon_conf_pkt(rfcomm_pkt_buff,xon) \
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_HW_XON_FIELD_OFFSET),\
		(xon)))

#define get_rfcomm_msg_rpn_hw_xoff_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) \
			+ RPN_HW_XOFF_FIELD_OFFSET)))

#define set_rfcomm_msg_rpn_hw_xoff_conf_pkt(rfcomm_pkt_buff,xoff) \
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + RPN_HW_XOFF_FIELD_OFFSET),\
		(xoff)))

#define get_rfcomm_msg_pm_lsb_offset(rfcomm_pkt_buff)	\
		((get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)) + \
									(RPN_PM_FIELD_LSB_OFFSET))

#define get_rfcomm_msg_pm_msb_offset(rfcomm_pkt_buff)	\
		((get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)) + \
									(RPN_PM_FIELD_MSB_OFFSET))

#define get_rfcomm_msg_rpn_pm_lsb_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_pm_lsb_offset(rfcomm_pkt_buff))))
			

#define get_rfcomm_msg_rpn_pm_msb_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff, \
		(get_rfcomm_msg_pm_msb_offset(rfcomm_pkt_buff))))
			

#define get_rfcomm_msg_rpn_pm_conf_pkt(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_rpn_pm_msb_conf_pkt(rfcomm_pkt_buff) << 8) |\
		(get_rfcomm_msg_rpn_pm_lsb_conf_pkt(rfcomm_pkt_buff)))


#define set_rfcomm_msg_rpn_pm_lsb_conf_pkt(rfcomm_pkt_buff,opt)	\
		write8_buff_le(rfcomm_pkt_buff,\
					get_rfcomm_msg_pm_lsb_offset(rfcomm_pkt_buff),\
					opt)
					
#define set_rfcomm_msg_rpn_pm_msb_conf_pkt(rfcomm_pkt_buff,opt)	\
		write8_buff_le(rfcomm_pkt_buff,\
					get_rfcomm_msg_pm_msb_offset(rfcomm_pkt_buff),\
					opt)

#define set_rfcomm_msg_pm_conf_pkt(rfcomm_pkt_buff,opt)	\
		set_rfcomm_msg_pm_lsb_conf_pkt(rfcomm_pkt_buff,(opt & 0xFF));\
		set_rfcomm_msg_pm_msb_conf_pkt(rfcomm_pkt_buff,((opt & 0xFF00)>>8))


#define get_rfcomm_msg_rls_conf_pkt(rfcomm_pkt_buff)	\
	(read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff)\
			+ RLS_OFFSET)))

#define get_rfcomm_msg_rls_oe_status(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_rls_conf_pkt(rfcomm_pkt_buff) & RLS_OVERRUN_ERROR) >> 1)

#define get_rfcomm_msg_rls_pe_status(rfcomm_pkt_buff)	\
	((get_rfcomm_msg_rls_conf_pkt(rfcomm_pkt_buff) & RLS_PARITY_ERROR) >> 2)

#define get_rfcomm_msg_rls_fe_status(rfcomm_pkt_buff) \
	((get_rfcomm_msg_rls_conf_pkt(rfcomm_pkt_buff) & RLS_FRAMING_ERROR) >> 3)
	
#define enable_rfcomm_msg_nsc_cmd_type_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET))& \
				~(EA_ADDR_LEN_MASK))))

#define disable_rfcomm_msg_nsc_cmd_type_ea_conf_pkt(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET))| \
				(EA_ADDR_LEN_MASK))))

#define set_rfcomm_msg_msc_nsc_cmd_type_pkt(rfcomm_pkt_buff,cmd_type)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET))& \
				(CMD_RESP_MASK|EA_ADDR_LEN_MASK))|(cmd_type<<2)))

#define enable_rfcomm_msg_nsc_cmd_type_cr(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET))| \
				(CMD_RESP_MASK))))

#define disable_rfcomm_msg_nsc_cmd_type_cr(rfcomm_pkt_buff)	\
	(write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET),\
		(read8_buff_le(rfcomm_pkt_buff,\
			(get_rfcomm_msg_payload_offset(rfcomm_pkt_buff) + NSC_CMD_TYPE_OFFSET))& \
				~(CMD_RESP_MASK))))

#define get_rfcomm_credits(rfcomm_pkt_buff)	\
	read8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_payload_offset(rfcomm_pkt_buff) + RFCOMM_CREDIT_OFFSET))

#define set_rfcomm_credits(rfcomm_pkt_buff,credits) \
	write8_buff_le(rfcomm_pkt_buff,\
		(get_rfcomm_payload_offset(rfcomm_pkt_buff) + RFCOMM_CREDIT_OFFSET),\
		credits)


void process_rfcomm_pkt(
					uint8_t *rfcomm_pkt_buff,
					uint16_t pkt_len
					);


uint8_t verify_fcs(uint8_t *rfcomm_pkt_buff,
					uint8_t len,
					uint8_t recvd_fcs);
	
uint8_t create_fcs(uint8_t *rfcomm_pkt_buff,uint8_t len);

void create_sabm_pkt(uint8_t *rfcomm_pkt_buff);
void create_ua_pkt(uint8_t chaddr,uint8_t *rfcomm_pkt_buff);
void create_uih_pkt(uint8_t *rfcomm_pkt_buff,
					uint8_t chaddr,
					uint16_t len,
					uint8_t pfbit,
					uint8_t cmdresp);

uint8_t get_rfcomm_len_size(uint8_t *rfcomm_pkt_buff);
uint8_t get_rfcomm_payload_len(uint8_t *rfcomm_pkt_buff);
void dump_pkt(uint8_t *rfcomm_pkt_buff,uint8_t start_index,uint8_t len);
uint8_t get_rfcomm_fcs(uint8_t *rfcomm_pkt_buff);
void create_credit_pkt(uint8_t *rfcomm_pkt_buff,
					uint8_t chaddr,
					uint16_t len,
					uint8_t cmdresp,
					uint8_t credits
					);

void create_msc_msg(
					uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp,
					uint8_t control_signal
					);

void create_rfcomm_pkt(
					uint8_t *rfcomm_pkt_buff,
					uint8_t chaddr,
					uint16_t len,
					uint8_t pfbit,
					uint8_t cmdresp,
					RFCOMM_FRAME_TYPE rfcomm_frame_type
					);

void set_rfcomm_fcs(uint8_t *rfcomm_pkt_buff);

uint8_t get_rfcomm_pkt_size(uint8_t *rfcomm_pkt_buff);


#endif
