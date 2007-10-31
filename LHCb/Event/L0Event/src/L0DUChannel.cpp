// $Id: L0DUChannel.cpp,v 1.3 2007-10-31 14:31:15 odescham Exp $
// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUChannel.h"

//-------------------------------–—----------------------------------------------
// Auxilliary Implementation file for L0DUChannel Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------


LHCb::L0DUChannel*  LHCb::L0DUChannel::emulate(){

  if(m_emulated)return this; // emulation already processed
  
  // Init
  m_fired = true;
  // Is the channel fired (And'ed elementary conditions)
  if( 0 == m_elementaryConditions.size() )m_fired = false;
  for(LHCb::L0DUElementaryCondition::Map::iterator icond = m_elementaryConditions.begin() ;
      icond != m_elementaryConditions.end() ; icond++ ){
    m_fired =  m_fired && (*icond).second->value() ;
  }
  // Cyclic counter ('downscaling' procedure)
  bool accept = false ;  
  if( m_fired )m_counter += m_rate;
  if( m_counter >= LHCb::L0DUCounter::Scale){
    accept = true ;
    m_counter -= LHCb::L0DUCounter::Scale; 
  }
  // the downscaling procedure takes the final decision
  m_decision = false ;
  if(m_fired && accept)m_decision = true;
  m_emulated = true;
  return this;
}

std::string LHCb::L0DUChannel::summary(){

  std::stringstream oscond(" ");
  std::stringstream s(" ");
  for(LHCb::L0DUElementaryCondition::Map::iterator 
        icond = m_elementaryConditions.begin()  ;
      icond != m_elementaryConditions.end() ; icond++){
    std::stringstream os(" ");
    oscond << ((*icond).second)->summary() << std::endl;
  }
  s << " " << std::endl
    << " <=====  Channel (" << m_name   << " ) " 
    << "  ================> <** CHANNEL DECISION **> = <"<< m_decision << ">" << std::endl
    << "    - Accept Rate = " << m_rate << "/" << LHCb::L0DUCounter::Scale    <<  " -  Counter status " << m_counter  << std::endl
    << "    ===> Channel is fired ? <" << m_fired << "> - ENABLE = " << m_enable<< std::endl
    << "    - Based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl
    << oscond.str();
  return s.str();


}

std::string LHCb::L0DUChannel::description(){

  std::stringstream oscond(" ");
  std::stringstream s(" ");
  for(LHCb::L0DUElementaryCondition::Map::iterator 
        icond = m_elementaryConditions.begin()  ;
      icond != m_elementaryConditions.end() ; icond++){
    std::stringstream os(" ");
    oscond << ((*icond).second)->description() << std::endl;
  }
  s << " " << std::endl
    << " -----  Channel (" << m_name   << " ) " << "> - ENABLE = " << m_enable 
    << " ------ Accept Rate = " << m_rate << "/" << LHCb::L0DUCounter::Scale    <<  std::endl
    << "    - Based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl
    << oscond.str();
  return s.str();


}

