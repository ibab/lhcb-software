// Gaudi
#include "GaudiKernel/IRegistry.h"

// xml geometry
#include "STDet/DeSTDetector.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"

// local
#include "STClusterClassification.h"

#include <boost/assign/std/vector.hpp>

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterClassification )

//--------------------------------------------------------------------
//
//  STClusterClassification: ...
//
//--------------------------------------------------------------------

STClusterClassification::STClusterClassification(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  ST::HistoAlgBase(name, pSvcLocator),
  m_nEvent(0) 
{
  // constructer
  this->declareProperty("SpillVector", m_spillVector);
  declareSTConfigProperty("inputData", m_clusterLocation , STClusterLocation::TTClusters);

  using namespace boost::assign;
  m_spillVector += "/", "/Prev/", "/PrevPrev/", "/Next/"; 
  m_hitLocation = MCHitLocation::TT;
  m_asctLocation = m_clusterLocation + "2MCHits";

  // register to be flippable from IT to TT
  addToFlipList(&m_hitLocation);
  addToFlipList(&m_asctLocation);
}


STClusterClassification::~STClusterClassification()
{
  // destructer
}

StatusCode STClusterClassification::initialize()
{ 
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin() ;
  for (; iSpillName != m_spillVector.end(); ++iSpillName){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+m_hitLocation;
    m_spillNames.push_back(mcHitPath);    
  } // iterSpillName
  
  return StatusCode::SUCCESS;

}

StatusCode STClusterClassification::execute()
{
  // retrieve clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);
  ++m_nEvent;

  // linker
  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  // histos per digit
  STClusters::const_iterator iterClus = clusterCont->begin() ;
  for( ; iterClus != clusterCont->end(); ++iterClus ) {
  Range range = aTable->relations(*iterClus);
    if (range.empty()){
      //empty range
      ++m_infoMap["noise"];
    }
    else {
      this->fillInfo((*range.begin()).to());
    }
  } // loop iterDep

  return StatusCode::SUCCESS;
}

StatusCode  STClusterClassification::finalize()
{
  // normalization
  unsigned total = tCount();
  unsigned int eventSpill = m_infoMap["primary"] + m_infoMap["secondary"]
    + m_infoMap["unknown"];
  unsigned int spillover = total - eventSpill - m_infoMap["noise"];

  info() << "--- Cluster Classification " << endmsg;
  info() << "Event Spill " << eventSpill/ double(total) << endmsg;
  info() << "   |---> Primary " << m_infoMap["primary"]/double(total) << endmsg;
  info() << "   |---> Secondary " << m_infoMap["secondary"]/ double(total) 
         << endmsg;
  info() << "   |---> Unknown " << m_infoMap["unknown"]/double(total) << endmsg;
  info() << "Noise " << m_infoMap["noise"]/double(total) <<" # per event " <<  m_infoMap["noise"]/m_nEvent << endmsg ;
  info() << "Spillover "  << spillover/double(total) << endmsg;
  
  std::map<std::string, unsigned int>::const_iterator iter = m_infoMap.begin();
  for(; iter != m_infoMap.end(); ++iter) {
    if ((iter->first != "noise")&&(iter->first != "primary") && 
        (iter->first != "unknown") &&(iter->first != "secondary")){
      info() << iter->first+" " << iter->second/ double(total) << endmsg;
    }
  }
  info() <<"----------------------------------------" << endmsg;
  
  return  ST::HistoAlgBase::finalize();
}

unsigned int STClusterClassification::tCount() const
{
  unsigned  total = 0;
  std::map<std::string, unsigned int>::const_iterator iter = m_infoMap.begin();
  for(; iter != m_infoMap.end(); ++iter) total += iter->second;
  return total;
}

StatusCode STClusterClassification::fillInfo(const MCHit* aHit) const
{
  // find out which spill came from....
  std::string spill = this->findSpill(aHit);

  if (spill == "/"){
    // event spill...
 
    const MCParticle* particle = aHit->mcParticle();
    if (0 != particle){
      const MCVertex* origin = particle->originVertex();
      if (origin != 0){
        if( (origin->type() < MCVertex::HadronicInteraction) &&
            (origin->type() != 0)){
          ++m_infoMap["primary"];  
        } 
        else if (origin->type() == 0){
          ++m_infoMap["unknown"];
        }
        else {
          ++m_infoMap["secondary"];
        }
      }  // vertex
    } // particle
  }
  else {
    ++m_infoMap[spill];   
  } 
  return StatusCode::SUCCESS;
}

std::string STClusterClassification::findSpill(const MCHit* aHit) const
{
  const ObjectContainerBase* parentCont = aHit->parent();
  const IRegistry* reg =  parentCont->registry();
  const std::string& idName = reg->identifier();

  unsigned int index = 0;
  while (index<m_spillNames.size()&&(m_spillNames[index]!=idName)){
    ++index;
  } //  iterS
  
  return index < m_spillNames.size() ? m_spillVector[index] : "Unknown";
}
