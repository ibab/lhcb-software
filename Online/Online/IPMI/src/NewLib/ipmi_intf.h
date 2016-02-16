/*
 * intf.h
 *
 *  Created on: Jan 29, 2016
 *      Author: beat
 */

#ifndef ONLINE_IPMI_SRC_NEWLIB_IPMI_INTF_H_
#define ONLINE_IPMI_SRC_NEWLIB_IPMI_INTF_H_

/*
 * Copyright (c) 2003 Sun Microsystems, Inc.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(HAVE_CONFIG_H)
# include <config.h>
#endif

//#if defined(IPMI_INTF_LAN) || defined (IPMI_INTF_LANPLUS)
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <ifaddrs.h>
//#include <unistd.h>
//#include <netdb.h>
//#endif

//#include <ipmitool/ipmi_intf.h>
//#include <ipmitool/ipmi.h>
//#include <ipmitool/ipmi_sdr.h>
//#include <ipmitool/log.h>

enum LANPLUS_SESSION_STATE {
    LANPLUS_STATE_PRESESSION = 0,
    LANPLUS_STATE_OPEN_SESSION_SENT,
    LANPLUS_STATE_OPEN_SESSION_RECEIEVED,
    LANPLUS_STATE_RAKP_1_SENT,
    LANPLUS_STATE_RAKP_2_RECEIVED,
    LANPLUS_STATE_RAKP_3_SENT,
    LANPLUS_STATE_ACTIVE,
    LANPLUS_STATE_CLOSE_SENT,
};
#include "lan.h"

#define IPMI_AUTHCODE_BUFFER_SIZE 20
#define IPMI_SIK_BUFFER_SIZE      20
#define IPMI_KG_BUFFER_SIZE       21 /* key plus null byte */

#define IPMI_AUTHSTATUS_PER_MSG_DISABLED    0x10
#define IPMI_AUTHSTATUS_PER_USER_DISABLED   0x08
#define IPMI_AUTHSTATUS_NONNULL_USERS_ENABLED   0x04
#define IPMI_AUTHSTATUS_NULL_USERS_ENABLED  0x02
#define IPMI_AUTHSTATUS_ANONYMOUS_USERS_ENABLED 0x01



#define IPMI_DEFAULT_PAYLOAD_SIZE   25

//#ifdef IPMI_INTF_OPEN
//extern ipmi_intf ipmi_open_intf;
//#endif
//#ifdef IPMI_INTF_IMB
//extern ipmi_intf ipmi_imb_intf;
//#endif
//#ifdef IPMI_INTF_LIPMI
//extern ipmi_intf ipmi_lipmi_intf;
//#endif
//#ifdef IPMI_INTF_BMC
//extern ipmi_intf ipmi_bmc_intf;
//#endif
//#ifdef IPMI_INTF_LAN
//extern ipmi_intf ipmi_lan_intf;
//#endif
//#ifdef IPMI_INTF_LANPLUS
//extern ipmi_intf ipmi_lanplus_intf;
//#endif
//#ifdef IPMI_INTF_FREE
//extern ipmi_intf ipmi_free_intf;
//#endif
//#ifdef IPMI_INTF_SERIAL
//extern ipmi_intf ipmi_serial_term_intf;
//extern ipmi_intf ipmi_serial_bm_intf;
//#endif
//#ifdef IPMI_INTF_DUMMY
//extern ipmi_intf ipmi_dummy_intf;
//#endif


/* ipmi_intf_print  -  Print list of interfaces
 *
 * no meaningful return code
 */
class ipmi_session;
class lan;
class ipmi_intf: public lan
{
  public:
    char name[16];
    char desc[128];
    char *devfile;
    int fd;
    int opened;
    int abort;
    int noanswer;
    int picmg_avail;
    IPMI_OEM manufacturer_id;

    ipmi_session *session;
//    ipmi_oem_handle *oem;
    ipmi_cmd * cmdlist;
    uint8_t target_ipmb_addr;
    uint32_t my_addr;
    uint32_t target_addr;
    uint8_t target_lun;
    uint8_t target_channel;
    uint32_t transit_addr;
    uint8_t transit_channel;
    uint16_t max_request_data_size;
    uint16_t max_response_data_size;

    uint8_t devnum;

    ipmi_session *session;
    void ipmi_intf_print(ipmi_intf_support * intflist);

    /* ipmi_intf_load  -  Load an interface from the interface table above
     *                    If no interface name is given return first entry
     *
     * @name:   interface name to try and load
     *
     * returns pointer to inteface structure if found
     * returns NULL on error
     */
    ipmi_intf * ipmi_intf_load(char * name);

    void ipmi_intf_session_set_hostname(char * hostname);

    void ipmi_intf_session_set_username(char * username);

    void ipmi_intf_session_set_password(char * password);

    void ipmi_intf_session_set_privlvl( uint8_t level);
    void ipmi_intf_session_set_lookupbit(uint8_t lookupbit);

    void ipmi_intf_session_set_cipher_suite_id(uint8_t cipher_suite_id);

    void ipmi_intf_session_set_sol_escape_char(char sol_escape_char);

    void ipmi_intf_session_set_kgkey( char * kgkey);

    void ipmi_intf_session_set_port( int port);

    void ipmi_intf_session_set_authtype(uint8_t authtype);

    void ipmi_intf_session_set_timeout(uint32_t timeout);

    void ipmi_intf_session_set_retry( int retry);

    void ipmi_intf_session_cleanup();

    void ipmi_cleanup(ipmi_intf * intf);
//#if defined(IPMI_INTF_LAN) || defined (IPMI_INTF_LANPLUS)
    int ipmi_intf_socket_connect();
//#endif

    uint16_t ipmi_intf_get_max_request_data_size();

    uint16_t ipmi_intf_get_max_response_data_size();

    void ipmi_intf_set_max_request_data_size(uint16_t size);

    void ipmi_intf_set_max_response_data_size(uint16_t size);
    int setup(){return ipmi_lan_setup(this);};
    int open(){return ipmi_lan_open(this);};
    int close(){return ipmi_lan_close(this);};
    ipmi_rs * sendrecv(struct ipmi_rq * req){return ipmi_lan_send_cmd(this,req);};
    int sendrsp(struct ipmi_rs *rsp){return ipmi_lan_send_rsp(this,rsp);};
    struct ipmi_rs *recv_sol(){return  ipmi_lan_recv_sol(this);};
    struct ipmi_rs *send_sol(struct ipmi_v2_payload * payload){return ipmi_lan_send_sol(this,payload);};
    int keepalive(){return ipmi_lan_keepalive(this);};
    void set_max_request_data_size(uint16_t size){return  ipmi_lan_set_max_rq_data_size(this,size);};
    void set_max_response_data_size(uint16_t size){return ipmi_lan_set_max_rp_data_size(this,size);};
    ipmi_intf()
    {
      target_addr=  IPMI_BMC_SLAVE_ADDR;
    }




};
#endif /* ONLINE_IPMI_SRC_NEWLIB_IPMI_INTF_H_ */
