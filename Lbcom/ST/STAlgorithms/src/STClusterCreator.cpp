// $Id: STClusterCreator.cpp,v 1.3 2005-12-20 15:50:25 cattanem Exp $
//
// This File contains the implementation of the STClusterCreator
// C++ code for 'LHCb Tracking package(s)'
//

#include "GaudiKernel/AlgFactory.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/LHCbMath.h"
#include "Kernel/LHCbConstants.h"
#include "Kernel/ISTClusterPosition.h"

// ST includes
#include "STClusterCreator.h"
#include "Kernel/STDetSwitch.h"

#include "Kernel/ISTSignalToNoiseTool.h"

using namespace LHCb;
// Needed for the creation of TrSimpleLayersCreator objects.
static const AlgFactory<STClusterCreator> s_factory;
const IAlgFactory& STClusterCreatorFactory = s_factory;
//_________________________________________________
// STClusterCreator
//
// An TrClusterCreator is a Gaudi sub algorithm that creates Inner tracker Cluster 
// from the digitizations and stores them in the correct layer
// 
STClusterCreator::STClusterCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_tracker(0),
  m_sigNoiseTool(0),
  m_positionTool(0)
{
  // STClusterCreator constructor
  //read in DataCard Value
 
  this->declareProperty("digitSignal2Noise",m_digitSig2NoiseThreshold = 3.0);
  this->declareProperty("clusterSignal2Noise",m_clusterSig2NoiseThreshold = 3.6);
  this->declareProperty("highSignal2Noise",m_highThreshold = 6.0);
  this->declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  this->declareProperty("positionTool",m_positionToolName = "STOnlinePosition");

  this->declareProperty("inputLocation",  m_inputLocation = STDigitLocation::TTDigits);
  this->declareProperty("outputLocation",  m_outputLocation = STClusterLocation::TTClusters);
 
  this->declareProperty("outputVersion", m_outputVersion = 1);
  this->declareProperty("size", m_maxSize = 32);
  this->declareProperty("detType", m_detType = "TT");
  this->declareProperty("byBeetle", m_byBeetle = true);
  
}

STClusterCreator::~STClusterCreator()
{
  // STClusterCreator destructor
}

StatusCode STClusterCreator::initialize()
{
  // Initializes STClusterCreator at the begin of program execution.
 
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // geometry   
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  STDetSwitch::flip(m_detType,m_inputLocation);
  STDetSwitch::flip(m_detType,m_outputLocation);

  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName+m_detType);

  // calculate cut values
  const std::vector<DeSTSector*>& tSectors =m_tracker->sectors(); 
  for (std::vector<DeSTSector*>::const_iterator iterS = tSectors.begin(); iterS != tSectors.end(); ++iterS ){
    double adc =  m_sigNoiseTool->noiseInADC(*iterS);
    m_digitSig2NoiseCut[*iterS] = m_digitSig2NoiseThreshold*adc;
    m_clusterSig2NoiseCut[*iterS] = m_clusterSig2NoiseThreshold*adc; 
     m_highSig2NoiseCut[*iterS] = m_highThreshold*adc;
  } // iterS

 
  // position tool
  m_positionTool = tool<ISTClusterPosition>(m_positionToolName);
 
  return StatusCode::SUCCESS;
}


StatusCode STClusterCreator::execute()
{
  // Executes STClusterCreator for one event.

  // retrieve Digitizations
  STDigits* digitCont = get<STDigits>(m_inputLocation);

  // clusterize
  STClusters* clusterCont = new STClusters();
  StatusCode sc = this->createClusters(digitCont, clusterCont);
  
  // version number
  clusterCont->setVersion(m_outputVersion);

  //register layers in the transient data store.....
  put(clusterCont,m_outputLocation);

  return sc;
}


