// $Id: TESFilter.cpp,v 1.2 2005-01-28 13:18:04 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h" 

// Event model
#include "Event/GenHeader.h"

// local
#include "TESFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TESFilter
//
// 2003-04-30 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TESFilter>          s_factory ;
const        IAlgFactory& TESFilterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TESFilter::TESFilter( const std::string& name, ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) {

  this->declareProperty( "DroppedContainers", m_containers );
  this->declareProperty( "EventType", m_evtType = 0 );
}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode TESFilter::initialize() {

  if( 0 != m_containers.size() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "These containers will be removed";
    if( 0 != m_evtType ) msg << " for event type " << m_evtType;
    msg  << ": ";
    for( std::vector<std::string>::const_iterator it = m_containers.begin();
         it!= m_containers.end(); it++ ){
      msg << " " << *it;
    }
    msg << endmsg;
  }
  else
    Print( "No containers have been selected for filtering" );
  
  return GaudiAlgorithm::initialize();
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TESFilter::execute() {

  if( 0 == m_containers.size() ) return StatusCode::SUCCESS;
  
  if( 0 != m_evtType ) {
    GenHeader* genEvt = get<GenHeader>( GenHeaderLocation::Default );
    if( genEvt->evType() != m_evtType ) return StatusCode::SUCCESS;
  }

  for( std::vector<std::string>::const_iterator it = m_containers.begin();
                                                it!= m_containers.end(); it++ ){
    DataObject* myCont = 0;
    StatusCode sc = evtSvc()->retrieveObject( *it, myCont );
    if( sc.isFailure() ) {
      Warning( *it + " not found in TES" );
    }
    else {
      sc = evtSvc()->unregisterObject( *it );
      if( sc.isSuccess() ) { myCont->release(); }
    }
  }
  return StatusCode::SUCCESS;
};
