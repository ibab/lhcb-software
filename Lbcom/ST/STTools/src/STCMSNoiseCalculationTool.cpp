// Include files 

// STTELL1Event
#include "Event/STTELL1Data.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// standard
#include <bitset>
#include <cmath>

// STTELL1Event
#include "Event/STTELL1Data.h"

// local
#include "STCMSNoiseCalculationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STCMSNoiseCalculationTool
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace ST {
  DECLARE_TOOL_FACTORY( STCMSNoiseCalculationTool )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STCMSNoiseCalculationTool::STCMSNoiseCalculationTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : ST::STNoiseToolBase ( type, name , parent )
{
  declareInterface<ST::ISTNoiseCalculationTool>(this);

  /// Which CMS algorithm should be used
  declareProperty( "CMSAlg", m_cmsAlg = "Simple" );

  /// Print ADC values during processing
  declareProperty("PrintADCs",        m_printADCs       = false ); 
  declareProperty("PrintCMSSteps",    m_printCMSSteps   = 1000 ); 

  // Number of events used to build the pedestals
  declareProperty("PedestalBuildup",  m_pedestalBuildup = 300 );
  
  // Read pedestal values from conditions database (change base class default)
  m_readPedestals = true;

  // Use integer algebra in calculations
  declareProperty("UseIntegers", m_useInts=false );

  // Remove outliers
  declareProperty("SkipOutlierRemoval", m_skipOutliers    = 1000);
  declareProperty("RawThreshold",       m_rawThreshold    = 4);
  declareProperty("RawOutliers",        m_rawOutliers     = true); 
  declareProperty("RawOutliersCMS",     m_rawOutliersCMS  = false); 

  declareProperty("CMSHitDetection",    m_cmsHitDetection = false);
  declareProperty("CMSThreshold",       m_cmsThreshold    = 13.0);
  declareProperty("FixedThreshold",     m_fixedThreshold  = false);

  // Set neighbour strips to zero in CMS calculation
  declareProperty("ZeroNeighbours",   m_zeroNeighbours  = true);

}

//==============================================================================
// initialize tool
//==============================================================================
StatusCode ST::STCMSNoiseCalculationTool::initialize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "initialize" << endmsg;
  StatusCode sc = ST::STNoiseToolBase::initialize();
  if (sc.isFailure()) return sc;

  // 4 ports per Beetle
  m_portHeader.resize(4, 0);

  m_printAlgError = true;

  m_skipCMS   = true;

  // Choose CMS algorithm
  whichCMSAlg();

  // Resize maps/vectors for the CMS noise calculation
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int TELL1SourceID = (*itT).first;
    // 8 pedestal configs for each TELL1
    if(!m_readPedestals) {
      m_pedestalMaps[TELL1SourceID].resize(8);
      for (int i = 0; i < 8; i++) {
        m_pedestalMaps[TELL1SourceID][i].resize(3072, std::make_pair(0.0, 0));
      }
    } else {
      //      m_pedestalBuildup = 0;
    }
    
  }
  return StatusCode::SUCCESS;
}

void ST::STCMSNoiseCalculationTool::whichCMSAlg() {
  if(m_cmsAlg == "Simple") {
    info() << "Simple CMS algorithm selected" << endmsg;
    m_useAlgo = 1;
  } else if(m_cmsAlg == "ST") {
    info() << "ST CMS algorithm selected" << endmsg;
    m_useAlgo = 3 ;
  } else if(m_cmsAlg == "STInt") {
    info() << "Integer ST CMS algorithm selected" << endmsg;
    m_useAlgo = 4;
  } else if(m_cmsAlg == "VELO") {
    info() << "Integer ST CMS algorithm selected" << endmsg;
    m_useAlgo = 5;
  } else if(m_cmsAlg == "Achim") {
    info() << "Integer ST CMS algorithm selected" << endmsg;
    m_useAlgo = 6;
  } else {
    warning() << "No CMS subtraction algorithm is specified." << endmsg;
  }
  if(m_useInts) info() << "Using integers for pedestal/noise calculations" << endmsg;
}
//=============================================================================
// Destructor
//=============================================================================
ST::STCMSNoiseCalculationTool::~STCMSNoiseCalculationTool() {} 

//========================================================================================================================
// 
// Data accessors
//
//========================================================================================================================

std::vector<double> ST::STCMSNoiseCalculationTool::rawMean(const unsigned int TELL) const {
  return m_rawMeanMap.find(TELL)->second;
}
std::vector<double> ST::STCMSNoiseCalculationTool::rawMeanSq(const unsigned int TELL) const {
  return m_rawMeanSqMap.find(TELL)->second;
}
std::vector<double> ST::STCMSNoiseCalculationTool::rawNoise(const unsigned int TELL) const {
  return m_rawNoiseMap.find(TELL)->second;
} 
std::vector<unsigned int> ST::STCMSNoiseCalculationTool::rawN(const unsigned int TELL) const {
  return m_rawNEventsPP.find(TELL)->second;
}

std::vector<double> ST::STCMSNoiseCalculationTool::cmsMean(const unsigned int TELL) const {
  return m_cmsMeanMap.find(TELL)->second;
}
std::vector<double> ST::STCMSNoiseCalculationTool::cmsMeanSq(const unsigned int TELL) const {
  return m_cmsMeanSqMap.find(TELL)->second;
}
std::vector<double> ST::STCMSNoiseCalculationTool::cmsNoise(const unsigned int TELL) const {
  return m_cmsNoiseMap.find(TELL)->second;
}
std::vector<unsigned int> ST::STCMSNoiseCalculationTool::cmsN(const unsigned int TELL) const {
  return m_cmsNEventsPP.find(TELL)->second;
}

