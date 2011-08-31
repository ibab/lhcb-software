// $Id: STNoiseToolBase.cpp,v 1.3 2009-11-25 11:25:11 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

// standard
#include "gsl/gsl_math.h"

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

  // Use number of events per strip in noise calculations
  declareProperty("UseEventsPerStrip", m_evtsPerStrip = false);

  // Choose selected steps (for CCEScan)
  declareProperty("Steps", m_steps);
  
}

StatusCode ST::STNoiseToolBase::initialize() {

  StatusCode sc = ST::HistoToolBase::initialize();
  if (sc.isFailure()) return sc;
  m_evtNumber = 0;
  m_debug = msgLevel( MSG::DEBUG );

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
    std::string condPath = m_condPath + "/TELL1Board" + boost::lexical_cast<std::string>(TELL1SourceID);
    mgrSvc->registerCondition(this, condPath, &ST::STNoiseToolBase::cacheTELL1Parameters);
  }
  // attempt to re-cache geometry/bad channels
  mgrSvc->registerCondition( this,  const_cast<IGeometryInfo*>(this->tracker()->geometry()),
                             &ST::STNoiseToolBase::cacheStripStatus );

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

  debug() << "Resetting noise counters for " << TELL1SourceID << endmsg;
  m_rawPedestalMap[TELL1SourceID].resize(3072, 0.0);
  m_rawMeanMap[TELL1SourceID].resize(3072, 0.0);
  m_rawMeanSqMap[TELL1SourceID].resize(3072, 0.0);
  m_rawNoiseMap[TELL1SourceID].resize(3072, 0.0);
  m_rawNEventsPP[TELL1SourceID].resize(4,0);
    
  m_cmsMeanMap[TELL1SourceID].resize(3072, 0.0);
  m_cmsMeanSqMap[TELL1SourceID].resize(3072, 0.0);
  m_cmsNoiseMap[TELL1SourceID].resize(3072, 0.0);
  m_cmsNEventsPP[TELL1SourceID].resize(4,0);
    
  m_nEvents[TELL1SourceID].resize(3072, std::make_pair(0,0));

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
  if(m_readPedestals | m_readThresholds) {
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
  debug() << "Reading TELL1 parameters for " << TELL1SourceID << endmsg;
  m_pedestalMaps[TELL1SourceID].resize(1);
  std::string condPath = m_condPath + "/TELL1Board" + boost::lexical_cast<std::string>(TELL1SourceID);
  debug() << "Getting condition: " << condPath << endmsg;
  Condition* condition = getDet<Condition>(condPath);
  if(condition != 0) {
    if(m_readPedestals) {
      std::vector<int> pedestalValues = condition->param<std::vector<int> >("pedestal");
      std::vector<int>::iterator itPed = pedestalValues.begin();
      std::vector<std::pair<double, int> >* pedestals = &m_pedestalMaps[TELL1SourceID][0];
      for(; itPed != pedestalValues.end(); ++itPed) {
        pedestals->push_back(std::make_pair((*itPed), 1));
      }
    }
    if(m_readThresholds) {
      std::vector<int> hitThresholds = condition->param<std::vector<int> >("hit_threshold");
      std::vector<int> cmsThresholds = condition->param<std::vector<int> >("cms_threshold");
      std::vector<int>::iterator hitIt = hitThresholds.begin();
      std::vector<int>::iterator cmsIt = cmsThresholds.begin();
      std::vector< std::pair<double, double> >* thresholds = &m_thresholdMap[TELL1SourceID];
      for(; hitIt != hitThresholds.end(); ++hitIt, ++cmsIt) {
        thresholds->push_back(std::make_pair((*hitIt), (*cmsIt)));
      }
    }
  } else {
    error() << "No condition: " << condPath << endmsg;
  }
}
//======================================================================================================================
// Cache strip status / loop over all tell1, get sector+convert offline then get strip status
//======================================================================================================================
StatusCode ST::STNoiseToolBase::cacheStripStatus() {
  info() << "==> Caching strip status" << endreq;
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  int notOKAll = 0;
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int TELL1SourceID = (*itT).first;
    STTell1Board* board = readoutTool()->findByBoardID(STTell1ID(TELL1SourceID));
    std::vector<bool>* status = &m_statusMap[TELL1SourceID];
    status->resize(3072,false);
    std::vector<bool>::iterator itStat = status->begin();
    unsigned int strip = 0;
    int notOK=0;
    for(; itStat != status->end(); ++itStat, ++strip) {
      //      unsigned int link = static_cast<unsigned int>(strip/128);
      LHCb::STChannelID channelID = (board->DAQToOffline(0, STDAQ::v4, STDAQ::StripRepresentation(strip)).first);
      DeSTSector* sector = tracker()->findSector(channelID);
      if(sector != 0) {
        *itStat = m_maskBad ? sector->isOKStrip( channelID ) : true;
        if(! *itStat ) {
          notOK++;
          notOKAll++;
        }
        if(m_verbose) verbose() << detType()+"StripStatus\t" << (*itT).second << "\t" 
                                << strip << "\t" << sector->stripStatus( channelID ) << endreq;
      }
    }
    if(m_debug) {
      debug() << boost::to_upper_copy("--------------------------------> "+detType()+"tell") 
              << (*itT).second << "\t" << notOK << endreq;
      debug() << *status << endreq;
    }
  }
  if(m_debug) debug() << "Sum bad channels:\t" << notOKAll << endreq;

  return StatusCode::SUCCESS;
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

/// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseToolBase::pedestalEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawPedestalMap.find(TELL1SourceID) == m_rawPedestalMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawPedestalMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the pedestal value of the first channel for a given TELL1 source ID
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
( const unsigned int TELL1SourceID ) const
{
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

/// Return an iterator corresponding to the source ID of the first TELL1 in the event containing an NZS bank
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::tell1WithNZSBegin( ) const {
  return m_tell1WithNZS.begin();
}

/// Return an iterator corresponding to the source ID of the last TELL1 in the event containing an NZS bank
std::vector<unsigned int>::const_iterator ST::STNoiseToolBase::tell1WithNZSEnd( ) const {
  return m_tell1WithNZS.end();
}

/** Return an iterator corresponding to the number of events used in the noise calculations
    for the first channel of a given TELL1 source ID 
    - 1st is number of events used in RAW noise calculation after outlier removal
    - 2nd is number of events used in CMS noise calculation after outlier removal
**/
std::vector<std::pair<unsigned int, unsigned int> >::const_iterator ST::STNoiseToolBase::nEventsBegin
( const unsigned int TELL1SourceID ) const {
  if(m_nEvents.find(TELL1SourceID) == m_nEvents.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_nEvents.find(TELL1SourceID)->second.begin();
}

/** Return an iterator corresponding to the number of events used in the noise calculations
    for the last channel of a given TELL1 source ID 
    - 1st is number of events used in RAW noise calculation after outlier removal
    - 2nd is number of events used in CMS noise calculation after outlier removal
**/
std::vector<std::pair<unsigned int, unsigned int> >::const_iterator ST::STNoiseToolBase::nEventsEnd
( const unsigned int TELL1SourceID ) const {
  if(m_nEvents.find(TELL1SourceID) == m_nEvents.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_nEvents.find(TELL1SourceID)->second.end();
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

