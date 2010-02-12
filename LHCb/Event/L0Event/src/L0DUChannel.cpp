// $Id: L0DUChannel.cpp,v 1.8 2010-02-12 23:41:48 odescham Exp $
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
  //bool accept = false ;  
  if( m_updateCounter){
    if( m_emulatedPreDecision )m_counter += m_rate;
    if( m_counter >= LHCb::L0DUCounter::Scale){
      m_accept = true ;
      m_counter -= LHCb::L0DUCounter::Scale; //cyclic counter
    }else{
      m_accept = false;
    } 
  }
  
  // the downscaling procedure takes the final decision
  m_emulatedDecision = false ;
  if(m_emulatedPreDecision && m_accept)m_emulatedDecision = true;
  m_emulated = true;
  return this;
}

std::string LHCb::L0DUChannel::summary(){

  std::ostringstream oscond(" ");
  std::ostringstream s(" ");
  for(LHCb::L0DUElementaryCondition::Map::iterator 
        icond = m_elementaryConditions.begin()  ;
      icond != m_elementaryConditions.end() ; icond++){
    std::ostringstream os(" ");
    oscond << ((*icond).second)->summary() << std::endl;
  }
  s << " " << std::endl
    << " <=====  Channel (" << m_name   << " ) " 
    << "  ================> <** CHANNEL EMULATED DECISION **> = <"<< m_emulatedDecision << ">" << std::endl
    << "    ===> Channel PreDecision  ? = <" << m_emulatedPreDecision 
    << "> - DecisionType = " << L0DUDecision::Name[m_decisionType] << std::endl
    << "    ===> Downscale decision : <" << isDownscaled() << "> ( Accept Rate = " << m_rate << "/" 
    << LHCb::L0DUCounter::Scale    
    << " -  Counter status " << m_counter  <<")"<< std::endl
    << "    - Based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl
    << oscond.str();
  return s.str();


}

std::string LHCb::L0DUChannel::description(){

  std::ostringstream oscond(" ");
  std::ostringstream s(" ");
  for(LHCb::L0DUElementaryCondition::Map::iterator 
        icond = m_elementaryConditions.begin()  ;
      icond != m_elementaryConditions.end() ; icond++){
    std::ostringstream os(" ");
    oscond << ((*icond).second)->description() << std::endl;
  }
  s << " " << std::endl
    << " -----  Channel ('" << L0DUDecision::Name[m_decisionType] << "|" << m_name   << "' ) " 
    << " ------ Accept Rate = " << m_rate << "/" << LHCb::L0DUCounter::Scale    <<  std::endl
    << "    - Based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl
    << oscond.str();
  return s.str();


}

