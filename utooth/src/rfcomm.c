#include <stdio.h>

#include "rfcomm.h"
#include "l2cap.h"
#include "hal_board.h"
#include "hal_usb.h"

const unsigned char crctable[256] = {
	 //reversed, 8-bit, poly=0x07
	 0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98,
	 0xEA, 0x7B, 0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 
	 0x15, 0x84, 0xF6, 0x67, 0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 
	 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43, 0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 
	 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F, 0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6,
	 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B, 0x6C, 0xFD, 0x8F, 0x1E, 
	 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17, 0x48, 0xD9, 
	 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33, 
	 0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 
	 0xBE, 0x2F, 0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 
	 0xE9, 0x78, 0x0A, 0x9B, 0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63,
	 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87, 0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
	 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3, 0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 
	 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF, 0x90, 0x01, 0x73, 0xE2, 
	 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB, 0x8C, 0x1D, 
	 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7, 
	 0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 
	 0x42, 0xD3, 0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 
	 0xBD, 0x2C, 0x5E, 0xCF
};



void rfcomm_init(struct rfcomm_info *rfcomm_info) {
	rfcomm_info->credit_cnt = MAX_CREDIT_CNT;
	init_rfcomm_state(rfcomm_info->rfcomm_state);
	init_rfcomm_transition(rfcomm_info->rfcomm_state_transition);


	/* Initialize pn */
	set_rfcomm_msg_pn_dlci_conf(rfcomm_info->rfcomm_conf_opt.pn_config,20);//Use a dlci generator.
	set_rfcomm_msg_pn_uih_frame_conf(rfcomm_info->rfcomm_conf_opt.pn_config,0);
	set_rfcomm_msg_pn_credit_support_conf(rfcomm_info->rfcomm_conf_opt.pn_config,0xF);
	set_rfcomm_msg_pn_priority_conf(rfcomm_info->rfcomm_conf_opt.pn_config,7);

	set_rfcomm_msg_pn_timer_ack_conf(rfcomm_info->rfcomm_conf_opt.pn_config,0);
	set_rfcomm_msg_pn_max_frame_size_conf(rfcomm_info->rfcomm_conf_opt.pn_config,336);
	set_rfcomm_msg_pn_max_frame_retrans_conf(rfcomm_info->rfcomm_conf_opt.pn_config,0);
	set_rfcomm_msg_pn_credits_issued_conf(rfcomm_info->rfcomm_conf_opt.pn_config,7);


	/* Initialize rpn  */


}

uint8_t verify_fcs(uint8_t *rfcomm_pkt_buff,uint8_t len,uint8_t recvd_fcs) {
	uint8_t FCS = 0xFF;

	while(len--) {
		FCS = crctable[FCS^(*rfcomm_pkt_buff)];
		rfcomm_pkt_buff++;
	}
	
	FCS = crctable[FCS^recvd_fcs];

	if(FCS == 0xCF)
		return FCS_PASS;
	else
		return FCS_FAIL;
}

uint8_t create_fcs(uint8_t *rfcomm_pkt_buff,uint8_t len) {
	uint8_t FCS = 0xFF;

	while(len--) {
		FCS = crctable[FCS^(*rfcomm_pkt_buff)];
		rfcomm_pkt_buff++;
	}

	FCS = 0xFF - FCS;

	return FCS;
}


void dump_pkt(uint8_t *rfcomm_pkt_buff,
				uint8_t start_index,
				uint8_t len) {
	uint8_t i = 0;
	char tmpbuff[6];

	rfcomm_pkt_buff += start_index;

	for(i = 0;i<len;i++) {
		sprintf(tmpbuff,"%02x ",rfcomm_pkt_buff[i]);
		halUsbSendStr(tmpbuff);
	}

	halUsbSendChar('\n');
}

void dump_pkt_alpha(uint8_t *rfcomm_pkt_buff,
				uint8_t start_index,
				uint8_t len) {
	uint8_t i = 0;
	rfcomm_pkt_buff += start_index;

//	halUsbSendStr("pkt_alpha\n");
	for(i = 0;i<len;i++) 
		halUsbSendChar(rfcomm_pkt_buff[i]);
	

	//halUsbSendChar('\n');
}

void dump_data_payload(uint8_t *rfcomm_pkt_buff) {
	dump_pkt_alpha(rfcomm_pkt_buff,
		get_rfcomm_payload_offset(rfcomm_pkt_buff),
		get_rfcomm_payload_len(rfcomm_pkt_buff));
}

