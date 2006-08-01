// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
// ITDet

#include "Kernel/OTChannelID.h"
#include "Kernel/OTDataFunctor.h"

#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTModule.h"
#include "OTDet/DeOTQuarter.h"

#include "OTDataSvc.h"

DECLARE_TOOL_FACTORY( OTDataSvc );


OTDataSvc::OTDataSvc(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_firstStation(0){

  // allow to skip TT hits
 this->declareProperty("nPartitionsPerLayer",m_partitionsPerLayer = 2);

 // interfaces
 declareInterface<IOTDataSvc>(this);
}

OTDataSvc::~OTDataSvc(){
  // destructer
}

StatusCode OTDataSvc::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }
  // get geometry
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default);

  // initialize partitions stuff
  m_firstStation = m_tracker->firstStation();
  this->initPartitions();  
 
  return StatusCode::SUCCESS;
}

StatusCode OTDataSvc::initPartitions()  {

 // Intialize partitions vector 
 int vectorSize = 0;
 
 // quadrants....
 unsigned int quad[2] = {0u, 2u};
 const DeOTDetector::Layers& tLayers = m_tracker->layers();
 DeOTDetector::Layers::const_iterator iterL = tLayers.begin();
 for (; iterL != tLayers.end() ; ++iterL){

    unsigned int nModules = (*iterL)->quarters().front()->modules().size();

    for (unsigned int iPart = 0; iPart< (unsigned int)m_partitionsPerLayer; ++iPart){
  
       LHCb::OTChannelID aChan((*iterL)->elementID().station(),
                               (*iterL)->elementID().layer(), quad[iPart], 1u ,0u);       
       m_Mapping[aChan] = vectorSize;
       m_lastQuad.push_back(quad[iPart]+1); 
       m_lastModule.push_back(nModules); 
       ++vectorSize;
    }  // iPart 
 }

  
 m_iterVector.resize(vectorSize); 
 m_partitionsPerStation = vectorSize/(m_tracker->nStation()-m_firstStation+1);  
 
 return StatusCode::SUCCESS;
}

StatusCode OTDataSvc::initializeEvent(){

 // intialize partitions structure once per event
  unsigned int iPart = 0u;  

 // retrieve clusters
 Tsa::OTClusters* clusCont = get<Tsa::OTClusters>(Tsa::OTClusterLocation::Default);
 
 // iterate over the map
 std::map<LHCb::OTChannelID,unsigned int>::iterator iterMap = m_Mapping.begin();
 Tsa::OTClusters::iterator clusIter;
 Tsa::OTClusters::iterator cachedIter = clusCont->begin();
 while (iterMap != m_Mapping.end()){

   // find first cluster
   LHCb::OTChannelID firstChan(iterMap->first);   

   // find
   clusIter = std::lower_bound(cachedIter,clusCont->end(),firstChan,
                               OTDataFunctor::compByModule_LB<const Tsa::OTCluster*>());

   if (clusIter != clusCont->end()){

     cachedIter = clusIter;

     LHCb::OTChannelID lastChan(firstChan.station(), 
                          firstChan.layer(), m_lastQuad[iPart],
                          firstChan.module()+
                          nModulesInPartition(firstChan.station(),firstChan.layer()), 0u);  

   
     clusIter = std::lower_bound(cachedIter,clusCont->end(),lastChan,
        OTDataFunctor::compByModule_LB<const Tsa::OTCluster*>());
 
     m_iterVector[iterMap->second] = Tsa::OTRange(cachedIter,clusIter);
       
   }
   else {
     // no data for this partition...
     m_iterVector[iterMap->second] = Tsa::OTRange(clusCont->begin());
      
   }
  ++iterMap;
  ++iPart;
 } // iterMap

 return StatusCode::SUCCESS;
}


Tsa::OTRange OTDataSvc::layer(const int& iStation, const int& iLayer) const{
  unsigned int start = this->offSet(iStation, iLayer);
  unsigned int stop = start + m_partitionsPerLayer;

  while ((start != stop) && (m_iterVector[start].empty() == true)) ++start;
  while ((start != stop) && (m_iterVector[stop].empty() == true)) --stop;

  // now go through the cases...
  return start == stop ?  m_iterVector[start] : Tsa::OTRange(m_iterVector[start].begin(), 
                                                             m_iterVector[stop].end()); 
}

Tsa::OTRange OTDataSvc::partition(const int& iStation, const int& iLayer, const int& iSector) const{
  return (m_iterVector[offSet(iStation,iLayer)+iSector-1]);
}

unsigned int OTDataSvc::nModulesInPartition(const int& iStation, const int& iLayer) const{
  return (m_lastModule[offSet(iStation,iLayer)]);
}

unsigned int OTDataSvc::lastQuad(const int& iStation, const int& iLayer) const{
  return (m_lastQuad[offSet(iStation,iLayer)]);
}


