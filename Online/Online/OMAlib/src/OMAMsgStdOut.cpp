// $Id: OMAMsgStdOut.cpp,v 1.1 2008-08-19 22:45:32 ggiacomo Exp $
#include <iostream>

#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "OMAlib/OMAMsgStdOut.h"
#include "OMAlib/OMAMessage.h"
//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgStdOut
//
// 2008-08-14 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OMAMsgStdOut );



OMAMsgStdOut::OMAMsgStdOut( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IOMAMsgTool>(this);

}

OMAMsgStdOut::~OMAMsgStdOut() {} 



bool OMAMsgStdOut::raiseAlarm(OMAMessage& message) {
  std::cout << std::endl << "===================== Analysis ID "<< message.id() 
            <<" =========================="<<std::endl;
  std::cout << message.levelString() << " from histogram "<< message.histogramName() <<std::endl;
  std::cout << "   " << message.text() <<std::endl;
  std::cout  << "==================================================================="<<std::endl<< std::endl;
  return true;
}

bool OMAMsgStdOut::lowerAlarm(OMAMessage& message) {
  std::cout << std::endl << "===================== Analysis ID "<< message.id() 
            <<" =========================="<<std::endl;
  std::cout << message.levelString() << " has gone" <<std::endl;
  std::cout  << "==================================================================="<<std::endl<< std::endl;
  return true;
}
