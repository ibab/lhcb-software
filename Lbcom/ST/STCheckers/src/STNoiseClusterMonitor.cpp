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
#include "GaudiKernel/IRegistry.h"

// Histogramming
#include "AIDA/IHistogram1D.h"

#include "STNoiseClusterMonitor.h"

// xml geometry
#include "STDet/DeSTDetector.h"


// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"

#include "Kernel/STDetSwitch.h"

using namespace LHCb;

static const AlgFactory<STNoiseClusterMonitor> s_Factory;
const IAlgFactory& STNoiseClusterMonitorFactory = s_Factory;

//--------------------------------------------------------------------
//
//  STClusterMonitor : Check Clusterization procedure for the outer tracker
//
//--------------------------------------------------------------------

STNoiseClusterMonitor::STNoiseClusterMonitor(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0) 
{
  // constructer
  this->declareProperty("spillVector", m_SpillVector);
  this->declareProperty("detType", m_detType = "TT");

}

STNoiseClusterMonitor::~STNoiseClusterMonitor(){
  // destructer
}

StatusCode STNoiseClusterMonitor::initialize(){

  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");   
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // detector element
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  m_hitLocation = MCHitLocation::TT;
  m_clusterLocation = STClusterLocation::TTClusters;
  STDetSwitch::flip(m_detType,m_clusterLocation);
  STDetSwitch::flip(m_detType,m_hitLocation);
  m_asctLocation = m_clusterLocation + "2MCHits";

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_SpillVector.begin() ;
  for (; iSpillName!=m_SpillVector.end(); ++iSpillName){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+m_hitLocation;
    m_spillNames.push_back(mcHitPath);    
    if (*iSpillName == "/") {
      // store index of main spill
      m_eventIndex = m_spillNames.size()-1;
    }
  } // iterSpillName
  
  return StatusCode::SUCCESS;

}

StatusCode STNoiseClusterMonitor::execute(){

  // retrieve clusters
  STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  // linker
  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  // histos per digit
  STClusters::const_iterator iterClus = clusterCont->begin() ;
  for( ; iterClus != clusterCont->end(); ++iterClus){
  Range range = aTable->relations(*iterClus);
    if (range.empty()){
      //empty range
    }
    else {
      this->fillHistograms((*range.begin()).to(), *iterClus);
    }
  } // loop iterDep

  return StatusCode::SUCCESS;
}


StatusCode STNoiseClusterMonitor::fillHistograms(const MCHit* aHit, const STCluster* aCluster) const{

 
  // type
  DeSTSector* aSector = m_tracker->findSector(aCluster->channelID());

  if ((0 != aHit)&&(0 != aSector)){

    // find out which spill came from....
    int spill = this->findSpillIndex(aHit);

     // histo per station 
    plot(spill,"spill histo"+aSector->type(), -0.5, 10.5, 11);
    if (spill == m_eventIndex){
      // hit from event spill
      plot(0.,"case "+ aSector->type(),-0.5,10.5 ,11);
    }
    else {
      // hit from another spill
      plot(1.,"case"+ aSector->type(),-0.5,10.5 ,11);
    } 

  }
  else {
    // pure noise or xtalk
    plot(2.,"case"+aSector->type(),-0.5,10.5 ,11);
  }

  return StatusCode::SUCCESS;
}

int STNoiseClusterMonitor::findSpillIndex(const MCHit* aHit) const{
  
  int spillID = -1;

  const ObjectContainerBase* parentCont = aHit->parent();
  const IRegistry* reg =  parentCont->registry();
  const std::string& idName = reg->identifier();

  unsigned int index = 0;
  while (index<m_spillNames.size()&&(m_spillNames[index]!=idName)){
    index++;
  } //  iterS
  
  if (index != m_spillNames.size()){
    spillID = (int)index;
  }

  return spillID;
}