uint8_t get_rfcomm_payload_len(uint8_t *rfcomm_pkt_buff) {

	if(get_rfcomm_len_ea_status(rfcomm_pkt_buff) == EA_ENABLE) {
		return ((get_rfcomm_payload_len_lsb(rfcomm_pkt_buff) >> 1) | 
					(get_rfcomm_payload_len_msb(rfcomm_pkt_buff) << 7));
	} else {
		return (get_rfcomm_payload_len_lsb(rfcomm_pkt_buff) >> 1);
	}
}

//TODO: Adjust length size based on the length extension field.
uint8_t get_rfcomm_fcs(uint8_t *rfcomm_pkt_buff) {
	if(get_rfcomm_control_field(rfcomm_pkt_buff) == UIH)
		return read8_buff_le(rfcomm_pkt_buff,RFCOMM_HEADER_SIZE 
			+ get_rfcomm_payload_len(rfcomm_pkt_buff)
			+ get_rfcomm_len_size(rfcomm_pkt_buff)
			+ get_control_field_poll_final_bit(rfcomm_pkt_buff));
	else
		return read8_buff_le(rfcomm_pkt_buff,RFCOMM_HEADER_SIZE 
			+ get_rfcomm_payload_len(rfcomm_pkt_buff)
			+ get_rfcomm_len_size(rfcomm_pkt_buff));
			
}

uint8_t get_rfcomm_len_size(uint8_t *rfcomm_pkt_buff) {
	if(get_rfcomm_len_ea_status(rfcomm_pkt_buff) == EA_ENABLE)
		return EXTENDED_HEADER_LEN_SIZE;
	else
		return NORMAL_HEADER_LEN_SIZE;
}

/*
 * SABM pkt
 * --------
 * The addr or the dlci is equal to 0.
 * The station wishing to establish a DLC transmits a SABM frame with the P-bit
 * set to 1. The address field contains the DLCI value associated with the desired 
 * connection. If the responding station is ready to establish the connection it 
 * will reply with a UA frame with the F-bit set to 1. If the responding station 
 * is not ready or unwilling to establish the particular DLC it will reply with a 
 * DM frame with the F-bit set to 1.
 */

/*
void create_sabm_pkt(uint8_t *rfcomm_pkt_buff) {
	disable_ea_addr_field(rfcomm_pkt_buff);
	
	enable_cr_addr_field(rfcomm_pkt_buff);
	disable_dir_addr_field(rfcomm_pkt_buff);

	set_rfcomm_server_channel(rfcomm_pkt_buff,RFCOMM_SABM_ADDR);
	set_rfcomm_ctrl_field(rfcomm_pkt_buff,SABM);
	set_rfcomm_poll_final_bit(rfcomm_pkt_buff);
	
	set_rfcomm_len_field_lsb(rfcomm_pkt_buff,0);
	disable_rfcomm_len_ea(rfcomm_pkt_buff);
	set_rfcomm_fcs(rfcomm_pkt_buff);
}*/

/* 
 * Send UA (Unnumbered acknowledgement) packet to acknowledge the
 * receipt and acceptance of SABM and DISC commands
 */

/*
void create_ua_pkt(uint8_t chaddr,uint8_t *rfcomm_pkt_buff) {
	disable_ea_addr_field(rfcomm_pkt_buff);

	enable_cr_addr_field(rfcomm_pkt_buff);
	disable_dir_addr_field(rfcomm_pkt_buff);
	set_rfcomm_server_channel(rfcomm_pkt_buff,chaddr);

	set_rfcomm_ctrl_field(rfcomm_pkt_buff,UA);
	set_rfcomm_poll_final_bit(rfcomm_pkt_buff);

	set_rfcomm_len_field_lsb(rfcomm_pkt_buff,0);
	disable_rfcomm_len_ea(rfcomm_pkt_buff);

	set_rfcomm_fcs(rfcomm_pkt_buff);
}*/


