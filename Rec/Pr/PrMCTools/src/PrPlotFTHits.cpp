// Include files

// Linker
#include "Linker/AllLinks.h"

#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCProperty.h"
// local
#include "PrPlotFTHits.h"


#include "GaudiAlg/IHistoTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrPlotFTHits
//
// 2014-05-08 : Michel De Cian
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( PrPlotFTHits )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPlotFTHits::PrPlotFTHits( const std::string& name,
                        ISvcLocator* pSvcLocator)
  //: GaudiAlgorithm ( name , pSvcLocator ),
: GaudiHistoAlg ( name , pSvcLocator ),
  m_histoTool( nullptr ),
  m_ghtool( nullptr ),
  m_ftHitManager( nullptr ),
  m_extrapolator( nullptr ),
  m_idealStateCreator( nullptr ),
  m_zone( 24 )
{ 
  declareProperty( "PlotStates" , m_plotState = false);
  declareProperty( "ExcludeElectrons", m_excludeElectrons = true );
  declareProperty( "PlotHitEfficiency", m_plotHitEfficiency = true);
  declareProperty( "PlotAllFTHits", m_plotAllFTHits = true);
  declareProperty( "PlotFTHitsOnTrack", m_plotFTHitsOnTrack = true);
  declareProperty( "PlotTrackingEfficiency", m_plotTrackingEfficiency = true);
  declareProperty( "PlotMCHits", m_plotMCHits = true);
  declareProperty( "OnlyLongAndDownForMCHits", m_onlyLongDownForMCHits = true );
  declareProperty( "PlotOccupancy", m_plotOccupancy = false);
}
//=============================================================================
// Destructor
//=============================================================================
PrPlotFTHits::~PrPlotFTHits() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrPlotFTHits::initialize()
{

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  IHistoTool* htool = tool<IHistoTool>( "HistoTool","PrPlotFTHitsHistos",this ) ;
  m_ghtool = dynamic_cast<GaudiHistoTool*>(htool) ;
  
  // -- catch the possible failure of the dynamic cast
  if( m_ghtool == NULL){
    error() << "Dynamic cast of Gaudi Histogramming Tool failed!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_ftHitManager = tool<PrHitManager>("PrFTHitManager");
  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",this);
  m_idealStateCreator = tool<IIdealStateCreator>("IdealStateCreator", "IdealStateCreator", this);
  
  m_ghtool->setHistoDir("Track/PrPlotFTHits") ;
  m_histoTool = htool;
  
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrPlotFTHits::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  if( m_plotOccupancy ) plotOccupancy();
  if( m_plotHitEfficiency ) plotHitEfficiency();
  if( m_plotAllFTHits ) plotAllFTHits();
  if( m_plotFTHitsOnTrack ) plotFTHitsOnTrack();
  if( m_plotTrackingEfficiency) plotTrackingEfficiency();
  if( m_plotMCHits) plotMCHits();
  if( m_plotState) plotState();
  return StatusCode::SUCCESS;
}
void PrPlotFTHits::plotOccupancy(){
  m_ghtool->setHistoDir("Track/PrPlotFTHits/Occupancy");
  std::map<int,int> m_Occupancy;
  const LHCb::MCHits *mcHitsNext = getIfExists<LHCb::MCHits>("Next/"+LHCb::MCHitLocation::FT);
  const LHCb::MCHits* mcHitsPrev = getIfExists<LHCb::MCHits>("Prev/"+LHCb::MCHitLocation::FT);
    
  bool noSpill = false;
  if(! mcHitsNext && !mcHitsPrev){
    info()<<"The sample has been digitized without spillover, Occupancy Plots for them are suppressed"<<endmsg;
    noSpill = true;
  }
  //"/Event/Prev/MC/FT/Hits"
  LinkedFrom<LHCb::MCHit,LHCb::MCParticle> myMCHitLink( evtSvc(), msgSvc(), LHCb::MCParticleLocation::Default + "2MC" + "FT" + "Hits" );   
    
  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );         
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myFTCluster2MCHitLink ( evtSvc(),msgSvc(), LHCb::FTClusterLocation::Default + "2MCHits");
  //"/Event/Prev/MC/FT/Hits"                                             
  char layerName[100];                                                   
  char Title[100];                                                       
  std::vector<int> nHits(12,0);                                          
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );                           
    
  LHCb::MCVertices* mcVert = getIfExists<LHCb::MCVertices>(LHCb::MCVertexLocation::Default );                                                        
  if(  mcVert == nullptr ){
    error() << "Could not find MCVertices at " << LHCb::MCParticleLocation::Default << endmsg;
  }
  unsigned int NbPrimaries = 0;
  for ( LHCb::MCVertices::iterator itV = mcVert->begin(); mcVert->end() != itV; ++itV ){
    if ( (*itV)->isPrimary() ){
      NbPrimaries++;
    }
  }
  sprintf(Title,"Nb Primary vertices;Counts");
  plot(NbPrimaries,"NbPrimary",Title,-0.5,25.5,25);
  int recoble = 0;
  int all =0;
  int recoblewanted = 0;
  LHCb::MCParticles* mcParts = getIfExists<LHCb::MCParticles> ( LHCb::MCParticleLocation::Default );                                           
  LHCb::MCParticle* mcPart = nullptr;
  for( LHCb::MCParticles::const_iterator iPart = mcParts->begin(); iPart != mcParts->end(); ++iPart){
    mcPart = (*iPart);
    bool isRecoble = trackInfo.hasT(mcPart);
    bool recobleWanted = isRecoble && (mcPart->originVertex()->isPrimary() || mcPart->originVertex()->isDecay()); //really fast checker if mcParticle is a wanted one or not
    all++;
    if(isRecoble) recoble++;
    if(recobleWanted) recoblewanted++;
  }
  plot(all,"NbMCParticle_All","Nb. MCParticle",0.,10000.,200);
  plot(recoble,"NbMCParticle_Recoble","Nb. MCParticle",-0.5,1000.,100);
  plot(recoblewanted,"NbMCParticle_RecobleWanted","Nb. MCParticle",-0.5,1000.5,100);
  double recobleper = (double)recoble/all ;
  double recoblewantedper = (double)recoblewanted/all;
  plot( recoblewantedper,"NbMCParticle_RecobleWantedPerc","Fraction Reconstructible",-0.5,2,200);
  plot( recobleper,"NbMCParticle_RecoblePerc","Fraction Reconstructible",-0.5,2,200);
  for(unsigned int zoneI = 0; zoneI <24; ++zoneI){
    PrHitZone* zone = m_ftHitManager->zone(zoneI);
    int layer = zoneI/2;
    sprintf(layerName,"Layer%i Occupancy",layer);
    PrHit* hit = nullptr;
    for(PrHits::iterator itH = zone->hits().begin(); zone->hits().end()!=itH;++itH){
      hit = (*itH);   
      nHits[layer]++;                       
      LHCb::FTLiteCluster liteCluster = getLiteCluster(hit->id());
      int Module = liteCluster.channelID().module();
      int Quarter = liteCluster.channelID().quarter();          
      int SipmID = liteCluster.channelID().sipmId();
      int layer = liteCluster.channelID().layer();
      int module2 = Module;        
      if(Module == 11 && Module > 4 ) module2 = 1;
      if(Module != 11 && Module > 4 ) module2 = Module-3;
      if(Module <  5  ) module2 = 6-Module;
      if(Module == 10 ) module2 = 1;
      int index = Quarter*128 + 16*module2+ std::floor(SipmID);
      // if(index<0 || index>){
      //   always()<<"Problem in Sipm Index; Correct for that"<<endmsg;
      // }
      sprintf(layerName,"Layer%i/AllContributions",layer);
      sprintf(Title,"Sipm Index Layer %i;SiPm Id;Clusters/Sipm/Event",layer);
      m_histoTool->plot1D(index,layerName,Title,0.5,512.5,512);
      bool fromMCParticleContrib = false;
      bool spillCluster = false;
      bool NoiseCluster = false;
        
      LHCb::MCParticle* mcPart0 = myClusterLink.first( hit->id().ftID());
        
      LHCb::MCParticle* mcPart1 = mcPart0;
      double maxw = myClusterLink.weight();
      double w = maxw;
      
      while( mcPart0!=nullptr){
        w = myClusterLink.weight();
        if( w>maxw){
          maxw = w;
          mcPart1 = mcPart;
        }
        mcPart0 = myClusterLink.next();
      }
      LHCb::MCHit* mcHit = nullptr;
      LHCb::MCHit* mcHitHigh = myFTCluster2MCHitLink.first( hit->id().ftID());
      double maXweight = myFTCluster2MCHitLink.weight();
      double weight = maXweight;
      mcHit = mcHitHigh;
      while( mcHitHigh!=nullptr){
        weight = myFTCluster2MCHitLink.weight();
        if( weight > maXweight){
          maXweight = weight;
          mcHit = mcHitHigh;
        }
        mcHitHigh = myFTCluster2MCHitLink.next();
      }
      if(!noSpill){
        if(mcHit!=nullptr && (mcHit->parent() == mcHitsNext || mcHit->parent() == mcHitsPrev)){
          spillCluster = true;
        }
      }
      if( mcPart1 != nullptr){
        fromMCParticleContrib = true;
      }
      if( mcHit==nullptr){
        NoiseCluster = true;
      }
      
      bool RecobleMCParticle = false;
      if(fromMCParticleContrib && trackInfo.hasT(mcPart1)) RecobleMCParticle = true;
      if(RecobleMCParticle){
        sprintf(layerName,"Layer%i/FromRecobleMCParticle",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        m_histoTool->plot1D( index,layerName,Title,0.5,512.5,512);
        //0.5,96.5,96);
        if(mcPart->originVertex()->isPrimary() || mcPart->originVertex()->isDecay()){
          sprintf(layerName,"Layer%i/FromRecobleMCParticleWanted",layer);
          sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
          m_histoTool->plot1D( index,layerName,Title,0.5,512.5,512);
        }
      }
      if(fromMCParticleContrib){
        sprintf(layerName,"Layer%i/FromMCParticle",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        m_histoTool->plot1D( index,layerName,Title,0.5,512.5,512);
        //0.5,96.5,96);
      }
      if(NoiseCluster){
        sprintf(layerName,"Layer%i/FromNoise",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        m_histoTool->plot1D(index,layerName,Title,0.5,512.5,512);
        //0.5,96.5,96);                                                    
      }
      if(spillCluster && !noSpill){
        sprintf(layerName,"Layer%i/FromSpillover",layer); 
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        m_histoTool->plot1D(index,layerName,Title,0.5,512.5,512);
      }
    }//loop hits
  }//loop zones
}

         






void PrPlotFTHits::plotState(){
  m_ghtool->setHistoDir("Track/PrPlotFTHits/States");
  LinkedTo<LHCb::MCParticle, LHCb::Track > mySeedLink( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  LinkedTo<LHCb::MCParticle, LHCb::Track > myForwardLink( evtSvc(), msgSvc(), LHCb::TrackLocation::Forward);
  
  // -- get the tracks container
  
  LHCb::Tracks* forwardTracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Forward );
  LHCb::Tracks* seedTracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Seed );
  
  if( !forwardTracks ) info() << "No tracks found in: " << LHCb::TrackLocation::Forward << endmsg;                                                                                                                                        
  if( !seedTracks ) info() << "No tracks found in: " << LHCb::TrackLocation::Seed << endmsg;
  char  stateZ[100];
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );  
  if( seedTracks)
  {
    
    for( LHCb::Tracks::const_iterator iTrack = seedTracks->begin(); iTrack!=seedTracks->end();++iTrack){
      const LHCb::Track* track = *iTrack;
      LHCb::MCParticle* mcPart = mySeedLink.first( track->key());
      // std::vector<State*> TrackStates = track->states();
      if( mcPart!=nullptr){
        std::vector<LHCb::State*> TrackStates = track->states();
        if( TrackStates.size() == 0) info()<<"No states for track"<<endmsg;
        // LHCb::State Idstate;
        LHCb::State* Trstate = nullptr;
        //loop trough the states of the track
        for( std::vector<LHCb::State*>::iterator trstate = TrackStates.begin() ; trstate!=TrackStates.end(); ++trstate){
          Trstate = (*trstate);
          LHCb::State IdState;
          m_idealStateCreator->createState( mcPart,Trstate->z(), IdState);
          sprintf(stateZ,"Seeding/StateZ_%f/DeltaX",Trstate->z());
          // do comparison here 
          m_histoTool->plot1D(Trstate->x()-IdState.x(),stateZ,"Track x - IdState x;#Delta_{x} [mm]",-8.,8.,300);
          sprintf(stateZ,"Seeding/StateZ_%f/DeltaY",Trstate->z());
          m_histoTool->plot1D(Trstate->y()-IdState.y(),stateZ,"Track y - IdState y;#Delta_{y} [mm]",-8.,8.,300);
          sprintf(stateZ,"Seeding/StateZ_%f/DeltaTx",Trstate->z());
          m_histoTool->plot1D(1000*(Trstate->tx()-IdState.tx()),stateZ,"Track tx(zState) - IdState tx(zState);#Delta_{tx} [mrad];Counts/0.03 mrad",-5.,5.,300);
          // m_histoTool->plot2D(1000*(Trstate->tx()-IdState.tx()),stateZ, "Trac
          sprintf(stateZ,"Seeding/StateZ_%f/DeltaTy",Trstate->z());
          m_histoTool->plot1D(1000*(Trstate->ty()-IdState.ty()),stateZ,"Track ty(zState) - IdState ty(zState);#Delta_{ty} [mrad];Counts/0.03 mrad",-5.,5.,300);
        }
      }
    }
  }
  
  
  if( forwardTracks){
    for( LHCb::Tracks::const_iterator iTrack = forwardTracks->begin(); iTrack!=forwardTracks->end();++iTrack){                                                                                                      
      const LHCb::Track* track = *iTrack;                                                                                                                                              
      LHCb::MCParticle* mcPart = mySeedLink.first( track->key());                                                                                                                                                                                          
      // std::vector<State*> TrackStates = track->states();                                                                                                                                                                                                                                  
      if( mcPart!=nullptr){
        std::vector<LHCb::State*> TrackStates = track->states();                                                                                                                                                                                                            
        if( TrackStates.size() == 0) info()<<"No states for track"<<endmsg;                                                                                                                                                           
        // LHCb::State Idstate;                                                                                                                                                                                                                                                              
        LHCb::State* Trstate = nullptr;
        //loop trough the states of the track                                                                                                                                                                                                                                                
        for( std::vector<LHCb::State*>::iterator trstate = TrackStates.begin() ; trstate!=TrackStates.end(); ++trstate){                                                         
          Trstate = (*trstate);                                                                                                                                                 
          LHCb::State IdState;                                                                                                                                                                                                                                           
          m_idealStateCreator->createState( mcPart,Trstate->z(), IdState);                                                                                                                                                                                                
          sprintf(stateZ,"Forward/StateZ_%f/DeltaX",Trstate->z());                                                                                                                                                             
          // do comparison here                                                                                                                                                                                                                                                              
          m_histoTool->plot1D(Trstate->x()-IdState.x(),stateZ,"Track x - IdState x;#Delta_{x} [mm]",-8.,8.,300);
          m_histoTool->plot1D(Trstate->y()-IdState.y(),stateZ,"Track y - IdState y;#Delta_{y} [mm]",-8.,8.,300);
          sprintf(stateZ,"Forward/StateZ_%f/DeltaTx",Trstate->z());
          m_histoTool->plot1D(1000*(Trstate->tx()-IdState.tx()),stateZ,"Track tx(zState) - IdState tx(zState);#Delta_{tx} [mrad];Counts/0.03 mrad",-5.,5.,300);
          // m_histoTool->plot2D(1000*(Trstate->tx()-IdState.tx()),stateZ, "Trac
          sprintf(stateZ,"Forward/StateZ_%f/DeltaTy",Trstate->z());
          m_histoTool->plot1D(1000*(Trstate->ty()-IdState.ty()),stateZ,"Track ty(zState) - IdState ty(zState);#Delta_{ty} [mrad];Counts/0.03 mrad",-5.,5.,300);
        }
      }
    }
  }
}




