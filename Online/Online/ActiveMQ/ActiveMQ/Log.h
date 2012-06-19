// $Id: Log.h,v 1.1 2010/11/01 17:20:22 frankb Exp $
//====================================================================
//  Stomp
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp plugin 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/Log.h,v 1.1 2010/11/01 17:20:22 frankb Exp $
#ifndef STOMP_LOG_H
#define STOMP_LOG_H 1

// C++ include files
#include <iostream>

/*
 *   Stomp namespace declaration
 */
namespace ActiveMQ {
  void install_printer(int level);
  std::ostream& log();
}      // End namespace Stomp
#endif /* STOMP_LOG_H */

