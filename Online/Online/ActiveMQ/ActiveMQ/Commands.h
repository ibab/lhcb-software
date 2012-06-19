// $Id: Commands.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
//====================================================================
//  ActiveMQ
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ plugin for Apache
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/Commands.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_COMMANDS_H
#define ACTIVEMQ_COMMANDS_H 1

namespace ActiveMQ {

  enum Commands {
    CMD_SUBSCRIBE,
    CMD_UNSUBSCRIBE,
    CMD_DATA,
    CMD_PUBLISH
  };
}

#endif /* ACTIVEMQ_COMMANDS_H */

