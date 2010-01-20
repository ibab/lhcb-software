// $Id: L0DUConfig.cpp,v 1.5 2010-01-20 15:59:06 odescham Exp $
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

  std::ostringstream oscond(" ");
  std::ostringstream s(" ");
  for(LHCb::L0DUChannel::Map::iterator 
        icond = m_channels.begin()  ; icond != m_channels.end() ; icond++){
    std::ostringstream os(" ");
    oscond << ((*icond).second)->summary() << std::endl;
  }
  std::ostringstream ostrig("");
  if(triggers().size() > 0 ){
    ostrig << " --> " << triggers().size() << " Trigger sets are defined : " << std::endl;
    for(LHCb::L0DUTrigger::Map::const_iterator it = triggers().begin(); it!=triggers().end();++it){
      ostrig << (*it).second->summary();
    } 
  }

  std::ostringstream dec("");
  int mask = LHCb::L0DUDecision::Any;
  int typ  = 0x1;
  while(mask != 0x0){
    dec << "   Emulated Decision (" << LHCb::L0DUDecision::Name[typ] << "): " << emulatedDecision(typ) << std::endl;
    typ  = typ  << 1;
    mask = mask >> 1;
  }

  s << " " << std::endl
    << " <-----  L0DUConfig (" << key()   << " ) " << " --------------->" <<std::endl
    << dec.str()
    << "   Recipe name : '" << m_recipe << "'" << std::endl
    << "   Algorithm description : " << m_definition << std::endl
    << oscond.str()
    << " ======= Trigger Sets "
    << ostrig.str();
  return s.str();


}



std::string LHCb::L0DUConfig::description(){

  std::ostringstream oscond(" ");
  std::ostringstream s(" ");
  for(LHCb::L0DUChannel::Map::iterator 
        icond = m_channels.begin()  ; icond != m_channels.end() ; icond++){
    std::ostringstream os(" ");
    oscond << ((*icond).second)->description() << std::endl;
  }

  std::ostringstream ostrig("");
  if(triggers().size() > 0 ){
    ostrig << " --> " << triggers().size() << " SubTrigger(s) are defined : " << std::endl;
    for(LHCb::L0DUTrigger::Map::const_iterator it = triggers().begin(); it!=triggers().end();++it){
      ostrig << (*it).second->description() ;
    } 
  }

  s << " " << std::endl
    << " <-----  L0DUConfig (" << key()   << " ) " << " --------------->" <<std::endl
    << "   Recipe name : '" << m_recipe << "'" <<std::endl
    << "   Description : " << m_definition << std::endl
    << oscond.str()
    << " ======= Trigger Sets "
    << ostrig.str() << std::endl;
  return s.str();


}

