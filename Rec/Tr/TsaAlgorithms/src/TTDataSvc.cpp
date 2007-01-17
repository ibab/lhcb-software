// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
// ITDet

#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeSTSector.h"

#include "TTDataSvc.h"

#include <algorithm>

DECLARE_TOOL_FACTORY( TTDataSvc );

TTDataSvc::TTDataSvc(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){

 // interfaces

 declareProperty("inputLocation", m_inputLocation = Tsa::STClusterLocation::TT);
 declareProperty("sortData",m_sortData = false);

 declareInterface<ITTDataSvc>(this);
}

TTDataSvc::~TTDataSvc(){
  // destructer
}

StatusCode TTDataSvc::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }
  // get geometry
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::TT);

  // initialize partitions stuff
  this->initPartitions();  
 
  return StatusCode::SUCCESS;
}

void TTDataSvc::initPartitions()  {

  // Intialize partitions vector 
  int vectorSize = 0;

  const DeSTDetector::Layers& detLayers = m_tracker->layers();

  // copy to a temporary vector and sort
  std::vector<DeSTLayer*> tLayers;
  tLayers.insert(tLayers.begin(), detLayers.begin(), detLayers.end());
  std::sort(tLayers.begin(), tLayers.end(), STDataFunctor::Less_by_ElementID<DeSTLayer*>());

  for (std::vector<DeSTLayer*>::const_iterator iterLayer = tLayers.begin();
       iterLayer != tLayers.end(); ++iterLayer){
    m_Mapping[(*iterLayer)->elementID()] = vectorSize;
   
    // get the last sector - a bit ugly
    const DeSTLayer::Sectors& sectors = (*iterLayer)->sectors();
    LHCb::STChannelID lastChan;
    for (DeSTLayer::Sectors::const_iterator iterSector = sectors.begin(); iterSector != sectors.end(); ++iterSector){
      if ((*iterSector)->elementID() > lastChan) lastChan = (*iterSector)->elementID();
    }  //iterSector 

    m_nSectorsPerPartition.push_back(lastChan.sector());
    m_nRegions.push_back(lastChan.detRegion());

    ++vectorSize;
  
  } // iterLayer


 m_layersPerStation = 2; 
  
 m_nPartitions = vectorSize;
 m_iterVector.resize(m_nPartitions);
 
 return;
}

StatusCode TTDataSvc::initializeEvent(){
  Tsa::STClusters* clusCont = get<Tsa::STClusters>(m_inputLocation);
  return initializeEvent(clusCont);
}

StatusCode TTDataSvc::initializeEvent(Tsa::STClusters* clusCont){

 // intialize partitions structure once per event

 // retrieve clusters
  // Tsa::STClusters* clusCont = get<Tsa::STClusters>(m_inputLocation);

 m_dataSize = clusCont->size();

 // might need to sort
 if (m_sortData == true){
   std::stable_sort(clusCont->begin(),clusCont->end(), STDataFunctor::Less_by_Channel<const Tsa::STCluster*>());
 }


 // iterate over the map
 Tsa::STClusters::iterator clusIter;
 Tsa::STClusters::iterator cachedIter = clusCont->begin();

 std::map<LHCb::STChannelID,unsigned int>::iterator iterMap = m_Mapping.begin();
 for (;iterMap != m_Mapping.end(); ++iterMap){


   // find first cluster
   LHCb::STChannelID firstChan(iterMap->first);   

   // find
   clusIter = std::lower_bound(cachedIter,clusCont->end(),firstChan,
                               STDataFunctor::compBySector_LB<const Tsa::STCluster*>());

   if (clusIter != clusCont->end()){

     cachedIter = clusIter;
     
     LHCb::STChannelID lastChan(LHCb::STChannelID::typeTT, firstChan.station(), 
                                firstChan.layer(), m_nRegions[iterMap->second],
                                firstChan.sector() + m_nSectorsPerPartition[iterMap->second] + 1 , 0u);
 
     clusIter = std::lower_bound(cachedIter,clusCont->end(),lastChan,
        STDataFunctor::compBySector_LB<const Tsa::STCluster*>());
 
     m_iterVector[iterMap->second] = Tsa::STRange(cachedIter,clusIter);
       
   }
   else {
     // no data for this partition...
     m_iterVector[iterMap->second] = Tsa::STRange(clusCont->begin());
   }

 } // iterMap

 return StatusCode::SUCCESS;
}


Tsa::STRange TTDataSvc::layer(const int& iStation, const int& iLayer) const{
  return (m_iterVector[offSet(iStation,iLayer)]);
}

unsigned int TTDataSvc::dataSize() const{
  return m_dataSize;
}
