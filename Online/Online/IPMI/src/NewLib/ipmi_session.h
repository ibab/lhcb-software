/*
 * ipmi_session.h
 *
 *  Created on: Jan 29, 2016
 *      Author: beat
 */

#ifndef ONLINE_IPMI_SRC_NEWLIB_IPMI_SESSION_H_
#define ONLINE_IPMI_SRC_NEWLIB_IPMI_SESSION_H_

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


typedef enum
{
  IPMI_SESSION_REQUEST_CURRENT = 0,
  IPMI_SESSION_REQUEST_ALL,
  IPMI_SESSION_REQUEST_BY_ID,
  IPMI_SESSION_REQUEST_BY_HANDLE
} Ipmi_Session_Request_Type;

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include "ipmi.h"

#define IPMI_GET_SESSION_INFO 0x3D

/*
 * From table 22.25 of the IPMIv2 specification
 */
#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct get_session_info_rsp
{
    uint8_t session_handle;

    #if WORDS_BIGENDIAN
    uint8_t __reserved1        : 2;
    uint8_t session_slot_count : 6; /* 1-based */
    #else
    uint8_t session_slot_count : 6; /* 1-based */
    uint8_t __reserved1        : 2;
    #endif

    #if WORDS_BIGENDIAN
    uint8_t __reserved2          : 2;
    uint8_t active_session_count : 6; /* 1-based */
    #else
    uint8_t active_session_count : 6; /* 1-based */
    uint8_t __reserved2          : 2;
    #endif

    #if WORDS_BIGENDIAN
    uint8_t __reserved3          : 2;
    uint8_t user_id              : 6;
    #else
    uint8_t user_id              : 6;
    uint8_t __reserved3          : 2;
    #endif

    #if WORDS_BIGENDIAN
    uint8_t __reserved4          : 4;
    uint8_t privilege_level      : 4;
    #else
    uint8_t privilege_level      : 4;
    uint8_t __reserved4          : 4;
    #endif

    #if WORDS_BIGENDIAN
    uint8_t auxiliary_data       : 4;
    uint8_t channel_number       : 4;
    #else
    uint8_t channel_number       : 4;
    uint8_t auxiliary_data       : 4;
    #endif

    union
    {
        /* Only exists if channel type is 802.3 LAN */
        struct
        {
            uint8_t console_ip[4];  /* MSBF */
            uint8_t console_mac[6]; /* MSBF */
            uint16_t      console_port;   /* LSBF */
        } lan_data;

        /* Only exists if channel type is async. serial modem */
        struct
        {
            uint8_t session_channel_activity_type;

            #if WORDS_BIGENDIAN
            uint8_t __reserved5          : 4;
            uint8_t destination_selector : 4;
            #else
            uint8_t destination_selector : 4;
            uint8_t __reserved5          : 4;
            #endif

            uint8_t console_ip[4];   /* MSBF */

            /* Only exists if session is PPP */
            uint16_t console_port;        /* LSBF */
        } modem_data;
    } channel_data;
} /*ATTRIBUTE_PACKING*/;
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif


class ipmi_intf;

class ipmi_session
{
  public:
#include "newnew.h"

    /*
     * print_session_info_csv
     */
    ipmi_intf *iface;
    char *hostname; /* Numeric IP adress or DNS name - see RFC 1034/RFC 1035 */
    uint8_t username[17];
    uint8_t authcode[IPMI_AUTHCODE_BUFFER_SIZE + 1];
    uint8_t challenge[16];
    uint8_t authtype;
    uint8_t authtype_set;
#define IPMI_AUTHSTATUS_PER_MSG_DISABLED    0x10
#define IPMI_AUTHSTATUS_PER_USER_DISABLED   0x08
#define IPMI_AUTHSTATUS_NONNULL_USERS_ENABLED   0x04
#define IPMI_AUTHSTATUS_NULL_USERS_ENABLED  0x02
#define IPMI_AUTHSTATUS_ANONYMOUS_USERS_ENABLED 0x01
    uint8_t authstatus;
    uint8_t authextra;
    uint8_t privlvl;
    uint8_t cipher_suite_id;
    char sol_escape_char;
    int password;
    int port;
    int active;
    int retry;

    uint32_t session_id;
    uint32_t in_seq;
    uint32_t out_seq;
    uint32_t timeout;

    struct sockaddr_storage addr;
    socklen_t addrlen;
    int ai_family; /* Protocol family for socket.  */

    /*
     * This struct holds state data specific to IPMI v2 / RMCP+ sessions
     */
    struct {
        enum LANPLUS_SESSION_STATE session_state;

        /* These are the algorithms agreed upon for the session */
        uint8_t requested_auth_alg;
        uint8_t requested_integrity_alg;
        uint8_t requested_crypt_alg;
        uint8_t auth_alg;
        uint8_t integrity_alg;
        uint8_t crypt_alg;
        uint8_t max_priv_level;
        uint8_t lookupbit;

        uint32_t console_id;
        uint32_t bmc_id;

        /*
         * Values required for RAKP mesages
         */

        /* Random number generated byt the console */
        uint8_t console_rand[16];
        /* Random number generated by the BMC */
        uint8_t bmc_rand[16];

        uint8_t bmc_guid[16];
        uint8_t requested_role;   /* As sent in the RAKP 1 message */
        uint8_t rakp2_return_code;

        uint8_t sik[IPMI_SIK_BUFFER_SIZE]; /* Session integrity key */
        uint8_t kg[IPMI_KG_BUFFER_SIZE];   /* BMC key */
        uint8_t k1[20];   /* Used for Integrity checking? */
        uint8_t k2[20];   /* First 16 bytes used for AES  */
    } v2_data;


    /*
     * This data is specific to the Serial Over Lan session
     */
    struct {
        uint16_t max_inbound_payload_size;
        uint16_t max_outbound_payload_size;
        uint16_t port;
        uint8_t sequence_number;

        /*  This data describes the last SOL packet */
        uint8_t last_received_sequence_number;
        uint8_t last_received_byte_count;
        void (*sol_input_handler)(struct ipmi_rs * rsp);
    } sol_data;

    void print_session_info_csv(
        const struct get_session_info_rsp * session_info, int data_len);

    /*
     * print_session_info_verbose
     */
    void print_session_info_verbose(const struct get_session_info_rsp * session_info, int data_len);

    void print_session_info( const struct get_session_info_rsp * session_info, int data_len);

    /*
     * ipmi_get_session_info
     *
     * returns 0 on success
     *         -1 on error
     */
    int ipmi_get_session_info( ipmi_intf * intf,
        Ipmi_Session_Request_Type session_request_type, uint32_t id_or_handle);

    void printf_session_usage(void);

    int ipmi_session_main( ipmi_intf * intf, int argc, char ** argv);
    ipmi_session(ipmi_intf *i)
    {
      iface = i;
      hostname = 0;
    };
};

#endif /* ONLINE_IPMI_SRC_NEWLIB_IPMI_SESSION_H_ */
