/*
 * lan.h
 *
 *  Created on: Jan 29, 2016
 *      Author: beat
 */
#ifndef ONLINE_IPMI_SRC_NEWLIB_LAN_H_
#define ONLINE_IPMI_SRC_NEWLIB_LAN_H_

#define IPMI_LAN_TIMEOUT 2
#define IPMI_LAN_RETRY  4
#define IPMI_LAN_PORT  0x26f
#define IPMI_LAN_CHANNEL_E 0x0e

#define IPMI_LAN_MAX_REQUEST_SIZE 38 /* 45 - 7 */
#define IPMI_LAN_MAX_RESPONSE_SIZE 34 /* 42 - 8 */

//#include <string>
//using namespace std;
/*
 * LAN interface is required to support 45 byte request transactions and
 * 42 byte response transactions.
 */
typedef enum
{
  SessSup_PingFail =1,
  SessSup_auth_capabilities_cmd,
  SessSup_BadUserName,
  SessSup_NoSlot,
  SessSup_NoSlotUser,
  SessSup_ActFail,
  SessSup_PrivFail,


};
class ipmi_intf;
class ipmi_auth;
class lan
{
  public:
#include "newnew.h"
    struct ipmi_rq_entry * ipmi_req_entries;
    struct ipmi_rq_entry * ipmi_req_entries_tail;
    uint8_t bridge_possible;
    char name[16];//: "lan"
    char desc[128];//("IPMI v1.5 LAN Interface");
    lan();
    ~lan();
    ipmi_auth *auth;
    int sessionsup_fail;
    int get_session_setup_code(){return sessionsup_fail;};
    struct ipmi_rq_entry *ipmi_req_add_entry( ipmi_intf * intf, struct ipmi_rq * req, uint8_t req_seq);
    struct ipmi_rq_entry *ipmi_req_lookup_entry(uint8_t seq, uint8_t cmd);
    void ipmi_req_remove_entry(uint8_t seq, uint8_t cmd);
    void ipmi_req_clear_entries(void);
    int get_random(void *data, int len);
    int ipmi_lan_send_packet( ipmi_intf * intf, uint8_t * data,
        int data_len);
    struct ipmi_rs * ipmi_lan_recv_packet( ipmi_intf * intf,struct ipmi_rs *);
    int ipmi_handle_pong( ipmi_intf * intf, struct ipmi_rs * rsp);
    int ipmi_lan_ping( ipmi_intf * intf);
    void ipmi_lan_thump_first( ipmi_intf * intf);
    void ipmi_lan_thump( ipmi_intf * intf);
    struct ipmi_rs * ipmi_lan_poll_recv( ipmi_intf * intf,struct ipmi_rs *);
    struct ipmi_rq_entry * ipmi_lan_build_cmd( ipmi_intf * intf,
        struct ipmi_rq * req, int isRetry);
    struct ipmi_rs * ipmi_lan_send_cmd( ipmi_intf * intf,
        struct ipmi_rq * req,struct ipmi_rs *);
    uint8_t * ipmi_lan_build_rsp( ipmi_intf * intf, struct ipmi_rs * rsp,
        int * llen);
    int ipmi_lan_send_rsp( ipmi_intf * intf, struct ipmi_rs * rsp);
    uint8_t * ipmi_lan_build_sol_msg( ipmi_intf * intf,
        struct ipmi_v2_payload * payload, int * llen);
    int is_sol_packet(struct ipmi_rs * rsp);
    int sol_response_acks_packet( ipmi_rs * rsp,
        struct ipmi_v2_payload * payload);
    struct ipmi_rs * ipmi_lan_send_sol_payload( ipmi_intf * intf,
        struct ipmi_v2_payload * payload,struct ipmi_rs *);
    int is_sol_partial_ack( ipmi_v2_payload * v2_payload,
        struct ipmi_rs * rsp);
    void set_sol_packet_sequence_number( ipmi_intf * intf,
        struct ipmi_v2_payload * v2_payload);
    struct ipmi_rs * ipmi_lan_send_sol( ipmi_intf * intf,
        struct ipmi_v2_payload * v2_payload,struct ipmi_rs *);
    int check_sol_packet_for_new_data( ipmi_intf * intf,
        struct ipmi_rs *rsp);
    void ack_sol_packet( ipmi_intf * intf, struct ipmi_rs * rsp);
    struct ipmi_rs * ipmi_lan_recv_sol( ipmi_intf * intf,struct ipmi_rs *);
    int ipmi_lan_keepalive( ipmi_intf * intf);
    int ipmi_get_auth_capabilities_cmd( ipmi_intf * intf);
    int ipmi_get_session_challenge_cmd( ipmi_intf * intf);
    int ipmi_activate_session_cmd( ipmi_intf * intf);
    int ipmi_set_session_privlvl_cmd( ipmi_intf * intf);
    int ipmi_close_session_cmd( ipmi_intf * intf);
    int ipmi_lan_activate_session( ipmi_intf * intf);
    void ipmi_lan_close( ipmi_intf * intf);
    int ipmi_lan_open( ipmi_intf * intf);
    int ipmi_lan_setup( ipmi_intf * intf);
    void ipmi_lan_set_max_rq_data_size( ipmi_intf * intf, uint16_t size);
    void ipmi_lan_set_max_rp_data_size( ipmi_intf * intf, uint16_t size);
};

#endif /* ONLINE_IPMI_SRC_NEWLIB_LAN_H_ */
