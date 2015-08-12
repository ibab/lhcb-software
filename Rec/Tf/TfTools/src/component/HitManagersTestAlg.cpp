// Include files

// local
#include "HitManagersTestAlg.h"

//-----------------------------------------------------------------------------
// Instantiate instances of the hit managers
template class Tf::TStationHitManager<Tf::HitManagersTestAlg::TestHit>;
typedef Tf::TStationHitManager<Tf::HitManagersTestAlg::TestHit> MyTStationHitManager;
DECLARE_TOOL_FACTORY( MyTStationHitManager )
template class Tf::TTStationHitManager<Tf::HitManagersTestAlg::TestHit >;
typedef Tf::TTStationHitManager< Tf::HitManagersTestAlg::TestHit > MyTTStationHitManager;
DECLARE_TOOL_FACTORY( MyTTStationHitManager )
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HitManagersTestAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HitManagersTestAlg::HitManagersTestAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_tMan(NULL), m_ttMan(NULL) { }

//=============================================================================
// Destructor
//=============================================================================
HitManagersTestAlg::~HitManagersTestAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HitManagersTestAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  // load the hit mamangers
  m_tMan  = tool<THitMan> ( "Tf::TStationHitManager<Tf::HitManagersTestAlg::TestHit>",
                            "THitManager", this );
  m_ttMan = tool<TTHitMan>( "Tf::TTStationHitManager<Tf::HitManagersTestAlg::TestHit>",
                            "TTHitManager", this );

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HitManagersTestAlg::execute()
{
  typedef THitMan::HitRange  THitRange;
  typedef TTHitMan::HitRange TTHitRange;

  const unsigned int maxTries = 100;

  // toss a coin to call method to prepare all hits first 10% of the time
  if ( m_rndm() < 0.1 )
  {
    m_tMan->prepareHits();
    m_ttMan->prepareHits();
  }

  // T tests
  for ( unsigned int iTry = 0; iTry<maxTries; ++iTry )
  {

    // test getting hits for one region
    if ( m_rndm() < 0.3 )
    {
      const TStationID tStatID = tStationID();
      const TLayerID   tLayID  = tLayerID();
      const TRegionID  tRegID  = tRegionID();
      if ( m_rndm() < 0.1 ) m_tMan->prepareHits(tStatID,tLayID,tRegID);
      THitRange trange = m_tMan->hits(tStatID,tLayID,tRegID);
      debug() << "Found " << trange.size() << " T hits for "
              << tStatID << " " << tLayID << " " << tRegID << endreq;
      for ( THitRange::const_iterator iT = trange.begin(); iT != trange.end(); ++iT ) 
      { debug() << **iT << endreq;}
    }

    // test getting hits for one layer
    if ( m_rndm() < 0.3 )
    {
      const TStationID tStatID = tStationID();
      const TLayerID   tLayID  = tLayerID();
      if ( m_rndm() < 0.1 ) m_tMan->prepareHits(tStatID,tLayID);
      THitRange trange = m_tMan->hits(tStatID,tLayID);
      debug() << "Found " << trange.size() << " T hits for "
              << tStatID << " " << tLayID << endreq;
      for ( THitRange::const_iterator iT = trange.begin(); iT != trange.end(); ++iT )
      { debug() << **iT << endreq;}
    }

    // test getting hits for one station
    if ( m_rndm() < 0.3 )
    {
      const TStationID tStatID = tStationID();
      if ( m_rndm() < 0.1 ) m_tMan->prepareHits(tStatID);
      THitRange trange = m_tMan->hits(tStatID);
      debug() << "Found " << trange.size() << " T hits for "
              << tStatID << endreq;
      for ( THitRange::const_iterator iT = trange.begin(); iT != trange.end(); ++iT )
      { debug() << **iT << endreq;}
    }

    // test getting all hits
    if ( m_rndm() < 0.3 )
    {
      if ( m_rndm() < 0.1 ) m_tMan->prepareHits();
      THitRange trange = m_tMan->hits();
      debug() << "Found " << trange.size() << " T hits" << endreq;
      for ( THitRange::const_iterator iT = trange.begin(); iT != trange.end(); ++iT )
      { debug() << **iT << endreq;}
    }

  }

  // TT tests
  for ( unsigned int iTry = 0; iTry<maxTries; ++iTry )
  {

    // test getting hits for one region
    if ( m_rndm() < 0.3 )
    {
      const TTStationID ttStatID = ttStationID();
      const TTLayerID   ttLayID  = ttLayerID();
      const TTRegionID  ttRegID  = ttRegionID();
      if ( m_rndm() < 0.1 ) m_ttMan->prepareHits(ttStatID,ttLayID,ttRegID);
      TTHitRange ttrange = m_ttMan->hits(ttStatID,ttLayID,ttRegID);
      debug() << "Found " << ttrange.size() << " TT hits for "
              << ttStatID << " " << ttLayID << " " << ttRegID << endreq;
      for ( TTHitRange::const_iterator iTT = ttrange.begin(); iTT != ttrange.end(); ++iTT )
      { debug() << **iTT << endreq; }
    }

    // test getting hits for one layer
    if ( m_rndm() < 0.3 )
    {
      const TTStationID ttStatID = ttStationID();
      const TTLayerID   ttLayID  = ttLayerID();
      if ( m_rndm() < 0.1 ) m_ttMan->prepareHits(ttStatID,ttLayID);
      TTHitRange ttrange = m_ttMan->hits(ttStatID,ttLayID);
      debug() << "Found " << ttrange.size() << " TT hits for "
              << ttStatID << " " << ttLayID << endreq;
      for ( TTHitRange::const_iterator iTT = ttrange.begin(); iTT != ttrange.end(); ++iTT )
      { debug() << **iTT << endreq; }
    }

    // test getting hits for one station
    if ( m_rndm() < 0.3 )
    {
      const TTStationID ttStatID = ttStationID();
      if ( m_rndm() < 0.1 ) m_ttMan->prepareHits(ttStatID);
      TTHitRange ttrange = m_ttMan->hits(ttStatID);
      debug() << "Found " << ttrange.size() << " TT hits for " << ttStatID << endreq;
      for ( TTHitRange::const_iterator iTT = ttrange.begin(); iTT != ttrange.end(); ++iTT ) 
      { debug() << **iTT << endreq; }
    }

    // test getting all hits
    if ( m_rndm() < 0.3 )
    {
      if ( m_rndm() < 0.1 ) m_ttMan->prepareHits();
      TTHitRange ttrange = m_ttMan->hits();
      debug() << "Found " << ttrange.size() << " TT hits" << endreq;
      for ( TTHitRange::const_iterator iTT = ttrange.begin(); iTT != ttrange.end(); ++iTT ) 
      { debug() << **iTT << endreq; }
    }

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
