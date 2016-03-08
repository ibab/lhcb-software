// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUChannel.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0DUChannel Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------


LHCb::L0DUChannel* LHCb::L0DUChannel::emulate() {

  if(m_emulated)return this; // emulation already processed
  
  // Init
  // Is the channel triggered (And'ed elementary conditions)
  m_emulatedPreDecision =  !m_elementaryConditions.empty();
  for(const auto& cond : m_elementaryConditions ) {
    m_emulatedPreDecision = m_emulatedPreDecision && cond.second->emulatedValue() ;
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
  m_emulatedDecision = (m_emulatedPreDecision && m_accept);
  m_emulated = true;
  return this;
}

std::string LHCb::L0DUChannel::summary() const {
  std::ostringstream s(" ");
  s << " " << std::endl
    << " <=====  Channel (" << m_name   << " ) " 
    << "  ================> <** CHANNEL EMULATED DECISION **> = <"<< m_emulatedDecision << ">" << std::endl
    << "    ===> Channel PreDecision  ? = <" << m_emulatedPreDecision 
    << "> - DecisionType = " << L0DUDecision::Name[m_decisionType] << std::endl
    << "    ===> Downscale decision : <" << isDownscaled() << "> ( Accept Rate = " << m_rate << "/" 
    << LHCb::L0DUCounter::Scale    
    << " -  Counter status " << m_counter  <<")"<< std::endl
    << "    - Based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl;
  for(const auto& cond : m_elementaryConditions ) {
    s << cond.second->summary() << std::endl;
  }
  return s.str();
}

std::string LHCb::L0DUChannel::description() const {
  std::ostringstream s(" ");
  s << " " << std::endl
    << " -----  Channel ('" << L0DUDecision::Name[m_decisionType] << "|" << m_name   << "' ) " 
    << " ------ Accept Rate = " << m_rate << "/" << LHCb::L0DUCounter::Scale    <<  std::endl
    << "    - Based on " << m_elementaryConditions.size() << " Elementary Condition(s)  : " << std::endl;
  for(const auto& cond : m_elementaryConditions ) {
    s << cond.second->description() << std::endl;
  }
  return s.str();


}

