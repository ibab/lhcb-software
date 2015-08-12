// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/TimeSpanFSR.h"

// local
#include "TimeAccounting.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TimeAccounting
//
// 2009-01-19 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TimeAccounting )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TimeAccounting::TimeAccounting( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fileRecordSvc(NULL),
    m_DataName(""),
    m_timeSpanFSRs(NULL),
    m_timeSpanFSR(NULL),
    m_current_fname(""),
    m_count_files(0)
{
  declareProperty( "RawEventLocation"  ,  m_rawEventLocation = LHCb::RawEventLocation::Default );
  declareProperty( "OutputDataContainer", m_FSRName = LHCb::TimeSpanFSRLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
TimeAccounting::~TimeAccounting() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TimeAccounting::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);

  // file counting 
  m_current_fname = "";
  m_count_files = 0;

  // prepare TDS for FSR
  m_timeSpanFSRs = new LHCb::TimeSpanFSRs();
  m_timeSpanFSR = 0;
  put(m_fileRecordSvc, m_timeSpanFSRs, m_FSRName);
  // create a new FSR and append to TDS
  m_timeSpanFSR = new LHCb::TimeSpanFSR();
  m_timeSpanFSRs->insert(m_timeSpanFSR);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TimeAccounting::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // registry from raw data - only correct if file catalogue used 
  std::string event_fname;
  if( !exist<LHCb::RawEvent>(m_rawEventLocation) ){
    return Warning("RawBank cannot be loaded", StatusCode::SUCCESS);
  } else {
    LHCb::RawEvent* event = get<LHCb::RawEvent>(m_rawEventLocation);
    IOpaqueAddress* eAddr = event->registry()->address();
    // obtain the fileID
    if ( eAddr ) {
      event_fname = eAddr->par()[0];
      if ( msgLevel(MSG::DEBUG) ) debug() << "RunInfo record from Event: " << event_fname << endmsg;
    } else {
      return Error("Registry cannot be loaded from Event", StatusCode::SUCCESS);
    }
  }

  // get ODIN
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    // should remain flagged as an error and stop the run
    return Error("ODIN cannot be loaded", StatusCode::FAILURE);
  }
  // obtain the run number from ODIN
  unsigned int run = odin->runNumber();  
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN RunNumber: " << run << endmsg;

  // check if the file ID is new
  std::string fname = "";
  if ( fname != event_fname ) {
    fname = event_fname;
    if ( fname != m_current_fname ) {
      // this is a new file
      m_count_files++;
      m_current_fname = fname;
      if ( msgLevel(MSG::DEBUG) ) debug() << "RunInfo record: " << fname << endmsg;
      /* This code is now in initialize:
      // create a new FSR and append to TDS
      m_timeSpanFSR = new LHCb::TimeSpanFSR();
      m_timeSpanFSRs->insert(m_timeSpanFSR);
      */
    }
  }
  // get the event time
  ulonglong eventTime = odin->gpsTime();
  // enter new event time
  (*m_timeSpanFSR) += eventTime;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TimeAccounting::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // some printout of FSRs
  if ( msgLevel(MSG::INFO) ) {
    info() << "number of files seen: " << m_count_files << endmsg;
    // FSR - use the class method which prints it
    LHCb::TimeSpanFSRs::iterator fsr;
    for ( fsr = m_timeSpanFSRs->begin(); fsr != m_timeSpanFSRs->end(); fsr++ ) {
      // print the individual FSR
      info() << "FSR: " << *(*fsr) << endmsg; 
    }
  }

  // check if the FSRs can be retrieved from the TS
  if ( msgLevel(MSG::DEBUG) ) {
    LHCb::TimeSpanFSRs* readFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, m_FSRName);
    LHCb::TimeSpanFSRs::iterator fsr;
    for ( fsr = readFSRs->begin(); fsr != readFSRs->end(); fsr++ ) {
      // print the FSR just retrieved from TS
      debug() << "READ FSR: " << *(*fsr) << endmsg; 
    }
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