//=====================================================================================================================
//
// Here is where the noise is calculated
//
//=====================================================================================================================
StatusCode ST::STCMSNoiseCalculationTool::calculateNoise() {

  // Get the data
  const LHCb::STTELL1Datas* data = getIfExists<LHCb::STTELL1Datas>(m_dataLocation);
  if ( NULL == data ) {
    // Skip if there is no Tell1 data
    return StatusCode::SUCCESS;
  }
  //debug() << "Found " << data->size() << " boards." << endmsg;
  if(data->empty()) return Warning("Data is empty", StatusCode::SUCCESS, 10);

  // Calculate the PCN/Header combinations for each Beetle port
  calcPCNConfigs(data);
  if (m_pedestalBuildup > 0) {
    if(!m_readPedestals) {
      sumPedestals(data);
      if (m_pedestalBuildup == 1) {
        divPedestals();
        info() << "Pedestals ready, starting noise calc." << endmsg;
      }
    }
    m_pedestalBuildup--;
    return StatusCode::SUCCESS;
  }

  m_evtNumber++;
  
  if( m_evtNumber <= m_skipEvents ) {

    //return StatusCode::SUCCESS;
    m_skipCMS = true;  
    
  } else {
    m_skipCMS = false;
  }
  // loop over the data
  LHCb::STTELL1Datas::const_iterator iterBoard = data->begin(); 
  for (; iterBoard != data->end() ; ++iterBoard){

    // get the tell board and the data headers
    unsigned int tellID = (*iterBoard)->TELL1ID();
    // Limit to selected tell1s
    if ( m_selectedTells && 
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber(tellID))) {
      continue;
    }
    const LHCb::STTELL1Data::Data& dataValues = (*iterBoard)->data();

    // Store TELL1s with an NZS bank
    m_tell1WithNZS.push_back(tellID);
    // Local vectors for given TELL1: output is CMS noise
    std::vector<double>* cmsMean = &m_cmsMeanMap[tellID];
    std::vector<double>* cmsMeanSq = &m_cmsMeanSqMap[tellID];
    std::vector<double>* cmsNoise = &m_cmsNoiseMap[tellID];
    std::vector<unsigned int>* cmsNEvents = &m_cmsNEventsPP[tellID];

    // Need to store raw noise: needed for outlier removal in pedestal sub.
    std::vector<double>* rawMean = &m_rawMeanMap[tellID];
    std::vector<double>* rawMeanSq = &m_rawMeanSqMap[tellID];
    std::vector<double>* rawNoise = &m_rawNoiseMap[tellID];
    std::vector<unsigned int>* rawNEvents = &m_rawNEventsPP[tellID];
    
    std::vector<double>* rawPedestal = &m_rawPedestalMap[tellID];

    std::vector< std::vector<std::pair<double, int> > >* pedestals = &m_pedestalMaps[tellID];

    // Loop over the PPs that have sent data
    std::vector<unsigned int> sentPPs = (*iterBoard)->sentPPs();
    std::vector<unsigned int>::const_iterator iPP = sentPPs.begin();
    for( ; iPP != sentPPs.end(); ++iPP ) {
      unsigned int pp = *iPP;
      
      // Count the number of events per PP
      (*rawNEvents)[pp]++;
      if(m_countRoundRobin) this->countRoundRobin(tellID, pp);
      
      // Cumulative average up to m_followingPeriod; after that
      // exponential moving average
      int nCMS = 0;
      int nEvt = (*rawNEvents)[pp];     
      if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) nEvt = m_followingPeriod;
	
      if ( !m_skipCMS ) {
        (*cmsNEvents)[pp]++;
        nCMS = (*cmsNEvents)[pp];
        if( m_followingPeriod > 0 && nCMS > m_followingPeriod ) {
          nCMS = m_followingPeriod;
        }
      }
	 
      // Loop over the links (i.e. Beetles)
      unsigned int iBeetle = 0;
      for ( ; iBeetle < STDAQ::nBeetlesPerPPx; ++iBeetle ){
        unsigned int beetle = iBeetle + pp*STDAQ::nBeetlesPerPPx;
        
        m_dump = false;
     
        //const BeetleADC& rawADC = dataValues[beetle];
        // Raw ADC values for Beetle
        std::vector<signed int> rawADC(LHCbConstants::nStripsInBeetle, 0);
        std::vector<std::pair <double, bool> > tmpADC(LHCbConstants::nStripsInBeetle, 
                                                      std::make_pair( 0.0, false ) );
        std::vector<double> meanPerPort(4, 0.0);

        dumpBeetleADCs( dataValues[beetle], "raw: " );
        
        copy(dataValues[beetle].begin(), dataValues[beetle].end(), rawADC.begin());

        std::vector<signed int> pedSubADCs;
        if ( m_pedestalBuildup == 0 ) {
          subtractPCNPedestals(rawADC, pedestals, beetle, pedSubADCs);
        }
	    
        if (!m_skipCMS) {
          subtractPedestals(pedSubADCs, rawMean->begin(), rawNoise->begin(), beetle, 
                             tmpADC, meanPerPort );
          
          if (beetle == m_printCMSSteps) {
            m_dump = true;
          }
          switch (m_useAlgo) {
          case 1:  cmsSimple(tmpADC, meanPerPort);
            break;
          case 3:  cmsST(tmpADC, meanPerPort);
            break;
          case 4:  cmsSTInt( tmpADC );
            break;
          case 5:  cmsVelo(tmpADC, meanPerPort);
            break;
          case 6:  cmsAchim( tmpADC );
            break;
          default: 
            if ( m_printAlgError ) {
              error() << "no CMS Algorithm selected" << endmsg;
              m_printAlgError = false;
            }
          }
        } // End of CMS calculation
        // Loop over the strips in this link
        unsigned int iStrip = 0;
        std::vector<signed int>::const_iterator rawADCStrip = rawADC.begin();
        std::vector<signed int>::const_iterator dataStrip = pedSubADCs.begin();
        std::vector<std::pair <double, bool> >::iterator tmpStrip  = tmpADC.begin();
        const int strip_offset = beetle * LHCbConstants::nStripsInBeetle;
        std::vector<double>::iterator rawPedStrip   = rawPedestal->begin()   + strip_offset;
        std::vector<double>::iterator rawMeanStrip   = rawMean->begin()   + strip_offset;
        std::vector<double>::iterator rawMeanSqStrip = rawMeanSq->begin() + strip_offset;
        std::vector<double>::iterator rawNoiseStrip  = rawNoise->begin() + strip_offset;
        std::vector<double>::iterator cmsMeanStrip   = cmsMean->begin()    + strip_offset;
        std::vector<double>::iterator cmsMeanSqStrip = cmsMeanSq->begin()  + strip_offset;
        std::vector<double>::iterator cmsNoiseStrip  = cmsNoise->begin()  + strip_offset;
        for ( ; iStrip < LHCbConstants::nStripsInBeetle ; ++iStrip) {
          // Get the ADC value
          double valueRaw  = *dataStrip;
          double valueCMS  = tmpStrip->first;
          double valuePed = *rawADCStrip;
          if ( m_rawOutliers & tmpStrip->second & (m_evtNumber > m_skipOutliers) ) {
            valueRaw = *rawMeanStrip;
            if ( m_rawOutliersCMS ) {
              valueCMS  = *cmsMeanStrip;
            }
            //continue;
          }
          
          // Calculate the pedestal and the pedestal squared
          *rawPedStrip = ( *rawPedStrip * (nEvt-1) + valuePed ) / nEvt;
          *rawMeanStrip   = (*rawMeanStrip   *(nEvt-1) + valueRaw )           / nEvt;
          *rawMeanSqStrip = (*rawMeanSqStrip *(nEvt-1) + std::pow(valueRaw,2)) / nEvt;
          *rawNoiseStrip = sqrt( *rawMeanSqStrip - std::pow( *rawMeanStrip,2 ) );

          // Calculate the mean(adc) and mean(adc^2) on cms values
          if (!m_skipCMS) {
            *cmsMeanStrip   = (*cmsMeanStrip   *(nCMS-1) + valueCMS )            / nCMS;
            *cmsMeanSqStrip = (*cmsMeanSqStrip *(nCMS-1) + std::pow(valueCMS,2) ) / nCMS;
            *cmsNoiseStrip = sqrt( *cmsMeanSqStrip - std::pow( *cmsMeanStrip,2) );
          }	    
          // move to next strip
          dataStrip++; tmpStrip++; 
          rawADCStrip++; rawPedStrip++;
          rawMeanStrip++; rawMeanSqStrip++; rawNoiseStrip++;
          cmsMeanStrip++; cmsMeanSqStrip++; cmsNoiseStrip++;
	  
        } // strip
      }  // beetle
        
      // Resets the event counter
      if( m_resetRate > 0  && nEvt%m_resetRate == 0 ) {
        (*rawNEvents)[pp] = 0;
      }
      
    } // FPGA-PP
  } // boards
  return StatusCode::SUCCESS;
}

