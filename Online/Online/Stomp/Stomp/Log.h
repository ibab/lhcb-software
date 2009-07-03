// $Id: Log.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/Stomp/Log.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
#ifndef STOMP_LOG_H
#define STOMP_LOG_H 1

// C++ include files
#include <iostream>

/*
 *   Stomp namespace declaration
 */
namespace Stomp {
  std::ostream& log();
}      // End namespace Stomp
#endif /* STOMP_LOG_H */

