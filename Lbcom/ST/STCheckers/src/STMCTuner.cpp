// $Id: STMCTuner.cpp,v 1.8 2006-12-22 12:23:01 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// LHCbKernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTSignalToNoiseTool.h"
#include "Kernel/IMCParticleSelector.h"

// local
#include "STMCTuner.h"

DECLARE_ALGORITHM_FACTORY( STMCTuner );

using namespace LHCb;

//--------------------------------------------------------------------
//
//  STMCTuner: Plots S/N and charge of STClusters for each readout sector
//
//--------------------------------------------------------------------

STMCTuner::STMCTuner(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator)
 
{
  // constructer
  declareProperty("SigNoiseTool", m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("DetType",      m_detType          = "TT"                 );
  declareProperty("SelectorName", m_selectorName     = "MCParticleSelector" );
}

STMCTuner::~STMCTuner()
{
  // destructer
}

StatusCode STMCTuner::initialize()
{
  // intialize
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");  
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // detector element
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName+m_detType );

  // MCParticle selection tool
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName,this);

  m_clusterLocation = STClusterLocation::TTClusters;
  STDetSwitch::flip(m_detType,m_clusterLocation);
  m_asctLocation = m_clusterLocation + "2MCHits";
  return StatusCode::SUCCESS;
}

StatusCode STMCTuner::execute()
{
  // retrieve clusters
  STClusters* clusterCont = get<STClusters>(m_clusterLocation);

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

StatusCode STMCTuner::fillHistograms( const STCluster* aCluster,
                                      const MCHit* aHit )
{
  // fill histograms
  if (0 != aHit){ 
    // histo cluster size for physics tracks
    if ( m_selector->accept(aHit->mcParticle()) == true ) {
      DeSTSector* aSector = m_tracker->findSector(aCluster->channelID());
      if (aSector != 0){
        plot(m_sigNoiseTool->signalToNoise(aCluster),"SN_"+aSector->type(),0.,
             50., 100);
        plot(aCluster->totalCharge(),"charge_"+aSector->type(), 0., 200., 200);
      }
    } 
  }
  return StatusCode::SUCCESS;
}
