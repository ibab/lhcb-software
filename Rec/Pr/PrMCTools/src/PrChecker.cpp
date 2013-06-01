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

#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/IHistoTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrChecker
//
// 2005-03-29 : Olivier Callot
// modified by Wenbin Qian for Upgrade
// 2013-05-31 : Sascha Stahl, Thomas Nikodem upgraded to write Histograms
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( PrChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrChecker::PrChecker( const std::string& name,
                        ISvcLocator* pSvcLocator)
  //: GaudiAlgorithm ( name , pSvcLocator ),
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_velo(NULL),
    m_forward(NULL),
    m_match(NULL),
    m_upTrack(NULL),
    m_tTrack(NULL),
    m_downTrack(NULL),
    m_best(NULL),
    m_bestLong(NULL),
    m_bestDownstream(NULL),
    m_writeVeloHistos(-1),
    m_writeForwardHistos(-1),
    m_writeMatchHistos(-1),
    m_writeSeedHistos(-1),
    m_writeDownHistos(-1),
    m_writeUpHistos(-1),
    m_writeTTrackHistos(-1),
    m_writeBestHistos(-1),
    m_eta25cut(false)

{
  declareProperty( "VeloTracks",        m_veloTracks      = LHCb::TrackLocation::Velo       );
  declareProperty( "ForwardTracks",     m_forwardTracks   = LHCb::TrackLocation::Forward    );
  declareProperty( "MatchTracks",       m_matchTracks     = LHCb::TrackLocation::Match      );
  declareProperty( "SeedTracks",        m_seedTracks      = LHCb::TrackLocation::Seed       );
  declareProperty( "DownTracks",        m_downTracks      = LHCb::TrackLocation::Downstream );
  declareProperty( "UpTracks",          m_upTracks        = LHCb::TrackLocation::VeloTT     );
 
  declareProperty( "WriteVeloHistos",    m_writeVeloHistos      = -1 );
  declareProperty( "WriteForwardHistos", m_writeForwardHistos   = -1 );
  declareProperty( "WriteMatchHistos",   m_writeMatchHistos     = -1 );
  declareProperty( "WriteSeedHistos",    m_writeSeedHistos      = -1 );
  declareProperty( "WriteDownHistos",    m_writeDownHistos      = -1 );
  declareProperty( "WriteUpHistos",      m_writeUpHistos        = -1 );
  declareProperty( "WriteTTrackHistos",  m_writeTTrackHistos    = -1 );
  declareProperty( "WriteBestHistos",    m_writeBestHistos      = -1 );
 
  declareProperty( "Eta25Cut",           m_eta25cut             = false    );
}
//=============================================================================
// Destructor
//=============================================================================
PrChecker::~PrChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrChecker::initialize()
{

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  IHistoTool* htool = tool<IHistoTool>( "HistoTool","PrCheckerHistos",this ) ;
  GaudiHistoTool* ghtool = dynamic_cast<GaudiHistoTool*>(htool) ;
  ghtool->setHistoDir("Track/PrChecker") ;
  m_histoTool = htool;

  m_velo = tool<PrCounter>( "PrCounter", "Velo", this );
  m_velo->setContainer( m_veloTracks );
  m_velo->setWriteHistos(m_writeVeloHistos);
  m_velo->setSelectId( 3 );
  //m_velo->addSelection( "All" ,false);
  m_velo->addSelection( "OK_for_Velo" ,true);
  m_velo->addSelection( "long" ,true);
  m_velo->addSelection( "long>5GeV" ,true);
  m_velo->addSelection( "Strange+long" ,false);
  m_velo->addSelection( "Strange+long>5GeV" ,false);
  m_velo->addSelection( "long_from_B" ,false);
  m_velo->addSelection( "long_from_B>5GeV" ,false);

  m_forward = tool<PrCounter>( "PrCounter", "Forward", this );
  m_forward->setContainer( m_forwardTracks );
  m_forward->setWriteHistos(m_writeForwardHistos);
  m_forward->setSelectId( 8 );
  m_forward->addSelection( "long" ,true);
  m_forward->addSelection( "long>5GeV" ,true);
  m_forward->addSelection( "Strange+long" ,false);
  m_forward->addSelection( "Strange+long>5GeV" ,false);
  m_forward->addSelection( "long_from_B" ,false);
  m_forward->addSelection( "long_from_B>5GeV" ,false);

  m_match = tool<PrCounter>( "PrCounter", "Match", this );
  m_match->setContainer( m_matchTracks );
  m_match->setWriteHistos(m_writeMatchHistos);
  m_match->setSelectId( 8 );
  m_match->addSelection( "long" ,true);
  m_match->addSelection( "long>5GeV" ,true);
  m_match->addSelection( "Strange+long" ,false);
  m_match->addSelection( "Strange+long>5GeV" ,false);
  m_match->addSelection( "long_from_B" ,false);
  m_match->addSelection( "long_from_B>5GeV" ,false);

  m_upTrack = tool<PrCounter>( "PrCounter", "Upstream", this );
  m_upTrack->setContainer( m_upTracks );
  m_upTrack->setWriteHistos(m_writeUpHistos);
  m_upTrack->setSelectId( 4 );
  m_upTrack->addSelection( "long" ,true);
  m_upTrack->addSelection( "long>5GeV" ,true);
  m_upTrack->addSelection( "Strange+long" ,false);
  m_upTrack->addSelection( "Strange+long>5GeV" ,false);
  m_upTrack->addSelection( "long_from_B" ,false);
  m_upTrack->addSelection( "long_from_B>5GeV" ,false);

  m_tTrack = tool<PrCounter>( "PrCounter", "TTrack", this );
  m_tTrack->setContainer( m_seedTracks );
  m_tTrack->setWriteHistos(m_writeTTrackHistos );
  m_tTrack->setSelectId( 8 );
  m_tTrack->addSelection( "has_T_hits" ,true);
  m_tTrack->addSelection( "long" ,true);
  m_tTrack->addSelection( "long>5GeV" ,true);
  m_tTrack->addSelection( "Strange+T+TT" ,false);
  m_tTrack->addSelection( "Strange+T+TT>5GeV" ,false);
  m_tTrack->addSelection( "Strange+T+TT+noVelo" ,false);
  m_tTrack->addSelection( "Strange+T+TT+noVelo>5GeV" ,false);

  m_downTrack = tool<PrCounter>( "PrCounter", "Downstream", this );
  m_downTrack->setContainer( m_downTracks );
  m_downTrack->setWriteHistos(m_writeDownHistos );
  m_downTrack->setSelectId( 4 );
  m_downTrack->addSelection( "hasT+TThits" ,true);
  m_downTrack->addSelection( "T+TT>5GeV" ,true);
  m_downTrack->addSelection( "Strange+T+TT" ,false);
  m_downTrack->addSelection( "Strange+T+TT>5GeV" ,false);
  m_downTrack->addSelection( "Strange+T+TT+noVelo" ,false);
  m_downTrack->addSelection( "Strange+T+TT+noVelo>5GeV" ,false);
  m_downTrack->addSelection( "T+TT+from_ B" ,false);
  m_downTrack->addSelection( "T+TT+from_B>5 GeV" ,false);
  m_downTrack->addSelection( "T+TT+noVelo+from_B" ,false);
  m_downTrack->addSelection( "T+TT+noVelo_from_B>5GeV" ,false);
  

  m_best = tool<PrCounter>( "PrCounter", "Best", this );
  m_best->setContainer( LHCb::TrackLocation::Default );
  m_best->setWriteHistos(m_writeBestHistos);
  m_best->setSelectId( 15 );
  m_best->addSelection( "long" ,true);
  m_best->addSelection( "long>5GeV" ,true);
  m_best->addSelection( "Strange+long" ,false);
  m_best->addSelection( "Strange+long>5GeV" ,false);
  m_best->addSelection( "long_from_B" ,false);
  m_best->addSelection( "long_from_B>5GeV" ,false);

  m_bestLong = tool<PrCounter>( "PrCounter", "BestLong", this );
  m_bestLong->setContainer( LHCb::TrackLocation::Default );
  m_bestLong->setWriteHistos(m_writeBestHistos);
  m_bestLong->setSelectId( 15 );
  m_bestLong->setTrackType( LHCb::Track::Long );
  m_bestLong->addSelection( "long" ,true);
  m_bestLong->addSelection( "long>5GeV" ,true);
  m_bestLong->addSelection( "Strange+long" ,false);
  m_bestLong->addSelection( "Strange+long>5GeV" ,false);
  m_bestLong->addSelection( "long_from_B" ,false);
  m_bestLong->addSelection( "long_from_B>5GeV" ,false);

  m_bestDownstream = tool<PrCounter>( "PrCounter", "BestDown", this );
  m_bestDownstream->setContainer( LHCb::TrackLocation::Default );
  m_bestDownstream->setWriteHistos(m_writeBestHistos);
  m_bestDownstream->setSelectId( 12 );
  m_bestDownstream->setTrackType( LHCb::Track::Downstream ); 
  m_bestDownstream->addSelection( "hasT+TThits" ,true);
  m_bestDownstream->addSelection( "T+TT>5GeV" ,true);
  m_bestDownstream->addSelection( "Strange+T+TT" ,false);
  m_bestDownstream->addSelection( "Strange+T+TT>5GeV" ,false);
  m_bestDownstream->addSelection( "Strange+T+TT+noVelo" ,false);
  m_bestDownstream->addSelection( "Strange+T+TT+noVelo>5GeV" ,false);
  m_bestDownstream->addSelection( "T+TT+from_ B" ,false);
  m_bestDownstream->addSelection( "T+TT+from_B>5 GeV" ,false);
  m_bestDownstream->addSelection( "T+TT+noVelo+from_B" ,false);
  m_bestDownstream->addSelection( "T+TT+noVelo_from_B>5GeV" ,false);

  m_allCounters.push_back( m_velo  );
  m_allCounters.push_back( m_forward );
  m_allCounters.push_back( m_match );
  m_allCounters.push_back( m_upTrack );
  m_allCounters.push_back( m_tTrack );
  m_allCounters.push_back( m_downTrack );
  m_allCounters.push_back( m_best  );
  m_allCounters.push_back( m_bestLong  );
  m_allCounters.push_back( m_bestDownstream  );

  for ( std::vector<PrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->setUseEta25Cut(m_eta25cut);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrChecker::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

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
      if ( nbVisible > 4 )  
      ++nPrim;
    }
  }

  for ( std::vector<PrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->initEvent(m_histoTool,nPrim);
  }

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

    //Add Eta Cut!
    bool isLong  = trackInfo.hasVeloAndT( part );
    isLong = isLong && ( abs( part->particleID().pid() ) != 11 ); // and not electron

    bool isDown  = trackInfo.hasT( part ) &&  trackInfo.hasTT( part );
    isDown = isDown && ( abs( part->particleID().pid() ) != 11 ); // and not electron
    
    bool over5       = 5000. < fabs( part->p() );
    bool isInVelo    = trackInfo.hasVelo( part );
    bool strangeLong = false;
    bool strangeDown = false;
    bool fromB       = false;
    //bool zunder100   = part->originVertex()->position().Z() < 100.; 
    bool eta25       = !m_eta25cut || (part->momentum().Eta() > 2 && part->momentum().Eta() < 5);
    
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
    //flags.push_back( true );
    flags.push_back( eta25 && isInVelo );
    flags.push_back( eta25 && isLong  );
    flags.push_back( eta25 && isLong && over5   );
    flags.push_back( eta25 && isLong && strangeLong );
    flags.push_back( eta25 && isLong && strangeLong && over5 );
    flags.push_back( eta25 && isLong && fromB );
    flags.push_back( eta25 && isLong && fromB && over5 );
    m_velo->countAndPlot(m_histoTool,part,flags,ids,nPrim);

    flags.clear();
    flags.push_back( eta25 && isLong  );
    flags.push_back( eta25 && isLong && over5   );
    flags.push_back( eta25 && isLong && strangeLong );
    flags.push_back( eta25 && isLong && strangeLong && over5 );
    flags.push_back( eta25 && isLong && fromB );
    flags.push_back( eta25 && isLong && fromB && over5 );
    m_forward->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_match->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_upTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_best->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_bestLong->countAndPlot(m_histoTool,part,flags,ids,nPrim);

    flags.clear();
    flags.push_back( eta25 && trackInfo.hasT( part ) );
    flags.push_back( eta25 && isLong );
    flags.push_back( eta25 && isLong && over5 );
    flags.push_back( eta25 && strangeDown );
    flags.push_back( eta25 && strangeDown && over5 );
    flags.push_back( strangeDown && !isInVelo );
    flags.push_back( strangeDown && over5 && !isInVelo );
    m_tTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);

    flags.clear();
    flags.push_back( eta25 && isDown );
    flags.push_back( eta25 && isDown && over5);
    flags.push_back( eta25 && strangeDown );
    flags.push_back( eta25 && strangeDown && over5 );
    flags.push_back( eta25 && strangeDown && !isInVelo );
    flags.push_back( eta25 && strangeDown && over5 && !isInVelo );
    flags.push_back( eta25 && isDown && fromB );
    flags.push_back( eta25 && isDown && fromB && over5 );
    flags.push_back( eta25 && isDown && fromB && !isInVelo);
    flags.push_back( eta25 && isDown && fromB && over5 && !isInVelo);
    m_downTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_bestDownstream->countAndPlot(m_histoTool,part,flags,ids,nPrim);

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
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

