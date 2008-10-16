// $Id: STClusterChecker.cpp,v 1.17 2008-10-16 13:10:34 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"

// LHCbKernel
#include "Kernel/ISTSignalToNoiseTool.h"

#include "MCInterfaces/IMCParticleSelector.h"

// local
#include "STClusterChecker.h"

DECLARE_ALGORITHM_FACTORY( STClusterChecker );

using namespace LHCb;

//--------------------------------------------------------------------
//
//  STClusterChecker: Plots S/N and charge of STClusters for each readout sector
//
//--------------------------------------------------------------------

STClusterChecker::STClusterChecker( const std::string& name, 
                      ISvcLocator* pSvcLocator) :
  ST::HistoAlgBase(name, pSvcLocator),
  m_clusterLocation(STClusterLocation::TTClusters)
{
  // constructer
  declareProperty("SigNoiseTool", m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("SelectorName", m_selectorName     = "MCParticleSelector" );

  addToFlipList(&m_clusterLocation);
  m_asctLocation = m_clusterLocation + "2MCHits"; 
  addToFlipList(&m_asctLocation);

  setForcedInit();
}

STClusterChecker::~STClusterChecker()
{
  // destructer
}

StatusCode STClusterChecker::initialize()
{
  // intialize
  if( "" == histoTopDir() ) setHistoTopDir(detType()+"/");  
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  
  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName+detType() );

  // MCParticle selection tool
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName,this);

 
  return StatusCode::SUCCESS;
}

StatusCode STClusterChecker::execute()
{
  // retrieve clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  // linker
  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  // histos per cluster
  STClusters::const_iterator iterClus =  clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){
    // get MC truth for this cluster
    Range range = aTable->relations(*iterClus);
    if ( !range.empty() ) {
      this->fillHistograms(*iterClus,(*range.begin()).to());
    }
  } // loop iterClus

  return StatusCode::SUCCESS;
}

void STClusterChecker::fillHistograms( const STCluster* aCluster,
                                       const MCHit* aHit )
{
  // fill histograms
  if (0 != aHit){ 
    // histo cluster size for physics tracks
    if ( m_selector->accept(aHit->mcParticle()) == true ) {

      std::string dType = detectorType(aCluster->channelID());
      plot(aCluster->totalCharge(),dType+"/1","Charge", 0., 200., 200);
        plot(m_sigNoiseTool->signalToNoise(aCluster),
             dType+"/2","S/N",0.,100., 100);
    }  // accepted
  } // aHit 
}
