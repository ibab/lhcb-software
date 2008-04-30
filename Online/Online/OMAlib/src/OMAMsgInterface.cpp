// $Id: OMAMsgInterface.cpp,v 1.2 2008-04-30 13:28:54 ggiacomo Exp $
#include <iostream>
#include "OMAlib/OMAMsgInterface.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgInterface
//
// 2008-02-29 : Giacomo Graziani
//-----------------------------------------------------------------------------

OMAMsgInterface::OMAMsgInterface(  ) : msgMode(LOCAL) {

}

OMAMsgInterface::~OMAMsgInterface() {} 


void OMAMsgInterface::raiseMessage(OMAMsgLevel level,
                                   std::string message,
                                   std::string histogramName ) {
  
  // to be implemented ... dummy version for the moment
  
  std::cout << std::endl << "==================================================================="<<std::endl;
  std::cout << (level == ALARM ? "ALARM" : "WARNING") << " from histogram "<<histogramName <<std::endl;
  std::cout << "   " << message <<std::endl;
  std::cout  << "==================================================================="<<std::endl<< std::endl;

}