//==============================================================================
// Header bit from beetle [0][1][2] where 2 is LSB
// Header bit 0 is always low (0) 
// PCN odd/even given by LSB of port 0
//  
// Use header bit 0 to store PCN parity:
// Store eight different configurations
// 000, 001, 010, 011, 100, 101, 110, 111
//==============================================================================
void ST::STCMSNoiseCalculationTool::calcPCNConfigs( const LHCb::STTELL1Datas* data ) {
  const LHCb::STTELL1Data::Data& header = (*(data->begin()))->header(); 
  for ( int iPort = 0; iPort < 4; iPort++ ) {
    std::bitset<3> binary;  
    if(!m_readPedestals) {
      // Header configs
      binary.set(0,(header[0][iPort*3+2] >= 129));
      binary.set(1,(header[0][iPort*3+1] >= 129));
      binary.set(2,(header[0][2] >= 129));
      m_portHeader[iPort] = binary.to_ulong();
    } else {
      binary.set(0,0);
      binary.set(1,0);
      binary.set(2,0);
      m_portHeader[iPort] = binary.to_ulong();
    }
  }
}
//==============================================================================
// Calculate sum of pedestals
// 1st of pair is sum of ADC values
// 2nd of pair is number of events used to build pedestals
//============================================================================== 
void ST::STCMSNoiseCalculationTool::sumPedestals(const LHCb::STTELL1Datas* data ) {
  LHCb::STTELL1Datas::const_iterator iterBoard = data->begin(); 
  for (; iterBoard != data->end() ; ++iterBoard){

    unsigned int tellID = (*iterBoard)->TELL1ID();
    const LHCb::STTELL1Data::Data& dataValues = (*iterBoard)->data();
    
    // Limit to selected tell1s
    if ( m_selectedTells && 
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber(tellID))) {
      continue;
    };
    // Local copy of pedestal values for a given TELL1
    std::vector< std::vector<std::pair<double, int> > >* pedestalValues = &m_pedestalMaps[tellID];

    // Loop over the PPs that have sent data
    std::vector<unsigned int> sentPPs = (*iterBoard)->sentPPs();
    std::vector<unsigned int>::const_iterator iPP = sentPPs.begin();
    for( ; iPP != sentPPs.end(); ++iPP ) {
      unsigned int pp = *iPP;

      unsigned int iBeetle = 0;
      for ( ; iBeetle < STDAQ::nBeetlesPerPPx; ++iBeetle ){
        
        unsigned int beetle = iBeetle + pp*STDAQ::nBeetlesPerPPx;

        std::vector<signed int>::const_iterator dataStrip = dataValues[beetle].begin();
	
        int iPort;
        for ( iPort = 0; iPort < 4; iPort++ ) {
          unsigned long header = m_portHeader[iPort];
          unsigned int iStrip = 0;
          for ( ; iStrip < LHCbConstants::nStripsInPort ; ++iStrip) {
            int strip = iStrip + iPort*LHCbConstants::nStripsInPort + beetle*LHCbConstants::nStripsInBeetle;
            (*pedestalValues)[header][strip].first += *dataStrip;
            (*pedestalValues)[header][strip].second++;
            dataStrip++;
          } // strip
        } // port
      }  // beetle
    } // FPGA-PP
  } // boards
}

