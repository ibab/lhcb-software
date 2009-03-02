// $Id: STClusterCreator.cpp,v 1.18 2009-03-02 08:10:50 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTClusterPosition.h"
#include "Kernel/ISTSignalToNoiseTool.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// local
#include "STClusterCreator.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterCreator );

STClusterCreator::STClusterCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_sigNoiseTool(0),
  m_positionTool(0)
{
  declareProperty("DigitSignal2Noise",m_digitSig2NoiseThreshold = 3.0);
  declareProperty("ClusterSignal2Noise",m_clusterSig2NoiseThreshold = 4.0);
  declareProperty("HighSignal2Noise",m_highThreshold = 10.0);
  declareProperty("SigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("PositionTool",m_positionToolName = "STOnlinePosition");
  declareSTConfigProperty("InputLocation",m_inputLocation, STDigitLocation::TTDigits);
  declareSTConfigProperty("OutputLocation",
                  m_outputLocation , STClusterLocation::TTClusters);
  declareProperty("OutputVersion", m_outputVersion = 1);
  declareProperty("Size", m_maxSize = 4);
  declareProperty("ByBeetle", m_byBeetle = true);  
  declareProperty("Spill", m_spillName = "Central");

  setForcedInit();

}

STClusterCreator::~STClusterCreator()
{
  // STClusterCreator destructor
}

StatusCode STClusterCreator::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, 
                                              m_sigNoiseToolName+detType());

  // calculate cut values
  const std::vector<DeSTSector*>& tSectors = tracker()->sectors(); 
  std::vector<DeSTSector*>::const_iterator iterS = tSectors.begin();
  for ( ; iterS != tSectors.end(); ++iterS ){
    const double adc =  m_sigNoiseTool->noiseInADC(*iterS);
    m_digitSig2NoiseCut[*iterS] = m_digitSig2NoiseThreshold*adc;
    m_clusterSig2NoiseCut[*iterS] = m_clusterSig2NoiseThreshold*adc; 
    m_highSig2NoiseCut[*iterS] = m_highThreshold*adc; 
  }
  // position tool
  m_positionTool = tool<ISTClusterPosition>(m_positionToolName);
 
  m_spill = STCluster::SpillToType(m_spillName);

  return StatusCode::SUCCESS;
}


StatusCode STClusterCreator::execute()
{
  // retrieve Digitizations
  const STDigits* digitCont = get<STDigits>(m_inputLocation);

  // clusterize
  STClusters* clusterCont = new STClusters();
  // set version number
  clusterCont->setVersion(m_outputVersion);
  // register STClusters in the transient data store
  put(clusterCont,m_outputLocation);


  // mak the clusters
  StatusCode sc = createClusters(digitCont, clusterCont);
  if (sc.isFailure()){
    return Warning("Failed to create clusters, register empty container", StatusCode::FAILURE);
  }  

  return sc;
}

StatusCode STClusterCreator::createClusters( const STDigits* digitCont, 
                                             STClusters* clusterCont ) const
{
  STDigits::const_iterator iterDigit = digitCont->begin();
  STDigits::const_iterator jterDigit;
  STDigits::const_iterator startCluster;

  while (iterDigit != digitCont->end()){

    jterDigit = iterDigit;
    ++jterDigit;
    
    DeSTSector* aSector= tracker()->findSector((*iterDigit)->channelID()); ;
    if (aboveDigitSignalToNoise(*iterDigit, aSector)) {

      // make a cluster !
      startCluster = iterDigit;
      SmartRefVector<STDigit> clusteredDigits;
      clusteredDigits.push_back(*iterDigit);      
      double totCharge = (*iterDigit)->depositedCharge(); 

      // clustering loop
      while ( (jterDigit != digitCont->end()) &&
              (iterDigit !=digitCont->end()) &&
              (clusteredDigits.size()<(unsigned int)m_maxSize) &&
              (keepClustering(*iterDigit,*jterDigit, aSector) == true) ) {
        clusteredDigits.push_back(*jterDigit);
        totCharge +=  (*jterDigit)->depositedCharge(); 
        ++iterDigit;
        ++jterDigit;
      } // clustering loop
  
      if (aboveClusterSignalToNoise(totCharge, aSector)){

	// calculate the interstrip fraction a la Tell1
        ISTClusterPosition::Info measValue = 
          m_positionTool->estimate(clusteredDigits);
      
        const double nSum = neighbourSum(startCluster,iterDigit,digitCont );

        // make cluster +set things
        const STLiteCluster clusterLite(measValue.strip, measValue.fractionalPosition,
                                  clusteredDigits.size(),
                                  hasHighThreshold(totCharge,aSector) );

        // link to the tell
        STDAQ::chanPair aPair = readoutTool()->offlineChanToDAQ(measValue.strip,
                                                                measValue.fractionalPosition);
        if (aPair.first.id() == STTell1ID::nullBoard) {
        // screwed
         err()  << "failed to link " << uniqueSector(measValue.strip) << endreq;
         return Error("Failed to find Tell1 board" ,  StatusCode::FAILURE);
        }       

        STCluster* newCluster = new STCluster(clusterLite,
                                              strips(clusteredDigits,
                                              measValue.strip), nSum,
                                              aPair.first.id() ,
                                              aPair.second, m_spill);
        // add to container
        clusterCont->insert(newCluster,measValue.strip);
      }
    } // if found cluster seed

    iterDigit = jterDigit;  
  } // iterDigit

  return StatusCode::SUCCESS;
}

