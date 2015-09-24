// Include files

// Linker
#include "Linker/AllLinks.h"

#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCVertex.h"
#include "Event/MCProperty.h"
// local
#include "PrChecker2.h"

#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/IHistoTool.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PrChecker2
//
// 2005-03-29 : Olivier Callot
// modified by Wenbin Qian for Upgrade
// 2013-05-31 : Sascha Stahl, Thomas Nikodem upgraded to write Histograms
// 2014-12-22 : Svende Braun, Michel De Cian upgraded for configurable selection cuts
// 2015-01-17 : Svende Braun, Michel De Cian upgraded to faster version
//-----------------------------------------------------------------------------



DECLARE_ALGORITHM_FACTORY( PrChecker2 )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrChecker2::PrChecker2( const std::string& name,
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
  m_new(NULL),
  m_ttForward(NULL),
  m_ttMatch(NULL),
  m_ttDownst(NULL),
  m_ttnew(NULL),
  m_writeVeloHistos(-1),
  m_writeForwardHistos(-1),
  m_writeMatchHistos(-1),
  m_writeDownHistos(-1),
  m_writeUpHistos(-1),
  m_writeTTrackHistos(-1),
  m_writeBestHistos(-1),
  m_writeBestLongHistos(-1),
  m_writeBestDownstreamHistos(-1),
  m_writeNewHistos(-1),
  m_writeUTHistos(-1),
  m_writeTTForwardHistos(-1),
  m_writeTTMatchHistos(-1),
  m_writeTTDownstHistos(-1),
  m_writeTTNewHistos(-1),
  m_selectIdNew(1),
  m_selectIdNewTT(1),
  m_eta25cut(false),
  m_triggerNumbers(false),
  m_vetoElectrons(true),
  m_trackextrapolation(false),
  m_upgrade(true),
  m_histoTool(NULL)
{
  declareProperty( "VeloTracks",                m_veloTracks                = LHCb::TrackLocation::Velo       );
  declareProperty( "ForwardTracks",             m_forwardTracks             = LHCb::TrackLocation::Forward    );
  declareProperty( "MatchTracks",               m_matchTracks               = LHCb::TrackLocation::Match      );
  declareProperty( "SeedTracks",                m_seedTracks                = LHCb::TrackLocation::Seed       );
  declareProperty( "DownTracks",                m_downTracks                = LHCb::TrackLocation::Downstream );
  declareProperty( "UpTracks",                  m_upTracks                  = LHCb::TrackLocation::VeloTT     );
  declareProperty( "BestTracks",                m_bestTracks                = LHCb::TrackLocation::Default    );
  declareProperty( "NewTracks",                 m_newTracks                 = LHCb::TrackLocation::VeloTT     );
  declareProperty( "NewTTTracks",               m_ttnewTracks               = LHCb::TrackLocation::VeloTT     );
  // -- histograming options
  declareProperty( "WriteVeloHistos",           m_writeVeloHistos           = -1 );
  declareProperty( "WriteForwardHistos",        m_writeForwardHistos        = -1 );
  declareProperty( "WriteMatchHistos",          m_writeMatchHistos          = -1 );
  declareProperty( "WriteDownHistos",           m_writeDownHistos           = -1 );
  declareProperty( "WriteUpHistos",             m_writeUpHistos             = -1 );
  declareProperty( "WriteTTrackHistos",         m_writeTTrackHistos         = -1 );
  declareProperty( "WriteBestHistos",           m_writeBestHistos           = -1 );
  declareProperty( "WriteBestLongHistos",       m_writeBestLongHistos       = -1 );
  declareProperty( "WriteBestDownstreamHistos", m_writeBestDownstreamHistos = -1 );
  declareProperty( "WriteNewHistos",            m_writeNewHistos            = -1 );
  declareProperty( "WriteUTHistos",             m_writeUTHistos             = -1 );
  declareProperty( "WriteTTForwardHistos",      m_writeTTForwardHistos      = -1 );
  declareProperty( "WriteTTMatchHistos",        m_writeTTMatchHistos        = -1 );
  declareProperty( "WriteTTDownstHistos",       m_writeTTDownstHistos       = -1 );
  declareProperty( "WriteTTNewHistos",          m_writeTTNewHistos          = -1 );
  // -- option for additional track container
  declareProperty( "SelectIdNewContainer",      m_selectIdNew               = 1  );
  declareProperty( "SelectIdNewTTContainer",    m_selectIdNewTT             = 1  );
  // -- global cut options
  declareProperty( "Eta25Cut",                  m_eta25cut                  = false    );
  declareProperty( "TriggerNumbers",            m_triggerNumbers            = false    );
  declareProperty( "VetoElectrons",             m_vetoElectrons             = true     );
  declareProperty( "TrackExtrapolation",        m_trackextrapolation        = false    );
  // -- custom cut options
  declareProperty( "MyForwardCuts",             m_map_forward               = DefaultCutMap("Forward")     );
  declareProperty( "MyVeloCuts",                m_map_velo                  = DefaultCutMap("Velo")        );
  declareProperty( "MyUpCuts",                  m_map_up                    = DefaultCutMap("Upstream")    );
  declareProperty( "MyTtrackCuts",              m_map_ttrack                = DefaultCutMap("Ttrack")      );
  declareProperty( "MyDownCuts",                m_map_down                  = DefaultCutMap("Downstream")  );
  declareProperty( "MyNewCuts",                 m_map_new                   = DefaultCutMap("New")         );
  declareProperty( "MyTTForwardCuts",           m_map_ttforward             = DefaultCutMap("TTForward")   );
  declareProperty( "MyTTDownCuts",              m_map_ttdown                = DefaultCutMap("TTDownstream"));
  declareProperty( "MyTTNewCuts",               m_map_ttnew                 = DefaultCutMap("TTNew")       );

  declareProperty( "Upgrade",                   m_upgrade                   = true);

}
//=============================================================================
// Destructor
//=============================================================================
PrChecker2::~PrChecker2() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrChecker2::initialize()
{

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  IHistoTool* htool = tool<IHistoTool>( "HistoTool","PrChecker2Histos",this ) ;
  GaudiHistoTool* ghtool = dynamic_cast<GaudiHistoTool*>(htool) ;
    
  // -- catch the possible failure of the dynamic cast
  if( ghtool == NULL){
    error() << "Dynamic cast of Gaudi Histogramming Tool failed!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  ghtool->setHistoDir("Track/PrChecker2") ;
  m_histoTool = htool;

  if(m_triggerNumbers){//if true add extra cuts in maps
    m_map_forward.insert(  stringpair("07_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger"       ));
    m_map_forward.insert(  stringpair("08_TT_long_fromB_P>3GeV_Pt>0.5GeV" ,"isLong & fromB & trigger & isTT"));
    m_map_velo.insert(     stringpair("08_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger"       ));
    m_map_velo.insert(     stringpair("09_TT_long_fromB_P>3GeV_Pt>0.5GeV" ,"isLong & fromB & trigger & isTT"));
    m_map_up.insert(       stringpair("11_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger"       ));
    m_map_up.insert(       stringpair("12_TT_long_fromB_P>3GeV_Pt>0.5GeV" ,"isLong & fromB & trigger & isTT"));
    m_map_new.insert(      stringpair("long_fromB_P>3GeV_Pt>0.5GeV"       ,"isLong & fromB & trigger"       ));
    m_map_new.insert(      stringpair("TT_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger & isTT"));
    m_map_ttforward.insert(stringpair("03_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger"       ));
    m_map_ttforward.insert(stringpair("04_TT_long_fromB_P>3GeV_Pt>0.5GeV" ,"isLong & fromB & trigger & isTT"));
    m_map_ttdown.insert(   stringpair("07_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger"       ));
    m_map_ttdown.insert(   stringpair("08_TT_long_fromB_P>3GeV_Pt>0.5GeV" ,"isLong & fromB & trigger & isTT"));
    m_map_ttnew.insert(    stringpair("long_fromB_P>3GeV_Pt>0.5GeV"       ,"isLong & fromB & trigger"       ));
    m_map_ttnew.insert(    stringpair("TT_long_fromB_P>3GeV_Pt>0.5GeV"    ,"isLong & fromB & trigger & isTT"));
  }
  
  m_velo = tool<IPrCounter>( "PrCounter2", "Velo", this );
  m_velo->setContainer( m_veloTracks );
  m_velo->setWriteHistos(m_writeVeloHistos);
  m_velo->setSelectId( 3 );
  for( auto pair : m_map_velo ){
    m_velo->addSelection( pair.first ,true);
  }
  
  m_forward = tool<IPrCounter>( "PrCounter2", "Forward", this );
  m_forward->setContainer( m_forwardTracks );
  m_forward->setWriteHistos(m_writeForwardHistos);
  m_forward->setSelectId( 8 );
  for( auto pair : m_map_forward ){
    m_forward->addSelection( pair.first ,true);
  }
  
  m_match = tool<IPrCounter>( "PrCounter2", "Match", this );
  m_match->setContainer( m_matchTracks );
  m_match->setWriteHistos(m_writeMatchHistos);
  m_match->setSelectId( 8 );
  for( auto pair : m_map_forward ){
    m_match->addSelection( pair.first ,true);
  }
  
  m_upTrack = tool<IPrCounter>( "PrCounter2", "Upstream", this );
  m_upTrack->setContainer( m_upTracks );
  m_upTrack->setWriteHistos(m_writeUpHistos);
  m_upTrack->setSelectId( 4 );
  for( auto pair : m_map_up ){
    m_upTrack->addSelection( pair.first ,true);
  }
  
  m_tTrack = tool<IPrCounter>( "PrCounter2", "TTrack", this );
  m_tTrack->setContainer( m_seedTracks );
  m_tTrack->setWriteHistos(m_writeTTrackHistos );
  m_tTrack->setSelectId( 8 );
  for( auto pair : m_map_ttrack ){
    m_tTrack->addSelection( pair.first ,true);
  }
  
  m_downTrack = tool<IPrCounter>( "PrCounter2", "Downstream", this );
  m_downTrack->setContainer( m_downTracks );
  m_downTrack->setWriteHistos(m_writeDownHistos );
  m_downTrack->setSelectId( 4 );
  for( auto pair : m_map_down ){
    m_downTrack->addSelection( pair.first ,true);
  }
  
  m_best = tool<IPrCounter>( "PrCounter2", "Best", this );
  m_best->setContainer( m_bestTracks );
  m_best->setWriteHistos(m_writeBestHistos);
  m_best->setSelectId( 15 );
  for( auto pair : m_map_forward ){
    m_best->addSelection( pair.first ,true);
  }
  
  m_bestLong = tool<IPrCounter>( "PrCounter2", "BestLong", this );
  m_bestLong->setContainer( m_bestTracks );
  m_bestLong->setWriteHistos(m_writeBestLongHistos);
  m_bestLong->setSelectId( 15 );
  m_bestLong->setTrackType( LHCb::Track::Long );
  for( auto pair : m_map_forward ){
    m_bestLong->addSelection( pair.first ,true);
  }
  
  m_bestDownstream = tool<IPrCounter>( "PrCounter2", "BestDown", this );
  m_bestDownstream->setContainer( m_bestTracks );
  m_bestDownstream->setWriteHistos(m_writeBestDownstreamHistos);
  m_bestDownstream->setSelectId( 12 );
  m_bestDownstream->setTrackType( LHCb::Track::Downstream ); 
  for( auto pair : m_map_down ){
    m_bestDownstream->addSelection( pair.first ,true);
  }
  
  m_new = tool<IPrCounter>( "PrCounter2", "New", this );
  m_new->setContainer( m_newTracks );
  m_new->setWriteHistos(m_writeNewHistos);
  m_new->setSelectId( m_selectIdNew );
  for( auto pair : m_map_new ){
    m_new->addSelection( pair.first ,true);
  }

  m_allCounters.push_back( m_velo  );
  m_allCounters.push_back( m_forward );
  m_allCounters.push_back( m_match );
  m_allCounters.push_back( m_upTrack );
  m_allCounters.push_back( m_tTrack );
  m_allCounters.push_back( m_downTrack );
  m_allCounters.push_back( m_best  );
  m_allCounters.push_back( m_bestLong  );
  m_allCounters.push_back( m_bestDownstream  );
  if(!m_map_new.empty()){
    m_allCounters.push_back( m_new  );
  }
 
  m_ttForward = tool<IPrTTCounter>( "PrTTCounter", "TTForward", this );
  m_ttForward->setContainer( m_forwardTracks );
  m_ttForward->setWriteHistos(m_writeTTForwardHistos);
  for( auto pair : m_map_ttforward ){
    m_ttForward->addSelection( pair.first ,true);
  }
  
  m_ttMatch = tool<IPrTTCounter>( "PrTTCounter", "TTMatch", this );
  m_ttMatch->setContainer( m_matchTracks );
  m_ttMatch->setWriteHistos(m_writeTTMatchHistos);
  for( auto pair : m_map_ttforward ){
    m_ttMatch->addSelection( pair.first ,true);
  }
  
  m_ttDownst = tool<IPrTTCounter>( "PrTTCounter", "TTDownstream", this );
  m_ttDownst->setContainer( m_downTracks );
  m_ttDownst->setWriteHistos(m_writeTTDownstHistos);
  for( auto pair : m_map_ttdown ){
    m_ttDownst->addSelection( pair.first ,true);
  }

  m_ttnew = tool<IPrTTCounter>( "PrTTCounter", "TTNew", this );
  m_ttnew->setContainer( m_ttnewTracks );
  m_ttnew->setWriteHistos(m_writeTTNewHistos);
  m_ttnew->setSelectId( m_selectIdNewTT );
  for( auto pair : m_map_ttnew ){
    m_ttnew->addSelection( pair.first ,true);
  }

  m_allTTCounters.push_back( m_ttForward );
  m_allTTCounters.push_back( m_ttMatch   );
  m_allTTCounters.push_back( m_ttDownst  );
  if(!m_map_ttnew.empty()){
    m_allTTCounters.push_back( m_ttnew  );
  }
  
  for ( std::vector<IPrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->setUseEta25Cut(m_eta25cut);
    (*itC)->setTriggerNumbers(m_triggerNumbers);
    (*itC)->setTrackExtrapolation(m_trackextrapolation);
  }
  
  for ( std::vector<IPrTTCounter*>::iterator itCt = m_allTTCounters.begin();
        m_allTTCounters.end() != itCt; ++itCt ) {
    (*itCt)->setUseEta25Cut(m_eta25cut);
    (*itCt)->setTriggerNumbers(m_triggerNumbers);
    (*itCt)->setTrackExtrapolation(m_trackextrapolation);
    }

  

  m_Cuts.insert( std::pair<std::string, strings>( "Forward",      getMyCut( m_map_forward   )) );
  m_Cuts.insert( std::pair<std::string, strings>( "Velo",         getMyCut( m_map_velo      )) );
  m_Cuts.insert( std::pair<std::string, strings>( "Upstream",     getMyCut( m_map_up        )) );
  m_Cuts.insert( std::pair<std::string, strings>( "Ttrack",       getMyCut( m_map_ttrack    )) );
  m_Cuts.insert( std::pair<std::string, strings>( "Downstream",   getMyCut( m_map_down      )) );
  m_Cuts.insert( std::pair<std::string, strings>( "TTForward",    getMyCut( m_map_ttforward )) );
  m_Cuts.insert( std::pair<std::string, strings>( "TTDownstream", getMyCut( m_map_ttdown    )) );
  m_Cuts.insert( std::pair<std::string, strings>( "New",          getMyCut( m_map_new       )) );
  m_Cuts.insert( std::pair<std::string, strings>( "TTNew",        getMyCut( m_map_ttnew     )) );

  // -- often used aliases for convenience
  strings cutAliases ={"isNotLong", 
                       "isLong",  
                       "isVelo", 
                       "isNotVelo", 
                       "isDown", 
                       "isNotDown", 
                       "isUp", 
                       "isNotUp", 
                       "isTT", 
                       "isNotTT", 
                       "isSeed", 
                       "isNotSeed", 
                       "fromB", 
                       "fromKsFromB", 
                       "strange", 
                       "fromD", 
                       "isElectron", 
                       "isNotElectron", 
                       "BOrDMother" };
 
  // -- initialize the LoKi cuts
  m_factory = tool<LoKi::IMCHybridFactory> ("LoKi::Hybrid::MCTool/MCHybridFactory:PUBLIC" , this ) ;
  if ( nullptr == m_factory ){ return Error ( "Could not locate IMCHybridFactory" ) ; } // RETURN

  // -- convert all strings into functors
  for( auto cutPair : m_Cuts ){//loop over m_Cuts

    std::vector< addOtherCuts >       otherCutsVec;
    std::vector< LoKi::Types::MCCut > lokiCutsVec; //vector von LoKi cuts
    
    for( std::string cutString : cutPair.second ){//loop over 2nd element of Cuts = strings of cuts
      
      otherCutsVec.push_back( addOtherCuts() );
      
      // -- loop over cutAliases
      // -- extract them from cutString and replace with 'MCTRUE'
      for( std::string alias : cutAliases ){
        
        std::size_t found = cutString.find( alias );
        
        if( found != std::string::npos ){//if found then
          otherCutsVec.back().addCut( isTrack( m_lookuptable[alias]) );//other components are already filled //add this category of cuts to addOtherCuts()
          cutString.replace(found, alias.length(), "MCTRUE");//replace found at position found, with length of string to replace, replace it with string "" (Loki Cut)
        }
      }
      
      // -- Veto electrons or not
      if(m_vetoElectrons == true) {
        const std::string eString("isNotElectron");
        otherCutsVec.back().addCut( isTrack( m_lookuptable[eString] ) );
        std::size_t found = cutString.find( eString );
        if( found != std::string::npos ){
          cutString.replace(found, eString.length(), "MCTRUE");
        }
      }
      
      
      // -- LoKi cuts: define aliases for better use
      if(m_eta25cut == true) cutString.append(" & eta25");
      const std::string etaString("eta25");
      std::size_t found = cutString.find( etaString );
      if( found != std::string::npos ){
        cutString.replace(found, etaString.length(), "(MCETA > 2.0) & (MCETA < 5.0)");
      }
      
      const std::string over5String("over5");
      found = cutString.find( over5String );
      if( found != std::string::npos ){
        cutString.replace(found,  over5String.length(), "(MCP > 5000)");
      }
      
      const std::string triggerString("trigger");
      found = cutString.find( triggerString );
      if( found != std::string::npos ){
        cutString.replace(found,  triggerString.length(), "(MCP>3000) & (MCPT>500)");
      }
      // ---------------------------------------------------------------------------------
      
      LoKi::Types::MCCut tmp =  LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant( false );//
      sc = m_factory->get ( cutString , tmp );
      lokiCutsVec.push_back(tmp);
      if ( sc.isFailure() ){ return Error ( "Error from IMCHybridFactory",sc ) ; } // RETURN
    }
    
    //-- split cuts into LoKi cuts and other (non-LoKi) cuts
    m_LoKiCuts.insert(  std::pair<std::string, std::vector<LoKi::Types::MCCut>>( cutPair.first, lokiCutsVec ) );//give same names as in m_Cuts = 1st, LoKicuts = 2nd
    m_otherCuts.insert( std::pair<std::string, std::vector<addOtherCuts>>(       cutPair.first, otherCutsVec ) );//give same names as in m_Cuts = 1st, othercuts = 2nd
    
  }

  
  m_veloMCCuts          = m_LoKiCuts["Velo"];
  m_veloMCCuts2         = m_otherCuts["Velo"];
  m_forwardMCCuts       = m_LoKiCuts["Forward"];
  m_forwardMCCuts2      = m_otherCuts["Forward"];
  m_upstreamMCCuts      = m_LoKiCuts["Upstream"];
  m_upstreamMCCuts2     = m_otherCuts["Upstream"];
  m_ttrackMCCuts        = m_LoKiCuts["Ttrack"];
  m_ttrackMCCuts2       = m_otherCuts["Ttrack"];
  m_downstreamMCCuts    = m_LoKiCuts["Downstream"];
  m_downstreamMCCuts2   = m_otherCuts["Downstream"];
  m_newMCCuts           = m_LoKiCuts["New"];
  m_newMCCuts2          = m_otherCuts["New"];
  m_ttforwardMCCuts     = m_LoKiCuts["TTForward"];
  m_ttforwardMCCuts2    = m_otherCuts["TTForward"];
  m_ttdownstreamMCCuts  = m_LoKiCuts["TTDownstream"];
  m_ttdownstreamMCCuts2 = m_otherCuts["TTDownstream"];
  m_ttnewMCCuts         = m_LoKiCuts["TTNew"];
  m_ttnewMCCuts2        = m_otherCuts["TTNew"];
  
  return StatusCode::SUCCESS;
  
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode PrChecker2::execute() {

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
  
  for ( std::vector<IPrTTCounter*>::iterator itCt = m_allTTCounters.begin();
        m_allTTCounters.end() != itCt; ++itCt ) {
    (*itCt)->initEvent(m_histoTool);
  }
  
  //== Build a table (vector of vectors) of ids per MCParticle, indexed by MCParticle key.
  std::string hitLocation = "Pat/LHCbID";
  if(m_upgrade){
    hitLocation = "Pr/LHCbID";
  }
  
  AllLinks<LHCb::MCParticle> allIds( evtSvc(), msgSvc(), hitLocation);
  
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
    if ( 0 == trackInfo.fullInfo( part ) )   continue;
    if( msgLevel(MSG::VERBOSE) ) verbose() << "checking MCPart " << part->key() << endmsg;
    
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
    //Velo cuts
    for(unsigned int i = 0; i < m_veloMCCuts.size(); ++i){
      flags.push_back( m_veloMCCuts[i](part) && m_veloMCCuts2[i](part, &trackInfo) );
    }
    m_velo->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    // Forward cuts
    flags.clear();
    for(unsigned int i = 0; i < m_forwardMCCuts.size(); ++i){
      flags.push_back( m_forwardMCCuts[i](part) && m_forwardMCCuts2[i](part, &trackInfo) );
    }
    m_forward->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_match->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_best->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_bestLong->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    // Upstream cuts
    flags.clear();
    for(unsigned int i = 0; i < m_upstreamMCCuts.size(); ++i){
      flags.push_back( m_upstreamMCCuts[i](part) && m_upstreamMCCuts2[i](part, &trackInfo) );
    }
    m_upTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    // Ttrack cuts
    flags.clear();
    for(unsigned int i = 0; i < m_ttrackMCCuts.size(); ++i){
      flags.push_back( m_ttrackMCCuts[i](part) && m_ttrackMCCuts2[i](part, &trackInfo) );
    }
    m_tTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    //Downstream cuts
    flags.clear();
    for(unsigned int i = 0; i < m_downstreamMCCuts.size(); ++i){
      flags.push_back( m_downstreamMCCuts[i](part) && m_downstreamMCCuts2[i](part, &trackInfo) );
    }
    m_downTrack->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    m_bestDownstream->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    

    //New cuts (for new track container)
    flags.clear();
    for(unsigned int i = 0; i < m_newMCCuts.size(); ++i){
      flags.push_back( m_newMCCuts[i](part) && m_newMCCuts2[i](part, &trackInfo) );
    }
    m_new->countAndPlot(m_histoTool,part,flags,ids,nPrim);
    
    
    //TTForward cuts
    flags.clear();
    for(unsigned int i = 0; i < m_ttforwardMCCuts.size(); ++i){
      flags.push_back( m_ttforwardMCCuts[i](part) && m_ttforwardMCCuts2[i](part, &trackInfo) );
    }    
    m_ttForward->countAndPlot( m_histoTool, part, flags, ids );
    m_ttMatch->countAndPlot( m_histoTool, part, flags, ids );
    
    //TTDownstream cuts
    flags.clear();
    for(unsigned int i = 0; i < m_ttdownstreamMCCuts.size(); ++i){
      flags.push_back( m_ttdownstreamMCCuts[i](part) && m_ttdownstreamMCCuts2[i](part, &trackInfo) );
    }
    m_ttDownst->countAndPlot( m_histoTool, part, flags, ids );
    
    
    //New cuts (for new TTtrack container)
    flags.clear();
    for(unsigned int i = 0; i < m_ttnewMCCuts.size(); ++i){
      flags.push_back( m_ttnewMCCuts[i](part) && m_ttnewMCCuts2[i](part, &trackInfo) );
    }    
    m_ttnew->countAndPlot(m_histoTool, part, flags, ids);
    
    
    
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrChecker2::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  for ( std::vector<IPrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->printStatistics();
  }

  for ( std::vector<IPrTTCounter*>::iterator itCt = m_allTTCounters.begin();
        m_allTTCounters.end() != itCt; ++itCt ) {    
    (*itCt)->printStatistics();
  }
  
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

