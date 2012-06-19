// $Id: Config.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/Config.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_CONFIG_H
#define ACTIVEMQ_CONFIG_H 1

#ifdef _WIN32
#include "yvals.h"
#undef _GLOBAL_USING
#ifdef APR_DECLARE_EXPORT
#define ACTIVEMQ_API_DECLARE  __declspec(dllexport)
#else
#define ACTIVEMQ_API_DECLARE  __declspec(dllimport)
#endif
#endif

#endif /* ACTIVEMQ_CONFIG_H */

