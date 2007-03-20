// $Id: TsaSTClusterCreator.cpp,v 1.8 2007-03-20 13:12:05 mneedham Exp $

//GaudiKernel
#include "GaudiKernel/AlgFactory.h"

// Det
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// Kernel
#include "Kernel/STChannelID.h"
#include "Kernel/STDetSwitch.h"

#include "Kernel/ISTClusterPosition.h"
#include "Kernel/IUsedLHCbID.h"

// Tsa includes
#include "Kernel/LHCbConstants.h"
#include "TsaSTClusterCreator.h"

//STL
#include <algorithm>

DECLARE_ALGORITHM_FACTORY( TsaSTClusterCreator );

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
  declareProperty("maxOcc", m_maxOcc = 48);
  this->declareProperty("inputLocation", m_inputLocation = LHCb::STLiteClusterLocation::ITClusters);
  this->declareProperty("positionTool",m_positionToolName = "STOnlinePosition");
  this->declareProperty("detType", m_detType = "IT");
  declareProperty("outputLocation", m_outputLocation = Tsa::STClusterLocation::IT);
  declareProperty("clusterFilterName", m_clusterFilterName = "TrackUsedLHCbID");
  declareProperty("filterClusters", m_filterClusters = false );
 
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
  STDetSwitch::flip(m_detType,m_outputLocation);
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // position tool
  m_positionTool = tool<ISTClusterPosition>(m_positionToolName);

  m_usedClusterTool = tool<IUsedLHCbID>(m_clusterFilterName , "TsaClusterFilter" );

  return StatusCode::SUCCESS;
}

StatusCode TsaSTClusterCreator::execute(){
  // Executes TsaSTClusterCreator for one event.

  // init
  //  startTimer();

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


  put(clusCont,m_outputLocation);

  //  stopTimer();

  if (m_hotBeetle != 0){
    return Warning(" hot Beetle", StatusCode::SUCCESS, 1);
  }

  return StatusCode::SUCCESS;

}

StatusCode TsaSTClusterCreator::finalize(){
   
  return TsaBaseAlg::finalize();
}


void TsaSTClusterCreator::convert(FastContainer*   liteCont, 
                                        Tsa::STClusters* clusCont) {
 
 // convert to patt rec clusters
 FastContainer::iterator clusIter = liteCont->begin();
 while (clusIter != liteCont->end()){
   // collect hits by beetle...
   FastContainer::iterator endBeetle;
   bool isHot = !processBeetle(clusIter, liteCont->end(), endBeetle);
   if (isHot == true) ++m_hotBeetle;    

   // sector info
   this->findSector(clusIter->channelID());
   const double pitch = m_cachedSector->pitch();

   for (; clusIter != endBeetle; ++clusIter){

     const double error = pitch * m_positionTool->error(clusIter->pseudoSize());
   
     LHCb::LHCbID id = LHCb::LHCbID(clusIter->channelID());
     bool take = true;
     if (m_filterClusters == true) take = !m_usedClusterTool->used(id);
    
     if (take == true){     
       Tsa::STCluster* aCluster = new Tsa::STCluster(m_cachedSector,error,*clusIter, isHot); 
       clusCont->add(aCluster);
     }
   } // iterBeetle
   
   clusIter = endBeetle;
   
 } // clusIter

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