void create_uih_pkt(uint8_t *rfcomm_pkt_buff,
						uint8_t chaddr,
						uint16_t len,
						uint8_t pfbit,
						uint8_t cmdresp) {

	disable_ea_addr_field(rfcomm_pkt_buff);
	
	if(cmdresp == MSG_CMD)
		enable_cr_addr_field(rfcomm_pkt_buff);
	else
		disable_cr_addr_field(rfcomm_pkt_buff);

	disable_dir_addr_field(rfcomm_pkt_buff);
	set_rfcomm_server_channel(rfcomm_pkt_buff,chaddr);

	set_rfcomm_ctrl_field(rfcomm_pkt_buff,UIH);

	if(pfbit)
		set_rfcomm_poll_final_bit(rfcomm_pkt_buff);
	else
		clear_rfcomm_poll_final_bit(rfcomm_pkt_buff);

	
	set_rfcomm_len_field_lsb(rfcomm_pkt_buff,len);
	disable_rfcomm_len_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD)
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	else
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);

	set_rfcomm_fcs(rfcomm_pkt_buff);
}


void create_msc_msg(
					uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp,
					uint8_t control_signal
					) {

	
	set_rfcomm_msg_type(rfcomm_pkt_buff,MSC);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}

	set_rfcomm_msg_type_len(rfcomm_pkt_buff,2);

	set_rfcomm_msg_msc_dlci_conf_pkt(rfcomm_pkt_buff,20);
	disable_rfcomm_msg_msc_dlci_ea_conf_pkt(rfcomm_pkt_buff);

 	enable_rfcomm_msg_msc_dlci_cr(rfcomm_pkt_buff);

	/* Control signal */
	set_rfcomm_msg_msc_ctrl_sig(rfcomm_pkt_buff,control_signal);
	disable_rfcomm_msg_msc_ctrl_sig_ea_conf_pkt(rfcomm_pkt_buff);
	
	/* Break signal */
	enable_rfcomm_msg_msc_break_sig_ea_conf_pkt(rfcomm_pkt_buff);
	set_rfcomm_msg_msc_break_signal_conf_pkt(rfcomm_pkt_buff,0);
	set_rfcomm_msg_msc_break_signal_len_conf_pkt(rfcomm_pkt_buff,0);

}


void create_pn_msg(
				uint8_t *rfcomm_pkt_buff,
				uint8_t cmdresp,
				uint8_t *pn_config
				) {

	set_rfcomm_msg_type(rfcomm_pkt_buff,PN);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,8);

	set_rfcomm_msg_pn_dlci_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_dlci_conf(pn_config));
	set_rfcomm_msg_pn_uih_frame_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_uih_frame_conf(pn_config));


	set_rfcomm_msg_pn_credit_support_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_credit_support_conf(pn_config));


	set_rfcomm_msg_pn_priority_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_priority_conf(pn_config));

	//ack timer set to 0.
	set_rfcomm_msg_pn_max_frame_size_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_max_frame_size_conf(pn_config));

	set_rfcomm_msg_pn_max_frame_retrans_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_max_frame_retrans_conf(pn_config));
	set_rfcomm_msg_pn_credits_issued_conf_pkt(rfcomm_pkt_buff,
						get_rfcomm_msg_pn_credits_issued_conf(pn_config));

/*	set_rfcomm_msg_dlci_conf_pkt(rfcomm_pkt_buff,20);
	set_rfcomm_msg_uih_frame_conf_pkt(rfcomm_pkt_buff,0);
	set_rfcomm_msg_credit_conf_pkt(rfcomm_pkt_buff,0xF);
	set_rfcomm_msg_priority_conf_pkt(rfcomm_pkt_buff,7);

	//ack timer set to 0.
	set_rfcomm_msg_max_frame_size_conf_pkt(rfcomm_pkt_buff,336);
	set_rfcomm_msg_max_frame_retrans_conf_pkt(rfcomm_pkt_buff,0);
	set_rfcomm_msg_credits_issued_conf_pkt(rfcomm_pkt_buff,7);*/
	
}

