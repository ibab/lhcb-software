// $Id: L0DUChannel.cpp,v 1.2 2006-07-31 13:57:07 cattanem Exp $
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


void LHCb::L0DUChannel::process(){
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
  if( m_fired )m_counter += m_acceptRate;
  if( m_counter >= LHCb::L0DUCounter::Scale){
    accept = true ;
    m_counter -= LHCb::L0DUCounter::Scale; 
  }
  // the downscaling procedure takes the final decision
  m_decision = false ;
  if(m_fired && accept)m_decision = true;
}

std::string LHCb::L0DUChannel::summary(){

  std::stringstream oscond(" ");
  std::stringstream s(" ");
  for(LHCb::L0DUElementaryCondition::Map::const_iterator 
        icond = m_elementaryConditions.begin()  ;
      icond != m_elementaryConditions.end() ; icond++){
    std::stringstream os(" ");
    oscond << (*icond).second->summary() << std::endl;
  }
  s << " " << std::endl
    << " -----  Channel (" << m_name   << " ) " << " ---------------" <<std::endl
    << " Channel is fired ? <" << m_fired << "> - "  
    << " Accept Rate = " << m_acceptRate << "/" << LHCb::L0DUCounter::Scale 
    <<  " -  Counter " << m_counter 
    << "  <** CHANNEL DECISION **> = <"<< m_decision << ">"<< std::endl
    << " based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl
    << oscond.str();
  return s.str();


}

