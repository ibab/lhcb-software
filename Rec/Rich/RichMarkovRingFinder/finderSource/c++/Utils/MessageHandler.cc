// $Id: MessageHandler.cc,v 1.1 2008-06-08 11:38:33 jonrob Exp $
// Include files 

// local
#include "Utils/MessageHandler.h"

Lester::MessageHandler & Lester::messHandle()
{
  static Lester::MessageHandler handler;
  return handler;
}

