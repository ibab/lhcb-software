// $Id: Commands.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
//====================================================================
//  Stomp
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp plugin for Apache
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/Stomp/Commands.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
#ifndef STOMP_COMMANDS_H
#define STOMP_COMMANDS_H 1

namespace Stomp {

  enum Commands {
    CMD_SUBSCRIBE,
    CMD_UNSUBSCRIBE,
    CMD_DATA,
    CMD_PUBLISH
  };
}

#endif /* STOMP_COMMANDS_H */