//==============================================================================
// Calculate Pedestal values
// Loops over all strips and calculates pedestal values.
//==============================================================================
void ST::STCMSNoiseCalculationTool::divPedestals() {
  PedestalMaps::iterator       it1  = m_pedestalMaps.begin();
  PedestalMaps::const_iterator end1 = m_pedestalMaps.end();
  while ( it1 != end1 ) {// TELL1s
    std::vector< std::vector<std::pair<double, int> > >::iterator it2 =  (*it1).second.begin();
    std::vector< std::vector<std::pair<double, int> > >::const_iterator end2 = (*it1).second.end();
    while ( it2 < end2 ) {// Header configs
      std::vector<std::pair<double, int> >::iterator it = it2->begin();
      std::vector<std::pair<double, int> >::const_iterator end = it2->end();
      while ( it < end ) { // channels
        if (it->second > 0) {
          it->first /= it->second;
        } else {
          it->first = 0;
        }
        it++;
      }
      it2++;
    }
    it1++;
  }
  plotPedestals();
}

void ST::STCMSNoiseCalculationTool::plotPedestals() {
  PedestalMaps::iterator       it1  = m_pedestalMaps.begin();
  while ( it1 != m_pedestalMaps.end() ) {
    int TELL=(*it1).first;
    if (! ( m_selectedTells && 
            !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber(TELL))) ) {
      std::vector< std::vector<std::pair<double, int> > >::iterator it2 =  (*it1).second.begin();
      int header=0;
      for(; it2 != (*it1).second.end(); ++it2, ++header) {
        std::string idh="Pedestal, TELL "+std::to_string((this->readoutTool())->SourceIDToTELLNumber(TELL))
          +", header "+std::to_string(header);
        
        std::vector<std::pair<double, int> >::iterator itStrip = it2->begin();
        int strip=0;
        for(; itStrip != it2->end(); ++itStrip, ++strip) {
          plot1D(strip,idh,idh,-0.5,3071.5,3072,(*itStrip).first);
        }
      }
    }
    ++it1;
  }
}

// Subtract pedetals from ADC values..
/// Calculate the pedestal subtracted ADC values for one beetle
void ST::STCMSNoiseCalculationTool::subtractPedestals(const std::vector<signed int>& BeetleADCs, 
                                                       std::vector<double>::const_iterator rawMeansIt,
                                                       std::vector<double>::const_iterator rawNoiseIt,
                                                       const signed int beetle,
                                                       std::vector<std::pair <double, bool> >& pedSubADC,
                                                       std::vector<double>& BeetleMeans) {

  int stripOffset = beetle * LHCbConstants::nStripsInBeetle;  
  std::vector<double>::const_iterator meanMapIt = rawMeansIt + stripOffset;
  std::vector<double>::const_iterator rmsMapIt  = rawNoiseIt + stripOffset;

  std::vector<signed int>::const_iterator adcIt    = BeetleADCs.begin();
  std::vector<std::pair <double, bool> >::iterator pedSubIt = pedSubADC.begin();

  int iPort;
  for ( iPort = 0 ; iPort < 4; iPort++) {
    double mean = 0;
    
    unsigned int iStrip;

    for ( iStrip = 0; iStrip < LHCbConstants::nStripsInPort; iStrip++) {
      // subtract pedestal
      if ( m_useInts ) {
        pedSubIt->first = *adcIt - floor(*meanMapIt+0.5);
      } else {
        pedSubIt->first = *adcIt - *meanMapIt;
      }
      
      // add ADC value to pedestal subtracted mean
      mean += pedSubIt->first;
      
      // flag outliers
      if ((*rmsMapIt > 0.01) && 
          (fabs(pedSubIt->first) > m_rawThreshold * (*rmsMapIt))) {
        pedSubIt->second = true;
      }

      // advance to next channel
      adcIt++; pedSubIt++; meanMapIt++; rmsMapIt++;
    }
    BeetleMeans[iPort] = mean / LHCbConstants::nStripsInPort;
  }
}

