// $Id: OMAMsgInterface.cpp,v 1.3 2008-08-11 08:05:16 ggiacomo Exp $
#include <iostream>
#include "OMAlib/OMAMsgInterface.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgInterface
//
// 2008-02-29 : Giacomo Graziani
//-----------------------------------------------------------------------------

OMAMsgInterface::OMAMsgInterface(  ) : m_msgMode(LOCAL) {

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
