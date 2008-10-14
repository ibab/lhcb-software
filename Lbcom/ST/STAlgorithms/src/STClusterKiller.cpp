// $Id: STClusterKiller.cpp,v 1.2 2008-10-14 08:49:04 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STDetSwitch.h"
#include "STClusterKiller.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterKiller );

STClusterKiller::STClusterKiller( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_uniformDist( (IRndmGen*)0 ) {

  declareSTConfigProperty("InputLocation",m_inputLocation, STClusterLocation::TTClusters);
  declareProperty("Efficiency", m_eff = 0.995); 
}

STClusterKiller::~STClusterKiller()
{
  // STClusterKiller destructor
}

StatusCode STClusterKiller::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize, flat generator...
  IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = tRandNumSvc->generator( Rndm::Flat(0.,1.0), m_uniformDist.pRef() );
  if (sc.isFailure()) return Error( "Failed to init generator ", sc);
  sc = release(tRandNumSvc);
  if (sc.isFailure()) return Error( "Failed to release RndmGenSvc ", sc);
  
  return StatusCode::SUCCESS;
}


StatusCode STClusterKiller::execute()
{
  
  STClusters* clusterCont = get<STClusters>(m_inputLocation);

  // make list of clusters to remove
  std::vector<STChannelID> chanList; 
  removedClusters(clusterCont,chanList);

  // remove from the container
  std::vector<STChannelID>::reverse_iterator iterVec = chanList.rbegin(); 
  for (;  iterVec != chanList.rend(); ++iterVec){
    clusterCont->erase(*iterVec);
  } // iterVec

  return StatusCode::SUCCESS;
}

void STClusterKiller::removedClusters(const LHCb::STClusters* clusterCont,
                                      std::vector<LHCb::STChannelID>& deadClusters) const{

  const unsigned int nCluster = clusterCont->size();
  deadClusters.reserve((unsigned int)(nCluster*(1.0-m_eff)));
  STClusters::const_iterator iterC = clusterCont->begin();
  for ( ; iterC != clusterCont->end(); ++iterC){
    if (m_uniformDist->shoot() > m_eff){
      deadClusters.push_back((*iterC)->key());
    }
  } // iterC
}


