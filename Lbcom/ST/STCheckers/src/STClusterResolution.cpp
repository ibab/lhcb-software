//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Histogramming
#include "AIDA/IHistogram1D.h"

// BOOST
#include "boost/lexical_cast.hpp"

#include "STClusterResolution.h"
#include "Kernel/STDetSwitch.h"

//Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// STTools interfaces from LHCbKernel
#include "Kernel/IMCParticleSelector.h"
#include "Kernel/ISTClusterPosition.h"

using namespace LHCb;

#include "Kernel/PhysicalConstants.h"
static const AlgFactory<STClusterResolution> s_Factory;
const IAlgFactory& STClusterResolutionFactory = s_Factory;

//--------------------------------------------------------------------
//
//  STClusterResolution : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

STClusterResolution::STClusterResolution(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
 
{
  // constructer
  this->declareProperty("positionToolName", m_positionToolName = "STOnlinePosition");
  this->declareProperty("selectorName", m_selectorName = "MCParticleSelector" );
  this->declareProperty("detType", m_detType = "TT");
  this->declareProperty("asctLocation", m_asctLocation = "TTClusters2MCHits");
}

STClusterResolution::~STClusterResolution(){
  // destructer
}

StatusCode STClusterResolution::initialize(){

  // 
  if ( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // detector element
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // selector
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName, this);

  // position tool
  m_positionTool = tool<ISTClusterPosition>(m_positionToolName, "positionTool" , this);

  m_clusterLocation = STClusterLocation::TTClusters;
  STDetSwitch::flip(m_detType,m_clusterLocation);
  m_asctLocation = m_clusterLocation+"2MCHits";


  // intialize histos
  this->initHistograms();

  return StatusCode::SUCCESS;
}

StatusCode STClusterResolution::execute(){

  // execute once per event
  
  // retrieve clusters
  STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  // linker
  AsctTool associator(evtSvc(), m_clusterLocation+"2MCHits");
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  // histos per digit
  STClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){
    // get MC truth for this cluster
    Range range = aTable->relations(*iterClus);
    if (range.empty()){
      //empty range
    }
    else {
      this->fillHistograms(*iterClus,(*range.begin()).to());
    }
  } // loop iterClus

  return StatusCode::SUCCESS;
}

StatusCode STClusterResolution::initHistograms(){

 int id;
 for (id=1; id<=4;++id){
   
   AIDA::IHistogram1D* resHisto = book(id+10,
		 "resolution"+boost::lexical_cast<std::string>(id+10),
		 -250., 250., 200);
    
   m_resHistoVector.push_back(resHisto); 

      
   AIDA::IHistogram1D* onlineResHisto = book(id+30,
		 "online resolution"+boost::lexical_cast<std::string>(id+30),
		 -2.5, 2.5, 200);
    
   m_onlineResHistoVector.push_back(onlineResHisto); 

   AIDA::IHistogram1D* pullHisto = book(id+20,
		  "pull"+boost::lexical_cast<std::string>(id+20),
		 -10.,10., 200);
 
   m_pullHistoVector.push_back(pullHisto); 

     
 } // histID

 return StatusCode::SUCCESS;
}

StatusCode STClusterResolution::fillHistograms(const STCluster* aCluster,
                                               const MCHit* aHit) const{

  // fill histograms

  if ((0 != aHit)&&(m_selector->accept(aHit->mcParticle()))){

    // get true u - need stereoangle/z info from channel
    const STChannelID aChan = aCluster->channelID();
    const DeSTSector* aSector = m_tracker->findSector(aChan);
    const double uTrue = this->calculateUTrue(aHit,aSector);
    
    // rec u - offline
    ISTClusterPosition::Info measVal = m_positionTool->estimate(aCluster);
    double uRec = (measVal.fractionalPosition * aSector->pitch()) + aSector->localU(measVal.strip.strip());
    
    // determine which histos to fill based on cluster size
    const int id = this->histoId((int)aCluster->size());

    // fill double error = measVal.second;
    const double error = measVal.fractionalError * aSector->pitch();
    m_resHistoVector[id]->fill(uRec-uTrue);
    m_pullHistoVector[id]->fill((uRec-uTrue)/error);

    // now the online version
    double uOnline = (aCluster->interStripFraction() * aSector->pitch()) 
                   + aSector->localU(aChan.strip());
    m_onlineResHistoVector[id]->fill(uOnline-uTrue);
   
  } // aHit

  // end
  return StatusCode::SUCCESS;
}


int STClusterResolution::histoId(const int clusterSize) const{

 // determine which histogram to fill
 return (clusterSize <= 4 ?  clusterSize - 1 : 3);

}

double STClusterResolution::calculateUTrue(const MCHit* aHit,
                                           const DeSTSector* aSector) const{
 
  Gaudi::XYZPoint localPoint = aSector->toLocal(aHit->midPoint());
  return localPoint.x();
}
















