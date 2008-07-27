// $Id: BeamGasTrigClusterCut.cpp,v 1.1.1.2 2008-07-27 11:35:09 phopchev Exp $
#include "GaudiKernel/AlgFactory.h"

#include "Event/Track.h"
#include "TfKernel/DefaultVeloRHitManager.h"

#include "BeamGasTrigClusterCut.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigClusterCut
//
// 2008-06-16 : Plamen Hopchev <plamen.hopchev@cern.ch>
//
// The algorithm rejects the event if
// We don't have enough unused clusters
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigClusterCut );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigClusterCut::BeamGasTrigClusterCut(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty( "HitManagerName", m_hitManagerName = "DefaultVeloRHitManager" );
  declareProperty( "SensorsBegin", m_sensorsBegin = 22 );
  declareProperty( "SensorsEnd",   m_sensorsEnd   = 39 );
  declareProperty( "FracUnusedClustersCut", m_fracUnusedClustersCut = 0.0 );    
}

//=============================================================================
// Destructor
//=============================================================================
BeamGasTrigClusterCut::~BeamGasTrigClusterCut() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasTrigClusterCut::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_rHitManager = tool<Tf::DefaultVeloRHitManager>( "Tf::DefaultVeloRHitManager", m_hitManagerName );
  
  if( m_sensorsBegin < 0 || m_sensorsEnd > 41 || m_sensorsBegin > m_sensorsEnd ){
    fatal() << "Incorrect Velo Sensors Subset !!!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigClusterCut::execute()
{
  debug() << "==> Execute" << endmsg;
  
  // Evaluate the fraction of unused clusters in a subset of the Velo Sensors and
  // set final decision of the BeamGasClusterCut algorithm
  Tf::DefaultVeloRHitManager::StationIterator si   = m_rHitManager->stationIterAll(m_sensorsBegin);
  Tf::DefaultVeloRHitManager::StationIterator sEnd = m_rHitManager->stationIterAll(m_sensorsEnd);
    
  int nRHits=0;
  int nUsedRHits = 0;
    
  for ( ; si <= sEnd; ++si ) {
    Tf::DefaultVeloRHitManager::Station* station = *si;
    for ( unsigned int zone=0; zone<station->nZones(); ++zone ) {
      Tf::VeloRHitRange hits = station->hits(zone);
      for ( Tf::VeloRHits::const_iterator ih=hits.begin(); ih != hits.end(); ++ih ) {
        const Tf::VeloRHit* hit = *ih;
	++nRHits;
        if ( hit->isUsed() ) ++nUsedRHits;
      }
    }
  } 
  
  float frUnusedRClusters = nRHits ? ( ( nRHits - nUsedRHits ) / (float)nRHits ) : 0.0001 ;
   
  bool trigPass = false;
  if ( frUnusedRClusters > m_fracUnusedClustersCut ) trigPass = true;
  
  setFilterPassed(trigPass);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BeamGasTrigClusterCut::finalize()
{
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


