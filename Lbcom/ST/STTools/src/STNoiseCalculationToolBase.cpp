// $Id: STNoiseCalculationToolBase.cpp,v 1.3 2009-11-25 11:25:11 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// STTELL1Event
#include "Event/STTELL1Data.h"

// ODIN
#include "Event/ODIN.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

#include "AIDA/IHistogram2D.h"

// standard
#include "gsl/gsl_math.h"

// local
#include "STNoiseCalculationToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STNoiseCalculationToolBase
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// namespace ST {
//   DECLARE_TOOL_FACTORY( STNoiseCalculationToolBase );
// }

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STNoiseCalculationToolBase::STNoiseCalculationToolBase( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : ST::HistoToolBase ( type, name , parent )
{
  // Limit calculation to vector of tell1s given in terms of TELLID (eg TTTELL1 = 1) 
  declareProperty("LimitToTell",      m_limitToTell     ); 

  // Count number of round robin events per PP
  declareProperty("CountRoundRobin", m_countRoundRobin = false);
}

StatusCode ST::STNoiseCalculationToolBase::initialize() {

  StatusCode sc = ST::HistoToolBase::initialize();
  if (sc.isFailure()) return sc;
  m_evtNumber = 0;
  // Reset the maps for each tell1
  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int TELL1SourceID = (*itT).first;
    m_rawPedestalMap[TELL1SourceID].resize(3072, 0.0);
    m_rawMeanMap[TELL1SourceID].resize(3072, 0.0);
    m_rawMeanSqMap[TELL1SourceID].resize(3072, 0.0);
    m_rawNoiseMap[TELL1SourceID].resize(3072, 0.0);
    m_rawNEvents[TELL1SourceID].resize(4,0);
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

//=============================================================================
// Destructor
//=============================================================================
ST::STNoiseCalculationToolBase::~STNoiseCalculationToolBase() {} 

//======================================================================================================================
//
// Noise calculation
//
//======================================================================================================================
StatusCode ST::STNoiseCalculationToolBase::updateNoise() {
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
    if(newRun) {
      // TO DO: reset counters at the start of a run
    }
    m_tell1WithNZS.clear();
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
void ST::STNoiseCalculationToolBase::countRoundRobin(unsigned int TELL1SourceID, unsigned int PP) {
  m_2d_nEventsPerPP->fill(readoutTool()->SourceIDToTELLNumber(TELL1SourceID), PP);
}
//======================================================================================================================
//
// Data Accessors
//
//======================================================================================================================
/// Return an iterator corresponding to the pedestal value of the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::pedestalBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawPedestalMap.find(TELL1SourceID) == m_rawPedestalMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawPedestalMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::pedestalEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawPedestalMap.find(TELL1SourceID) == m_rawPedestalMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawPedestalMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the pedestal value of the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawNoiseBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW RMS noise on the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawNoiseEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the RAW mean ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawMeanBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanMap.find(TELL1SourceID) == m_rawMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW mean ADC value for the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawMeanEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanMap.find(TELL1SourceID) == m_rawMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the RAW mean squared ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawMeanSquaredBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanSqMap.find(TELL1SourceID) == m_rawMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanSqMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW mean squared ADC value for the last channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawMeanSquaredEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanSqMap.find(TELL1SourceID) == m_rawMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanSqMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseCalculationToolBase::rawNEventsBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawNEvents.find(TELL1SourceID) == m_rawNEvents.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNEvents.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseCalculationToolBase::rawNEventsEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawNEvents.find(TELL1SourceID) == m_rawNEvents.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNEvents.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the source ID of the first TELL1 in the event containing an NZS bank
std::vector<unsigned int>::const_iterator ST::STNoiseCalculationToolBase::tell1WithNZSBegin( ) const {
  return m_tell1WithNZS.begin();
}

/// Return an iterator corresponding to the source ID of the last TELL1 in the event containing an NZS bank
std::vector<unsigned int>::const_iterator ST::STNoiseCalculationToolBase::tell1WithNZSEnd( ) const {
  return m_tell1WithNZS.end();
}

