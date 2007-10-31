// $Id: L0DUConfig.cpp,v 1.1 2007-10-31 14:31:15 odescham Exp $
// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUConfig.h"

//-------------------------------–—----------------------------------------------
// Auxilliary Implementation file for L0DUConfig Event classes
//
// 19/10/2007 : Olivier Deschamps
//-----------------------------------------------------------------------------



std::string LHCb::L0DUConfig::summary(){

  std::stringstream oscond(" ");
  std::stringstream s(" ");
  for(LHCb::L0DUChannel::Map::iterator 
        icond = m_channels.begin()  ; icond != m_channels.end() ; icond++){
    std::stringstream os(" ");
    oscond << ((*icond).second)->summary() << std::endl;
  }
  s << " " << std::endl
    << " <-----  L0DUConfig (" << key()   << " ) " << " --------------->" <<std::endl
    << "   Decision : " << decision() << std::endl
    << "   Algorithm description : " << m_definition << std::endl
    << oscond.str();
  return s.str();


}



std::string LHCb::L0DUConfig::description(){

  std::stringstream oscond(" ");
  std::stringstream s(" ");
  for(LHCb::L0DUChannel::Map::iterator 
        icond = m_channels.begin()  ; icond != m_channels.end() ; icond++){
    std::stringstream os(" ");
    oscond << ((*icond).second)->description() << std::endl;
  }
  s << " " << std::endl
    << " <-----  L0DUConfig (" << key()   << " ) " << " --------------->" <<std::endl
    << "   Description : " << m_definition << std::endl
    << oscond.str();
  return s.str();


}

