// $Id: TsaSTClusterCreator.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $

//GaudiKernel
#include "GaudiKernel/AlgFactory.h"

// Det
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// Kernel
#include "Kernel/STChannelID.h"
#include "Kernel/STDetSwitch.h"

#include "Kernel/ISTClusterPosition.h"

// Tsa includes
#include "Kernel/LHCbConstants.h"
#include "TsaSTClusterCreator.h"

//STL
#include <algorithm>

// Needed for the creation of TrSimpleLayersCreator objects.
static const AlgFactory<TsaSTClusterCreator> s_factory;
const IAlgFactory& TsaSTClusterCreatorFactory = s_factory;
//_________________________________________________
// TsaSTClusterCreator
//
// An TrClusterCreator is a Gaudi sub algorithm that creates Inner tracker Cluster 
// from the digitizations and stores them in the correct layer
// 


TsaSTClusterCreator::TsaSTClusterCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator),
  m_cachedSector(0),
  m_cachedSectorID(0)
{
  // TsaSTClusterCreator constructor
  declareProperty("maxOcc", m_maxOcc = 32);
  this->declareProperty("inputLocation", m_inputLocation = LHCb::STLiteClusterLocation::ITClusters);
  this->declareProperty("positionTool",m_positionToolName = "STOnlinePosition");
  this->declareProperty("detType", m_detType = "IT");
}

TsaSTClusterCreator::~TsaSTClusterCreator()
{
  // TsaSTClusterCreator destructor
}

StatusCode TsaSTClusterCreator::initialize()
{
  // Initializes TsaSTClusterCreator at the begin of program execution.
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  // get geometry
  STDetSwitch::flip(m_detType,m_inputLocation);
 m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // position tool
  m_positionTool = tool<ISTClusterPosition>(m_positionToolName);

  return StatusCode::SUCCESS;
}

StatusCode TsaSTClusterCreator::execute(){
  // Executes TsaSTClusterCreator for one event.

  // init
  startTimer();

  m_cachedSector = 0;
  m_cachedSectorID = 0;
  m_hotBeetle = 0; 
 
  // retrieve clusters
  FastContainer* liteCont = get<FastContainer>(m_inputLocation);

  // plot(liteCont->size(),"size" ,0., 2000. ,100);
 
  // clusterize
  Tsa::STClusters* clusCont = new Tsa::STClusters();
  clusCont->reserve(liteCont->size());
  this->convert(liteCont, clusCont);

  put(clusCont,Tsa::STClusterLocation::IT);

  stopTimer();

  if (m_hotBeetle != 0){
    return Warning(" hot Beetle", StatusCode::SUCCESS, 1);
  }

  return StatusCode::SUCCESS;

}

StatusCode TsaSTClusterCreator::finalize(){
   
  return TsaBaseAlg::finalize();
}


StatusCode TsaSTClusterCreator::convert(FastContainer*   liteCont, 
                                        Tsa::STClusters* clusCont) {
 
 // convert to patt rec clusters
 FastContainer::iterator clusIter = liteCont->begin();
 while (clusIter != liteCont->end()){
   // collect hits by beetle...
   FastContainer::iterator endBeetle;
   if ( processBeetle(clusIter, liteCont->end(), endBeetle) == true){
     // we want to process this beetle
     this->findSector(clusIter->channelID());
     const double pitch = m_cachedSector->pitch();

     for (; clusIter != endBeetle; ++clusIter){

       //  std::auto_ptr<LHCb::Trajectory> traj = m_cachedSector->trajectory(clusIter->channelID(), clusIter->interStripFraction());
       const double error = pitch * m_positionTool->error(clusIter->pseudoSize());
        
       Tsa::STCluster* aCluster = new Tsa::STCluster(m_cachedSector,error,*clusIter); 
       clusCont->add(aCluster);
     } // iterBeetle
   }
   else {
     ++m_hotBeetle;
   }
   
   clusIter = endBeetle;
   
 } // clusIter

 return StatusCode::SUCCESS;

}


bool TsaSTClusterCreator::processBeetle(FastContainer::iterator start, 
                                        FastContainer::iterator stop,
                                        FastContainer::iterator& iterCluster){

 // collect all hits in one beetle
 int iCount = 0u;

 iterCluster = start;
 const unsigned int startSector = start->channelID().uniqueSector();
 const unsigned int startBeetle = (start->channelID().strip()-1)/LHCbConstants::nStripsInBeetle;
 while ((iterCluster != stop)
	&&(iterCluster->channelID().uniqueSector() == startSector)
        &&((iterCluster->channelID().strip()-1)/LHCbConstants::nStripsInBeetle == startBeetle)){
   iCount += iterCluster->pseudoSize();
   ++iterCluster; 
 } // iterDigit

 return (iCount > m_maxOcc ? false : true);
}