//=============================================================================
//  Plot hit efficiency
//=============================================================================
void PrPlotFTHits::plotHitEfficiency(){
  m_ghtool->setHistoDir("Track/PrPlotFTHits/HitEff") ;
  // -- Link different track containers to MCParticles
  LinkedTo<LHCb::MCParticle, LHCb::Track> myForwardLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Forward);
  LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  // -----------------------------------------------------------------------------------
  // Plot hit efficiency as function of x and y
  // -----------------------------------------------------------------------------------
  for (unsigned int zone = 0; zone < m_zone; ++zone) {
    HitRange range = m_ftHitManager->hits(zone);
    char layerNameRecoble[100];
    char layerNameRecod[100];
    unsigned int layer = zone/2; // merges two zones in a layer
    for (HitRange::const_iterator iHit = range.begin(); range.end() != iHit; ++iHit) {
      // -- As the key of an FTCluster is its channelID, we can link LHCbID and MCParticle directly!
      // -- Caveat: Only take the first link. This might not be fully correct if high precision is needed.
      const LHCb::MCParticle* mcPart1 = myClusterLink.first( (*iHit)->id().ftID() );
      if( mcPart1 == nullptr ) continue;
      if(m_excludeElectrons){
        if( abs(mcPart1->particleID().pid()) == 11 ) continue; // No electrons!
      }
      bool isLong = trackInfo.hasVeloAndT( mcPart1 );
      bool isSeed  = trackInfo.hasT( mcPart1 );
      // -- We only care about tracks that have at least a T-station segment
      if( !isLong && !isSeed ) continue;
      // -- Long tracks
      if( isLong ){
        sprintf(layerNameRecoble, "hitsInLayerReconstructibleLong%i",layer);
        sprintf(layerNameRecod, "hitsInLayerReconstructedLong%i",layer);
        LHCb::State state;
        m_idealStateCreator->createState( mcPart1, (*iHit)->z(), state );
      
        // -- Plot all reconstructible hits that belong to this particle
        m_histoTool->plot2D( state.x(),state.y(), layerNameRecoble,"Reconstructible hit",-3000,3000.,-3000.,3000.0,100,100);
        std::vector<const LHCb::Track*> tracks = getTrack( (*iHit)->id(), LHCb::TrackLocation::Forward );
        if( !tracks.empty() ){
          bool assoc = false;
          // -- If one of the tracks associated to this LHCbID is associated to the same MCParticle 
          // -- as the LHCbID is associated, then it is associated...
          // -- (aka: if LHCbID->MCParticle == LHCbID->Track->MCParticle, then the hit was "efficient")
          for(  const LHCb::Track* track : tracks){
            LHCb::MCParticle* mcPart2 = myForwardLink.first( track->key() );
            if( mcPart1 == mcPart2 ){
              assoc = true;
              break;
            }
          }
          // -- Plot all reconstructed hits that belong to this particle
          if(assoc)  m_histoTool->plot2D( state.x(),state.y(), layerNameRecod, 
                                          "Reconstructed hit",-3000,3000.,-3000.,3000.0,100,100);
        }
      }
      // -- Seed tracks
      if( isSeed ){ 
        sprintf(layerNameRecoble, "hitsInLayerReconstructibleSeed%i",layer);
        sprintf(layerNameRecod, "hitsInLayerReconstructedSeed%i",layer);
        LHCb::State state;
        m_idealStateCreator->createState( mcPart1, (*iHit)->z(), state );
        m_histoTool->plot2D( state.x(),state.y(), layerNameRecoble,"Reconstructible hit",-3000,3000.,-3000.,3000.0,100,100);
        std::vector<const LHCb::Track*> tracks = getTrack( (*iHit)->id(), LHCb::TrackLocation::Seed );
        if( !tracks.empty() ){
          bool assoc = false;
          // -- If one of the tracks associated to this LHCbID is associated to the same MCParticle 
          // -- as the LHCbID is associated, then it is associated...
          // -- (aka: if LHCbID->MCParticle == LHCbID->Track->MCParticle, then the hit was "efficient")
          for(  const LHCb::Track* track : tracks){
            LHCb::MCParticle* mcPart2 = mySeedLink.first( track->key() );
            if( mcPart1 == mcPart2 ){
              assoc = true;
              break;
            }
          }
          // -- Plot all reconstructed hits that belong to this particle
          if(assoc)  m_histoTool->plot2D( state.x(),state.y(), layerNameRecod,"Reconstructed hit",-3000,3000.,-3000.,3000.0,100,100);
        }
      }
      // -- Seed tracks, but not long
      if( isSeed && !isLong){ 
        sprintf(layerNameRecoble, "hitsInLayerReconstructibleSeedOnly%i",layer);
        sprintf(layerNameRecod, "hitsInLayerReconstructedSeedOnly%i",layer);
        LHCb::State state;
        m_idealStateCreator->createState( mcPart1, (*iHit)->z(), state );
        m_histoTool->plot2D( state.x(),state.y(), layerNameRecoble,"Reconstructible hit",-3000,3000.,-3000.,3000.0,100,100);
        std::vector<const LHCb::Track*> tracks = getTrack( (*iHit)->id(), LHCb::TrackLocation::Seed );
        if( !tracks.empty() ){
          bool assoc = false;
          // -- If one of the tracks associated to this LHCbID is associated to the same MCParticle 
          // -- as the LHCbID is associated, then it is associated...
          // -- (aka: if LHCbID->MCParticle == LHCbID->Track->MCParticle, then the hit was "efficient")
          for(  const LHCb::Track* track : tracks){
            LHCb::MCParticle* mcPart2 = mySeedLink.first( track->key() );
            if( mcPart1 == mcPart2 ){
              assoc = true;
              break;
            }
          }
          // -- Plot all reconstructed hits that belong to this particle
          if(assoc)  m_histoTool->plot2D( state.x(),state.y(), layerNameRecod, 
                                          "Reconstructed hit",-3000,3000.,-3000.,3000.0,100,100);
        }
      }
    }
  }
}
//=============================================================================
//  Plot all FT hits (and the cluster size)
//=============================================================================
void PrPlotFTHits::plotAllFTHits(){
  
  m_ghtool->setHistoDir("Track/PrPlotFTHits/AllFTHits") ;

  char layerName[10];

  // ---------------------------------------------------------------
  // -- plot all FT hits for all zones (and their corresponding cluster size)
  // ---------------------------------------------------------------
  for (unsigned int zone = 0; zone < m_zone; ++zone) {
     HitRange range = m_ftHitManager->hits(zone);
     unsigned int layer = zone/2;
     sprintf(layerName, "hitsInLayer%i",layer);
     for (HitRange::const_iterator iHit = range.begin(); range.end() != iHit; ++iHit) {
       m_histoTool->plot2D( (*iHit)->x(), 0.5*( (*iHit)->yMin() + (*iHit)->yMax()) ,
                            layerName, "hits in given layer",-3000,3000.,-3000.,3000.0,100,2);
       m_histoTool->plot1D( (*iHit)->size(),
                            "clusterSize", "cluster size",0,10,10);
     }
  }
}
//=============================================================================
//  Plot FT hits on a truthmatched track
//=============================================================================
void PrPlotFTHits::plotFTHitsOnTrack(){
  
  m_ghtool->setHistoDir("Track/PrPlotFTHits/FTHitsOnTrack") ;

  // ----------------------------------------------------------------
  // -- plot FT hit on a given (truthmatched) track
  // ----------------------------------------------------------------
  LinkedTo<LHCb::MCParticle, LHCb::Track> myForwardLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Forward);
  LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  
  LHCb::Tracks* forwardTracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Forward );
  LHCb::Tracks* seedTracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Seed );
  
  if( !forwardTracks ) info() << "No tracks found in: " << LHCb::TrackLocation::Forward << endmsg;
  if( !seedTracks ) info() << "No tracks found in: " << LHCb::TrackLocation::Seed << endmsg;

  
  char layerName[10];
  
  // -- Forward tracks
  if(forwardTracks){
    
    for( LHCb::Tracks::const_iterator iTrack = forwardTracks->begin(); iTrack != forwardTracks->end(); ++iTrack){
      
      const LHCb::Track* track = *iTrack;
      LHCb::MCParticle* mcPart = myForwardLink.first( track->key() ); 
      
      const std::vector<LHCb::LHCbID> lhcbIDs = track->lhcbIDs();

      // --------------------------------------------------------------
      // -- truthmatched tracks
      if( mcPart != nullptr){ 
        
        if ( msgLevel( MSG::DEBUG ) ) debug() << "Forward truthmatched track has: " << lhcbIDs.size() << " hits in total" << endmsg;
        
        for(const LHCb::LHCbID ID : lhcbIDs){
        
          if( !(ID.isFT()) ) continue;
          
          unsigned int zone = 0;
          const PrHit* hit = getPrHit( ID, zone);
          if( hit == nullptr ) continue;
          unsigned int layer = zone/2;
          
          LHCb::State state = track->closestState( hit->z() );
          
          // -- Protect against nonphysical states
          if( isnan( state.x() ) || isnan( state.y() ) ) continue;
          
          StatusCode sc = m_extrapolator->propagate( state, hit->z() );
          if(!sc) continue;
          
          sprintf(layerName, "hitsOnTruthMatchedForwardTrackInLayer%i",layer);
          m_histoTool->plot2D( hit->x(),state.y() ,layerName, layerName,-3000,3000.,-3000.,3000.0,100,100);
        }
        // --------------------------------------------------------------
        // -- ghost tracks
      }else{ 
        
        if ( msgLevel( MSG::DEBUG ) ) debug() << "Forward ghost track has: " << lhcbIDs.size() << " hits in total" << endmsg;
        for(const LHCb::LHCbID ID : lhcbIDs){
          if( !(ID.isFT()) ) continue;
          unsigned int zone = 0;
          const PrHit* hit = getPrHit( ID, zone);
          if( hit == nullptr ) continue;
          unsigned int layer = zone/2;
          LHCb::State state = track->closestState( hit->z() );
          // -- Protect against nonphysical states
          if( isnan( state.x() ) || isnan( state.y() ) ) continue;
          StatusCode sc = m_extrapolator->propagate( state, hit->z() );
          if(!sc) continue;
          sprintf(layerName, "hitsOnGhostForwardTrackInLayer%i",layer);
          m_histoTool->plot2D( hit->x(),state.y() ,layerName, layerName,-3000,3000.,-3000.,3000.0,100,100);
        }
      }
    }
  }
  
  // -- Seed tracks
  if(seedTracks){
    
    for( LHCb::Tracks::const_iterator iTrack = seedTracks->begin(); iTrack != seedTracks->end(); ++iTrack){
      
      const LHCb::Track* track = *iTrack;
      
      LHCb::MCParticle* mcPart = mySeedLink.first( track->key() ); 
      
      const std::vector<LHCb::LHCbID> lhcbIDs = track->lhcbIDs();

      // --------------------------------------------------------------
      // -- truthmatched tracks
      if( mcPart != nullptr){ // truthmatched tracks
        
        if ( msgLevel( MSG::DEBUG ) ) debug() << "Seed truthmatched track has: " << lhcbIDs.size() << " hits in total" << endmsg;
        
        for(const LHCb::LHCbID ID : lhcbIDs){
          
          if( !(ID.isFT()) ) continue;
                    
          unsigned int zone = 0;
          const PrHit* hit = getPrHit( ID, zone);
          if( hit == nullptr ) continue;
          unsigned int layer = zone/2;
          LHCb::State state = track->closestState( hit->z() );
          
          // -- Protect against nonphysical states
          if( isnan( state.x() ) || isnan( state.y() ) ) continue;
          
          StatusCode sc = m_extrapolator->propagate( state, hit->z() );
          if(!sc) continue;
          
          sprintf(layerName, "hitsOnTruthMatchedSeedTrackInLayer%i",layer);
          m_histoTool->plot2D( hit->x(),state.y() ,layerName, layerName,-3000,3000.,-3000.,3000.0,100,100);
                    
        }

        // --------------------------------------------------------------
        // -- ghost tracks
      }else{ 
        
        if ( msgLevel( MSG::DEBUG ) ) debug() << "Seed ghost track has: " << lhcbIDs.size() << " hits in total" << endmsg;
        
        for(const LHCb::LHCbID ID : lhcbIDs){
          
          if( !(ID.isFT()) ) continue;
           
          unsigned int zone = 0;
          const PrHit* hit = getPrHit( ID, zone);
          if( hit == nullptr ) continue;
          unsigned int layer = zone/2;
          LHCb::State state = track->closestState( hit->z() );
          
          // -- Protect against nonphysical states
          if( isnan( state.x() ) || isnan( state.y() ) ) continue;
          
          StatusCode sc = m_extrapolator->propagate( state, hit->z() );
          if(!sc) continue;
          
          sprintf(layerName, "hitsOnGhostSeedTrackInLayer%i",layer);
          m_histoTool->plot2D( hit->x(),state.y() ,layerName, layerName,-3000,3000.,-3000.,3000.0,100,100);
                    
        }
      }
    }
  }
}
//=============================================================================
//  Plot the tracking efficiency
//=============================================================================
void PrPlotFTHits::plotTrackingEfficiency(){
  
  m_ghtool->setHistoDir("Track/PrPlotFTHits/TrackEff") ;

  LinkedFrom<LHCb::Track, LHCb::MCParticle> myForwardLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Forward);
  LinkedFrom<LHCb::Track, LHCb::MCParticle> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  

  // -----------------------------------------------------------------------------------
  // Plot efficiency as function of x and y (the z position is more or less arbitrarily chosen in the middle of the T stations)
  // -----------------------------------------------------------------------------------
  
   LHCb::MCParticles* mcParts = getIfExists<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
   if ( msgLevel( MSG::ERROR ) && !mcParts ) error() << "Could not find MCParticles at: " <<  LHCb::MCParticleLocation::Default << endmsg;
   
   MCTrackInfo trackInfo( evtSvc(), msgSvc() );
   
   for(const LHCb::MCParticle* mcPart:  *mcParts){
     
     if(m_excludeElectrons){
       if( abs(mcPart->particleID().pid()) == 11 ) continue; // No electrons!
     }
     
     
     bool isLong  = trackInfo.hasVeloAndT( mcPart );
     bool isSeed  = trackInfo.hasT( mcPart );
     bool isDown  = trackInfo.hasT( mcPart) && trackInfo.hasTT( mcPart );
     
     if( !isLong && !isSeed && !isDown) continue;
     
     // -- Long tracks
     if( isLong ){
       
       LHCb::State state;
       m_idealStateCreator->createState( mcPart, 9000, state );
       
       // -- Protect against nonphysical states
       if( isnan( state.x() ) || isnan( state.y() ) ) continue;

       m_histoTool->plot2D( state.x(),state.y() ,"reconstructibleParticlesLong",
                            "Reconstructible particles",-3000,3000.,-3000.,3000.0,100,100);
       
       LHCb::Track* track = myForwardLink.first( mcPart ); 
       if( track ){
         if( track->type() != LHCb::Track::Long ) continue;
         m_histoTool->plot2D( state.x(),state.y() ,"reconstructedParticlesLong",
                              "Reconstructed particles",-3000,3000.,-3000.,3000.0,100,100);
       }
     }
     // -- Seed tracks, but potentially also long
     if( isSeed ){ 
       
       LHCb::State state;
       m_idealStateCreator->createState( mcPart, 9000, state );
       // -- Protect against nonphysical states
       if( isnan( state.x() ) || isnan( state.y() ) ) continue;
       m_histoTool->plot2D( state.x(),state.y() ,"reconstructibleParticlesSeed",
                            "Reconstructible particles",-3000,3000.,-3000.,3000.0,100,100);
       LHCb::Track* track = mySeedLink.first( mcPart ); 
       if( track ){
         if( track->type() != LHCb::Track::Ttrack ) continue;
         m_histoTool->plot2D( state.x(),state.y() ,"reconstructedParticlesSeed",
                              "Reconstructed particles",-3000,3000.,-3000.,3000.0,100,100);
       }
     }
     // -- Downstream tracks, but potentially also long
     if( isDown ){
       LHCb::State state;
       m_idealStateCreator->createState( mcPart, 9000, state );
       // -- Protect against nonphysical states
       if( isnan( state.x() ) || isnan( state.y() ) ) continue;
       m_histoTool->plot2D( state.x(),state.y() ,"reconstructibleParticlesDown",
                            "Reconstructible particles Down",-3000,3000.,-3000.,3000.0,200,200);
       LHCb::Track* track = mySeedLink.first( mcPart ); 
       if( track ){
         if( track->type() != LHCb::Track::Ttrack ) continue;
         m_histoTool->plot2D( state.x(),state.y() ,"reconstructedParticlesDown",
                              "Reconstructed particles Down",-3000,3000.,-3000.,3000.0,100,100);
       }
     }
     
     // -- Seed tracks, which are not long tracks 
     if( isSeed && !isLong){ 
       
       LHCb::State state;
       m_idealStateCreator->createState( mcPart, 9000, state );
       
       // -- Protect against nonphysical states
       if( isnan( state.x() ) || isnan( state.y() ) ) continue;
       
       m_histoTool->plot2D( state.x(),state.y() ,"reconstructibleParticlesSeedOnly",
                            "Reconstructible particles",-3000,3000.,-3000.,3000.0,100,100);
       
       LHCb::Track* track = mySeedLink.first( mcPart ); 
       if( track ){
         if( track->type() != LHCb::Track::Ttrack ) continue;
         m_histoTool->plot2D( state.x(),state.y() ,"reconstructedParticlesSeedOnly",
                              "Reconstructed particles",-3000,3000.,-3000.,3000.0,100,100);
       }
     }
     
   }
}
//=============================================================================
//  Plot MC hits belonging to a cluster/hit
//=============================================================================
void PrPlotFTHits::plotMCHits () {
  
  m_ghtool->setHistoDir("Track/PrPlotFTHits/MCHits") ;
  
  
  // -- MC linking
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myMCHitLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default + "2MCHits");
  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  for(unsigned int i = 0; i < m_zone; i++){
    
    const int layer = i/2;
    
    char layerName[100];
    sprintf(layerName, "Layer%i",layer);

    HitRange range = m_ftHitManager->hits(i);
    for( HitRange::const_iterator it = range.begin(); it != range.end(); ++it ){
      PrHit* hit = *it;
      
      const LHCb::MCParticle* mcPartHit = myClusterLink.first( hit->id().ftID() );

      // -- only care about MCHits that belong to long or downstream reconstructible tracks
      const bool isOK  = trackInfo.hasT( mcPartHit ) && (trackInfo.hasTT( mcPartHit ) || trackInfo.hasVelo( mcPartHit ));
      if( m_onlyLongDownForMCHits && !isOK ) continue;

      // -- Get the (first) MCHit matched to the MCParticle
      LHCb::MCHit* mcHit = myMCHitLink.first( hit->id().ftID() );
      

      while(mcHit != nullptr){
        m_histoTool->plot2D(  mcHit->midPoint().X(),mcHit->midPoint().Y(), layerName,
                              "mc hits in layer",-3200,3200,-3200,3200, 6400, 6400);
        mcHit = myMCHitLink.next();
      }

    }
  }
  
}
//=============================================================================
//  Get the PrHit corresponding to the LHCbID
//=============================================================================
const PrHit* PrPlotFTHits::getPrHit(const LHCb::LHCbID id, unsigned int& zone){
  
  PrHit* hit = nullptr;
  bool flag = false;
  
  for (zone = 0; zone < m_zone; ++zone) {
    
    HitRange range = m_ftHitManager->hits(zone);

    if ( msgLevel( MSG::DEBUG ) ) debug() << "zone: " << zone << " with hits: " << range.size() << endmsg;
    
    for (HitRange::const_iterator iHit = range.begin(); range.end() != iHit; ++iHit) {
      
      if ( msgLevel( MSG::DEBUG ) ) debug() << (*iHit)->zone() << endmsg;

      if( id == (*iHit)->id()){
        hit = *iHit;
        flag = true;
      }
      if(flag) break; // one of the few places, a "goto" would not be evil.
    }
    if(flag) break;
  }
  
  return hit;
}
//=============================================================================
//  Get the Track(s) corresponding to the LHCbID
//=============================================================================
std::vector<const LHCb::Track*> PrPlotFTHits::getTrack(const LHCb::LHCbID id, const std::string location){
  
  LHCb::Track::LHCbIDContainer idCont;
  idCont.push_back( id );

  std::vector< const LHCb::Track* > idTracks;
  idTracks.clear();

  const LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>( location );
  if( tracks == nullptr ) return idTracks;

  for( LHCb::Tracks::const_iterator it = tracks->begin(); it != tracks->end(); it++){

    const LHCb::Track* track = *it;

    if( track->containsLhcbIDs( idCont ) ) idTracks.push_back( track );
    
  }

  return idTracks;
  

  
}
//=============================================================================   
//  Get the LiteCluster associated to a LHCbID
//=============================================================================   
LHCb::FTLiteCluster PrPlotFTHits::getLiteCluster(const LHCb::LHCbID id)
{
  LHCb::FTLiteCluster cluster;
  FTLiteClusters* clusters = getIfExists<FTLiteClusters>( LHCb::FTLiteClusterLocation::Default );
  if( clusters == nullptr )
  {
    error() << "Could not find FTLite clusters at: " << LHCb::FTLiteClusterLocation::Default << endmsg;
  }
  for( FTLiteClusters::const_iterator it = clusters->begin(); it!=clusters->end();++it){
    if( (*it).channelID() == id.ftID()){
      cluster = (*it);
      break;
    }
  }
  return cluster;
}





//=============================================================================
//  Get the Cluster corresponding to the LHCbID
//=============================================================================
LHCb::FTCluster* PrPlotFTHits::getCluster(const LHCb::LHCbID id){
  
  if( !id.isFT() ) return NULL;
  
  LHCb::FTCluster* cluster = nullptr;
  LHCb::FTClusters* clusters = getIfExists<LHCb::FTClusters>( LHCb::FTClusterLocation::Default );
  

  if( clusters == nullptr && msgLevel( MSG::ERROR )){
    error() << "Could not find FT clusters at: " << LHCb::FTClusterLocation::Default << endmsg;
  }
  
  for( LHCb::FTClusters::const_iterator it = clusters->begin(); it != clusters->end(); it++){
    
    if( (*it)->channelID() == id.ftID()){
      cluster = (*it);
      break;
    }
  }
  
  return cluster;
  
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrPlotFTHits::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  always()<<"Ciao, for the occupancy plots, normalize the histograms for the amount of events processed"<<endmsg;
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

