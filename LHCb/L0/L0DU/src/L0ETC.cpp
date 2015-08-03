// from Gaudi
#include "GaudiKernel/IRegistry.h" 

// LHCb
#include "Event/MCHeader.h"
#include "Event/L0DUReport.h"

// local
#include "L0ETC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0ETC
//
// 2007-07-13 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0ETC )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0ETC::L0ETC( const std::string& name,
              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
    , m_events(0)
{
    declareProperty( "CollectionName", m_collectionName = "<not set>");
}
//=============================================================================
// Destructor
//=============================================================================
L0ETC::~L0ETC() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0ETC::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

    if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// @todo would be nice to get that from /Event or from L0DU.
  m_l0channels.push_back("L0electron_High");
  m_l0channels.push_back("L0electron_Low");
  m_l0channels.push_back("L0photon_High");
  m_l0channels.push_back("L0photon_Low");
  m_l0channels.push_back("L0localPi0_High");
  m_l0channels.push_back("L0localPi0_Low");
  m_l0channels.push_back("L0globalPi0_High");
  m_l0channels.push_back("L0globalPi0_Low");
  m_l0channels.push_back("L0hadron_High");
  m_l0channels.push_back("L0hadron_Low");
  m_l0channels.push_back("L0muon_High");
  m_l0channels.push_back("L0muon_Low");
  m_l0channels.push_back("L0dimuon_High");
  m_l0channels.push_back("L0dimuon_Low");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0ETC::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // always pass, will use selection on tag to reduce later...
  setFilterPassed(true);

  m_events++ ;
  
  // This is what it is about...
  Tuple tup = evtCol(1,m_collectionName);
  
  // pick up the location of the event --
  // this is what makes the tag collection a collection...
  DataObject* pObject = get<DataObject>("/Event");
  if (pObject!=0) {
    StatusCode sc = tup->column("Address", pObject->registry()->address() ); 
    if (!sc) return sc;
  } else {
    Error("    not able to retrieve IOpaqueAddress");
  }
  
  // Retrieve informations about event
  const LHCb::MCHeader *evtHeader = getIfExists<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
  if ( NULL != evtHeader ){
    tup->column("event", (int)evtHeader->evtNumber() ).ignore();
    tup->column("run",   (int)evtHeader->runNumber() ).ignore();
  } else {
    Warning("    not able to retrieve MCHeader").ignore();
    tup->column("event", -1 ).ignore();
    tup->column("run",   -1 ).ignore();
  }

  // get L0 result
  const LHCb::L0DUReport* L0 = getIfExists<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
  if ( NULL != L0 ){
    tup->column ( "L0",  L0->decision() ).ignore() ;
    for ( std::vector<std::string>::const_iterator c = m_l0channels.begin();
          c != m_l0channels.end(); ++c){
      if( msgLevel(MSG::VERBOSE) ) 
        verbose() << *c << " says " << L0->channelDecisionByName(*c) << endmsg ;
      tup->column ( (*c), L0->channelDecisionByName(*c)  ).ignore() ;
    }
  } else {
    Warning("    not able to retrieve L0DUReport").ignore();
    tup->column ( "L0", false ).ignore() ;
    for ( std::vector<std::string>::const_iterator c = m_l0channels.begin();
          c != m_l0channels.end(); ++c){
      tup->column ( (*c), false ).ignore() ;
    }
  }
  //  tup->write(); // do not write out. This is done by evtcol svc

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0ETC::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << "Number of events " << m_events << endmsg ;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
