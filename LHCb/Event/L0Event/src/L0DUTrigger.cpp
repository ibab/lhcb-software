// $Id: L0DUTrigger.cpp,v 1.2 2010-01-20 15:59:07 odescham Exp $
// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUTrigger.h"

//-------------------------------–—----------------------------------------------
// Auxilliary Implementation file for L0DUTrigger Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------


std::string LHCb::L0DUTrigger::summary(){

  std::ostringstream s(" ");
  s << description()
    << "  ===== <** EMULATED DECISION **> = <"<< emulatedDecision() << ">" << std::endl;
  return s.str();


}

std::string LHCb::L0DUTrigger::description(){
  std::ostringstream os(" ");
  std::ostringstream s(" ");
  unsigned int k=0;
  for(LHCb::L0DUChannel::Map::iterator ic = m_channels.begin()  ; ic != m_channels.end() ; ic++){
    k++;
    os << L0DUDecision::Name[((*ic).second)->decisionType()] <<"|" <<((*ic).second)->name()  ;
    if( k != m_channels.size() ) os << " || ";
  }
  s << " ====  Trigger '" << L0DUDecision::Name[m_decisionType] +"|"+m_name   << "' = [" << os.str() << "]" << std::endl;
  return s.str();


}

