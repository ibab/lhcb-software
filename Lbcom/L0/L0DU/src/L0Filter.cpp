// $Id: L0Filter.cpp,v 1.8 2010-01-29 12:49:27 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/L0DUReport.h"

// local
#include "L0Filter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0Filter
//
// 2007-07-11 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Filter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Filter::L0Filter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator )
  ,m_count(0)
  ,m_sel(0)
{
  m_l0channels.clear();
  declareProperty("L0DULocation", m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("OrChannels"  , m_l0channels );
  declareProperty("OrSubTriggers"  , m_l0triggers );
  declareProperty("TriggerBit"  , m_trig = "L0" ); // decision bit (L0/TTB/FB)
  declareProperty("L0DecisionMask", m_mask = LHCb::L0DUDecision::Physics ); //L0 decision type (Physics/Beam1/Beam2)
  declareProperty("Revert"        , m_revert = false );
}
//=============================================================================
// Destructor
//=============================================================================
L0Filter::~L0Filter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Filter::initialize() {
  StatusCode sc = L0AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  if( "Timing" == m_trig){
    info() << "Will require that L0DU 'Timing Trigger Bit' is fired " << endmsg;
  }else if( "Force" == m_trig) { 
    info() << "Will require that the L0DU 'Force Bit' is fired " << endmsg;
  }else if( "L0" == m_trig){
    info() << "Will require that : " << endmsg;
    if ( !m_l0channels.empty()) {
      info() << " - any of : [" ;
      for ( std::vector<std::string>::const_iterator c = m_l0channels.begin() ; 
            c != m_l0channels.end() ; ++c ) {
        info() << "'" << *c << "' " ;
      }
      info() << "] L0 channel(s) is passed "<< endmsg;
      if ( !m_l0triggers.empty()) info() << " OR " << endmsg;
    }

    if ( !m_l0triggers.empty()) {
      info() << " - any of : [" ;
      for ( std::vector<std::string>::const_iterator c = m_l0triggers.begin() ; 
            c != m_l0triggers.end() ; ++c ) {
        info() << *c << " " ;
      } 
      info() << "] L0 trigger(s) is passed "<< endmsg;
    } 

    if( m_l0triggers.empty() || m_l0channels.empty() )
      info() << " - trigger decision type " << LHCb::L0DUDecision::Name[ m_mask ] << " is passed " << endmsg;

  }
  else{
    error() << "The 'TriggerBit' property must be either : " << endmsg;
    error() << "   - 'L0'     : standard L0DU decision " <<endmsg;
    error() << "   - 'Timing' : L0DU timing trigger bit decision" <<endmsg;
    error() << "   - 'Force'  : L0DU force bit decision " <<endmsg;
    return StatusCode::FAILURE;
  }

  std::string def = ( m_revert ) ? "REJECT" : "ACCEPT" ;
  info() << "---- > TO " << def << " THE EVENT" << endreq;

  return StatusCode::SUCCESS;
} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Filter::execute() {

  debug() << "==> Execute" << endmsg;
  m_count++;

  bool accept = m_revert ? false : true;
  setFilterPassed( !accept ); // switch off by default
  std::string acc = m_revert ? "rejected " : "accepted";
  std::string rej = m_revert ?  "accepted" : "rejected " ;
  std::string loc = dataLocation( m_l0Location );


  if( !exist<LHCb::L0DUReport>( loc)){
    Warning("L0DUReport not found at location : the event is " + rej + loc ).ignore();
    counter("Report not found L0Filter reject") += 1;
    return StatusCode::SUCCESS;
  }

  const  LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>( loc );

  

  // Timing Trigger decision
  if( "Timing" == m_trig  ){
    if( l0->timingTriggerBit() )setFilterPassed( accept );
  }
  
  // Force decision
  if( "Force" == m_trig  ){
    if( l0->forceBit() )setFilterPassed( accept );
  }

  if( "L0" != m_trig)return StatusCode::FAILURE;

  // standard L0DU decision
  if ( !m_l0channels.empty()) {
    for ( std::vector<std::string>::const_iterator c = m_l0channels.begin() ; 
          c != m_l0channels.end() ; ++c ) {
      if (l0->channelDecisionByName(*c)){
        setFilterPassed( accept );
        if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by " 
                                               << *c << " Channel" << endmsg ;
      } else if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is not accepted by "
                                                    << *c << " Channel" << endmsg ; 
    }
  }

  if ( !m_l0triggers.empty()) {
    for ( std::vector<std::string>::const_iterator c = m_l0triggers.begin() ; 
          c != m_l0triggers.end() ; ++c ) {
      if (l0->triggerDecisionByName(*c)){
        setFilterPassed( accept );
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by " 
                                                 << *c << " SubTrigger" << endmsg ;
      } else if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is not accepted by " 
                                                    << *c << " SubTrigger" << endmsg ; 
    }
  }  

  if ( m_l0triggers.empty() && m_l0channels.empty() && l0->decision(m_mask)){
    if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by L0 decision " 
                                           << LHCb::L0DUDecision::Name[m_mask] <<endmsg ;
    setFilterPassed( accept );
  } else {
    setFilterPassed( !accept );
    verbose() << "Event is rejected by L0 decision " 
              << LHCb::L0DUDecision::Name[m_mask] <<endmsg ;
  }
  


  if ( filterPassed()){
    counter("L0Filter accept") += 1;
    if ( msgLevel(MSG::DEBUG))debug() << "Event is " << acc << endmsg ;
  }else{
    counter("L0Filter reject") += 1;
    if ( msgLevel(MSG::DEBUG))debug() << "Event is " << rej  << endmsg ;
  }
  if(filterPassed())m_sel++; 

  return StatusCode::SUCCESS; 
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Filter::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "Filtering : " << m_sel << " events among " << m_count << " processed " << endmsg;
  return L0AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
