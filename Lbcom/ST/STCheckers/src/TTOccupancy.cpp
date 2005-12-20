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

// CLHEP
#include "Kernel/PhysicalConstants.h"

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

static const AlgFactory<TTOccupancy> s_Factory;
const IAlgFactory& TTOccupancyFactory = s_Factory;

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

  // intialize histos
  this->initHistograms();

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


StatusCode TTOccupancy::initHistograms()
{

 int numInVector = 0;
 unsigned int nstrip = m_tracker->sectors().front()->nStrip();

 std::vector<DeSTLayer*>::const_iterator iterLayer = m_tracker->layers().begin();
 for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){
   DeSTLayer* aLayer = *iterLayer;

   for (unsigned int iCol = 1; iCol<= aLayer->childIDetectorElements().size(); ++iCol){

      IDetectorElement::IDEContainer children = aLayer->childIDetectorElements();
      int ttId = this->uniqueInt(aLayer->elementID().station(),
                                 aLayer->elementID().layer(),iCol); 

      // add to map
      m_Mapping[ttId] = numInVector;

      unsigned int numStripsInCol = children.size()*2*nstrip; 
      unsigned int ttBins = numStripsInCol/m_binSize;
      int ttHistoId = ttId+10000;

      AIDA::IHistogram1D* ttStripOccHisto = book(ttHistoId,
		       "Number of hits on strips"+boost::lexical_cast<std::string>(ttHistoId),
		       -0.5, ttBins*m_binSize, ttBins );

      m_stripOccVector.push_back(ttStripOccHisto);

      ++numInVector;
   } // iCol
 } // iStation

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

    // TT layer
    int iCol = ttSector->column();
    int ttId = uniqueInt(aChan.station(),aChan.layer(),iCol);
    int numInVector = m_Mapping[ttId];
    int offset = aChan.sector() - ttSector->getParent<DeTTSector>()->firstSector();
    m_stripOccVector[numInVector]->fill(binValue(aChan.strip())+(nstrips*offset));

  } // if above threshold

  return StatusCode::SUCCESS;
}

int TTOccupancy::uniqueInt(unsigned int iStation, unsigned int iLayer, 
                             unsigned int iCol) const {
  return ((iStation*1000)+(iLayer*100)+iCol);
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












