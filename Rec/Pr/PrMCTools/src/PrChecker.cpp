// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/AllLinks.h"

#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCProperty.h"
// local
#include "PrChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrChecker
//
// 2005-03-29 : Olivier Callot
//modified by Wenbin Qian for Upgrade
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( PrChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrChecker::PrChecker( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_velo(NULL),
    m_forward(NULL),
    m_tTrack(NULL),
    m_best(NULL)
{
  declareProperty( "VeloTracks",        m_veloTracks      = LHCb::TrackLocation::Velo    );
  declareProperty( "ForwardTracks",     m_forwardTracks   = LHCb::TrackLocation::Forward );
  declareProperty( "SeedTracks",        m_seedTracks      = LHCb::TrackLocation::Seed    );
  
}
//=============================================================================
// Destructor
//=============================================================================
PrChecker::~PrChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_velo = tool<PrCounter>( "PrCounter", "Velo", this );
  m_velo->setContainer( m_veloTracks );
  m_velo->setSelectId( 3 );
  m_velo->addSelection( "OK for Velo" );
  m_velo->addSelection( "long" );
  m_velo->addSelection( "long > 5 GeV" );
  m_velo->addSelection( "Strange+long" );
  m_velo->addSelection( "and > 5 GeV" );
  m_velo->addSelection( "long from B decay" );
  m_velo->addSelection( "and > 5 GeV" );

  m_forward = tool<PrCounter>( "PrCounter", "Forward", this );
  m_forward->setContainer( m_forwardTracks );
  m_forward->setSelectId( 8 );
  m_forward->addSelection( "long" );
  m_forward->addSelection( "long > 5 GeV" );
  m_forward->addSelection( "Strange+long" );
  m_forward->addSelection( "and > 5 GeV" );
  m_forward->addSelection( "long from B decay" );
  m_forward->addSelection( "and > 5 GeV" );

  m_tTrack = tool<PrCounter>( "PrCounter", "TTrack", this );
  m_tTrack->setContainer( m_seedTracks );
  m_tTrack->setSelectId( 8 );
  m_tTrack->addSelection( "has T hits" );
  m_tTrack->addSelection( "long" );
  m_tTrack->addSelection( "long > 5 GeV" );
  m_tTrack->addSelection( "Strange+T+TT" );
  m_tTrack->addSelection( "and > 5 GeV" );

  m_best = tool<PrCounter>( "PrCounter", "Best", this );
  m_best->setContainer( LHCb::TrackLocation::Default );
  m_best->setSelectId( 15 );
  m_best->addSelection( "long" );
  m_best->addSelection( "long > 5 GeV" );
  m_best->addSelection( "Strange+long" );
  m_best->addSelection( "and > 5 GeV" );
  m_best->addSelection( "long from B decay" );
  m_best->addSelection( "and > 5 GeV" );

  m_allCounters.push_back( m_velo  );
  m_allCounters.push_back( m_forward );
  m_allCounters.push_back( m_tTrack );
  m_allCounters.push_back( m_best  );

  m_counters.resize(20);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrChecker::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  for ( std::vector<PrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->initEvent();
  }

  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  LHCb::MCVertices* mcVert = get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default );
  unsigned int nPrim = 0;
  for ( LHCb::MCVertices::iterator itV = mcVert->begin(); mcVert->end() != itV; ++itV ) {
    if ( (*itV)->isPrimary() ) {
      int nbVisible = 0;
      for ( LHCb::MCParticles::iterator itP = mcParts->begin();
            mcParts->end() != itP; ++itP ) {
        if ( (*itP)->primaryVertex() == *itV ) {
          if ( trackInfo.hasVelo( *itP ) ) nbVisible++;
        }
      }
      if ( nbVisible > 4 ) ++nPrim;
    }
  }
  if ( nPrim >= m_counters.size() ) nPrim = m_counters.size()-1;
  m_counters[nPrim].nEvt++;
  m_counters[nPrim].total += m_forward->nbTrack();
  m_counters[nPrim].ghost += m_forward->nbGhost();

  //== Build a table (vector of vectors) of ids per MCParticle, indexed by MCParticle key.
  AllLinks<LHCb::MCParticle> allIds( evtSvc(), msgSvc(), "Pr/LHCbID" );
  std::vector< std::vector<int> > linkedIds;
  LHCb::MCParticle* part = allIds.first();
  while ( NULL != part ) {
    unsigned int minSize = part->key();
    while ( linkedIds.size() <= minSize ) {
      std::vector<int> dum;
      linkedIds.push_back( dum );
    }
    linkedIds[part->key()].push_back( allIds.key() );
    part = allIds.next();
  }

  LHCb::MCParticles::const_iterator itP;
  for ( itP = mcParts->begin(); mcParts->end() != itP; ++itP ) {
    LHCb::MCParticle* part = *itP;
    if ( 0 == trackInfo.fullInfo( part ) ) continue;
    if( msgLevel(MSG::VERBOSE) ) verbose() << "checking MCPart " << part->key() << endmsg;

    bool isLong  = trackInfo.hasVeloAndT( part );
    isLong = isLong && ( abs( part->particleID().pid() ) != 11 ); // and not electron
    
    bool over5       = 5000. < fabs( part->p() );
    bool isInVelo    = trackInfo.hasVelo( part );
    bool strangeLong = false;
    bool strangeDown = false;
    bool fromB       = false;
    
    if ( 0 != part->originVertex() ) {
      const LHCb::MCParticle* mother =  part->originVertex()->mother();
      if ( 0 != mother ) {
        if ( 0 != mother->originVertex() ) {
          double rOrigin = mother->originVertex()->position().rho();
          if ( fabs( rOrigin ) < 5. ) {
            int pid = abs( mother->particleID().pid() );
            if ( 130 == pid ||    // K0L
                 310 == pid ||    // K0S
                 3122 == pid ||   // Lambda
                 3222 == pid ||   // Sigma+
                 3212 == pid ||   // Sigma0
                 3112 == pid ||   // Sigma-
                 3322 == pid ||   // Xsi0
                 3312 == pid ||   // Xsi-
                 3334 == pid      // Omega-
                 ) {
              strangeDown = trackInfo.hasT( part ) &&  trackInfo.hasTT( part );
              strangeLong = trackInfo.hasVeloAndT( part );
            }
          }
        }
      }
      while( 0 != mother ) {
        if ( mother->particleID().hasBottom() ) fromB = true;
        mother = mother->originVertex()->mother();
      }
    }
    
    std::vector<LHCb::LHCbID> ids;
    if ( linkedIds.size() > (unsigned int) part->key() ) {
      for ( std::vector<int>::const_iterator itIm = linkedIds[part->key()].begin();
            linkedIds[part->key()].end() != itIm; ++itIm ) {
        LHCb::LHCbID temp;
        temp.setDetectorType( (*itIm) >> 28 );  // create LHCbId from int. Clumsy !
        temp.setID( (*itIm) );
        ids.push_back( temp );
      }
    }
    
    if( msgLevel(MSG::VERBOSE) ) verbose() << "MCPart " << part->key() << " has " << ids.size() << " LHCbIDs " <<endmsg;
    
    std::vector<bool> flags;
    flags.push_back( isInVelo );
    flags.push_back( isLong  );
    flags.push_back( isLong && over5   );
    flags.push_back( isLong && strangeLong );
    flags.push_back( isLong && strangeLong && over5 );
    flags.push_back( isLong && fromB );
    flags.push_back( isLong && fromB && over5 );

    m_velo->count( part, flags, ids );

    flags.clear();
    flags.push_back( isLong  );
    flags.push_back( isLong && over5   );
    flags.push_back( isLong && strangeLong );
    flags.push_back( isLong && strangeLong && over5 );
    flags.push_back( isLong && fromB );
    flags.push_back( isLong && fromB && over5 );

    int ok = m_forward->count( part, flags, ids );
    m_best->count( part, flags, ids );

    if ( isLong ) {
      m_counters[nPrim].nLong++;
      if ( 0 <= ok ) m_counters[nPrim].okLong++;
      if ( over5 ) {
        m_counters[nPrim].nLong5++;
        if ( 0 <= ok )  m_counters[nPrim].okLong5++;
      }
    }

    flags.clear();
    flags.push_back( trackInfo.hasT( part ) );
    flags.push_back( isLong );
    flags.push_back( isLong && over5 );
    flags.push_back( strangeDown );
    flags.push_back( strangeDown && over5 );

    m_tTrack->count( part, flags, ids );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrChecker::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  for ( std::vector<PrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->printStatistics();
  }
  for ( std::vector<EffCounter>::iterator itE = m_counters.begin(); m_counters.end() != itE; ++itE ) {
    if ( (*itE).total > 0 ) {
      float nTr = float( (*itE).total );
      float nGh = float( (*itE).ghost );
      float nl  = float( (*itE).nLong ) + 1.e-6;
      float okl = float( (*itE).okLong );
      float n5  = float( (*itE).nLong5 ) + 1.e-6;
      float ok5 = float( (*itE).okLong5 );
      
      float ghostRate = 100. * nGh / nTr;
      float effLong   = 100. * okl / nl;
      float effLong5  = 100. * ok5 / n5;
      float errGhost  = 100. * sqrt( nGh * (nTr-nGh) / nTr ) / nTr;
      float errEff = 100.* sqrt( okl * (nl-okl) / nl ) / nl;
      if ( (*itE).nLong == (*itE).okLong ) errEff = 100. / nl;
      float errEff5 = 100. * sqrt( ok5 * (n5-ok5) / n5 ) / n5;
      if ( (*itE).nLong5 == (*itE).okLong5 ) errEff5 = 100. / n5;
      info() << format( "NPrimary %2d  nEvt%6d  ghost rate %6.2f +- %6.2f   effLong %6.2f +-%6.2f  effLong5 %6.2f +- %6.2f",
                        itE - m_counters.begin(), (*itE).nEvt,
                        ghostRate, errGhost, effLong, errEff, effLong5, errEff5 )
             << endmsg;
    }
  }
  /*
  info() <<  "       ";
  for ( int kk = 0 ; 12 > kk ; ++kk ) info() << format( " %7d", kk );
  info() << endmsg;

  for ( float mu = 1. ; 6. > mu ; mu += 0.5 ) {
    float poisson = exp( -mu );
    info() << format( "mu %5.1f ", mu );
    for ( int kk = 0 ; 12 > kk ; ++kk ) {
      if ( 0 < kk ) poisson = poisson * mu / float(kk);
      info() << format( " %7.3f", poisson );
    }
    info() << endmsg;
  }
  */
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

