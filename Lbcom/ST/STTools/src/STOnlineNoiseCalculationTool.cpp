// $Id: STOnlineNoiseCalculationTool.cpp,v 1.2 2009-11-25 11:25:11 mtobin Exp $
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
#include "STOnlineNoiseCalculationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STOnlineNoiseCalculationTool
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace ST {
  DECLARE_TOOL_FACTORY( STOnlineNoiseCalculationTool );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STOnlineNoiseCalculationTool::STOnlineNoiseCalculationTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : ST::STNoiseToolBase ( type, name , parent )
{
  declareInterface<ST::ISTNoiseCalculationTool>(this);

  // Location of ADC values after the Common Mode Suppression
  declareSTConfigProperty("LCMSInputData" , m_LCMSLocation,
                          LHCb::STTELL1DataLocation::TTLCMSADCs);

  // Remove outliers from the CMS noise calculation
  declareProperty("RemoveOutliers", m_removeOutliers = true);

  m_evtsPerStrip = true;
  m_readPedestals = true;
  m_readThresholds = true;

}

StatusCode ST::STOnlineNoiseCalculationTool::initialize() {
  debug() << "intialize" << endmsg;
  StatusCode sc = ST::STNoiseToolBase::initialize();
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
ST::STOnlineNoiseCalculationTool::~STOnlineNoiseCalculationTool() {} 

//=============================================================================
StatusCode ST::STOnlineNoiseCalculationTool::calculateNoise() {
  m_evtNumber++;
  
  // Skip if there is no Tell1 data
  if (!exist<LHCb::STTELL1Datas>(m_dataLocation)) {
    return StatusCode::SUCCESS;
  }

  // Skip if there is no Tell1 data
  if (!exist<LHCb::STTELL1Datas>(m_LCMSLocation)) {
    return StatusCode::SUCCESS;
  }

  // Get the data
  const LHCb::STTELL1Datas* data = get<LHCb::STTELL1Datas>(m_dataLocation);
  if(data->empty()) return Warning("Data is empty "+m_dataLocation, StatusCode::SUCCESS, 10);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  // Get the data
  const LHCb::STTELL1Datas* lcms = get<LHCb::STTELL1Datas>(m_LCMSLocation);
  if(lcms->empty()) return Warning("Data is empty "+m_LCMSLocation, StatusCode::SUCCESS, 10);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  if(data->size() != lcms->size()) return Error("Mis-match in size of RAW and LCMS data", StatusCode::SUCCESS, 10);

  // loop over the data
  LHCb::STTELL1Datas::const_iterator iterFullData = data->begin(); 
  LHCb::STTELL1Datas::const_iterator iterLCMSData = lcms->begin(); 
  for (; iterFullData != data->end(); ++iterFullData, ++iterLCMSData){

    // get the tell board and the data headers
    unsigned int tellID = (*iterFullData)->TELL1ID();
    // Limit to selected tell1s
    if ( m_selectedTells && 
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber(tellID))) {
      continue;
    }
    const LHCb::STTELL1Data::Data& dataValues = (*iterFullData)->data();
    unsigned int tellIDLCMS = (*iterLCMSData)->TELL1ID();
    const LHCb::STTELL1Data::Data& lcmsValues = (*iterLCMSData)->data();
    if(tellID != tellIDLCMS) return Error("Different TELL1s", StatusCode::SUCCESS, 10);
    // Store tell1s with an NZS bank
    m_tell1WithNZS.push_back(tellID);

    // Local vectors for given TELL1
    std::vector<double>* rawPedestal = &m_rawPedestalMap[tellID];
    std::vector<double>* rawMean = &m_rawMeanMap[tellID];
    std::vector<double>* rawMeanSq = &m_rawMeanSqMap[tellID];
    std::vector<double>* rawNoise = &m_rawNoiseMap[tellID];
    std::vector<unsigned int>* nEventsPP = &m_rawNEventsPP[tellID];

    std::vector<double>* cmsMean = &m_cmsMeanMap[tellID];
    std::vector<double>* cmsMeanSq = &m_cmsMeanSqMap[tellID];
    std::vector<double>* cmsNoise = &m_cmsNoiseMap[tellID];
    std::vector<unsigned int>* cmsNEventsPP = &m_cmsNEventsPP[tellID];

    std::vector<std::pair<unsigned int, unsigned int> >* nEvents = &m_nEvents[tellID];

    // Local copy of pedestal and thresholds used on the TELL1
    //    std::vector<std::pair<double, int> >* pedestals = &m_pedestalMaps[tellID][0];
    std::vector<std::pair<double, double> >* thresholds = &m_thresholdMap[tellID];
    std::vector<bool>* stripStatus = &m_statusMap[tellID];
    // Loop over the PPs that have sent data
    std::vector<unsigned int> sentPPs = (*iterFullData)->sentPPs();
    std::vector<unsigned int>::const_iterator iPP = sentPPs.begin();
    for( ; iPP != sentPPs.end(); ++iPP ) {
      unsigned int pp = *iPP;
      
      // Count the number of events per PP
      (*nEventsPP)[pp]++;
      (*cmsNEventsPP)[pp]++;
      if(m_countRoundRobin) this->countRoundRobin(tellID, pp);

      // Loop over the links (i.e. Beetles)
      unsigned int iBeetle = 0;
      for ( ; iBeetle < STDAQ::nBeetlesPerPPx; ++iBeetle ){
        unsigned int beetle = iBeetle + pp*STDAQ::nBeetlesPerPPx;
        
        // Loop over the strips in this link
        unsigned int iStrip = 0;
        for ( ; iStrip < LHCbConstants::nStripsInBeetle ; ++iStrip){
          
          int strip = iStrip + beetle * LHCbConstants::nStripsInBeetle;
          if((*stripStatus)[strip]) {
            const int rawValue = dataValues[beetle][iStrip];
            const int lcmsValue = lcmsValues[beetle][iStrip];
            bool updateRaw=true; 
            bool updateCMS=true;
            if(m_removeOutliers) {
              //            if(fabs(rawValue-(*pedestals)[strip].first) > (*thresholds)[strip].first) updateRaw=false;
              if(fabs(static_cast<double>(lcmsValue)) > (*thresholds)[strip].second) updateCMS=false;
            }
            // Calculate the pedestal and the pedestal squared
            if(updateRaw) {
              (*nEvents)[strip].first += 1;
              // Cumulative average up to m_followingPeriod; after that exponential moving average
              int nEvt = (*nEvents)[strip].first;
              if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) 
                nEvt = m_followingPeriod;
              (*rawMean)[strip] = ((*rawMean)[strip]*(nEvt-1) + rawValue ) / nEvt;
              (*rawPedestal)[strip] = (*rawMean)[strip];
              (*rawMeanSq)[strip] = ((*rawMeanSq)[strip]*(nEvt-1) + gsl_pow_2(rawValue) ) / nEvt;
              (*rawNoise)[strip] = sqrt( (*rawMeanSq)[strip] - gsl_pow_2((*rawMean)[strip]) );
            }
            if(updateCMS) {
              (*nEvents)[strip].second += 1;
              // Cumulative average up to m_followingPeriod; after that exponential moving average
              int nEvt = (*nEvents)[strip].second;
              if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) 
                nEvt = m_followingPeriod;
              (*cmsMean)[strip] = ((*cmsMean)[strip]*(nEvt-1) + lcmsValue ) / nEvt;
              (*cmsMeanSq)[strip] = ((*cmsMeanSq)[strip]*(nEvt-1) + gsl_pow_2(lcmsValue) ) / nEvt;
              (*cmsNoise)[strip] = sqrt( (*cmsMeanSq)[strip] - gsl_pow_2((*cmsMean)[strip]) );
            }
          }// stripStatus
        } // strip
      }  // beetle
      
      // Resets the event counter
//       if( m_resetRate > 0  && nEvt%m_resetRate == 0 ) {
//         (*nEventsPP)[pp] = 0;
//       }
    } // FPGA-PP
  } // boards
  return StatusCode::SUCCESS;
}

