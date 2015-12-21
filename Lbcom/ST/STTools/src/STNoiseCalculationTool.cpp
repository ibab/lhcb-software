// Include files 

// STTELL1Event
#include "Event/STTELL1Data.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// local
#include "STNoiseCalculationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STNoiseCalculationTool
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace ST {
  DECLARE_TOOL_FACTORY( STNoiseCalculationTool )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STNoiseCalculationTool::STNoiseCalculationTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : ST::STNoiseToolBase ( type, name , parent )
{
  declareInterface<ST::ISTNoiseCalculationTool>(this);

}

StatusCode ST::STNoiseCalculationTool::initialize() {
  if( msgLevel(MSG::DEBUG) ) debug() << "initialize" << endmsg;
  StatusCode sc = ST::STNoiseToolBase::initialize();
  if (sc.isFailure()) return sc;

  // CMS maps not used in this tool so clear.
  m_cmsMeanMap.clear();
  m_cmsMeanSqMap.clear();
  m_cmsNoiseMap.clear();
  m_cmsNEventsPP.clear();
    

  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
ST::STNoiseCalculationTool::~STNoiseCalculationTool() {} 

//=============================================================================
StatusCode ST::STNoiseCalculationTool::calculateNoise() {
  m_evtNumber++;
  
  // Get the data
  const LHCb::STTELL1Datas* data = getIfExists<LHCb::STTELL1Datas>(m_dataLocation);
  if ( NULL == data ) {
    // Skip if there is no Tell1 data
    return StatusCode::SUCCESS;
  }
  if(data->empty()) return Warning("Data is empty", StatusCode::SUCCESS, 10);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  // loop over the data
  LHCb::STTELL1Datas::const_iterator iterBoard = data->begin(); 
  for (; iterBoard != data->end() ; ++iterBoard){

    // get the tell board and the data headers
    unsigned int tellID = (*iterBoard)->TELL1ID();
    const LHCb::STTELL1Data::Data& dataValues = (*iterBoard)->data();

    // Store tell1s with an NZS bank
    m_tell1WithNZS.push_back(tellID);
    // Local vectors for given TELL1
    std::vector<double>* pedestalTELL = &m_rawPedestalMap[tellID];
    std::vector<double>* meanTELL = &m_rawMeanMap[tellID];
    std::vector<double>* meanSqTELL = &m_rawMeanSqMap[tellID];
    std::vector<double>* noiseTELL = &m_rawNoiseMap[tellID];
    std::vector<unsigned int>* nEventsPP = &m_rawNEventsPP[tellID];

    // Loop over the PPs that have sent data
    std::vector<unsigned int> sentPPs = (*iterBoard)->sentPPs();
    std::vector<unsigned int>::const_iterator iPP = sentPPs.begin();
    for( ; iPP != sentPPs.end(); ++iPP ) {
      unsigned int pp = *iPP;
      
      // Count the number of events per PP
      (*nEventsPP)[pp]++;
      if(m_countRoundRobin) this->countRoundRobin(tellID, pp);

      // Cumulative average up to m_followingPeriod; after that
      // exponential moving average
      int nEvt = (*nEventsPP)[pp];
      if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) 
        nEvt = m_followingPeriod;
    
      // Loop over the links (i.e. Beetles)
      unsigned int iBeetle = 0;
      for ( ; iBeetle < STDAQ::nBeetlesPerPPx; ++iBeetle ){
        unsigned int beetle = iBeetle + pp*STDAQ::nBeetlesPerPPx;
        
        // Loop over the strips in this link
        unsigned int iStrip = 0;
        for ( ; iStrip < LHCbConstants::nStripsInBeetle ; ++iStrip){
          
          // Get the ADC value
          const int value = dataValues[beetle][iStrip];
          
          // Calculate the pedestal and the pedestal squared
          int strip = iStrip + beetle * LHCbConstants::nStripsInBeetle;
          (*meanTELL)[strip] = ((*meanTELL)[strip]*(nEvt-1) + value ) / nEvt;
          (*pedestalTELL)[strip] = (*meanTELL)[strip];
          (*meanSqTELL)[strip] = ((*meanSqTELL)[strip]*(nEvt-1) + std::pow(value,2) ) / nEvt;
          (*noiseTELL)[strip] = sqrt( (*meanSqTELL)[strip] - std::pow((*meanTELL)[strip],2) );
        } // strip
      }  // beetle
      
      // Resets the event counter
      if( m_resetRate > 0  && nEvt%m_resetRate == 0 ) {
        (*nEventsPP)[pp] = 0;
      }
    } // FPGA-PP
  } // boards
  return StatusCode::SUCCESS;
}

/// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationTool::cmsNoiseBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationTool::cmsNoiseEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawNoiseMap.find(TELL1SourceID) == m_rawNoiseMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNoiseMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationTool::cmsMeanBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanMap.find(TELL1SourceID) == m_rawMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationTool::cmsMeanEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanMap.find(TELL1SourceID) == m_rawMeanMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationTool::cmsMeanSquaredBegin( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanSqMap.find(TELL1SourceID) == m_rawMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanSqMap.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
std::vector<double>::const_iterator ST::STNoiseCalculationTool::cmsMeanSquaredEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawMeanSqMap.find(TELL1SourceID) == m_rawMeanSqMap.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawMeanSqMap.find(TELL1SourceID)->second.end();
}

/// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseCalculationTool::cmsNEventsPPBegin
( const unsigned int TELL1SourceID ) const {
  if(m_rawNEventsPP.find(TELL1SourceID) == m_rawNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNEventsPP.find(TELL1SourceID)->second.begin();
}

/// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
std::vector<unsigned int>::const_iterator ST::STNoiseCalculationTool::cmsNEventsPPEnd( const unsigned int TELL1SourceID ) const {
  if(m_rawNEventsPP.find(TELL1SourceID) == m_rawNEventsPP.end()) {
    error() << "This should never happen! Did you pass TELLID rather than source ID? " << TELL1SourceID << endmsg;
  }
  return m_rawNEventsPP.find(TELL1SourceID)->second.end();
}

std::vector<double> ST::STNoiseCalculationTool::rawMean(const unsigned int TELL) const {
  return m_rawMeanMap.find(TELL)->second;
}
std::vector<double> ST::STNoiseCalculationTool::rawMeanSq(const unsigned int TELL) const {
  return m_rawMeanSqMap.find(TELL)->second;
}
std::vector<double> ST::STNoiseCalculationTool::rawNoise(const unsigned int TELL) const {
  return m_rawNoiseMap.find(TELL)->second;
}
std::vector<unsigned int> ST::STNoiseCalculationTool::rawN(const unsigned int TELL) const {
  return m_rawNEventsPP.find(TELL)->second;
}

std::vector<double> ST::STNoiseCalculationTool::cmsMean(const unsigned int TELL) const {
  return m_rawMeanMap.find(TELL)->second;
}
std::vector<double> ST::STNoiseCalculationTool::cmsMeanSq(const unsigned int TELL) const {
  return m_rawMeanSqMap.find(TELL)->second;
}
std::vector<double> ST::STNoiseCalculationTool::cmsNoise(const unsigned int TELL) const {
  return m_rawNoiseMap.find(TELL)->second;
}
std::vector<unsigned int> ST::STNoiseCalculationTool::cmsN(const unsigned int TELL) const {
  return m_rawNEventsPP.find(TELL)->second;
}
