// Include files

// local
#include "TsaDataDebugAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TsaDataDebugAlg
//
// 2007-07-17 : Chris Jones
//-----------------------------------------------------------------------------

using namespace Tf::Tsa;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DataDebugAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DataDebugAlg::DataDebugAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ) 
{ 
  declareProperty("seedHitLocation", m_seedHitLocation = SeedHitLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
DataDebugAlg::~DataDebugAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DataDebugAlg::initialize()
{
  const StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) return sc;


  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DataDebugAlg::execute()
{
  StatusCode sc;

  sc = debugSeedHits();

  return sc;
}

//=============================================================================
// Debug the hits
//=============================================================================
StatusCode DataDebugAlg::debugSeedHits()
{
  // load the hits
  SeedHits* hits = get<SeedHits>(m_seedHitLocation);
  debug() << "Found " << hits->size() << " Seed Hits at " << m_seedHitLocation << endreq;

  // loop over the hits
  for ( SeedHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit )
  {
    verbose() << " -> Hit " << **iHit << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode DataDebugAlg::finalize()
{
  return GaudiHistoAlg::finalize();
}

//=============================================================================