//==============================================================================
// Subtract the PCN dependent pedestals
//==============================================================================
void ST::STCMSNoiseCalculationTool::subtractPCNPedestals( const std::vector<signed int>& RawADCs,
                                                           const std::vector< std::vector<std::pair<double, int> > >* pedestals,
                                                           const signed int beetle, 
                                                           std::vector<signed int>& PedSubADCs ) {

  int stripOffset = beetle * LHCbConstants::nStripsInBeetle;  
  
  std::vector<signed int>::const_iterator adcIt    = RawADCs.begin();
  
  int iPort;
  const std::vector< std::vector<std::pair<double, int> > >* pedMap = pedestals;
  for ( iPort = 0 ; iPort < 4; iPort++) {
    std::vector<std::pair<double, int> >::const_iterator pedIt = (*pedMap)[m_portHeader[iPort]].begin() + stripOffset;
    
    unsigned int iStrip;
    for ( iStrip = 0; iStrip < LHCbConstants::nStripsInPort; iStrip++) {


      PedSubADCs.push_back(*adcIt - (int)floor(pedIt->first+0.5));

      adcIt++; pedIt++; 
    }

    stripOffset += LHCbConstants::nStripsInPort;
  }
}

//==============================================================================
// CMS calculations
//==============================================================================
//
// Simple: TO DO: explain this.
void ST::STCMSNoiseCalculationTool::cmsSimple(std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                                              std::vector<double>& BeetleMeans) {
  int iPort = 0;
  for ( ; iPort < 4; iPort++) {
    tell1CMSIteration(BeetleTmpADCs, BeetleMeans, iPort);
  }

}

//==============================================================================
// tell1CMSIteration: TO DO : explain what this does
//==============================================================================
void ST::STCMSNoiseCalculationTool::tell1CMSIteration( std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                                                       std::vector<double>& BeetleMeans,
                                                       int iPort ) {

  const int    c_strips = LHCbConstants::nStripsInPort;
  const double c_divBy  = 2736.0; //(2<<12)/3.0;
  const double c_iMin   = -15.5;  //-16;

  const int    c_stripOffset = iPort * c_strips;

  const std::vector<std::pair <double, bool> >::iterator adcBegin = BeetleTmpADCs.begin()+c_stripOffset;
  std::vector<std::pair <double, bool> >::iterator adcIt    = adcBegin; 

  int i;
    
  // calculate slope
  double mean      = BeetleMeans[iPort];
  double slope     = 0;

  double iStrip    = c_iMin;
  for ( i = 0 ; i < c_strips; i++) {
    adcIt->first -= mean;
    slope        += iStrip*adcIt->first;

    // advance to next strip
    adcIt++; iStrip += 1.0;
  }
  slope /= c_divBy;
    
   
  // calculate common mode subtracted ADCs
  // and prepare mean for next round
  iStrip = c_iMin;
  adcIt  = adcBegin;
  for ( i = 0 ; i < c_strips; i++) {
    adcIt->first -= slope * iStrip;
      
    // advance to next strip
    adcIt++; iStrip += 1.0;
  }
}

// ST: TO DO: explain this
void ST::STCMSNoiseCalculationTool::cmsST( std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                                           std::vector<double>& BeetleMeans )
{
  int iPort = 0;
  for ( ; iPort < 4; iPort++) {
    stIteration(BeetleTmpADCs, BeetleMeans, iPort);
  }
}

// stIteration
/* ******************* *
 *  ST TEll1 Emulation *
 * ******************* */
void ST::STCMSNoiseCalculationTool::stIteration( std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                                                 std::vector<double>& BeetleMeans,
                                                 int iPort) {
  const int    c_strips = LHCbConstants::nStripsInPort;
  const double c_divBy  = 2736.0; //(2<<12)/3.0;
  const double c_iMin   = -15.5;  //-16;

  const int    c_stripOffset = iPort * c_strips;

  const std::vector<std::pair <double, bool> >::iterator adcBegin = BeetleTmpADCs.begin()+c_stripOffset;
  std::vector<std::pair <double, bool> >::iterator adcIt    = adcBegin; 

  std::vector<std::pair <double, bool> >           noHitADCs(c_strips, std::make_pair(0.0, false));
  std::vector<std::pair <double, bool> >::iterator noHitIt = noHitADCs.begin();

  int i;
    
  double mean     = BeetleMeans[iPort];
  double thValue  = m_cmsThreshold;
  if (!m_fixedThreshold) {
    thValue = -m_cmsThreshold;
  }
    
  if (m_dump) dumpPortADCs(BeetleTmpADCs, iPort, "stIteration: s0: ");
      
  adcIt = adcBegin;
  for ( i = 0 ; i < c_strips; i++) {
    adcIt->first       -= mean;
    noHitIt->first      = adcIt->first;
      
    // advance to next strip
    adcIt++; noHitIt++; 
  }
    
  if (m_dump) dumpPortADCs(noHitADCs, 0, "stIteration: s1: ");
    
  noHitIt = noHitADCs.begin();
  zeroHits( noHitIt, thValue );
    
  if (m_dump) dumpPortADCs(noHitADCs, 0, "stIteration: s2: ");
      
  //-------------------------------second iteration
  //-------------------------------average

  mean  = 0;
  noHitIt = noHitADCs.begin();
  for ( i = 0 ; i < c_strips; i++) {
    mean += noHitIt->first;
    noHitIt++;
  }
  mean /= c_strips;

    
  //-------------------------------subtract mean
  //-------------------------------and calc slope

  double slope     = 0;
    
  noHitIt = noHitADCs.begin();
  double iStrip    = c_iMin;
  for ( i = 0 ; i < c_strips; i++) {
    noHitIt->first  -= mean;
    slope           += iStrip * (noHitIt->first);

    // advance to next strip
    noHitIt++; iStrip += 1.0;
  }
  slope /= c_divBy;
    
  if (m_dump) dumpPortADCs(noHitADCs, 0, "stIteration: s3: ");
   
  // calculate common mode subtracted ADCs
  iStrip = c_iMin;
  adcIt  = adcBegin;
  for ( i = 0 ; i < c_strips; i++) {
    adcIt->first -= slope * iStrip + mean;
    //adcIt->first -= slope * iStrip;
      
    // advance to next strip
    adcIt++; iStrip += 1.0;
  }
    
  if (m_dump) dumpPortADCs(BeetleTmpADCs, iPort, "stIteration: s4: ");

}