std::vector<double> ST::STOnlineNoiseCalculationTool::rawMean(const unsigned int TELL) const {
  return m_rawMeanMap.find(TELL)->second;
}
std::vector<double> ST::STOnlineNoiseCalculationTool::rawMeanSq(const unsigned int TELL) const {
  return m_rawMeanSqMap.find(TELL)->second;
}
std::vector<double> ST::STOnlineNoiseCalculationTool::rawNoise(const unsigned int TELL) const {
  return m_rawNoiseMap.find(TELL)->second;
}
std::vector<unsigned int> ST::STOnlineNoiseCalculationTool::rawN(const unsigned int TELL) const {
  return m_rawNEventsPP.find(TELL)->second;
}

std::vector<double> ST::STOnlineNoiseCalculationTool::cmsMean(const unsigned int TELL) const {
  return m_rawMeanMap.find(TELL)->second;
}
std::vector<double> ST::STOnlineNoiseCalculationTool::cmsMeanSq(const unsigned int TELL) const {
  return m_rawMeanSqMap.find(TELL)->second;
}
std::vector<double> ST::STOnlineNoiseCalculationTool::cmsNoise(const unsigned int TELL) const {
  return m_rawNoiseMap.find(TELL)->second;
}
std::vector<unsigned int> ST::STOnlineNoiseCalculationTool::cmsN(const unsigned int TELL) const {
  return m_rawNEventsPP.find(TELL)->second;
}
