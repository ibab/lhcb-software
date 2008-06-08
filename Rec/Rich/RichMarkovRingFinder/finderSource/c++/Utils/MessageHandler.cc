// $Id: MessageHandler.cc,v 1.2 2008-06-08 12:54:55 jonrob Exp $
// Include files 

#include <iostream>

// local
#include "Utils/MessageHandler.h"

Lester::MessageHandler & Lester::messHandle()
{
  static Lester::MessageHandler handler;
  return handler;
}

std::string Lester::text( const Lester::MessageLevel level )
{
  switch( level )
  {
  case Lester::VERBOSE:       return "MCMC-VERBOSE : ";
  case Lester::DEBUG:         return "MCMC-DEBUG   : ";
  case Lester::INFO:          return "MCMC-INFO    : ";
  case Lester::WARNING:       return "MCMC-WARNING : ";
  case Lester::ERROR:         return "MCMC-ERROR   : ";
  case Lester::FATAL:         return "MCMC-FATAL   : ";
  default:                    return "???"; // should never happen
  }
}

void Lester::MessageHandler::message( const Lester::MessageLevel level,
                                      const std::string & message ) const
{
  if ( !m_handler )
  {
    std::cout << Lester::text(level) << message << std::endl;
  }
  else
  {
    m_handler->lesterMessage(level,message);
  }
}
