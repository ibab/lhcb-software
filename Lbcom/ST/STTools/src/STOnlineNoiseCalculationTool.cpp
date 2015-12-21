// Include files 

// STTELL1Event
#include "Event/STTELL1Data.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// local
#include "STOnlineNoiseCalculationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STOnlineNoiseCalculationTool
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace ST {
  DECLARE_TOOL_FACTORY( STOnlineNoiseCalculationTool )
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

  // Location of ADC values after pedestal subtraction
  declareSTConfigProperty("PedSubInputData", m_pedSubLocation, LHCb::STTELL1DataLocation::TTPedSubADCs);

  // Location of ADC values after the Common Mode Suppression
  declareSTConfigProperty("LCMSInputData",   m_LCMSLocation,   LHCb::STTELL1DataLocation::TTLCMSADCs);

  // Remove outliers from the CMS noise calculation
  declareProperty("RemoveOutliers", m_removeOutliers = true);

  m_evtsPerStrip = true;
  m_readPedestals = true;
  m_readThresholds = true;

}

StatusCode ST::STOnlineNoiseCalculationTool::initialize() {
  if( msgLevel(MSG::DEBUG) ) debug() << "initialize" << endmsg;
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
  
  // Get the RAW data
  const LHCb::STTELL1Datas* data = getIfExists<LHCb::STTELL1Datas>(m_dataLocation);
  if ( NULL == data ) {
    // Skip if there is no Tell1 data
    return StatusCode::SUCCESS;
  }
  if(data->empty()) return Warning("Data is empty "+m_dataLocation, StatusCode::SUCCESS, 10);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  // Get the pedestal substracted data
  const LHCb::STTELL1Datas* pedSub = get<LHCb::STTELL1Datas>(m_pedSubLocation);
  if(pedSub->empty()) return Warning("Data is empty "+m_pedSubLocation, StatusCode::SUCCESS, 10);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  // Get the LCMS data
  const LHCb::STTELL1Datas* lcms = getIfExists<LHCb::STTELL1Datas>(m_LCMSLocation);
  if ( NULL == lcms ) {
    // Skip if there is no Tell1 data
    return StatusCode::SUCCESS;
  }
  if(lcms->empty()) return Warning("Data is empty "+m_LCMSLocation, StatusCode::SUCCESS, 10);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  if(data->size() != lcms->size() && data->size() != pedSub->size() && lcms->size() != pedSub->size()) {
    return Error("Mis-match in size of data containers", StatusCode::SUCCESS, 10);
  }
  // loop over the data
  LHCb::STTELL1Datas::const_iterator iterFullData = data->begin(); 
  LHCb::STTELL1Datas::const_iterator iterLCMSData = lcms->begin(); 
  LHCb::STTELL1Datas::const_iterator iterPedSubData = pedSub->begin(); 
  for (; iterFullData != data->end(); ++iterFullData, ++iterLCMSData, ++iterPedSubData){

    // get the tell board and the data headers
    unsigned int tellID = (*iterFullData)->TELL1ID();
    // Limit to selected tell1s
    if ( m_selectedTells && 
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber(tellID))) {
      continue;
    }
    const LHCb::STTELL1Data::Data& dataValues = (*iterFullData)->data();
    unsigned int tellIDPedSub = (*iterPedSubData)->TELL1ID();
    const LHCb::STTELL1Data::Data& pedSubValues = (*iterPedSubData)->data();
    unsigned int tellIDLCMS = (*iterLCMSData)->TELL1ID();
    const LHCb::STTELL1Data::Data& lcmsValues = (*iterLCMSData)->data();
    if(tellID != tellIDLCMS && tellID != tellIDPedSub && tellIDLCMS != tellIDPedSub) {
      return Error("Data from different TELL1s", StatusCode::SUCCESS, 10);
    }
    // Store tell1s with an NZS bank
    m_tell1WithNZS.push_back(tellID);

    // Local vectors for given TELL1
    std::vector<unsigned int>* nEvents = &m_nEvents[tellID];

    std::vector<double>* rawPedestal = &m_rawPedestalMap[tellID];
    std::vector<double>* rawMean = &m_rawMeanMap[tellID];
    std::vector<double>* rawMeanSq = &m_rawMeanSqMap[tellID];
    std::vector<double>* rawNoise = &m_rawNoiseMap[tellID];
    std::vector<unsigned int>* rawNEventsPP = &m_rawNEventsPP[tellID];

    std::vector<double>* cmsMean = &m_cmsMeanMap[tellID];
    std::vector<double>* cmsMeanSq = &m_cmsMeanSqMap[tellID];
    std::vector<double>* cmsNoise = &m_cmsNoiseMap[tellID];
    std::vector<unsigned int>* cmsNEventsPP = &m_cmsNEventsPP[tellID];

    std::vector<double>* pedSubMean = &m_pedSubMeanMap[tellID];
    std::vector<double>* pedSubMeanSq = &m_pedSubMeanSqMap[tellID];
    std::vector<double>* pedSubNoise = &m_pedSubNoiseMap[tellID];
    std::vector<unsigned int>* pedSubNEventsPP = &m_pedSubNEventsPP[tellID];

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
      (*rawNEventsPP)[pp]++;
      (*cmsNEventsPP)[pp]++;
      (*pedSubNEventsPP)[pp]++;
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
            const int pedSubValue = pedSubValues[beetle][iStrip];
            bool updateRaw=true; 
            bool updatePedSub=true;
            bool updateCMS=true;
            if(m_removeOutliers) { // only update the plots if lcms value < cms_threshold
              //            if(fabs(rawValue-(*pedestals)[strip].first) > (*thresholds)[strip].first) updateRaw=false;
              //              if(fabs(static_cast<double>(pedSubValue)) > (*thresholds)[strip].first) updatePedSub=false;
              if(fabs(static_cast<double>(lcmsValue)) > (*thresholds)[strip].first) updateCMS=false;
            }
            // Calculate the pedestal and the pedestal squared
            // Cumulative average up to m_followingPeriod; after that exponential moving average
            if( updateRaw && updatePedSub && updateCMS ) { // only update the noise if both criteria are fulfilled
              (*nEvents)[strip] += 1;
              int nEvt = (*nEvents)[strip];
              if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) {
                nEvt = m_followingPeriod;
                (*nEvents)[strip] = m_followingPeriod;
              }
              // raw noise
              (*rawMean)[strip] = ((*rawMean)[strip]*(nEvt-1) + rawValue ) / nEvt;
              (*rawPedestal)[strip] = (*rawMean)[strip];
              (*rawMeanSq)[strip] = ((*rawMeanSq)[strip]*(nEvt-1) + std::pow(rawValue,2) ) / nEvt;
              (*rawNoise)[strip] = sqrt( (*rawMeanSq)[strip] - std::pow((*rawMean)[strip],2) );
              // pedestal subtracted noise
              (*pedSubMean)[strip] = ((*pedSubMean)[strip]*(nEvt-1) + pedSubValue ) / nEvt;
              (*pedSubMeanSq)[strip] = ((*pedSubMeanSq)[strip]*(nEvt-1) + std::pow(pedSubValue,2) ) / nEvt;
              (*pedSubNoise)[strip] = sqrt( (*pedSubMeanSq)[strip] - std::pow((*pedSubMean)[strip],2) );
              // cms noise
              (*cmsMean)[strip] = ((*cmsMean)[strip]*(nEvt-1) + lcmsValue ) / nEvt;
              (*cmsMeanSq)[strip] = ((*cmsMeanSq)[strip]*(nEvt-1) + std::pow(lcmsValue,2) ) / nEvt;
              (*cmsNoise)[strip] = sqrt( (*cmsMeanSq)[strip] - std::pow((*cmsMean)[strip],2) );
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
