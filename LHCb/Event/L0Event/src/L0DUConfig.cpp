// $Id: L0DUConfig.cpp,v 1.3 2008-05-29 14:10:07 odescham Exp $
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
  std::stringstream ostrig("");
  if(triggers().size() > 0 ){
    ostrig << " --> " << triggers().size() << " Trigger sets are defined : " << std::endl;
    for(LHCb::L0DUTrigger::Map::const_iterator it = triggers().begin(); it!=triggers().end();++it){
      ostrig << (*it).second->summary();
    } 
  }


  s << " " << std::endl
    << " <-----  L0DUConfig (" << key()   << " ) " << " --------------->" <<std::endl
    << "   Decision : " << emulatedDecision() << std::endl
    << "   Algorithm description : " << m_definition << std::endl
    << oscond.str()
    << " ======= Trigger Sets "
    << ostrig.str();
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

  std::stringstream ostrig("");
  if(triggers().size() > 0 ){
    ostrig << " --> " << triggers().size() << " Trigger sets are defined : " << std::endl;
    for(LHCb::L0DUTrigger::Map::const_iterator it = triggers().begin(); it!=triggers().end();++it){
      ostrig << (*it).second->description() ;
    } 
  }

  s << " " << std::endl
    << " <-----  L0DUConfig (" << key()   << " ) " << " --------------->" <<std::endl
    << "   Description : " << m_definition << std::endl
    << oscond.str()
    << " ======= Trigger Sets "
    << ostrig.str() << std::endl;
  return s.str();


}

