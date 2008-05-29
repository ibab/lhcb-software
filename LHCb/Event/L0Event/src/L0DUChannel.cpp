// $Id: L0DUChannel.cpp,v 1.6 2008-05-29 14:10:07 odescham Exp $
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
  m_emulatedPreDecision = true;
  // Is the channel triggered (And'ed elementary conditions)
  if( 0 == m_elementaryConditions.size() )m_emulatedPreDecision = false;
  for(LHCb::L0DUElementaryCondition::Map::iterator icond = m_elementaryConditions.begin() ;
      icond != m_elementaryConditions.end() ; icond++ ){
    m_emulatedPreDecision =  m_emulatedPreDecision && (*icond).second->emulatedValue() ;
  }
  // Cyclic counter ('downscaling' procedure)
  bool accept = false ;  
  if( m_emulatedPreDecision)m_counter += m_rate;
  if( m_counter >= LHCb::L0DUCounter::Scale){
    accept = true ;
    m_counter -= LHCb::L0DUCounter::Scale; //cyclic counter
  }
  // the downscaling procedure takes the final decision
  m_emulatedDecision = false ;
  if(m_emulatedPreDecision && accept)m_emulatedDecision = true;
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
    << "  ================> <** CHANNEL EMULATED DECISION **> = <"<< m_emulatedDecision << ">" << std::endl
    << "    ===> Channel PreDecision  ? = <" << m_emulatedPreDecision << "> - ENABLE = " << m_enable<< std::endl
    << "    ===> Is downscaled ? = <" << isDownscaled() << "> ( Accept Rate = " << m_rate << "/" << LHCb::L0DUCounter::Scale    
    << " -  Counter status " << m_counter  <<")"<< std::endl
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

