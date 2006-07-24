// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
// ITDet

#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include "ITDataSvc.h"

static const ToolFactory<ITDataSvc>  s_factory;
const IToolFactory& ITDataSvcFactory = s_factory;

ITDataSvc::ITDataSvc(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_firstStation(0){

  // allow to skip TT hits
 this->declareProperty("nPartitionsPerLayer",m_partitionsPerLayer = 2);

 // interfaces
 declareInterface<IITDataSvc>(this);
}

ITDataSvc::~ITDataSvc(){
  // destructer
}

StatusCode ITDataSvc::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }
  // get geometry
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::IT);

  // initialize partitions stuff
  this->initPartitions();  
 
  return StatusCode::SUCCESS;
}

StatusCode ITDataSvc::initPartitions()  {

 // Intialize partitions vector 
 int vectorSize = 0;

 DeSTDetector::Sectors tSectors = m_tracker->sectors();
 unsigned int cachedLayer = 0;

 for (DeSTDetector::Sectors::const_iterator iterSector = tSectors.begin(); 
      iterSector != tSectors.end(); ++iterSector){

   if ((*iterSector)->elementID().uniqueLayer() != cachedLayer ){
     // cache
     m_Mapping[(*iterSector)->elementID()] = vectorSize;
     cachedLayer = (*iterSector)->elementID().uniqueLayer();
     ++vectorSize;
   } 
 } // iterSector
  
 m_nPartitions = vectorSize;
 m_iterVector.resize(m_nPartitions); 
 m_firstStation = m_tracker->firstStation();
 m_partitionsPerStation = m_nPartitions/(m_tracker->nStation()+1-m_firstStation);

 unsigned int nBox = 4u; // need to get this from somewhere....
 m_partitionsPerLayer = m_partitionsPerStation/nBox;    
 m_sectorsPerPartition = tSectors.size()/m_nPartitions;

 return StatusCode::SUCCESS;
}

StatusCode ITDataSvc::initializeEvent(){

 // intialize partitions structure once per event

 // retrieve clusters
 Tsa::STClusters* clusCont = get<Tsa::STClusters>(Tsa::STClusterLocation::IT);
 
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
     
     LHCb::STChannelID lastChan(LHCb::STChannelID::typeIT, firstChan.station(), 
                                firstChan.layer(), firstChan.detRegion(),
                                firstChan.sector() + m_sectorsPerPartition, 0u);
 
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

Tsa::STRange ITDataSvc::layer(const int& iStation, const int& iLayer) const{

  unsigned int start = this->offSet(iStation, iLayer);
  unsigned int stop = start + m_partitionsPerLayer;

  while ((start != stop) && (m_iterVector[start].empty() == true)) ++start;
  while ((start != stop) && (m_iterVector[stop].empty() == true)) --stop;

  // now go through the cases...
  return start == stop ?  m_iterVector[start] : Tsa::STRange(m_iterVector[start].begin(), 
                                                             m_iterVector[stop].end());
}

Tsa::STRange ITDataSvc::partition(const int& iStation, const int& iLayer, const int& iBox) const{
  return (m_iterVector[offSet(iStation,iLayer)+iBox-1]);
}





