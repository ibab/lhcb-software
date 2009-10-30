// $Id: STNoiseCalculationToolBase.cpp,v 1.1 2009-10-30 12:59:47 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// STTELL1Event
#include "Event/STTELL1Data.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

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
    m_rawMeanMap[TELL1SourceID].resize(3072, 0.0);
    m_rawMeanSqMap[TELL1SourceID].resize(3072, 0.0);
    m_rawNoiseMap[TELL1SourceID].resize(3072, 0.0);
    m_rawNEvents[TELL1SourceID].resize(4,0);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
ST::STNoiseCalculationToolBase::~STNoiseCalculationToolBase() {} 

/// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationToolBase::rawNoiseBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
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

/// Return an iterator corresponding to the RAW mean ADC value for the first channel for a given TELL1 source ID
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

/// Return an iterator corresponding to the RAW mean squared ADC value for the first channel for a given TELL1 source ID
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