/// ZERO hits
void ST::STCMSNoiseCalculationTool::zeroHitsUseRawNoise( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs ) {
  unsigned int i;
  if (m_zeroNeighbours) {

    if (itBeetleTmpADCs->second) {
      itBeetleTmpADCs->first     = 0;
      (itBeetleTmpADCs+1)->first = 0;
    }
    itBeetleTmpADCs++;
    for ( i = 1 ; i < LHCbConstants::nStripsInPort-1 ; i++) {
      if (itBeetleTmpADCs->second) {
        (itBeetleTmpADCs-1)->first = 0;
        itBeetleTmpADCs->first     = 0;
        (itBeetleTmpADCs+1)->first = 0;
      }
      itBeetleTmpADCs++; 
    }
    if (itBeetleTmpADCs->second) {
      (itBeetleTmpADCs-1)->first = 0;
      itBeetleTmpADCs ->first    = 0;
    }
  } else {
    for ( i = 0 ; i < LHCbConstants::nStripsInPort ; i++) { 
      if (itBeetleTmpADCs->second) {
        itBeetleTmpADCs->first  = 0;
      }
      itBeetleTmpADCs++;
    }
  }
    
}


void ST::STCMSNoiseCalculationTool::zeroHitsNormal( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs, 
                                                    double threshold) {
  unsigned int i;
  double val;
  if (m_zeroNeighbours) {
    val = (itBeetleTmpADCs+1)->first;
	
    if (fabs(itBeetleTmpADCs->first) > threshold) {
      itBeetleTmpADCs->first     = 0;
      (itBeetleTmpADCs+1)->first = 0;
    }
    itBeetleTmpADCs++; 
    for ( i = 1 ; i < LHCbConstants::nStripsInPort-1 ; i++) {
      if (fabs(val) > threshold) {
        val = (itBeetleTmpADCs+1)->first;
        (itBeetleTmpADCs-1)->first = 0;
        itBeetleTmpADCs->first     = 0;
        (itBeetleTmpADCs+1)->first = 0;
      } else {
        val = (itBeetleTmpADCs+1)->first;
      }	   
      itBeetleTmpADCs++; 
    }
    if (fabs(val) > threshold) {
      (itBeetleTmpADCs-1)->first = 0;
      itBeetleTmpADCs ->first    = 0;
    }
    itBeetleTmpADCs++;
  } else {
    for ( i = 0 ; i < LHCbConstants::nStripsInPort ; i++) { 
      if (fabs(itBeetleTmpADCs->first) > threshold) {
        //itBeetleTmpADCs->second = true;
        itBeetleTmpADCs->first  = 0;
      }
      itBeetleTmpADCs++;
    }
  }

}


void ST::STCMSNoiseCalculationTool::zeroHits( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs, 
                                              double threshold) {
  if ( threshold < 0 ) {
    zeroHitsUseRawNoise( itBeetleTmpADCs );
  } else {
    zeroHitsNormal( itBeetleTmpADCs, threshold );
  }
}

//======================================================================================================================
//
// cmsSTInt: calculation of cms noise in integer representations
//
//======================================================================================================================
void ST::STCMSNoiseCalculationTool::cmsSTInt( std::vector<std::pair <double, bool> >& BeetleTmpADCs ) {
  int iPort = 0;
  for ( ; iPort < 4; iPort++) {
    stTell1Code(BeetleTmpADCs, iPort);
  }
}

