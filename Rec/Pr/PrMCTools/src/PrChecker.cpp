// Include files

// Linker
#include "Linker/AllLinks.h"

#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
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
PrChecker::PrChecker( const std::string& name, ISvcLocator* pSvcLocator)
: GaudiHistoAlg ( name , pSvcLocator ),
  m_idLoc("Pr/LHCbID"),
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
  m_writeDownHistos(-1),
  m_writeUpHistos(-1),
  m_writeTTrackHistos(-1),
  m_writeBestHistos(-1),
  m_writeBestLongHistos(-1),
  m_writeBestDownstreamHistos(-1),
  m_writeUTHistos(-1),
  m_eta25cut(false),
  m_triggerNumbers(false),
  m_useElectrons(false),
  m_histoTool(NULL)
{
  declareProperty( "VeloTracks",        m_veloTracks      = LHCb::TrackLocation::Velo       );
  declareProperty( "ForwardTracks",     m_forwardTracks   = LHCb::TrackLocation::Forward    );
  declareProperty( "MatchTracks",       m_matchTracks     = LHCb::TrackLocation::Match      );
  declareProperty( "SeedTracks",        m_seedTracks      = LHCb::TrackLocation::Seed       );
  declareProperty( "DownTracks",        m_downTracks      = LHCb::TrackLocation::Downstream );
  declareProperty( "UpTracks",          m_upTracks        = LHCb::TrackLocation::VeloTT     );
  declareProperty( "BestTracks",        m_bestTracks      = LHCb::TrackLocation::Default    );
 
  declareProperty( "WriteVeloHistos",    m_writeVeloHistos      = -1 );
  declareProperty( "WriteForwardHistos", m_writeForwardHistos   = -1 );
  declareProperty( "WriteMatchHistos",   m_writeMatchHistos     = -1 );
  declareProperty( "WriteDownHistos",    m_writeDownHistos      = -1 );
  declareProperty( "WriteUpHistos",      m_writeUpHistos        = -1 );
  declareProperty( "WriteTTrackHistos",  m_writeTTrackHistos    = -1 );
  declareProperty( "WriteBestHistos",    m_writeBestHistos      = -1 );
  declareProperty( "WriteBestLongHistos",m_writeBestLongHistos  = -1 );
  declareProperty( "WriteBestDownstreamHistos",    m_writeBestDownstreamHistos      = -1 );
  declareProperty( "WriteUTHistos",      m_writeUTHistos        = -1 );
 
  declareProperty( "Eta25Cut",           m_eta25cut             = false    );
  declareProperty( "TriggerNumbers",     m_triggerNumbers       = false    );
  declareProperty( "UseElectrons",       m_useElectrons         = false    );
  declareProperty( "GhostProbCut",       m_ghostProbCut         = 1.0      );
  declareProperty( "Upgrade",            m_upgrade              = true     );

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
  
  // -- catch the possible failure of the dynamic cast
  if( ghtool == NULL){
    error() << "Dynamic cast of Gaudi Histogramming Tool failed!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  ghtool->setHistoDir(histoDir+name()) ;
  m_histoTool = htool;

  m_velo = tool<IPrCounter>( "PrCounter", "Velo", this );
  m_velo->setContainer( m_veloTracks );
  m_velo->setWriteHistos(m_writeVeloHistos);
  m_velo->setSelectId( 3 );
  m_velo->addSelection( "velo" ,true);
  m_velo->addSelection( "long" ,true);
  m_velo->addSelection( "long>5GeV" ,true);
  m_velo->addSelection( "long_strange" ,false);
  m_velo->addSelection( "long_strange>5GeV" ,false);
  m_velo->addSelection( "long_fromB" ,false);
  m_velo->addSelection( "long_fromB>5GeV" ,false);
  if(m_triggerNumbers){
    m_velo->addSelection( "long_fromB_P>3GeV_Pt>0.5GeV" ,true);
    m_velo->addSelection( "UT_long_fromB_P>3GeV_Pt>0.5GeV" ,true);
  }

  m_forward = tool<IPrCounter>( "PrCounter", "Forward", this );
  m_forward->setContainer( m_forwardTracks );
  m_forward->setWriteHistos(m_writeForwardHistos);
  m_forward->setSelectId( 8 );
  m_forward->addSelection( "long" ,true);
  m_forward->addSelection( "long>5GeV" ,true);
  m_forward->addSelection( "long_strange" ,false);
  m_forward->addSelection( "long_strange>5GeV" ,false);
  m_forward->addSelection( "long_fromB" ,false);
  m_forward->addSelection( "long_fromB>5GeV" ,false);
  if(m_triggerNumbers){
    m_forward->addSelection( "long_fromB_P>3GeV_Pt>0.5GeV" ,true);
    m_forward->addSelection( "UT_long_fromB_P>3GeV_Pt>0.5GeV" ,true);
  }

  m_match = tool<IPrCounter>( "PrCounter", "Match", this );
  m_match->setContainer( m_matchTracks );
  m_match->setWriteHistos(m_writeMatchHistos);
  m_match->setSelectId( 8 );
  m_match->addSelection( "long" ,true);
  m_match->addSelection( "long>5GeV" ,true);
  m_match->addSelection( "long_strange" ,false);
  m_match->addSelection( "long_strange>5GeV" ,false);
  m_match->addSelection( "long_fromB" ,false);
  m_match->addSelection( "long_fromB>5GeV" ,false);
  if(m_triggerNumbers){
    m_match->addSelection( "long_fromB_P>3GeV_Pt>0.5GeV" ,true);
    m_match->addSelection( "UT+long_fromB_P>3GeV_Pt>0.5GeV" ,true);
  }

  m_upTrack = tool<IPrCounter>( "PrCounter", "Upstream", this );
  m_upTrack->setContainer( m_upTracks );
  m_upTrack->setWriteHistos(m_writeUpHistos);
  m_upTrack->setSelectId( 4 );
  m_upTrack->addSelection( "velo" ,true);
  m_upTrack->addSelection( "velo+UT" ,true);
  m_upTrack->addSelection( "velo+UT>5GeV" ,false);
  m_upTrack->addSelection( "velo+notLong" ,false);
  m_upTrack->addSelection( "velo+UT+notLong" ,false);
  m_upTrack->addSelection( "velo+UT+notLong>5GeV" ,false);
  m_upTrack->addSelection( "long" ,true);
  m_upTrack->addSelection( "long>5GeV" ,true);
  m_upTrack->addSelection( "long_fromB" ,false);
  m_upTrack->addSelection( "long_fromB>5GeV" ,false);
  if(m_triggerNumbers){
    m_upTrack->addSelection( "long_fromB_P>3GeV_Pt>0.5GeV" ,true);
    m_upTrack->addSelection( "UT+long_fromB_P>3GeV_Pt>0.5GeV" ,true);
  }

  m_tTrack = tool<IPrCounter>( "PrCounter", "TTrack", this );
  m_tTrack->setContainer( m_seedTracks );
  m_tTrack->setWriteHistos(m_writeTTrackHistos );
  m_tTrack->setSelectId( 8 );
  m_tTrack->addSelection( "hasT" ,true);
  m_tTrack->addSelection( "long" ,true);
  m_tTrack->addSelection( "long>5GeV" ,true);
  m_tTrack->addSelection( "long_fromB" ,false);
  m_tTrack->addSelection( "long_fromB>5GeV" ,false);
  m_tTrack->addSelection( "UT+T_strange" ,false);
  m_tTrack->addSelection( "UT+T_strange>5GeV" ,false);
  m_tTrack->addSelection( "noVelo+UT+T_strange" ,false);
  m_tTrack->addSelection( "noVelo+UT+T_strange>5GeV" ,false);
  m_tTrack->addSelection( "UT+T_SfromDB" ,false);
  m_tTrack->addSelection( "UT+T_SfromDB>5GeV" ,false);
  m_tTrack->addSelection( "noVelo+UT+T_SfromDB" ,false);
  m_tTrack->addSelection( "noVelo+UT+T_SfromDB>5GeV" ,false);

  m_downTrack = tool<IPrCounter>( "PrCounter", "Downstream", this );
  m_downTrack->setContainer( m_downTracks );
  m_downTrack->setWriteHistos(m_writeDownHistos );
  m_downTrack->setSelectId( 4 );
  m_downTrack->addSelection( "UT+T" ,true);
  m_downTrack->addSelection( "UT+T>5GeV" ,true);
  m_downTrack->addSelection( "UT+T_strange" ,false);
  m_downTrack->addSelection( "UT+T_strange>5GeV" ,false);
  m_downTrack->addSelection( "noVelo+UT+T_strange" ,false);
  m_downTrack->addSelection( "noVelo+UT+T_strange>5GeV" ,false);
  m_downTrack->addSelection( "UT+T_fromB" ,false);
  m_downTrack->addSelection( "UT+T_fromB>5GeV" ,false);
  m_downTrack->addSelection( "noVelo+UT+T_fromB" ,false);
  m_downTrack->addSelection( "noVelo+UT+T_fromB>5GeV" ,false);
  m_downTrack->addSelection( "UT+T_SfromDB" ,false);
  m_downTrack->addSelection( "UT+T_SfromDB>5GeV" ,false);
  m_downTrack->addSelection( "noVelo+UT+T_SfromDB" ,false);
  m_downTrack->addSelection( "noVelo+UT+T_SfromDB>5GeV" ,false);

  m_best = tool<IPrCounter>( "PrCounter", "Best", this );
  m_best->setContainer( m_bestTracks );
  m_best->setWriteHistos(m_writeBestHistos);
  m_best->setSelectId( 15 );
  m_best->setGhostProbCut( m_ghostProbCut );
  m_best->addSelection( "long" ,true);
  m_best->addSelection( "long>5GeV" ,true);
  m_best->addSelection( "long_strange" ,false);
  m_best->addSelection( "long_strange>5GeV" ,false);
  m_best->addSelection( "long_fromB" ,false);
  m_best->addSelection( "long_fromB>5GeV" ,false);
  if(m_triggerNumbers){
    m_best->addSelection( "long_fromB_P>3GeV_Pt>0.5GeV" ,true);
    m_best->addSelection( "UT+long_fromB_P>3GeV_Pt>0.5GeV" ,true);
  }

  m_bestLong = tool<IPrCounter>( "PrCounter", "BestLong", this );
  m_bestLong->setContainer( m_bestTracks );
  m_bestLong->setWriteHistos(m_writeBestLongHistos);
  m_bestLong->setSelectId( 15 );
  m_bestLong->setGhostProbCut( m_ghostProbCut );
  m_bestLong->setTrackType( LHCb::Track::Long );
  m_bestLong->addSelection( "long" ,true);
  m_bestLong->addSelection( "long>5GeV" ,true);
  m_bestLong->addSelection( "long_strange" ,false);
  m_bestLong->addSelection( "long_strange>5GeV" ,false);
  m_bestLong->addSelection( "long_fromB" ,false);
  m_bestLong->addSelection( "long_fromB>5GeV" ,false);
  if(m_triggerNumbers){
    m_bestLong->addSelection( "long_fromB_P>3GeV_Pt>0.5GeV" ,true);
    m_bestLong->addSelection( "UT+long_fromB_P>3GeV_Pt>0.5GeV" ,true);
  }

  m_bestDownstream = tool<IPrCounter>( "PrCounter", "BestDown", this );
  m_bestDownstream->setContainer( m_bestTracks );
  m_bestDownstream->setWriteHistos(m_writeBestDownstreamHistos);
  m_bestDownstream->setSelectId( 12 );
  m_bestDownstream->setGhostProbCut( m_ghostProbCut );
  m_bestDownstream->setTrackType( LHCb::Track::Downstream ); 
  m_bestDownstream->addSelection( "UT+T" ,true);
  m_bestDownstream->addSelection( "UT+T>5GeV" ,true);
  m_bestDownstream->addSelection( "UT+T_strange" ,false);
  m_bestDownstream->addSelection( "UT+T_strange>5GeV" ,false);
  m_bestDownstream->addSelection( "noVelo+UT+T_strange" ,false);
  m_bestDownstream->addSelection( "noVelo+UT+T_strange>5GeV" ,false);
  m_bestDownstream->addSelection( "UT+T_fromB" ,false);
  m_bestDownstream->addSelection( "UT+T_fromB>5GeV" ,false);
  m_bestDownstream->addSelection( "noVelo+UT+T_fromB" ,false);
  m_bestDownstream->addSelection( "noVelo+UT+T_fromB>5GeV" ,false);
  m_bestDownstream->addSelection( "UT+T_SfromDB" ,false);
  m_bestDownstream->addSelection( "UT+T_SfromDB>5GeV" ,false);
  m_bestDownstream->addSelection( "noVelo+UT+T_SfromDB" ,false);
  m_bestDownstream->addSelection( "noVelo+UT+T_SfromDB>5GeV" ,false);

  m_allCounters.push_back( m_velo  );
  m_allCounters.push_back( m_forward );
  m_allCounters.push_back( m_match );
  m_allCounters.push_back( m_upTrack );
  m_allCounters.push_back( m_tTrack );
  m_allCounters.push_back( m_downTrack );
  m_allCounters.push_back( m_best  );
  m_allCounters.push_back( m_bestLong  );
  m_allCounters.push_back( m_bestDownstream  );

  // -- UT Counters (excluded for the moment)
  /*
  m_utForward = tool<IPrCounter>( "PrUTCounter", "UTForward", this );  
  m_utForward->setContainer( m_forwardTracks );
  //m_utForward->setFolderPlotName( m_forwardTracks );
  m_utForward->setWriteHistos(m_writeUTHistos);
  m_utForward->addSelection( "long", true);
  m_utForward->addSelection( "long>5GeV", true);
  m_utForward->addSelection( "UT_long>5GeV", true);
  if(m_triggerNumbers) m_utForward->addSelection( "long>p3pt05", true);
  m_utForward->addSelection( "long_from_B", true);
  m_utForward->addSelection( "long_from_B>5GeV", true);
  m_utForward->addSelection( "UT_long_from_B>5GeV", true);
  if(m_triggerNumbers) m_utForward->addSelection( "long_from_B>p3pt05", true);

  m_utMatch = tool<IPrCounter>( "PrUTCounter", "UTMatch", this );
  m_utMatch->setContainer( m_matchTracks );
  //m_utMatch->setFolderPlotName( m_matchTracks );
  m_utMatch->setWriteHistos(m_writeUTHistos);
  m_utMatch->addSelection( "long", true);
  m_utMatch->addSelection( "long>5GeV", true);
  m_utMatch->addSelection( "UT_long>5GeV", true);
  if(m_triggerNumbers) m_utMatch->addSelection( "long>p3pt05", true);
  m_utMatch->addSelection( "long_from_B", true);
  m_utMatch->addSelection( "long_from_B>5GeV", true);
  m_utMatch->addSelection( "UT_long_from_B>5GeV", true);
  if(m_triggerNumbers) m_utMatch->addSelection( "long_from_B>p3pt05", true);

  m_utbestLong = tool<IPrCounter>( "PrUTCounter", "UTBestLong", this );
  m_utbestLong->setContainer( LHCb::TrackLocation::Default );
  //m_utbestLong->setFolderPlotName("bestlong");
  m_utbestLong->setWriteHistos(m_writeUTHistos);
  m_utbestLong->setTrackType( LHCb::Track::Long );
  m_utbestLong->addSelection( "long" ,true);
  m_utbestLong->addSelection( "long>5GeV" ,true);
  m_utbestLong->addSelection( "UT_long>5GeV" ,true);
  if(m_triggerNumbers) m_utbestLong->addSelection( "long>p3pt05", true);
  m_utbestLong->addSelection( "long_from_B" ,true);
  m_utbestLong->addSelection( "long_from_B>5GeV" ,true);
  m_utbestLong->addSelection( "UT_from_B>5GeV" ,true);
  if(m_triggerNumbers) m_utbestLong->addSelection( "long_from_B>p3pt05", true);

  m_allCounters.push_back( m_utForward );
  m_allCounters.push_back( m_utMatch   );
  m_allCounters.push_back( m_utbestLong);
  */
  
  for ( std::vector<IPrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->setUseEta25Cut(m_eta25cut);
    (*itC)->setTriggerNumbers(m_triggerNumbers);
  }


  if( !m_upgrade ) m_idLoc = "Pat/LHCbID";
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrChecker::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::MCParticles* mcParts = getIfExists<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  if(  mcParts == nullptr){
    error() << "Could not find MCParticles at " << LHCb::MCParticleLocation::Default << endmsg;
    return StatusCode::SUCCESS;
  }
  

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  LHCb::MCVertices* mcVert = getIfExists<LHCb::MCVertices>(LHCb::MCVertexLocation::Default );
  if(  mcVert == nullptr ){
    error() << "Could not find MCVertices at " << LHCb::MCParticleLocation::Default << endmsg;
    return StatusCode::SUCCESS;
  }
  
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
 
  for ( std::vector<IPrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->initEvent(m_histoTool,nPrim);
  }
  
  //== Build a table (vector of vectors) of ids per MCParticle, indexed by MCParticle key.
  AllLinks<LHCb::MCParticle> allIds( evtSvc(), msgSvc(), m_idLoc );
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
    
    // -- Take electrons into account?
    bool isElectron = abs( part->particleID().pid() ) == 11;
    if(m_useElectrons){
      isElectron = false;
    }
    
      

    bool isLong  = trackInfo.hasVeloAndT( part );
    //isLong = isLong && !isElectron; // and not electron

    bool isDown  = trackInfo.hasT( part ) &&  trackInfo.hasTT( part );
    //isDown = isDown && !isElectron; // and not electron
    
    bool over5       = 5000. < fabs( part->p() );
    bool trigger     = 3000. < fabs( part->p() ) &&  500. < fabs( part->pt() );
    bool isInVelo    = trackInfo.hasVelo( part );
    bool isInUT      = trackInfo.hasTT( part );
    bool strangeLong = false;
    bool strangeDown = false;
    bool fromB       = false;
    bool fromD       = false;
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
        if ( mother->particleID().hasBottom() && 
             ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) ) fromB = true;
        
        if ( mother->particleID().hasCharm() &&
             ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) ) fromD = true;

        mother = mother->originVertex()->mother();
      }
    }
    


    // all ids associated to the mcparticle
    std::vector<LHCb::LHCbID> ids;    
    if ( linkedIds.size() > (unsigned int) part->key() ) {      
      for ( std::vector<int>::const_iterator itIm = linkedIds[part->key()].begin();
            linkedIds[part->key()].end() != itIm; ++itIm ) {
        LHCb::LHCbID temp;
        temp.setDetectorType( (*itIm) >> 28 );
        // create LHCbId from int. Clumsy !                                                                                                               
        temp.setID( (*itIm) );
        ids.push_back( temp );
      }
    }
    if( msgLevel(MSG::VERBOSE) ) verbose() << "MCPart " << part->key() 
                                           << " has " << ids.size() << " LHCbIDs " <<endmsg;
    //////////////////////////////////////    

    std::vector<bool> flags;
    //flags.push_back( true );
    flags.push_back( eta25 && isInVelo && !isElectron);
    flags.push_back( eta25 && isLong && !isElectron);
    flags.push_back( eta25 && isLong && over5 && !isElectron  );
    flags.push_back( eta25 && isLong && strangeLong && !isElectron);
    flags.push_back( eta25 && isLong && strangeLong && over5 && !isElectron);
    flags.push_back( eta25 && isLong && fromB && !isElectron);
    flags.push_back( eta25 && isLong && fromB && over5 && !isElectron);
    if(m_triggerNumbers){
      flags.push_back( eta25 && isLong && fromB  && trigger && !isElectron);
      flags.push_back( eta25 && isLong && isInUT && fromB && trigger && !isElectron);
    }
    m_velo->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    flags.clear();
    flags.push_back( eta25 && isLong && !isElectron );
    flags.push_back( eta25 && isLong && over5 && !isElectron  );
    flags.push_back( eta25 && isLong && strangeLong && !isElectron);
    flags.push_back( eta25 && isLong && strangeLong && over5 && !isElectron);
    flags.push_back( eta25 && isLong && fromB && !isElectron);
    flags.push_back( eta25 && isLong && fromB && over5 && !isElectron);
    if(m_triggerNumbers){
      flags.push_back( eta25 && isLong && fromB  && trigger && !isElectron);
      flags.push_back( eta25 && isLong && isInUT && fromB && trigger && !isElectron);
    }
    m_forward->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_match->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_best->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_bestLong->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    flags.clear();
    flags.push_back( eta25 && isInVelo && !isElectron);
    flags.push_back( eta25 && isInVelo && isInUT && !isElectron );
    flags.push_back( eta25 && isInVelo && isInUT && over5 && !isElectron);
    flags.push_back( eta25 && !isLong && isInVelo && !isElectron );
    flags.push_back( eta25 && !isLong && isInVelo && isInUT && !isElectron);
    flags.push_back( eta25 && !isLong && isInVelo && isInUT && over5 && !isElectron);
    flags.push_back( eta25 && isLong && !isElectron);
    flags.push_back( eta25 && isLong && over5 && !isElectron);
    flags.push_back( eta25 && isLong && fromB && !isElectron);
    flags.push_back( eta25 && isLong && fromB && over5 && !isElectron);
    if(m_triggerNumbers){
      flags.push_back( eta25 && isLong && fromB  && trigger && !isElectron);
      flags.push_back( eta25 && isLong && isInUT && fromB && trigger && !isElectron);
    }
    m_upTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
     
    flags.clear();
    flags.push_back( eta25 && trackInfo.hasT( part ) && !isElectron);
    flags.push_back( eta25 && isLong && !isElectron);
    flags.push_back( eta25 && isLong && over5 && !isElectron);
    flags.push_back( eta25 && isLong && fromB && !isElectron);
    flags.push_back( eta25 && isLong && fromB && over5 && !isElectron);
    flags.push_back( eta25 && strangeDown && !isElectron);
    flags.push_back( eta25 && strangeDown && over5 && !isElectron);
    flags.push_back( eta25 && strangeDown && !isInVelo && !isElectron);
    flags.push_back( eta25 && strangeDown && over5 && !isInVelo && !isElectron);
    flags.push_back( eta25 && strangeDown && (fromB || fromD) && !isElectron);
    flags.push_back( eta25 && strangeDown && over5 && (fromB || fromD) && !isElectron);
    flags.push_back( eta25 && strangeDown && !isInVelo && (fromB || fromD) && !isElectron);
    flags.push_back( eta25 && strangeDown && !isInVelo && over5 && (fromB || fromD) && !isElectron);
    m_tTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    flags.clear();
    flags.push_back( eta25 && isDown && !isElectron);
    flags.push_back( eta25 && isDown && over5 && !isElectron);
    flags.push_back( eta25 && strangeDown && !isElectron);
    flags.push_back( eta25 && strangeDown && over5 && !isElectron);
    flags.push_back( eta25 && strangeDown && !isInVelo && !isElectron);
    flags.push_back( eta25 && strangeDown && over5 && !isInVelo && !isElectron);
    flags.push_back( eta25 && isDown && fromB && !isElectron);
    flags.push_back( eta25 && isDown && fromB && over5 && !isElectron);
    flags.push_back( eta25 && isDown && fromB && !isInVelo && !isElectron);
    flags.push_back( eta25 && isDown && fromB && over5 && !isInVelo && !isElectron);
    flags.push_back( eta25 && strangeDown && (fromB || fromD) && !isElectron);
    flags.push_back( eta25 && strangeDown && over5 && (fromB || fromD) && !isElectron);
    flags.push_back( eta25 && strangeDown && !isInVelo && (fromB || fromD) && !isElectron);
    flags.push_back( eta25 && strangeDown && !isInVelo && over5 && (fromB || fromD) && !isElectron);
    m_downTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_bestDownstream->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    

    // -- UT Hits counters (excluded for the moment)
    /*
    flags.clear();    
    flags.push_back( eta25 && isLong  );    
    flags.push_back( eta25 && isLong && over5   );
    flags.push_back( eta25 && isLong && over5 && isInUT  );
    if(m_triggerNumbers){
      flags.push_back( eta25 && isLong && trigger  );
    }
    
    flags.push_back( eta25 && isLong && fromB );
    flags.push_back( eta25 && isLong && fromB && over5 );
    flags.push_back( eta25 && isLong && fromB && over5 && isInUT);
    
    if(m_triggerNumbers){
      flags.push_back( eta25 && isLong && fromB && trigger  );
    }
    
    m_utForward->countAndPlot( m_histoTool, part, flags, ids, nPrim );
    m_utMatch->countAndPlot( m_histoTool, part, flags, ids, nPrim );
    m_utbestLong->countAndPlot( m_histoTool, part, flags, ids, nPrim );
    */
  }

   
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrChecker::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  for ( std::vector<IPrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->printStatistics();
  }

  //for ( std::vector<IPrUTCounter*>::iterator itCt = m_allUTCounters.begin();
  //      m_allUTCounters.end() != itCt; ++itCt ) {    
  //  (*itCt)->printStatistics();
  //}

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