void create_rpn_msg(uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp,
					uint8_t *rpn_config) { 

	set_rfcomm_msg_type(rfcomm_pkt_buff,RPN);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,8);

	//TODO:Disable DLCI ea.
	set_rfcomm_msg_rpn_dlci_conf_pkt(rfcomm_pkt_buff,20);
	disable_rfcomm_msg_rpn_dlci_ea_conf_pkt(rfcomm_pkt_buff);
	enable_rfcomm_msg_rpn_dlci_cr(rfcomm_pkt_buff);

	set_rfcomm_msg_rpn_baud_rate_conf_pkt(rfcomm_pkt_buff,B115200);
	set_rfcomm_msg_rpn_data_conf_pkt(rfcomm_pkt_buff,BITS8);
	set_rfcomm_msg_rpn_stop_bit_conf_pkt(rfcomm_pkt_buff,0);
	set_rfcomm_msg_rpn_parity_bit_conf_pkt(rfcomm_pkt_buff,0);
	set_rfcomm_msg_rpn_flow_ctrl_conf_pkt(rfcomm_pkt_buff,0);


	set_rfcomm_msg_rpn_hw_xon_conf_pkt(rfcomm_pkt_buff,17);
	set_rfcomm_msg_rpn_hw_xoff_conf_pkt(rfcomm_pkt_buff,19);

	set_rfcomm_msg_rpn_pm_lsb_conf_pkt(rfcomm_pkt_buff,
			PM_BR_MASK|PM_DB_MASK|PM_SB_MASK|PM_P_MASK|PM_PT_MASK|
			PM_XON_MASK|PM_XOFF_MASK);

	set_rfcomm_msg_rpn_pm_msb_conf_pkt(rfcomm_pkt_buff,
			PM_XI_MASK|PM_XO_MASK|PM_RTRI_MASK|PM_RTRO_MASK|
			PM_RTCI_MASK|PM_RTCO_MASK);
}


void create_test_msg(
					uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp,
					uint8_t *buff,
					uint8_t len
					) {

	set_rfcomm_msg_type(rfcomm_pkt_buff,TEST);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,len);

	memcpy(rfcomm_pkt_buff+get_rfcomm_payload_offset(rfcomm_pkt_buff),
					buff,
					len);
}

void create_cld_msg(
					uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp
					) {

	set_rfcomm_msg_type(rfcomm_pkt_buff,CLD);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,0);
}

void create_psc_msg(
					uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp
					) {
	set_rfcomm_msg_type(rfcomm_pkt_buff,PSC);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,0);
}

void create_fcon_msg(
					uint8_t *rfcomm_pkt_buff,
					uint8_t cmdresp
					) {

	set_rfcomm_msg_type(rfcomm_pkt_buff,FCon);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,0);
}

void create_fcoff_msg(
				uint8_t *rfcomm_pkt_buff,
				uint8_t cmdresp
				) {

	set_rfcomm_msg_type(rfcomm_pkt_buff,FCoff);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,0);
}