StatusCode STClusterCreator::createClusters(const STDigits* digitCont, 
           STClusters* clusterCont) const{

  // perform clustering

  STDigits::const_iterator iterDigit = digitCont->begin();
  STDigits::const_iterator jterDigit;
  STDigits::const_iterator startCluster;

  while (iterDigit != digitCont->end()){

    jterDigit = iterDigit;
    ++jterDigit;

    DeSTSector* aSector= m_tracker->findSector((*iterDigit)->channelID()); ;
    if (aboveDigitSignalToNoise(*iterDigit, aSector)) {

      // make a cluster !
      startCluster = iterDigit;
      SmartRefVector<STDigit> clusteredDigits;
      clusteredDigits.push_back(*iterDigit);      
      double totCharge = (*iterDigit)->depositedCharge(); 

      // clustering loop
      int isize = 1;
      while ((jterDigit != digitCont->end())
	     &&(iterDigit !=digitCont->end())
             &&(isize<m_maxSize)
             &&(this->keepClustering(*iterDigit,*jterDigit, aSector) == true)){
        clusteredDigits.push_back(*jterDigit);
        totCharge +=  (*jterDigit)->depositedCharge(); 
        ++iterDigit;
        ++jterDigit;
        ++isize;
      } // clustering loop
 
  
      if (this->aboveClusterSignalToNoise(totCharge, aSector)){

	ISTClusterPosition::Measurement measValue = m_positionTool->estimate(clusteredDigits);
      
        STChannelID nearestStrip = measValue.first.first;
        double distToCenter = measValue.first.second*m_tracker->pitch();
    
        double nSum = neighbourSum(startCluster,iterDigit,digitCont );

        // make cluster +set things
        STCluster* newCluster = new STCluster(distToCenter, nSum, this->hasHighThreshold(totCharge,aSector));
     
        newCluster->setDigits(clusteredDigits);

        // add to container
        clusterCont->insert(newCluster,nearestStrip);
      }
    
    } // if found cluster seed

    iterDigit = jterDigit;
  
  } // iterDigit

  return StatusCode::SUCCESS;

}

bool STClusterCreator::aboveDigitSignalToNoise(const STDigit* aDigit, const DeSTSector* aSector) const{

  // digit above threshold ?
  return (aDigit->depositedCharge() > m_digitSig2NoiseCut[aSector]  ?  true : false);

}

bool STClusterCreator::aboveClusterSignalToNoise(const double charge, const DeSTSector* aSector) const{

  // cluster above threshold ?
  return (charge  > m_clusterSig2NoiseCut[aSector]  ?  true : false);
}

bool STClusterCreator::hasHighThreshold(const double charge, const DeSTSector* aSector) const{

  // cluster above threshold ?
  return (charge  > m_highSig2NoiseCut[aSector]  ?  true : false);
}

bool STClusterCreator::keepClustering(const STDigit* firstDigit, const STDigit* secondDigit, const DeSTSector* aSector) const{
 
  // check whether to continue clustering. Digits have to be above threshold and in 
  // consecutive channels in same wafer

 bool clusFlag = false;
 
 if (aboveDigitSignalToNoise(secondDigit, aSector) == true){
   const STChannelID firstChan = firstDigit->channelID();
   const STChannelID secondChan = secondDigit->channelID();
   if (m_tracker->nextRight(firstChan) == secondChan){
     if ((m_byBeetle == false)||(sameBeetle(firstChan,secondChan) == true)) clusFlag = true;  
   }
 }

 return clusFlag;
}

bool STClusterCreator::sameBeetle(const STChannelID firstChan, const STChannelID secondChan) const{
  unsigned int firstBeetle = (firstChan.strip() - 1)/LHCbConstants::nStripsInBeetle;
  unsigned int secondBeetle = (secondChan.strip() - 1)/LHCbConstants::nStripsInBeetle;
  return (firstBeetle == secondBeetle);
}

double STClusterCreator::neighbourSum(STDigits::const_iterator start, 
                                      STDigits::const_iterator stop, 
                                      const STDigits* digits) const{
  
  double nSum = 0;

  if (start != digits->begin()){
    STDigits::const_iterator iter = start;
    --iter;
    if ((*iter)->channelID() == m_tracker->nextLeft((*start)->channelID())) {
      nSum += (*iter)->depositedCharge();
    } 
  }

  if (stop != digits->end()){
    STDigits::const_iterator iter = stop;
    ++iter;
    if (iter != digits->end() && 
	((*iter)->channelID() == m_tracker->nextLeft((*start)->channelID()))) {
      nSum += (*iter)->depositedCharge();
    }
  }
  return nSum;
}





