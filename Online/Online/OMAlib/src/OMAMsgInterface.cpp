// $Id: OMAMsgInterface.cpp,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $
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
  
  std::cout << std::endl << "==================================================================="<<std::endl;
  std::cout << "   " << message <<std::endl;
  std::cout  << "==================================================================="<<std::endl<< std::endl;

}