bool STClusterCreator::aboveDigitSignalToNoise(const STDigit* aDigit,
                                               const DeSTSector* aSector) const
{
  // digit above threshold ?
  return (aDigit->depositedCharge() > m_digitSig2NoiseCut[aSector]  ?  
          true : false);
}

bool STClusterCreator::aboveClusterSignalToNoise(const double charge, 
                                                 const DeSTSector* sector) const
{
  // cluster above threshold ?
  return (charge  > m_clusterSig2NoiseCut[sector]  ?  true : false);
}

bool STClusterCreator::hasHighThreshold( const double charge,
                                         const DeSTSector* aSector ) const
{
  // cluster above threshold ?
  return (charge  > m_highSig2NoiseCut[aSector]  ?  true : false);
}

bool STClusterCreator::keepClustering(const STDigit* firstDigit, 
                                      const STDigit* secondDigit,
                                      const DeSTSector* aSector) const
{ 
  // check whether to continue clustering.
  // Digits have to be above threshold and in consecutive channels in same wafer
  bool clusFlag = false;
 
  if (aboveDigitSignalToNoise(secondDigit, aSector) == true){
    const STChannelID firstChan = firstDigit->channelID();
    const STChannelID secondChan = secondDigit->channelID();
    if (aSector->nextRight(firstChan) == secondChan){
      if ((m_byBeetle == false)||(sameBeetle(firstChan,secondChan) == true))
        clusFlag = true;  
    }
  }
  return clusFlag;
}

bool STClusterCreator::sameBeetle( const STChannelID firstChan, 
                                   const STChannelID secondChan ) const
{
  // check channels are on the sameBeetle
  const unsigned int firstBeetle = (firstChan.strip() - 1) / 
    LHCbConstants::nStripsInBeetle;
  const unsigned int secondBeetle = (secondChan.strip() - 1) / 
    LHCbConstants::nStripsInBeetle;
  return (firstBeetle == secondBeetle);
}

double STClusterCreator::neighbourSum( LHCb::STDigits::const_iterator start, 
                                       LHCb::STDigits::const_iterator stop, 
                                       const LHCb::STDigits* digits ) const
{  
  double nSum = 0;
  
  if (start != digits->begin()){
    STDigits::const_iterator iter = start;
    --iter;
    if ((*iter)->channelID() == tracker()->nextLeft((*start)->channelID())) {
      nSum += (*iter)->depositedCharge();
    }
  }

  if (stop != digits->end()){
    STDigits::const_iterator iter = stop;
    ++iter;
    if (iter != digits->end() && 
        ((*iter)->channelID() == tracker()->nextRight((*start)->channelID()))) {
      nSum += (*iter)->depositedCharge();
    }
  }
  return nSum;
}

STCluster::ADCVector 
STClusterCreator::strips(const SmartRefVector<STDigit>& clusteredDigits,
                         const STChannelID closestChan) const
{

  // make the vector of ADC values stored in the cluster
  SmartRefVector<STDigit>::const_iterator iter  = clusteredDigits.begin();
  STCluster::ADCVector tVec;
  for (; iter != clusteredDigits.end(); ++iter){
    const int strip = (*iter)->channelID().strip() - closestChan.strip();
    tVec.push_back(std::make_pair(strip,
                                  (unsigned int)((*iter)->depositedCharge())));
  } // i

  return tVec;
}
