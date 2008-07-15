#include "GaudiKernel/AlgFactory.h"

#include "Event/VeloLiteCluster.h"
#include "TfKernel/DefaultVeloRHitManager.h"
#include "BeamGasTrigExtractClusters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigExtractClusters
//
// 2008-06-16 : Kurt Rinnert <kurt.rinnert@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigExtractClusters );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigExtractClusters::BeamGasTrigExtractClusters(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty( "HitManagerName", m_hitManagerName = "DefaultVeloRHitManager"     );
  declareProperty( "OutputLocation", m_outputLocation = "Raw/Velo/UnusedRLiteClusters"     );
}

//=============================================================================
// Destructor
//=============================================================================
BeamGasTrigExtractClusters::~BeamGasTrigExtractClusters() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasTrigExtractClusters::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_rHitManager = tool<Tf::DefaultVeloRHitManager>( "Tf::DefaultVeloRHitManager", m_hitManagerName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigExtractClusters::execute()
{
  debug() << "==> Execute" << endmsg;

  LHCb::VeloLiteCluster::FastContainer* clusters = new LHCb::VeloLiteCluster::FastContainer();

  // make sure we have enough capacity in the container
  // to avoid unnecessary relocations
  // yes this is a guessed number based on <4% of signal events 
  // need more than clusters (one length doubling operation required)
  clusters->reserve(4096); 

  
  Tf::DefaultVeloRHitManager::StationIterator si   = m_rHitManager->stationsAllBegin();
  Tf::DefaultVeloRHitManager::StationIterator sEnd = m_rHitManager->stationsAllEnd();

  for ( ; si != sEnd; ++si ) {
    Tf::DefaultVeloRHitManager::Station* station = *si;
    for ( unsigned int zone=0; zone<station->nZones(); ++zone ) {
      Tf::VeloRHitRange hits = station->hits(zone);
      for ( Tf::VeloRHits::const_iterator ih=hits.begin(); ih != hits.end(); ++ih ) {
        const Tf::VeloRHit* hit = *ih;
        if ( hit->isUsed() ) continue;
        clusters->push_back(hit->cluster());
      }
    }
  } 
  
  put(clusters, m_outputLocation);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BeamGasTrigExtractClusters::finalize()
{
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


