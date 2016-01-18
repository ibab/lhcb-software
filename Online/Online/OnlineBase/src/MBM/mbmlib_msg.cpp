// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#include "MBM/bmmessage.h"

namespace {

#define CHECK(x) case MBMMessage::x : return #x ;
  const char* __msg_type(int typ)  {
    switch(typ)  {
      CHECK(INCLUDE)
        CHECK(EXCLUDE)
        //Consummer part
        CHECK(ADD_REQUEST)
        CHECK(DEL_REQUEST)
        CHECK(GET_EVENT)
        CHECK(WAIT_EVENT)
        CHECK(FREE_EVENT)
        CHECK(STOP_CONSUMER)
        CHECK(PAUSE)
        //Producer part
        CHECK(GET_SPACE_TRY)
        CHECK(GET_SPACE)
        CHECK(WAIT_SPACE_A)
        CHECK(FREE_SPACE)
        CHECK(SEND_SPACE)
        CHECK(SEND_EVENT)
        CHECK(DECLARE_EVENT)
        CHECK(CANCEL_REQUEST)
        CHECK(STOP_PRODUCER)
        CHECK(GRANT_UPDATE)
        //
        //statistics
        CHECK(PROCESS_EXISTS)
        CHECK(STAT_EVENTS_ACTUAL)
        CHECK(STAT_EVENTS_SEEN)
        CHECK(STAT_EVENTS_PRODUCED)
        CHECK(STAT_RESET)
        CHECK(STAT_MIN_ALLOC)
        CHECK(STAT_BUFFER_SIZE)
        CHECK(STAT_EVENTS_IN_BUFFER)
        CHECK(STAT_SPACE_IN_BUFFER)
    default:      return "Unknown";
    }
  }
}

const char* MBMMessage::typeStr(int typ) {
  return __msg_type(typ);
}

const char* MBMMessage::c_type() const   {
  return __msg_type(this->type);
}

const char* MBMMessage::c_user() const   {
  static char text[132];
  ::snprintf(text,sizeof(text),"%12lX",(unsigned long)this->user);
  return text;
}
