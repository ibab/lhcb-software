//
// This File contains the definition of the ITOccupancy -class
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
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeITLadder.h"

#include "ITOccupancy.h"
#include "Kernel/ISTSignalToNoiseTool.h"

#include "Event/STDigit.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( ITOccupancy );

//--------------------------------------------------------------------
//
//  ITOccupancy : Check occupancies in inner tracker
//
//--------------------------------------------------------------------

ITOccupancy::ITOccupancy(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_sigNoiseTool(0),
  m_tracker(0)
{
  // constructer
  declareProperty("threshold",m_Threshold);
  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("binSize", m_binSize = 32);
  declareProperty("dataLocation",m_dataLocation = STDigitLocation::ITDigits);
  declareProperty("detType", m_detType = "IT");

  m_Threshold.reserve(3);
  for (int iThres=0;iThres<3;++iThres){
    m_Threshold.push_back(3.0);
  } //i
}

ITOccupancy::~ITOccupancy(){
  // destructer
}

StatusCode ITOccupancy::initialize(){

  //
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // detector element
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::IT);
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName+m_detType);

  // intialize histos
  this->initHistograms();

  return StatusCode::SUCCESS;

}

StatusCode ITOccupancy::execute(){

  // execute once per event

  // init
  std::vector<double>::iterator iter = m_layerOccVector.begin();
  for ( ; iter!=m_layerOccVector.end();++iter){
    *iter=0.;
  }

  // retrieve Digitizations
  STDigits* digitsCont = get<STDigits>(m_dataLocation);

  
  // histos per digit
  STDigits::const_iterator iterDigit =  digitsCont->begin();
  for( ; iterDigit != digitsCont->end(); ++iterDigit){
    this->fillHistograms(*iterDigit);
  } // loop iterDigit

  // av occupancy in each layer
  int iCount = 0;
  for (iter=m_layerOccVector.begin();iter!=m_layerOccVector.end();++iter){
    if (m_nStripsVector[iCount] !=0) { 
      double occ = (double)(*iter)/m_nStripsVector[iCount];
      m_layerOccHistos[iCount]->fill(occ);
    }
    ++iCount;
  }
 
  return StatusCode::SUCCESS;
}



StatusCode ITOccupancy::initHistograms()
{

 int numInVector = 0;
 unsigned int nstrip = m_tracker->sectors().front()->nStrip();
  
 std::vector<DeSTLayer*>::const_iterator iterLayer = m_tracker->layers().begin();
 for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){

   // uniquely id using station and layer
   int id = this->uniqueInt((*iterLayer)->elementID()); 

   const DeITLayer* tLayer = dynamic_cast<const DeITLayer*>(*iterLayer);
   unsigned int nStripInLayer = nstrip*tLayer->ladders().size();

   // add to map
   m_Mapping[id] = numInVector;

   // number of strips in layer.... 
   unsigned int bins = nStripInLayer/m_binSize;
   
   int histID = id;

   IHistogram1D* stripOccHisto = book(histID,
                                    "Number of hits on strips"+boost::lexical_cast<std::string>(histID),
				    0., bins*m_binSize, bins);
   m_stripOccVector.push_back(stripOccHisto);
		       
   histID = id + 2000;      

   m_layerOccVector.push_back(0.);
   m_nStripsVector.push_back((double)nStripInLayer);

   IHistogram1D* layerOccHisto = book(histID,
				   "layer  occupancy."+boost::lexical_cast<std::string>(histID),
				    0.0, 0.2, 100);
   m_layerOccHistos.push_back(layerOccHisto);
 
   ++numInVector;

 } // iterLayer

 return StatusCode::SUCCESS;

}

StatusCode ITOccupancy::fillHistograms(const STDigit* aDigit){

  // retrieve geom info

  if (m_sigNoiseTool->signalToNoise(aDigit) >
      m_Threshold[aDigit->channelID().station()-m_tracker->firstStation()]){
  
    const STChannelID aChan = aDigit->channelID();  
    DeSTSector* aSector = m_tracker->findSector(aChan);

    int id = uniqueInt(aChan);
    int numInVector = m_Mapping[id];

    const unsigned int nstrips = aSector->nStrip();

    m_stripOccVector[numInVector]->fill(binValue(aChan.strip())+(nstrips*(aChan.sector()-1)));
    ++m_layerOccVector[numInVector];

  } // if above threshold

  return StatusCode::SUCCESS;
}

int ITOccupancy::uniqueInt(const STChannelID aChan) const {
  return ((aChan.station()*100)+(10*aChan.detRegion())+aChan.layer());
}


double ITOccupancy::binValue(const unsigned int strip) const{

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