void ST::STCMSNoiseCalculationTool::stTell1Code( std::vector<std::pair <double, bool> >& BeetleTmpADCs,
                                                 int iPort ) {
  int cms_threshold = (int)floor(m_cmsThreshold+0.5);
  int strip;
    
  long int  mean_format;
  int       mean_temp;
  long int  slope_format;
  int       slope_temp;
  int       data_without_hit[32], data_with_hit[32];

  const std::vector<std::pair <double, bool> >::iterator adcBegin = BeetleTmpADCs.begin() + iPort*LHCbConstants::nStripsInPort;
  std::vector<std::pair <double, bool> >::iterator adcIt = adcBegin;

  for(strip=0;strip<32;strip++)
  {
    data_with_hit[strip] = (int)floor(adcIt->first + 0.5);
    adcIt++;
  }
    
  //--------------------------------------first iteration
    
  mean_format = 0x10;    
    
  for(strip=0;strip<32;strip++)
  {	mean_format += data_with_hit[strip]; //sum_1
  }
  mean_temp = mean_format>>5;   //average

  for(strip=0;strip<32;strip++)
  {
    data_without_hit[strip] = data_with_hit[strip] - mean_temp; 
    data_with_hit[strip]    = data_with_hit[strip] - mean_temp; 	
  }
  saturate_for_8bits(data_without_hit,32);  //saturate to -128~127
  saturate_for_8bits(data_with_hit,32);  //saturate to -128~127
    
  if(abs(data_with_hit[0])>cms_threshold)
  {
    data_without_hit[0] = 0; 
    data_without_hit[1] = 0; 
  }
  for(strip=1;strip<31;strip++)
  {
    if(abs(data_with_hit[strip])>cms_threshold)
	  {
	    data_without_hit[strip-1] = 0; 
	    data_without_hit[strip] = 0; 
	    data_without_hit[strip+1] = 0; 
	  }
  }
  if(abs(data_with_hit[31])>cms_threshold)
  {
    data_without_hit[30] = 0; 
    data_without_hit[31] = 0; 

  }
	    
  //--------------------------------------second iteration
  //------------------------------------- average
    
  mean_format = 0x10;                     //sum_1
  for(strip=0;strip<32;strip++)
  {	mean_format += data_without_hit[strip]; //sum_1
  }
  mean_temp = mean_format>>5;   //average

		
  for(strip=0;strip<32;strip++)
  { 
    data_with_hit[strip] -= mean_temp; 
    data_without_hit[strip] -= mean_temp; 
  }
  saturate_for_8bits(data_with_hit,32);  //saturate to -128~127
  saturate_for_8bits(data_without_hit,32);  //saturate to -128~127
    
		
  //------------------------------------- slope
    
    
  slope_format = 0x20;
  for(strip=0;strip<32;strip++)
  {
    slope_format += (strip-16)*data_without_hit[strip];
  }

  slope_temp = slope_format>>6;

  if(slope_temp >127) slope_temp =127;
  else if(slope_temp <-128) slope_temp = -128;

  for(strip=0;strip<32;strip++)
  { 
    data_with_hit[strip] -= ((slope_temp*(strip-16)*3)+64)>>7; 
  }
  saturate_for_8bits(data_with_hit,32);  //saturate to -128~127

    
  //------------------------------------------over
  adcIt = adcBegin;
  for(strip=0;strip<32;strip++)
  {	
    adcIt->first = data_with_hit[strip];
    adcIt++;  
  }
}

void ST::STCMSNoiseCalculationTool::saturate_for_8bits(int *data, int line_numb)
{
  int i;
  for(i=0;i<line_numb;i++)
  {
    if(data[i]>127) data[i] =127;
    else if(data[i] < -128)data[i] = -128;
  }
}

//======================================================================================================================
//
// VELO CMS calculation
//
//======================================================================================================================
void ST::STCMSNoiseCalculationTool::cmsVelo( std::vector<std::pair <double, bool> >& BeetleTmpADCs,
                                             std::vector<double>& BeetleMeans) {
  int iPort = 0;
  for ( ; iPort < 4; iPort++) {

    if (m_dump) dumpPortADCs(BeetleTmpADCs, iPort, "cmsVelo: s0: ");
    // first iteration
    tell1CMSIteration( BeetleTmpADCs, BeetleMeans, iPort );

    if (m_dump) dumpPortADCs(BeetleTmpADCs, iPort, "cmsVelo: s1: ");
    // first hit detection
    veloHitDetection1( BeetleTmpADCs, BeetleMeans, iPort );
	
    if (m_dump) dumpPortADCs(BeetleTmpADCs, iPort, "cmsVelo: s2: ");
    // second iteration
    tell1CMSIteration( BeetleTmpADCs, BeetleMeans, iPort );
      
    if (m_dump) dumpPortADCs(BeetleTmpADCs, iPort, "cmsVelo: s3: ");

    // final hit detection
    //   implemented with const treshold value
    //veloHitDetection2( BeetleTmpADCs, iPort);
    //
    //if (m_dump) {
    //  info() << "s4: ";
    //  dumpPortADCs(BeetleTmpADCs, iPort, "cmsVelo: s0: ");
    //}
  }
}

void ST::STCMSNoiseCalculationTool::veloHitDetection1( std::vector<std::pair <double, bool> >& BeetleTmpADCs,
                                                       std::vector<double>& BeetleMeans, int iPort ) {
  const int    c_strips      = LHCbConstants::nStripsInPort;
  const int    c_stripOffset = iPort * c_strips;

  int i;

  std::vector<std::pair <double, bool> >::iterator adcIt = BeetleTmpADCs.begin() + c_stripOffset; 
    
  double thValue = m_cmsThreshold;
  if (!m_fixedThreshold) {
    thValue = calcThreshold( adcIt, m_cmsThreshold );
    adcIt -= c_strips;
  }
    
  // find outliers and set adc to 0
  zeroHits( adcIt, thValue );
    
  // calculate mean
  adcIt -= c_strips;
  double mean = 0;
  for ( i = 0 ; i < c_strips ; i++) { 
    mean += adcIt->first;
    adcIt++;
  }
  BeetleMeans[iPort] = mean/c_strips;
    
}

void ST::STCMSNoiseCalculationTool::veloHitDetection2( std::vector<std::pair <double, bool> >& BeetleTmpADCs, int iPort ) {
  const int    c_strips      = LHCbConstants::nStripsInPort;
  const int    c_stripOffset = iPort * c_strips;

  std::vector<std::pair <double, bool> >::iterator adcIt = BeetleTmpADCs.begin() + c_stripOffset; 

  int i;
  for ( i = 0 ; i < c_strips ; i++ ) {
    if (adcIt->first > m_cmsThreshold) {
      adcIt->first = 0;
    }
    adcIt++;
  }
}

