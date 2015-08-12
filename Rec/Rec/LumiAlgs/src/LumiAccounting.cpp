// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiCounters.h"
#include "Event/LumiMethods.h"

// CondDB
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"

// local
#include "LumiAccounting.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiAccounting
//
// 2009-01-19 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiAccounting )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiAccounting::LumiAccounting( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fileRecordSvc(NULL),
    m_lumiFSRs(NULL),
    m_lumiFSR(NULL),
    m_current_fname(""),
    m_count_files(0),
    m_condThresholds(NULL),
    m_calibThresholds(),
    m_statusThresholds(0)
{
  declareProperty( "RawEventLocation"  ,  m_rawEventLocation = LHCb::RawEventLocation::Default );
  declareProperty( "InputDataContainer",  m_DataName = LHCb::HltLumiSummaryLocation::Default );
  // it is assumed that we are only called for a single BXType and that the
  // output data container gets this name
  declareProperty( "OutputDataContainer", m_FSRName = LHCb::LumiFSRLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
LumiAccounting::~LumiAccounting() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiAccounting::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);

  // file counting 
  m_current_fname = "";
  m_count_files = 0;

  // prepare TDS for FSR
  m_lumiFSRs = new LHCb::LumiFSRs();
  m_lumiFSR = 0;
  put(m_fileRecordSvc, m_lumiFSRs, m_FSRName);
  // create a new FSR and append to TDS
  m_lumiFSR = new LHCb::LumiFSR();
  m_lumiFSRs->insert(m_lumiFSR);

  // initialize calibration factors
  for ( int key = 0; key <= LHCb::LumiCounters::Random; key++ ) {
    std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
    m_calibThresholds.push_back(0);
  }

  // get the detectorDataSvc
  m_dds = detSvc();
  if (m_dds == NULL) {
    error() << "No thresholds defined for lumi counters (no dds)" << endmsg ;
    m_statusThresholds = 0;         // no database
    return StatusCode::SUCCESS;
  }

  // register conditions for database acces
  sc = registerDB(); // must be executed first
  if ( sc.isFailure() ) {
    error() << "No thresholds defined for lumi counters (no registration)" << endmsg ;
    m_statusThresholds = 0;         // no database
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiAccounting::execute() {

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

  // get data container
  if( !exist<LHCb::HltLumiSummary>(m_DataName) ){
    // not sure that this is an error - physics events do not have the lumi bank
    return Warning("LumiSummary cannot be loaded", StatusCode::SUCCESS);
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "LumiSummary loaded!!" << endmsg;
  LHCb::HltLumiSummary* the_hltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

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
      m_lumiFSR = new LHCb::LumiFSR();
      m_lumiFSRs->insert(m_lumiFSR);
      */
    }
  }

  // insert new fileID only
  if ( !m_lumiFSR->hasFileID(fname) ) m_lumiFSR->addFileID(fname);
  // insert new run only
  if ( !m_lumiFSR->hasRunNumber(run) ) m_lumiFSR->addRunNumber(run);
  // increment FSR with summary info
  LHCb::HltLumiSummary::ExtraInfo::iterator summaryIter;
  LHCb::HltLumiSummary::ExtraInfo  summaryInfo = the_hltLumiSummary->extraInfo();
  for (summaryIter = summaryInfo.begin(); summaryIter != summaryInfo.end(); summaryIter++) {
    // get the key and value of the input info
    int key = summaryIter->first;
    int value = summaryIter->second;
    // increment!
    m_lumiFSR->incrementInfo(key, value);
    // check if over threshold and increment with offset
    double threshold = m_calibThresholds[key];
    int binary = value > threshold ? 1 : 0 ;
    m_lumiFSR->incrementInfo(key + LHCb::LumiMethods::PoissonOffset, binary);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiAccounting::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // some printout of FSRs
  if ( msgLevel(MSG::INFO) ) {
    info() << "number of files seen: " << m_count_files << endmsg;
    // FSR - use the class method which prints it
    LHCb::LumiFSRs::iterator fsr;
    LHCb::LumiFSR sumFSR;
    for ( fsr = m_lumiFSRs->begin(); fsr != m_lumiFSRs->end(); fsr++ ) {
      // sum up the information
      sumFSR += *(*fsr);
      // print the individual FSR
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "FSR: " << *(*fsr) << endmsg; 
    }
    // print the integral
    info() << "INTEGRAL: " << sumFSR << endmsg; 
  }

  // check if the FSRs can be retrieved from the TS
  if ( msgLevel(MSG::DEBUG) ) {
    LHCb::LumiFSRs* readFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, m_FSRName);
    LHCb::LumiFSRs::iterator fsr;
    for ( fsr = readFSRs->begin(); fsr != readFSRs->end(); fsr++ ) {
      // print the FSR just retrieved from TS
      debug() << "READ FSR: " << *(*fsr) << endmsg; 

      // print also the contents using the builtin lookup tables
      LHCb::LumiFSR::ExtraInfo::iterator infoIter;
      LHCb::LumiFSR::ExtraInfo  fsrInfo = (*fsr)->extraInfo();
      for (infoIter = fsrInfo.begin(); infoIter != fsrInfo.end(); infoIter++) {
        // get the key and value of the input info
        int key = infoIter->first;
        LHCb::LumiFSR::ValuePair values = infoIter->second;
        int incr = values.first;
        longlong count = values.second;
        const std::string keyName = LHCb::LumiCounters::counterKeyToString(key);
        int keyInt = LHCb::LumiCounters::counterKeyToType(keyName);
        debug() << "SUM: key: " << key 
                << " name: " << keyName << " KeyInt: " << keyInt 
                << " increment: " << incr << " integral: " << count << endmsg;
      }
    }
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=============================================================================
// DB access
//=============================================================================
StatusCode LumiAccounting::registerDB() {
  // register the DB conditions for the update maganer
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Register DB" << endmsg;

  // register thresholds
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/ThresholdCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/ThresholdCalibration",
                      m_condThresholds, &LumiAccounting::i_cacheThresholdData);
  }
  else {
    error() << "Conditions/Lumi/LHCb/ThresholdCalibration not found" << endmsg;
    m_statusThresholds = 0;        // no thresholds
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from relativeCalibration
//=========================================================================
StatusCode LumiAccounting::i_cacheThresholdData() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "callback ThresholdCalibration:" << endmsg;
  std::vector<double> cal = m_condThresholds->paramVect<double>("Thresholds");
  if ( cal.size() == m_calibThresholds.size() ) {
    m_calibThresholds = cal;
    return StatusCode::SUCCESS;
  }
  fatal() << "inconsistent number of parameters in RelativeCalibration:" << cal.size() << endmsg;
  m_statusThresholds = 0;        // no thresholds
  return StatusCode::SUCCESS;

}
