// Include files

// Linker
#include "Linker/AllLinks.h"

#include "Event/MCTrackInfo.h"
// local
#include "PatChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatChecker
//
// 2005-03-29 : Olivier Callot
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( PatChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatChecker::PatChecker( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_initTime(0)
  , m_partTime(0)
{
  declareProperty( "CheckMissedVeloSpace", m_checkMissedVeloSpace  = false );
  declareProperty( "CheckMissedForward",   m_checkMissedForward    = false );
  declareProperty( "CheckMissedSeed",      m_checkMissedSeed       = false );
  declareProperty( "CheckMissedDownstream",m_checkMissedDownstream = false );
  declareProperty( "CheckMatchNotForward", m_checkMatchNotForward  = false );
  declareProperty( "MeasureTime",          m_measureTime           = false );
  declareProperty( "VeloTrackLocation",    veloSpaceName = LHCb::TrackLocation::Velo);
  declareProperty( "HltRZVeloTrackLocation", m_hltRZVeloLocation = "");
  declareProperty( "HltVeloTrackLocation", m_hltVeloLocation = "");
  declareProperty( "HltVeloTTTrackLocation", m_hltVeloTTLocation = "");
  declareProperty( "HltForwardTrackLocation", m_hltForwardLocation = ""); 
}
//=============================================================================
// Destructor
//=============================================================================
PatChecker::~PatChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  std::string veloRzName    =  LHCb::TrackLocation::RZVelo;
  std::string veloTTName    =  LHCb::TrackLocation::VeloTT;
  std::string forwardName   =  LHCb::TrackLocation::Forward;
  if ( "Hlt" == context() ) {
    veloRzName    = m_hltRZVeloLocation;
    veloSpaceName = m_hltVeloLocation;
    veloTTName    = m_hltVeloTTLocation;
    forwardName   = m_hltForwardLocation;
  }

  m_veloRz = tool<PatCounter>( "PatCounter", "VeloRZ", this );
  m_veloRz->setContainer( veloRzName );
  m_veloRz->setSelectId( 1 );
  m_veloRz->addSelection( "   with hits" );
  m_veloRz->addSelection( " OK for Velo" );
  m_veloRz->addSelection( "        long" );
  m_veloRz->addSelection( "long > 5 GeV" );
  m_veloRz->addSelection( "      s+long" );
  m_veloRz->addSelection( " and > 5 GeV" );

  m_veloSpace = tool<PatCounter>( "PatCounter", "VeloSpace", this );
  m_veloSpace->setContainer( veloSpaceName );
  m_veloSpace->setSelectId( 3 );
  m_veloSpace->addSelection( "   with hits" );
  m_veloSpace->addSelection( " OK for Velo" );
  m_veloSpace->addSelection( "        long" );
  m_veloSpace->addSelection( "long > 5 GeV" );
  m_veloSpace->addSelection( "      s+long" );
  m_veloSpace->addSelection( " and > 5 GeV" );

  m_veloTT = tool<PatCounter>( "PatCounter", "VeloTT", this );
  m_veloTT->setContainer( veloTTName );
  m_veloTT->setSelectId( 7 );
  m_veloTT->addSelection( "   with hits" );
  m_veloTT->addSelection( " OK for Velo" );
  m_veloTT->addSelection( "        long" );
  m_veloTT->addSelection( "long > 5 GeV" );

  m_forward = tool<PatCounter>( "PatCounter", "Forward", this );
  m_forward->setContainer( forwardName );
  m_forward->setSelectId( 8 );
  m_forward->addSelection( "   with hits" );
  m_forward->addSelection( "        long" );
  m_forward->addSelection( "long > 5 GeV" );

  m_tsa = tool<PatCounter>( "PatCounter", "TSA", this );
  m_tsa->setContainer( LHCb::TrackLocation::Tsa );
  m_tsa->setSelectId( 8 );
  m_tsa->addSelection( "   with hits" );
  m_tsa->addSelection( "  has T hits" );
  m_tsa->addSelection( "        long" );
  m_tsa->addSelection( "long > 5 GeV" );
  m_tsa->addSelection( "      s+T+TT" );
  m_tsa->addSelection( " and > 5 GeV" );

  m_tTrack = tool<PatCounter>( "PatCounter", "TTrack", this );
  m_tTrack->setContainer( LHCb::TrackLocation::Seed );
  m_tTrack->setSelectId( 8 );
  m_tTrack->addSelection( "   with hits" );
  m_tTrack->addSelection( "  has T hits" );
  m_tTrack->addSelection( "        long" );
  m_tTrack->addSelection( "long > 5 GeV" );
  m_tTrack->addSelection( "      s+T+TT" );
  m_tTrack->addSelection( " and > 5 GeV" );

  m_match = tool<PatCounter>( "PatCounter", "Match", this );
  m_match->setContainer( LHCb::TrackLocation::Match );
  m_match->setSelectId( 8 );
  m_match->addSelection( "   with hits" );
  m_match->addSelection( "        long" );
  m_match->addSelection( "long > 5 GeV" );

  m_downst = tool<PatCounter>( "PatCounter", "Downstream", this );
  m_downst->setContainer( LHCb::TrackLocation::Downstream );
  m_downst->setSelectId( 12 );
  m_downst->addSelection( "   with hits" );
  m_downst->addSelection( "    has Seed" );
  m_downst->addSelection( "+noVelo,T+TT" );
  m_downst->addSelection( " + strange<-" );
  m_downst->addSelection( " and > 5 GeV" );
  m_downst->addSelection( "   pi<-Ks<-B" );
  m_downst->addSelection( " and > 5 GeV" );

  m_kSNew = tool<PatCounter>( "PatCounter", "KSNew", this );
  m_kSNew->setContainer( "Rec/Track/FromKs" );
  m_kSNew->setSelectId( 12 );
  m_kSNew->addSelection( "   pi<-Ks<-B" );
  m_kSNew->addSelection( " and > 5 GeV" );

  m_best = tool<PatCounter>( "PatCounter", "Best", this );
  m_best->setContainer( LHCb::TrackLocation::Default );
  m_best->setSelectId( 15 );
  m_best->addSelection( "   with hits" );
  m_best->addSelection( "        long" );
  m_best->addSelection( "long > 5 GeV" );

  m_allCounters.push_back( m_veloRz     );
  m_allCounters.push_back( m_veloSpace  );
  m_allCounters.push_back( m_veloTT     );
  m_allCounters.push_back( m_forward );
  m_allCounters.push_back( m_tsa    );
  m_allCounters.push_back( m_tTrack );
  m_allCounters.push_back( m_match );
  m_allCounters.push_back( m_downst  );
  m_allCounters.push_back( m_kSNew  );
  m_allCounters.push_back( m_best  );

  m_ttForward = tool<IPatTTCounter>( "PatTTCounter", "TTForward", this );
  m_ttForward->setContainer( forwardName );
  m_ttForward->addSelection( "   with hits" );
  m_ttForward->addSelection( "        long" );
  m_ttForward->addSelection( "long > 5 GeV" );

  m_ttMatch = tool<IPatTTCounter>( "PatTTCounter", "TTMatch", this );
  m_ttMatch->setContainer( LHCb::TrackLocation::Match );
  m_ttMatch->addSelection( "   with hits" );
  m_ttMatch->addSelection( "        long" );
  m_ttMatch->addSelection( "long > 5 GeV" );

  m_ttDownst = tool<IPatTTCounter>( "PatTTCounter", "TTDownstream", this );
  m_ttDownst->setContainer( LHCb::TrackLocation::Downstream );
  m_ttDownst->addSelection( "   with hits" );
  m_ttDownst->addSelection( "    has Seed" );
  m_ttDownst->addSelection( "+noVelo,T+TT" );
  m_ttDownst->addSelection( " + strange<-" );
  m_ttDownst->addSelection( " and > 5 GeV" );
  m_ttDownst->addSelection( "   pi<-Ks<-B" );
  m_ttDownst->addSelection( " and > 5 GeV" );

  m_allTTCounters.push_back( m_ttForward );
  m_allTTCounters.push_back( m_ttMatch   );
  m_allTTCounters.push_back( m_ttDownst  );

  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer->increaseIndent();
    m_initTime = m_timer->addTimer( "Init" );
    m_partTime = m_timer->addTimer( "Loop" );
    m_timer->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatChecker::execute() {

  debug() << "==> Execute" << endmsg;

  if ( m_measureTime ) m_timer->start( m_initTime );
  
  for ( std::vector<PatCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->initEvent();
  }

  for ( std::vector<IPatTTCounter*>::iterator itCt = m_allTTCounters.begin();
        m_allTTCounters.end() != itCt; ++itCt ) {
    (*itCt)->initEvent();
  }

  if ( m_measureTime ) {
    m_timer->stop(  m_initTime );
    m_timer->start( m_partTime );
  }

  LHCb::MCParticles* mcParts =
    get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  //== Build a table (vector of vectors) of ids per MCParticle, indexed by MCParticle key.
  AllLinks<LHCb::MCParticle,ContainedObject> allIds( evtSvc(), msgSvc(), "Pat/LHCbID" );
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

  std::vector<LHCb::MCParticle*> partFromKs;
  std::vector<int> seedFromKs;

  LHCb::MCParticles::const_iterator itP;
  for ( itP = mcParts->begin(); mcParts->end() != itP; ++itP ) {
    LHCb::MCParticle* part = *itP;
    if ( 0 == trackInfo.fullInfo( part ) ) continue;
    verbose() << "checking MCPart " << part->key() << endmsg;

    bool isLong  = trackInfo.hasVeloAndT( part );
    isLong = isLong && ( abs( part->particleID().pid() ) != 11 ); // and not electron
    
    bool over5       = 5000. < fabs( part->p() );
    bool isInVelo    = trackInfo.hasVelo( part );
    bool strangeDown = false;
    bool fromKsFromB = false;
    
    if ( 0 != part->originVertex() ) {
      const LHCb::MCParticle* mother =  part->originVertex()->mother();
      if ( 0 != mother ) {
        if ( 0 != mother->originVertex() ) {
          double rOrigin = mother->originVertex()->position().rho();
          if ( fabs( rOrigin ) < 8. ) {
            int pid = abs( mother->particleID().pid() );
            if ( 130 == pid ||
                 310 == pid ||
                 3122 == pid ||
                 3222 == pid ||
                 3212 == pid ||
                 3112 == pid ||
                 3322 == pid ||
                 3312 == pid ||
                 3334 == pid ) {
              strangeDown =  trackInfo.hasT( part ) &&  trackInfo.hasTT( part );
            }
            if ( 0 != mother->originVertex()->mother() ) {
              if  ( 310 == pid &&
                    2 == part->originVertex()->products().size()  &&
                    mother->originVertex()->mother()->particleID().hasBottom() &&
                    ( mother->originVertex()->mother()->particleID().isMeson() ||  
                      mother->originVertex()->mother()->particleID().isBaryon() )) {
                fromKsFromB = true;
              }
            }
            
          }
        }
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
    
    verbose() << "MCPart " << part->key() << " has " << ids.size() << " links " <<endmsg;
    
    std::vector<bool> flags;
    flags.push_back( true );
    flags.push_back( isInVelo );
    flags.push_back( isLong  );
    flags.push_back( isLong && over5   );
    flags.push_back( isLong && strangeDown );
    flags.push_back( isLong && strangeDown && over5 );

    int foundRZ    = m_veloRz->count( part, flags, ids );
    //if ( 0 > foundRZ ) continue; // Conditional filling of space velo
    int foundSpace = m_veloSpace->count( part, flags, ids );
    if ( m_checkMissedVeloSpace &&
         0 >  foundSpace &&
         0 <= foundRZ &&
         flags.size() == 4 &&
         flags[3] ) {
    }

    flags.clear();
    flags.push_back( true );
    flags.push_back( isInVelo );
    flags.push_back( isLong  );
    flags.push_back( isLong && over5   );
    m_veloTT->count( part, flags, ids );

    flags.clear();
    flags.push_back( true );
    flags.push_back( isLong );
    flags.push_back( isLong && over5 );

    int foundFwd = m_forward->count( part, flags, ids );
    if ( m_checkMissedForward &&
         0 >  foundFwd &&
         0 <= foundSpace &&
         10000. < fabs( part->p() ) &&
         isLong ) {
    }
    m_ttForward->count( part, flags, ids );
    m_best->count( part, flags, ids );

    int foundMatch = m_match->count( part, flags, ids );
    if ( m_checkMatchNotForward &&
         0 >  foundFwd &&
         0 <= foundMatch &&
         10000. < fabs( part->p() ) &&
         isLong ) {
    }

    m_ttMatch->count( part, flags, ids );

    flags.clear();
    flags.push_back( true );
    flags.push_back( trackInfo.hasT( part ) );
    flags.push_back( isLong );
    flags.push_back( isLong && over5 );
    if ( strangeDown ) {
      flags.push_back( true );
      flags.push_back( over5 );
    }    

    int foundSeed  = m_tsa->count( part, flags, ids );
    int foundSeed2 = m_tTrack->count( part, flags, ids );
    // take tsa or PatSeeding
    if ( 0 > foundSeed ) foundSeed = foundSeed2;
    
   
    flags.clear();
    flags.push_back( true );
    flags.push_back( foundSeed > 0 );
    if ( foundSeed > 0 &&
       !trackInfo.hasVelo( part ) &&
       trackInfo.hasT( part )     &&
       trackInfo.hasTT( part )    &&
       (abs( part->particleID().pid() ) != 11 ) ) {
     flags.push_back( true );
     flags.push_back( strangeDown );
     flags.push_back( strangeDown && over5 ); 
     flags.push_back( fromKsFromB );
     flags.push_back( fromKsFromB && over5 );
    }
    int foundDownst = m_downst->count( part, flags, ids );
    m_ttDownst->count( part, flags, ids );

    if ( m_checkMissedDownstream &&
         0 > foundDownst &&  
         3 < flags.size() && 
         fromKsFromB ) {
      info() << "=== Missed Downstream track key " << part->key() 
             << " seed key " << foundSeed << endmsg;
    }
    

    if ( 3 < flags.size() && fromKsFromB && 0 <= foundDownst ) {
      partFromKs.push_back( part );
      seedFromKs.push_back( foundDownst );
    }
  }

  if ( 2 == partFromKs.size() ) {
    for ( itP = partFromKs.begin(); partFromKs.end() != itP; ++itP ) {
      LHCb::MCParticle* part = *itP;

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
      std::vector<bool> flags;
      flags.push_back( true );
      flags.push_back( 5000. < fabs( part->p() ) );
      m_kSNew->count( part, flags, ids );  

    }
  }
  

  if ( m_measureTime ) m_timer->stop( m_partTime );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  for ( std::vector<PatCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->printStatistics();
  }

  for ( std::vector<IPatTTCounter*>::iterator itCt = m_allTTCounters.begin();
        m_allTTCounters.end() != itCt; ++itCt ) {
    (*itCt)->printStatistics();
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
