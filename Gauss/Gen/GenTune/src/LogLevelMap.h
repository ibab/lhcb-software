/*
 *  LogLevelMap.h
 *  RivetAnalysisHandler
 *
 *  Created by James Monk on 18/06/2008.
 * 
 *  Converts between the Gaudi Logging levels and the Rivet ones
 *
 */

#ifndef RIVET_I_LOGLEVELS_H
#define RIVET_I_LOGLEVELS_H

#include "Rivet/Tools/Logging.hh"
#include "GaudiKernel/IMessageSvc.h"

//Rivet::Log::Level
int rivetLevel(MSG::Level gaudiLevel)
{
  switch(gaudiLevel)
  {
  case MSG::NIL :
    return Rivet::Log::ERROR;
    break;
  case MSG::VERBOSE :
    return Rivet::Log::TRACE;
    break;
  case MSG::DEBUG :
    return Rivet::Log::DEBUG;
    break;
  case MSG::INFO :
    return Rivet::Log::INFO;
    break;
  case MSG::WARNING :
    return Rivet::Log::WARN;
    break;
  case MSG::ERROR :
    return Rivet::Log::ERROR;
    break;  
  case MSG::FATAL :
    return Rivet::Log::ERROR;
    break;
  case MSG::ALWAYS :
    return Rivet::Log::TRACE;
    break;  
  default:
    return Rivet::Log::INFO;
  };
  return Rivet::Log::INFO;
}

#endif