void create_nsc_msg(
				uint8_t *rfcomm_pkt_buff,
				uint8_t cmdresp,
				uint8_t cmdtype
				) {

	set_rfcomm_msg_type(rfcomm_pkt_buff,FCoff);
	disable_rfcomm_msg_type_ea(rfcomm_pkt_buff);

	if(cmdresp == MSG_CMD) {
		enable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	else {
		//Expecting the same control signals as the original packet.
		disable_rfcomm_msg_type_cr(rfcomm_pkt_buff);
	}
	
	disable_rfcomm_msg_type_len_ea(rfcomm_pkt_buff);
	set_rfcomm_msg_type_len(rfcomm_pkt_buff,1);


	/* Disable the ea and enable cr */
	disable_rfcomm_msg_nsc_cmd_type_ea_conf_pkt(rfcomm_pkt_buff);
	enable_rfcomm_msg_nsc_cmd_type_cr(rfcomm_pkt_buff);

	/* Set the cmd type */
	set_rfcomm_msg_msc_nsc_cmd_type_pkt(rfcomm_pkt_buff,cmdtype);
}

/*
void create_pn_msg(uint8_t *rfcomm_pkt_buff,
				uint8_t cmdresp,
				struct rfcomm_pn_msg_conf *pn_conf) {
	
}*/

/*
void create_pn_msg(uint8_t *msg_buff,
				uint8_t cmdresp,
				struct rfcomm_pn_msg_conf *pnmsgconf) {

	disable_ea_addr_field(msb_buff);
	
	if(cmdresp == MSG_CMD)
		enable_cr_addr_field(msg_buff);
	else
		disable_cr_addr_field(msb_buff);

}*/

/*TODO: Last modified here. CHECK!!!!! */



void set_rfcomm_fcs(uint8_t *rfcomm_pkt_buff) {

	if(get_rfcomm_control_field(rfcomm_pkt_buff) == UIH) {
		write8_buff_le(rfcomm_pkt_buff,
					get_rfcomm_payload_len(rfcomm_pkt_buff) + 
					get_rfcomm_len_size(rfcomm_pkt_buff) + 
					RFCOMM_HEADER_SIZE + 
					get_control_field_poll_final_bit(rfcomm_pkt_buff), 
					create_fcs(rfcomm_pkt_buff,FCS_SIZE_UIH));
	} else {
		write8_buff_le(rfcomm_pkt_buff,
					get_rfcomm_payload_len(rfcomm_pkt_buff) + 
					get_rfcomm_len_size(rfcomm_pkt_buff) + 
					RFCOMM_HEADER_SIZE,
					create_fcs(rfcomm_pkt_buff,FCS_SIZE));
	}
}

void create_credit_pkt(uint8_t *rfcomm_pkt_buff,
					uint8_t chaddr,
					uint16_t len,
					uint8_t cmdresp,
					uint8_t credits
					) {
	

	disable_ea_addr_field(rfcomm_pkt_buff);
	
	if(cmdresp == MSG_CMD)
		enable_cr_addr_field(rfcomm_pkt_buff);
	else
		disable_cr_addr_field(rfcomm_pkt_buff);

	disable_dir_addr_field(rfcomm_pkt_buff);
	set_rfcomm_server_channel(rfcomm_pkt_buff,chaddr);

	set_rfcomm_ctrl_field(rfcomm_pkt_buff,UIH);
	set_rfcomm_poll_final_bit(rfcomm_pkt_buff);
	
	set_rfcomm_len_field_lsb(rfcomm_pkt_buff,len);
	disable_rfcomm_len_ea(rfcomm_pkt_buff);

	set_rfcomm_credits(rfcomm_pkt_buff,credits);

	set_rfcomm_fcs(rfcomm_pkt_buff);
}

uint8_t get_rfcomm_pkt_size(uint8_t *rfcomm_pkt_buff) {	
	
	if(get_rfcomm_control_field(rfcomm_pkt_buff) ==  UIH) {
	 	return ((RFCOMM_HEADER_SIZE) + 
		(get_rfcomm_payload_len(rfcomm_pkt_buff)) +
		(get_rfcomm_len_size(rfcomm_pkt_buff)) +
		(RFCOMM_FCS_SIZE) + 
		(get_control_field_poll_final_bit(rfcomm_pkt_buff)));
	} else {
	 	return ((RFCOMM_HEADER_SIZE) + 
		(get_rfcomm_payload_len(rfcomm_pkt_buff)) +
		(get_rfcomm_len_size(rfcomm_pkt_buff)) +
		(RFCOMM_FCS_SIZE));
		
	}
}

/* 				 Address Field 				*/

/*
 *  +-------+----+-----+---+----+----+----+----+-----+
 *  |Bit No.| 1  |  2  | 3 | 4  |  5 |  6 |  7 |  8  |
 *  +-------+----+-----+---+----+----+----+----+-----+
 *  |RFCOMM | EA | C/R | D |   Server Channel        |
 *  +-------+----+-----+---+-------------------------+
 *
 */

/*
	 						RFCOMM PACKET FORMAT 
 							====================
	+------------+--------------+-------------+------------+-------------+
	|   Address  |  Control     | Length Ind  |    Info    |     FCS     |
	+------------+--------------+-------------+------------+-------------+
	|  1 octet   |  1 octet     |1 or 2 octets| Unspecified| 1 octet     |
	+------------+--------------+-------------+------------+-------------+

*/
void create_rfcomm_pkt(
					uint8_t *rfcomm_pkt_buff,
					uint8_t chaddr,
					uint16_t len,
					uint8_t pfbit,
					uint8_t cmdresp,
					RFCOMM_FRAME_TYPE rfcomm_frame_type) {
	
	/*      Address fields     */
	disable_ea_addr_field(rfcomm_pkt_buff);
	
	if(cmdresp == MSG_CMD)
		enable_cr_addr_field(rfcomm_pkt_buff);
	else
		disable_cr_addr_field(rfcomm_pkt_buff);

	disable_dir_addr_field(rfcomm_pkt_buff);
	set_rfcomm_server_channel(rfcomm_pkt_buff,chaddr);

	/*      Control fields     */
	set_rfcomm_ctrl_field(rfcomm_pkt_buff,rfcomm_frame_type);

	if(pfbit)
		set_rfcomm_poll_final_bit(rfcomm_pkt_buff);
	else
		clear_rfcomm_poll_final_bit(rfcomm_pkt_buff);

	
	/*      Length fields     */
	set_rfcomm_len_field_lsb(rfcomm_pkt_buff,len);
	disable_rfcomm_len_ea(rfcomm_pkt_buff);

	/*      	FCS			  */
	set_rfcomm_fcs(rfcomm_pkt_buff);
}