double ST::STCMSNoiseCalculationTool::calcThreshold( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs, 
                                                     double nSigma) {
  double threshold = 0;
  for (unsigned int i = 0 ; i < LHCbConstants::nStripsInPort ; ++i ) { 
    threshold   += std::pow(itBeetleTmpADCs->first,2);
    itBeetleTmpADCs++; 
  }
   
  return nSigma*sqrt(threshold/32.0);
}


//======================================================================================================================
//
// CMS calculation using Achim method
//
//======================================================================================================================
void ST::STCMSNoiseCalculationTool::cmsAchim ( std::vector<std::pair <double, bool> >& BeetleTmpADCs ) {
  int iPort = 0;
  for ( ; iPort < 4; iPort++) {
    achimIteration( BeetleTmpADCs, iPort );
  }
}

/** 
 * Achims proposed algorithm. 
 *   if called with m_useInts = true, the input and output
 *   ADCs get rounded to integer values but doubles are 
 *   used for mean, slope and shift, as these values 
 *   should be calculated with a higher precision 
 *   internally anyway.
 */
void ST::STCMSNoiseCalculationTool::achimIteration( std::vector<std::pair <double, bool> >& BeetleTmpADCs, int iPort ) {
  const int    c_strips = LHCbConstants::nStripsInPort;
  const int    c_stripOffset = iPort * c_strips;

  const std::vector<std::pair <double, bool> >::iterator adcBegin = BeetleTmpADCs.begin()+c_stripOffset;
  std::vector<std::pair <double, bool> >::iterator adcIt    = adcBegin; 

  // calculate slope
  double meanY[] = { 0, 0 };
  double meanX[] = { 0, 0 };
  double N[]     = { 0, 0 }; 
    
  int i, j;
  for (j = 0 ; j < 2 ; j++ ) {  
    for ( i = 0 ; i < c_strips/2 ; i++) {
      if ( m_useInts ) {
        adcIt->first = floor(adcIt->first+0.5);
      }
	
      // only add the channel if
      //   m_cmsHitDetection is true and the channel is not flagged
      // or if
      //   m_cmsHitDetection is false
      if ( !m_cmsHitDetection | !adcIt->second ) { 
        meanY[j] += adcIt->first;
        meanX[j] += i;
        N[j]     += 1.0;
      }

      // advance to next strip
      adcIt++; 
    }
    meanY[j] /= N[j];
    meanX[j] /= N[j];
  }
  meanX[1] += c_strips/2;
   
  double slope = (meanY[1] - meanY[0]) / (meanX[1] - meanX[0]);
  double shift = meanY[0] - slope * meanX[0];
    
  //info() << "slope/shift: " << slope << " / " << shift << endmsg;
    
  // calculate common mode subtracted ADCs
  // and prepare mean for next round
  adcIt  = adcBegin;
  if ( m_useInts ) {
    for ( i = 0 ; i < c_strips; i++) {
      adcIt->first -= floor(slope * i + shift + 0.5) ;
      adcIt++; 
    }
  } else {
    for ( i = 0 ; i < c_strips; i++) {
      adcIt->first -= slope * i + shift ;
      adcIt++; 
    }
  }
}


/** 
 * DUMP Functions
 */

void ST::STCMSNoiseCalculationTool::dumpBeetleADCs(const std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                                                   std::string s) {
  if ( !m_printADCs | m_skipCMS ) {
    return;
  }

  std::vector<std::pair <double, bool> >::const_iterator adcIt = BeetleTmpADCs.begin();

  info() << s;
  unsigned int i;
  for ( i = 0 ; i < LHCbConstants::nStripsInBeetle ; i++) {
    info() << adcIt->first << ", ";
    adcIt++;
  }
  info() << endmsg;
}

void ST::STCMSNoiseCalculationTool::dumpBeetleADCs(const std::vector<signed int>& BeetleADCs, 
                                                   std::string s ) {
  if ( !m_printADCs | m_skipCMS ) {
    return;
  }

  std::vector<signed int>::const_iterator adcIt = BeetleADCs.begin();
 
  info() << s;
  unsigned int i;
  for ( i = 0 ; i < LHCbConstants::nStripsInBeetle ; i++) {
    info() << *adcIt << ", ";
    adcIt++;
  }
  info() << endmsg;
}

void ST::STCMSNoiseCalculationTool::dumpPortADCs(const std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                                                 int port, std::string s ) {
  std::vector<std::pair <double, bool> >::const_iterator adcIt = BeetleTmpADCs.begin() 
    + LHCbConstants::nStripsInPort * port;

  info() << s;
  unsigned int i;
  for ( i = 0 ; i < LHCbConstants::nStripsInPort ; i++) {
    info() << adcIt->first << ", ";
    adcIt++;
  }
  info() << endmsg;
}

void ST::STCMSNoiseCalculationTool::dumpPortADCs(const std::vector<double>& adc, int port, 
                                                 std::string s ) {
  std::vector<double>::const_iterator adcIt = adc.begin() 
    + LHCbConstants::nStripsInPort * port;

  info() << s;
  unsigned int i;
  for ( i = 0 ; i < LHCbConstants::nStripsInPort ; i++) {
    info() << *adcIt << ", ";
    adcIt++;
  }
  info() << endmsg;
}



