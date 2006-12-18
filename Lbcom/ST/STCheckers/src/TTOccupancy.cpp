//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000


// BOOST
#include "boost/lexical_cast.hpp"


// Gaudi
#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/SystemOfUnits.h"

// Histogramming
#include "AIDA/IHistogram1D.h"

// xml geometry
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeTTLayer.h"


#include "TTOccupancy.h"
#include "Kernel/ISTSignalToNoiseTool.h"

#include "Event/STDigit.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TTOccupancy );

//--------------------------------------------------------------------
//
//  TTOccupancy : Check occupancies in inner tracker
//
//--------------------------------------------------------------------

TTOccupancy::TTOccupancy(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_sigNoiseTool(0),
  m_tracker(0)
{
  // constructer
  declareProperty("threshold",m_Threshold);
  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("binSize", m_binSize = 32);
  declareProperty("dataLocation",m_dataLocation = STDigitLocation::TTDigits);

  m_Threshold.reserve(5);
  for (int iThres=0;iThres<2;++iThres){
    m_Threshold.push_back(3.0);
  } //i
}

TTOccupancy::~TTOccupancy(){
  // destructer
}

StatusCode TTOccupancy::initialize(){

  //
  if( "" == histoTopDir() ) setHistoTopDir("TT/");
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // detector element
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::TT);

  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName + "TT");

  m_hMax = 512*8;
  m_nBins = m_hMax/m_binSize;

  return StatusCode::SUCCESS;

}

StatusCode TTOccupancy::execute(){

  // execute once per event

  // init
  
  // retrieve Digitizations
  STDigits* digitsCont = get<STDigits>(m_dataLocation);

  // histos per digit
  STDigits::const_iterator iterDigit =  digitsCont->begin();
  for( ; iterDigit != digitsCont->end(); ++iterDigit){
    this->fillHistograms(*iterDigit);
  } // loop iterDigit

 
  return StatusCode::SUCCESS;
}

StatusCode TTOccupancy::fillHistograms(const STDigit* aDigit){

  // retrieve geom info

  if (m_sigNoiseTool->signalToNoise(aDigit) >
      m_Threshold[aDigit->channelID().station()-1u]){
  
    const STChannelID aChan = aDigit->channelID();  

    DeSTSector* aSector = m_tracker->findSector(aChan);
    DeTTSector* ttSector = dynamic_cast<DeTTSector*>(aSector);

    const unsigned int nstrips = ttSector->nStrip();


    // construct the histogram id...
    std::string histo = "occ_st"+boost::lexical_cast<std::string>(aChan.station())
                         + "_lay"+boost::lexical_cast<std::string>(aChan.layer())
                         + "_dr"+boost::lexical_cast<std::string>(aChan.detRegion())
                         +"_row"+boost::lexical_cast<std::string>(ttSector->row());
    
    int offset;
    if (aChan.detRegion() == 1) {
      offset = ttSector->column();
    }
    else if (aChan.detRegion() == 2){
      aChan.station() == 1 ? offset = ttSector->column() - 6: offset = ttSector->column() - 7;
    }
    else if (aChan.detRegion() == 3){
      aChan.station() == 1 ? offset = ttSector->column() - 9: offset = ttSector->column() - 10;
    }
    else {
      return Warning( "unknown row " );
    } 
  
    plot(binValue(aChan.strip())+(nstrips*(offset-1)),histo, 0., m_hMax, m_nBins);

  } // if above threshold

  return StatusCode::SUCCESS;
}

double TTOccupancy::binValue(const unsigned int strip) const{

  // ensure get the correct bin....
  double value;
  unsigned int testValue = (strip-1)%m_binSize;
  switch (testValue){
  case (0):
    value = 0.01+(strip-1);     
    break;    
  case(31):
    value = strip-1-0.01;     
    break;    
  default:
    value = strip-1;
    break;  
  }
  return value;
}












