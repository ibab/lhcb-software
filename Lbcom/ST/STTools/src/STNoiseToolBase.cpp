// Include files 

// STTELL1Event
#include "Event/STTELL1Data.h"
#include "Kernel/STTell1Board.h"

// ODIN
#include "Event/ODIN.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// Detector description for access to conditions
#include "DetDesc/Condition.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"


#include "AIDA/IHistogram2D.h"

#include "boost/algorithm/string.hpp"

// local
#include "STNoiseToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STNoiseToolBase
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// namespace ST {
//   DECLARE_TOOL_FACTORY( STNoiseToolBase );
// }

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STNoiseToolBase::STNoiseToolBase( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : ST::HistoToolBase ( type, name , parent )
{
  // Location of input data used in raw noise calculation
  declareSTConfigProperty("InputData" , m_dataLocation,
                          LHCb::STTELL1DataLocation::TTFull);

  /* Calculation of noise is cumulative mean up to following period 
  and then exponential moving average after */
  declareProperty("FollowPeriod", m_followingPeriod = 2000);

  // Reset rate for counters used in noise calculation
  declareProperty("ResetRate", m_resetRate = -1);  

  // Skip events to allow time for the pedestals to be calculated
  declareProperty("SkipEvents", m_skipEvents = -1 );

  // Limit calculation to vector of tell1s given in terms of TELLID (eg TTTELL1 = 1) 
  declareProperty("LimitToTell",      m_limitToTell     ); 

  // Count number of round robin events per PP
  declareProperty("CountRoundRobin", m_countRoundRobin = false);

  // Read pedestals from conditions database
  declareProperty("PedestalsFromDB", m_readPedestals = false);

  // Read hit thresholds from conditions database
  declareProperty("ThresholdsFromDB", m_readThresholds = false);

  // Set the path for the conditions database
  declareProperty("CondPath", m_condPath = "CondDB");

  // Mask known bad channels
  declareProperty("MaskBadChannels", m_maskBad = true);

  // Read masked channels from conditions database
  declareProperty("PedestalMaskFromDB", m_readPedestalMask = true);

  // Use number of events per strip in noise calculations
  declareProperty("UseEventsPerStrip", m_evtsPerStrip = false);

  // Choose selected steps (for CCEScan)
  declareProperty("Steps", m_steps);
  
  // Reset counters after a run change
  declareProperty("ResetAfterRunChange", m_resetRunChange=true);
  
}

StatusCode ST::STNoiseToolBase::initialize() {

  StatusCode sc = ST::HistoToolBase::initialize();
  if (sc.isFailure()) return sc;
  m_evtNumber = 0;
  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE );
  if(m_debug) m_verbose = true;
  // Select small number of TELL1s
  m_selectedTells = false;
  if ( m_limitToTell.size() > 0 ) {
    m_selectedTells = true;
    std::sort(m_limitToTell.begin(), m_limitToTell.end());
  }
  m_selectedSteps = false;
  if(m_steps.size() > 0) {
    m_selectedSteps = true;
    std::sort(m_steps.begin(), m_steps.end());
  }
  // Get the tell1 mapping from source ID to tell1 number
  IUpdateManagerSvc* mgrSvc=svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int TELL1SourceID = (*itT).first;
    resetNoiseCounters(TELL1SourceID);
    std::string condPath = m_condPath + "/TELL1Board" + std::to_string(TELL1SourceID);
    mgrSvc->registerCondition(this, condPath, &ST::STNoiseToolBase::cacheTELL1Parameters);
  }
  StatusCode mgrSvcStatus=mgrSvc->update(this);
  if(mgrSvcStatus.isFailure()){
    return ( Error("Failed first UMS update", mgrSvcStatus) );
  }

  m_firstEvent = true;
  m_eventNumber = 0;
  m_runNumber = 0;
  m_tell1WithNZS.clear();
  if(m_countRoundRobin) {
    unsigned int m_nTELL1s = readoutTool()->nBoard();
    m_2d_nEventsPerPP = book2D("Number of NZS banks sent per PP", 0.5, m_nTELL1s+0.5, m_nTELL1s,-0.5, 3.5, 4);
  }
  return StatusCode::SUCCESS;
}

