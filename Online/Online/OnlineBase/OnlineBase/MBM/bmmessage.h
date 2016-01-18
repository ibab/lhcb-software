#ifndef ONLINEBASE_MBM_BMMESSAGE_H
#define ONLINEBASE_MBM_BMMESSAGE_H

// Framework include files
#include "MBM/bmstruct.h"

// Forward declarations
struct USER;

/** @class MBMMessage bmmessage.h MBM/bmmessage.h
 *
 *  Definition of the exchange structure between the MBM clients and the server.
 *
 *  @author  M.Frank
 *  @version 1.0
 */
struct MBMMessage {
  enum msg_magic { 
    MAGIC=0xFEEDBABE 
  };
  enum msg_type {
    INCLUDE        = 1,
    EXCLUDE        = 2,
    RECONNECT      = 3,
    // Consummer commands
    ADD_REQUEST    = 103,
    DEL_REQUEST    = 104,
    GET_EVENT      = 105,
    WAIT_EVENT     = 106,
    FREE_EVENT     = 107,
    STOP_CONSUMER  = 108,
    PAUSE          = 109,
    // Producer commands
    GET_SPACE_TRY  = 201,
    GET_SPACE      = 202,
    WAIT_SPACE_A   = 204,
    FREE_SPACE     = 205,
    SEND_SPACE     = 206,
    SEND_EVENT     = 207,
    DECLARE_EVENT  = 208,
    CANCEL_REQUEST = 209,
    STOP_PRODUCER  = 210,
    GRANT_UPDATE   = 211,
    // Server commands
    REQUIRE_CONS   = 401,
    UNREQUIRE_CONS = 402,
    //
    //statistics
    PROCESS_EXISTS       = 301,
    STAT_EVENTS_ACTUAL   = 302,
    STAT_EVENTS_SEEN     = 303,
    STAT_EVENTS_PRODUCED = 304,
    STAT_RESET           = 305,
    STAT_MIN_ALLOC       = 306,
    STAT_BUFFER_SIZE,
    STAT_EVENTS_IN_BUFFER,
    STAT_SPACE_IN_BUFFER
  };
  USER*        user;
  int          type;
  int          status;

  struct include_t { // size: 76
    char name[64];
    int  serverid;
    int  pid;
    int  partid;
  };
  typedef include_t process_exists_t;

  struct requirement_t  {  // size: 16+16+20=52
    unsigned int mask[BM_MASK_SIZE];
    unsigned int veto[BM_MASK_SIZE];
    int          masktype;
    int          usertype;
    int          freqmode;
    int          evtype;
    float        frequency;
  };

  struct cons_requirement_t  {  // size: 64+16+8 = 88
    char          name[64];
    unsigned int  mask[BM_MASK_SIZE];
    int           evtype;
    int           partid;
  };

  struct get_event_t { // size: 28
    unsigned int  trmask[BM_MASK_SIZE];
    int           offset;
    int           size;
    int           type;
  };

  struct get_space_t { // size: 8
    int           size;
    int           offset;
  }; 

  struct declare_event_t : public get_event_t { // size: 28+72
    int           freeAddr;
    int           freeSize;
    int           wait;
    char          dest[64];
  };

  struct send_event_t : public get_event_t { // size:72
    char  dest[64];
  };
  struct space_in_buffer_t { // size: 8
    int total;
    int large;
  };

  union  msg_structs  {
    include_t          include;
    requirement_t      requirement;
    cons_requirement_t cons_requirement;
    get_space_t        get_space;
    declare_event_t    declare_event;
    send_event_t       send_event;
    get_event_t        get_event;
    space_in_buffer_t  space_in_buffer;
    process_exists_t   process_exists;
    int                statistics;
  } data;
  unsigned int magic;

  explicit MBMMessage(int typ, USER* u=0, int stat=MBM_ERROR) 
    : user(u), type(typ), status(stat), magic(MAGIC) {}
#ifndef MBM_SERVER_IMPLEMENTATION
  //int communicate(MBMConnection& connection);
  //int communicate(MBMConnection& connection, int* cancelation_flag);

  /// Client communication
  //int send_request(MBMConnection& connection, bool clear_before);
  //int read_response(MBMConnection& connection, int* cancelation_flag);
  /// Server communication
  //int read_request(MBMConnection& connection);
  //int send_response(MBMConnection& connection);
  //static int clear_response(MBMConnection& connection);
#endif
  /// Clean possibly pending messages from the receive fifo (e.g. after a cancel)
  static const char* typeStr(int typ);
  const char* c_type() const;
  const char* c_user() const;
};

int _mbm_connections_use_fifos(MBMCommunication& com);
int _mbm_connections_use_asio(MBMCommunication& com);

#endif // ONLINEBASE_MBM_BMMESSAGE_H
