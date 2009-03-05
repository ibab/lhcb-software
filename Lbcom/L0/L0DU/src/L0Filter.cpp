// $Id: L0Filter.cpp,v 1.4 2009-03-05 15:32:45 odescham Exp $
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
  : GaudiAlgorithm ( name , pSvcLocator )
  ,m_count(0)
  ,m_sel(0)
{
  m_l0channels.clear();
  declareProperty("L0DULocation", m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("OrChannels", m_l0channels );
  declareProperty("TriggerBit", m_trig = "L0" );

}
//=============================================================================
// Destructor
//=============================================================================
L0Filter::~L0Filter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Filter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  if( "Timing" == m_trig){
    info() << "Will require that L0DU 'Timing Trigger Bit' is fired " << endreq;
  }else if( "Force" == m_trig) { 
    info() << "Will require that the L0DU 'Force Bit' is fired " << endreq;
  }else if( "L0" == m_trig){
    info() << "Will require that any " ;
    if ( !m_l0channels.empty()) {
      info() << "of " ;
      for ( std::vector<std::string>::const_iterator c = m_l0channels.begin() ; 
            c != m_l0channels.end() ; ++c ) {
        info() << *c << " " ;
      }
    }
    info() << "L0 channel(s) is passed" << endmsg ;
  }
  else{
    error() << "The 'TriggerBit' property must be either : " << endreq;
    error() << "   - 'L0'     : standard L0DU decision " <<endreq;
    error() << "   - 'Timing' : L0DU timing trigger bit decision" <<endreq;
    error() << "   - 'Force'  : L0DU force bit decision " <<endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Filter::execute() {

  debug() << "==> Execute" << endmsg;
  m_count++;
  const  LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>(m_l0Location);

  setFilterPassed(false); // switch off by default

  // Timing Trigger decision
  if( "Timing" == m_trig  ){
    if( l0->timingTriggerBit() )setFilterPassed(true);
    return StatusCode::SUCCESS;
  }
  // Force decision
  if( "Force" == m_trig  ){
    if( l0->forceBit() )setFilterPassed(true);
    return StatusCode::SUCCESS;
  }
  
  if( "L0" != m_trig)return StatusCode::FAILURE;
  // standard L0DU decision
  if (l0->decision()){ 
    if ( !m_l0channels.empty()) {
      for ( std::vector<std::string>::const_iterator c = m_l0channels.begin() ; 
            c != m_l0channels.end() ; ++c ) {
        if (l0->channelDecisionByName(*c)){
          setFilterPassed(true);
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by " 
                                                 << *c << endmsg ;
        } else if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is not accepted by " 
                                                      << *c << endmsg ; 
      }
    } else { // any decision will do
      if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by L0" << endmsg ;
      setFilterPassed(true);
    }
  } else {
    setFilterPassed(false);
    verbose() << "Event is rejected by L0" << endmsg ;
  }
  if ( msgLevel(MSG::DEBUG)){
    if ( filterPassed()) debug() << "Event is accepted" << endmsg ;
    else debug() << "Event is rejected" << endmsg ;
  }
  if(filterPassed())m_sel++;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Filter::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "Filtering : " << m_sel << " events among " << m_count << " processed " << endreq;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