// Reset all noise counters (@ initialise or after change in conditions)
void ST::STNoiseToolBase::resetNoiseCounters( const unsigned int TELL1SourceID ){

  info() << "Resetting noise counters for " << TELL1SourceID << endmsg;
  m_nEvents[TELL1SourceID].assign(3072, 0);
  m_rawPedestalMap[TELL1SourceID].assign(3072, 0.0);
  m_rawMeanMap[TELL1SourceID].assign(3072, 0.0);
  m_rawMeanSqMap[TELL1SourceID].assign(3072, 0.0);
  m_rawNoiseMap[TELL1SourceID].assign(3072, 0.0);
  m_rawNEventsPP[TELL1SourceID].assign(4,0);
    
  m_cmsMeanMap[TELL1SourceID].assign(3072, 0.0);
  m_cmsMeanSqMap[TELL1SourceID].assign(3072, 0.0);
  m_cmsNoiseMap[TELL1SourceID].assign(3072, 0.0);
  m_cmsNEventsPP[TELL1SourceID].assign(4,0);

  m_pedSubMeanMap[TELL1SourceID].assign(3072, 0.0);
  m_pedSubMeanSqMap[TELL1SourceID].assign(3072, 0.0);
  m_pedSubNoiseMap[TELL1SourceID].assign(3072, 0.0);
  m_pedSubNEventsPP[TELL1SourceID].assign(4,0);

}

// Reset all noise counters (after change in conditions)
void ST::STNoiseToolBase::resetNoiseCounters(){
  info() << "Resetting all noise counters" << endmsg;
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int TELL1SourceID = (*itT).first;
    resetNoiseCounters(TELL1SourceID);
  }  
}

//=============================================================================
// Destructor
//=============================================================================
ST::STNoiseToolBase::~STNoiseToolBase() {} 

//==============================================================================
// Read+cache TELL1 parameters from conditions data base
//==============================================================================
StatusCode ST::STNoiseToolBase::cacheTELL1Parameters() {
  info() << "==> Caching TELL1 parameters" << endmsg;
  // initialise condition updates
  if(m_readPedestals | m_readThresholds | m_readPedestalMask) {
    std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
    for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
      unsigned int TELL1SourceID = (*itT).first;
      resetNoiseCounters(TELL1SourceID);
      readTELL1Parameters(TELL1SourceID);
    }
  }
  return StatusCode::SUCCESS;
}
void ST::STNoiseToolBase::readTELL1Parameters(const unsigned int TELL1SourceID) {
  if( m_debug ) debug() << "----------> Reading TELL1 parameters for " << TELL1SourceID << endmsg;
  m_pedestalMaps[TELL1SourceID].resize(1);
  std::string condPath = m_condPath + "/TELL1Board" + std::to_string(TELL1SourceID);
  if( m_debug ) debug() << "Getting condition: " << condPath << endmsg;
  Condition* condition = getDet<Condition>(condPath);
  if(condition != 0) {
    if(m_readPedestals) {
      std::vector<int> pedestalValues = condition->param<std::vector<int> >("pedestal");
      std::vector<int>::iterator itPed = pedestalValues.begin();
      std::vector<std::pair<double, int> >* pedestals = &m_pedestalMaps[TELL1SourceID][0];
      pedestals->clear();
      for(; itPed != pedestalValues.end(); ++itPed) {
        pedestals->push_back(std::make_pair((*itPed), 1));
      }
      if(m_verbose) verbose() << "caching pedestals: "+detType()+"TELL" 
                              << (this->readoutTool())->SourceIDToTELLNumber(TELL1SourceID) 
                              << "\t" << (*pedestals) << std::endl;
    }
    if(m_readThresholds) {
      std::vector<int> hitThresholds = condition->param<std::vector<int> >("hit_threshold");
      std::vector<int> cmsThresholds = condition->param<std::vector<int> >("cms_threshold");
      std::vector<int>::iterator hitIt = hitThresholds.begin();
      std::vector<int>::iterator cmsIt = cmsThresholds.begin();
      std::vector< std::pair<double, double> >* thresholds = &m_thresholdMap[TELL1SourceID];
      thresholds->clear();
      for(; hitIt != hitThresholds.end(); ++hitIt, ++cmsIt) {
        thresholds->push_back(std::make_pair((*hitIt), (*cmsIt)));
      }
      if(m_verbose) verbose() << "caching thresholds: "+detType()+"TELL" 
                              << (this->readoutTool())->SourceIDToTELLNumber(TELL1SourceID) 
                              << "\t" << (*thresholds) << std::endl;
    }
    if(m_readPedestalMask) {// status: value is 1 if strip is ok, 0 if strip is not ok
      std::vector<int> pedestalMasks = condition->param<std::vector<int> >("pedestal_mask");
      std::vector<int>::iterator itMask = pedestalMasks.begin();
      std::vector<bool>* status = &m_statusMap[TELL1SourceID];
      //      std::vector<bool>::iterator itStatus = status->begin();
      status->assign(3072,false);
      std::vector<bool>::iterator itStat = status->begin();
      unsigned int channel = 0;
      STTell1Board* board = readoutTool()->findByBoardID(STTell1ID(TELL1SourceID));
      for(; itMask != pedestalMasks.end(); ++itMask, ++itStat, ++channel) { // check channel is valid
        LHCb::STChannelID channelID = (board->DAQToOffline(0, STDAQ::v4, STDAQ::StripRepresentation(channel)).first);
        DeSTSector* sector = tracker()->findSector(channelID);
        if(sector != 0) {
          *itStat = m_maskBad ? !static_cast<bool>((*itMask)) : true;
        }
      }
      if(m_verbose) verbose() << "caching pedestal masks: "+detType()+"TELL" 
                              << (this->readoutTool())->SourceIDToTELLNumber(TELL1SourceID)
                              << "\t statusMap" << (*status) << endmsg;
    }
  } else {
    error() << "No condition: " << condPath << endmsg;
  }
}

//======================================================================================================================
//
// Noise calculation
//
//======================================================================================================================
StatusCode ST::STNoiseToolBase::updateNoise() {
  // get event number, run number, then test that this is a new event.
  bool newEvent=false;
  bool newRun=false;
  const LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default); 
  const ulonglong eventNumber = odin->eventNumber();
  const unsigned int runNumber = odin->runNumber();
  if(m_firstEvent) {
    m_firstEvent = false;
    newEvent = true;
  } else {
    newRun = (m_runNumber != runNumber);
    newEvent = (m_eventNumber != eventNumber);
  }
  if(newEvent || newRun) {
    m_eventNumber = eventNumber;
    m_runNumber = runNumber;
    m_tell1WithNZS.clear();
    if(m_selectedSteps) {
      const unsigned int step = odin->calibrationStep();
      if(!binary_search(m_steps.begin(), m_steps.end(), step)) return StatusCode::SUCCESS;
    }
    if(m_resetRunChange && newRun) resetNoiseCounters();
    this->calculateNoise();
  } else {
    return Warning( "You should only call updateNoise once per event" , StatusCode::SUCCESS , 0);
  }
  return StatusCode::SUCCESS;
}
//======================================================================================================================
//
// Count number of events in Round Robin mode
//
//======================================================================================================================
void ST::STNoiseToolBase::countRoundRobin(unsigned int TELL1SourceID, unsigned int PP) {
  m_2d_nEventsPerPP->fill(readoutTool()->SourceIDToTELLNumber(TELL1SourceID), PP);
}
//======================================================================================================================
//
// Data Accessors
//
//======================================================================================================================
/// Return an iterator corresponding to the pedestal value of the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::pedestalBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawPedestalMap.find(TELL1SourceID) == m_rawPedestalMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawPedestalMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the pedestal value of the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::pedestalEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawPedestalMap.find(TELL1SourceID) == m_rawPedestalMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawPedestalMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::rawNoiseBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW RMS noise on the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::rawNoiseEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the RAW mean ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::rawMeanBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanMap.find(TELL1SourceID) == m_rawMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW mean ADC value for the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::rawMeanEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanMap.find(TELL1SourceID) == m_rawMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the RAW mean squared ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::rawMeanSquaredBegin
( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanSqMap.find(TELL1SourceID) == m_rawMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanSqMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW mean squared ADC value for the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::rawMeanSquaredEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanSqMap.find(TELL1SourceID) == m_rawMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanSqMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::rawNEventsPPBegin( const unsigned int TELL1SourceID ) 
  const {
  if(m_rawNEventsPP.find(TELL1SourceID) == m_rawNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNEventsPP.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::rawNEventsPPEnd
( const unsigned int TELL1SourceID ) const {
  if(m_rawNEventsPP.find(TELL1SourceID) == m_rawNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNEventsPP.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::cmsNoiseBegin( const unsigned int TELL1SourceID ) const {
  if(m_cmsNoiseMap.find(TELL1SourceID) == m_cmsNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsNoiseMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::cmsNoiseEnd( const unsigned int TELL1SourceID ) const {
  if(m_cmsNoiseMap.find(TELL1SourceID) == m_cmsNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsNoiseMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::cmsMeanBegin( const unsigned int TELL1SourceID ) const {
  if(m_cmsMeanMap.find(TELL1SourceID) == m_cmsMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsMeanMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::cmsMeanEnd( const unsigned int TELL1SourceID ) const {
  if(m_cmsMeanMap.find(TELL1SourceID) == m_cmsMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsMeanMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::cmsMeanSquaredBegin( const unsigned int TELL1SourceID ) const {
  if(m_cmsMeanSqMap.find(TELL1SourceID) == m_cmsMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsMeanSqMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::cmsMeanSquaredEnd( const unsigned int TELL1SourceID ) const {
  if(m_cmsMeanSqMap.find(TELL1SourceID) == m_cmsMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsMeanSqMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::cmsNEventsPPBegin
( const unsigned int TELL1SourceID ) const {
  if(m_cmsNEventsPP.find(TELL1SourceID) == m_cmsNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsNEventsPP.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::cmsNEventsPPEnd
( const unsigned int TELL1SourceID ) const {
  if(m_cmsNEventsPP.find(TELL1SourceID) == m_cmsNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_cmsNEventsPP.find(TELL1SourceID)->second.end();
}

/** Return an iterator corresponding to the RMS noise after pedestal subtraction on the first
    channel for a given TELL1 source ID **/
std::vector<double>::const_iterator ST::STNoiseToolBase::pedSubNoiseBegin(const unsigned int TELL1SourceID ) const {
  if(m_pedSubNoiseMap.find(TELL1SourceID) == m_pedSubNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubNoiseMap.find(TELL1SourceID)->second.begin();
}

/** Return an iterator corresponding to the RMS noise after pedestal subtraction on the last 
    channel for a given TELL1 source ID **/
std::vector<double>::const_iterator ST::STNoiseToolBase::pedSubNoiseEnd(const unsigned int TELL1SourceID ) const {
  if(m_pedSubNoiseMap.find(TELL1SourceID) == m_pedSubNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubNoiseMap.find(TELL1SourceID)->second.end();
}

/** Return an iterator corresponding to the mean ADC value after pedestal subtraction for the 
    first channel for a given TELL1 source ID **/
std::vector<double>::const_iterator ST::STNoiseToolBase::pedSubMeanBegin( const unsigned int TELL1SourceID ) const {
  if(m_pedSubMeanMap.find(TELL1SourceID) == m_pedSubMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubMeanMap.find(TELL1SourceID)->second.begin();
}

/** Return an iterator corresponding to the mean ADC value after pedestal subtraction for the last 
    channel for a given TELL1 source ID **/
std::vector<double>::const_iterator ST::STNoiseToolBase::pedSubMeanEnd( const unsigned int TELL1SourceID ) const {
  if(m_pedSubMeanMap.find(TELL1SourceID) == m_pedSubMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubMeanMap.find(TELL1SourceID)->second.end();
}

/** Return an iterator corresponding to the mean squared ADC value after pedestal subtraction for the first 
    channel for a given TELL1 source ID **/
std::vector<double>::const_iterator ST::STNoiseToolBase::pedSubMeanSquaredBegin( const unsigned int TELL1SourceID ) const {
  if(m_pedSubMeanSqMap.find(TELL1SourceID) == m_pedSubMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubMeanSqMap.find(TELL1SourceID)->second.begin();
}

/** Return an iterator corresponding to the mean squared ADC value after pedestal subtraction for the last 
    channel for a given TELL1 source ID **/
std::vector<double>::const_iterator ST::STNoiseToolBase::pedSubMeanSquaredEnd( const unsigned int TELL1SourceID ) const {
  if(m_pedSubMeanSqMap.find(TELL1SourceID) == m_pedSubMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubMeanSqMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::pedSubNEventsPPBegin( const unsigned int TELL1SourceID ) const {
  if(m_pedSubNEventsPP.find(TELL1SourceID) == m_pedSubNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubNEventsPP.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::pedSubNEventsPPEnd( const unsigned int TELL1SourceID ) const {
  if(m_pedSubNEventsPP.find(TELL1SourceID) == m_pedSubNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_pedSubNEventsPP.find(TELL1SourceID)->second.end();
}

/** Return an iterator corresponding to the number of events used in the calculation of noise (RAW, CMS, pedSub)
    after outlier removal for the first channel of a given TELL1 source ID **/
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::nEventsBegin( const unsigned int TELL1SourceID ) const {
  if(m_nEvents.find(TELL1SourceID) == m_nEvents.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_nEvents.find(TELL1SourceID)->second.begin();
}

/** Return an iterator corresponding to the number of events used in the calculation of noise (RAW, CMS, pedSub)
    after outlier removal for the last channel of a given TELL1 source ID **/
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::nEventsEnd( const unsigned int TELL1SourceID ) const {
  if(m_nEvents.find(TELL1SourceID) == m_nEvents.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_nEvents.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the source ID of the first TELL1 in the event containing an NZS bank
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::tell1WithNZSBegin( ) const {
  return m_tell1WithNZS.begin();
}

/// Return an iterator corresponding to the source ID of the last TELL1 in the event containing an NZS bank
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::tell1WithNZSEnd( ) const {
  return m_tell1WithNZS.end();
}

/// Return an iterator corresponding to the status of the first channel for a given TELL1 source ID
std::vector<bool>::const_iterator ST::STNoiseToolBase::stripStatusBegin( const unsigned int TELL1SourceID ) const {
  if(m_statusMap.find(TELL1SourceID) == m_statusMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_statusMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the status of the last channel for a given TELL1 source ID
std::vector<bool>::const_iterator ST::STNoiseToolBase::stripStatusEnd( const unsigned int TELL1SourceID ) const {
  if(m_statusMap.find(TELL1SourceID) == m_statusMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_statusMap.find(TELL1SourceID)->second.end();
}
