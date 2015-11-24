// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "PrClustersResidual.h"
#include "GaudiKernel/AlgFactory.h" 
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
//#include "Event/MCParticle.h"
// local
#include "Event/MCProperty.h"
#include "Linker/LinkedFrom.h"
#include "Event/MCTrackInfo.h"
#include "Linker/AllLinks.h"
//#include "Linker/LinksByKey.h"
#include "Linker/LinkedTo.h"
#include "Event/FTCluster.h"
#include "Kernel/FTChannelID.h"
#include "Event/ODIN.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PrClustersResidual
//
// 2015-01-29 : renato quagliani
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrClustersResidual )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrClustersResidual::PrClustersResidual( const std::string& name,
                                        ISvcLocator* pSvcLocator)
: GaudiTupleAlg ( name , pSvcLocator ),
  m_ftHitManager(nullptr),
  m_zone(24)
{
  declareProperty("MCHitsLocation",m_mcHitLocation = "/Event/MC/FT/Hits");
  declareProperty("HitManagerName",m_hitManagerName = "PrFTHitManager");
  declareProperty("DebugTracking",m_debugTracking = false);
  declareProperty("DoClusterResidual",m_doClusterResidual = false); //Cluster residual tuple
  declareProperty("DoTrackStudy",m_doTrackStudy = false); // Produce tuple for track studies
  declareProperty("DecodeData",m_decodeData = false); //ask to decode data ( is False if other algorithms are runned
  declareProperty("OnlyHasT",m_onlyHasT=false); //(keep only MCParticles with hasT = true
  declareProperty("RemoveClones",m_removeClones = true); //Observed clones MCHits when processed ( not understood )
  declareProperty("DumpAllHits",m_dumpAllHits = false);
  declareProperty("Occupancy",m_Occupancy = true);
  always()<<"***In the Constructor \n"
          <<"m_zone"<<m_zone<<endmsg;
}
//=============================================================================
// Destructor
//=============================================================================
PrClustersResidual::~PrClustersResidual() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrClustersResidual::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_ftHitManager = tool<PrHitManager>(m_hitManagerName);
  always()<<"Initialize ...."<<endmsg
          <<"Loading PrFTHitManager"<<endmsg
          <<"Setting to 0 the counters"<<endmsg;
  m_NClusters=0;
  m_NMCHit=0;
  m_NMCHit_inClusters=0;
  
  if(m_decodeData){
    if(msgLevel(MSG::DEBUG)) debug() <<"Decoding Data"<<endmsg;
    m_ftHitManager->decodeData();
  }
  
  m_nEvents=0;
  setHistoTopDir("ClusterResiduals/");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrClustersResidual::execute() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_nEvents++;
  if(m_doClusterResidual){
    if( msgLevel(MSG::DEBUG) ) debug() << "=====> Execute Cluster Residuals" << endmsg;
    ClusterResidual();
  }
  if(m_doTrackStudy){
    if( msgLevel(MSG::DEBUG) ) debug() << "=====> Execute Track Study " << endmsg;
    TrackStudy();
  }
  if(m_dumpAllHits){
    if( msgLevel(MSG::DEBUG) ) debug()<<"======> Dumping Hits "<<endmsg;
    //Dump all the PrHits
    DumpAllHits();
  }
  if(m_Occupancy)
  {
    Occupancy();
  }
  return StatusCode::SUCCESS;
}

void PrClustersResidual::Occupancy(){
  //m_ghtool->setHistoDir("Clusters/ClusterResidual/Occupancy");
  //DO stuff
  //char layerNameRecoble[100];      
  //sprintf(layerNameRecoble, "hitsInLayerReconstructibleLong%i",layer);          
  //m_histoTool->plot2D( state.x(),state.y(), layerNameRecoble,"Reconstructible hit",-3000,3000.,-3000.,3000.0,100,100);
  //Occupancy dirty ( only 1st layer module 10) : Zone = 0
  std::map<int,int> m_Occupancy;
  // const MCHits* mcHitsNext = 0;
  // const MCHits* mcHitsPrev = 0;
  
  const LHCb::MCHits *mcHitsNext = getIfExists<LHCb::MCHits>("Next/"+LHCb::MCHitLocation::FT);//"/Event/Next/MC/FT/Hits"
  const LHCb::MCHits* mcHitsPrev = getIfExists<LHCb::MCHits>("Prev/"+LHCb::MCHitLocation::FT);//"/Event/Prev/MC/FT/Hits"

  LinkedFrom<LHCb::MCHit,LHCb::MCParticle> myMCHitLink( evtSvc(), msgSvc(), LHCb::MCParticleLocation::Default + "2MC" + "FT" + "Hits" );
  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myFTCluster2MCHitLink ( evtSvc(),msgSvc(), LHCb::FTClusterLocation::Default + "2MCHits");
  //"/Event/Prev/MC/FT/Hits"
  // mcHitsNext = getIfExists<MCHits>("Next/"+LHCb::MCHitLocation::FT);
  // mcHitsPrev = getIfExists<MCHits>("Prev/"+LHCb::MCHitLocation::FT);
  char layerName[100];
  char Title[100];
  std::vector<int> nHits(12,0);
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );
  LHCb::MCVertices* mcVert = getIfExists<LHCb::MCVertices>(LHCb::MCVertexLocation::Default );                        
  if(  mcVert == nullptr ){                                  
    error() << "Could not find MCVertices at " << LHCb::MCParticleLocation::Default << endmsg;
  }
  unsigned int NbPrimaries = 0;
  for ( LHCb::MCVertices::iterator itV = mcVert->begin(); mcVert->end() != itV; ++itV ) 
  {
    if ( (*itV)->isPrimary() )
    {
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
    bool recobleWanted = isRecoble && (mcPart->originVertex()->isPrimary() || mcPart->originVertex()->isDecay());
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
  
  for(unsigned int zoneI = 0; zoneI <24; ++zoneI)
  {
    PrHitZone* zone = m_ftHitManager->zone(zoneI);
    int layer = zoneI/2;
    sprintf(layerName,"Layer%i Occupancy",layer);
    PrHit * hit = nullptr;
    for(PrHits::iterator itH = zone->hits().begin(); zone->hits().end()!=itH;++itH){
      hit = (*itH);
      nHits[layer]++;
      LHCb::FTLiteCluster liteCluster = getLiteCluster(hit->id());
      // int SipmCell = liteCluster.channelID().sipmCell();
      int Module = liteCluster.channelID().module();
      int Quarter = liteCluster.channelID().quarter();
      // int Mat = liteCluster.channelID().mat();
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
      plot1D(index,layerName,Title,0.5,512.5,512);
      bool fromMCParticleContrib = false;
      bool spillCluster = false;
      bool NoiseCluster = false;
      LHCb::MCParticle* mcPart1 = myClusterLink.first( hit->id().ftID());
      if( mcPart1!=nullptr ){
        fromMCParticleContrib=true;
      }
      if( mcPart1==nullptr){
        NoiseCluster = true;
      }
      LHCb::MCHit* mcHit = myFTCluster2MCHitLink.first( hit->id().ftID());
      if(mcHit!=nullptr && (mcHit->parent() == mcHitsNext || mcHit->parent() == mcHitsPrev)){
        spillCluster = true;
      }
      bool RecobleMCParticle = false;
      if(fromMCParticleContrib && trackInfo.hasT(mcPart1)) RecobleMCParticle = true;
      if(RecobleMCParticle){
        sprintf(layerName,"Layer%i/FromRecobleMCParticle",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        plot1D( index,layerName,Title,0.5,512.5,512);
        //0.5,96.5,96);
        if(mcPart->originVertex()->isPrimary() || mcPart->originVertex()->isDecay()){
          sprintf(layerName,"Layer%i/FromRecobleMCParticleWanted",layer);                                                                
          sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);                                                                                      
          plot1D( index,layerName,Title,0.5,512.5,512);                                    
        }
      }
      if(fromMCParticleContrib){
        sprintf(layerName,"Layer%i/FromMCParticle",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        plot1D( index,layerName,Title,0.5,512.5,512);//0.5,96.5,96);
      }
      if(NoiseCluster){
        sprintf(layerName,"Layer%i/FromNoise",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        plot1D(index,layerName,Title,0.5,512.5,512);//0.5,96.5,96);
      }
      if(spillCluster)
      {
        sprintf(layerName,"Layer%i/FromSpill",layer);
        sprintf(Title,"Sipm Index Layer %i;Sipm ID;Clusters/Sipm/Event",layer);
        plot1D(index,layerName,Title,0.5,96.5,96);
      }
    }
  }
}

void PrClustersResidual::DumpAllHits(){
  //ODIN for event
  LHCb::ODIN* odin =getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  Tuples::Tuple tuple = GaudiTupleAlg::nTuple("AllHits","Events");
  std::vector<int> LHCbIDs;
  std::vector<double> Hit_Xat0;
  std::vector<double> Hit_Zat0;
  std::vector<double> Hit_YMin;
  std::vector<double> Hit_YMax;
  std::vector<double> Hit_dxDy;
  std::vector<double> Hit_dzDy;
  std::vector<int> Hit_planeCode;
  std::vector<int> Hit_zone;
  std::vector<bool> Hit_isX;
  std::vector<double> Hit_charge;
  std::vector<double> Hit_size;
  std::vector<double> Hit_w2;
  std::vector<double> Hit_w;
  for(int Zone = 0 ; Zone<24; Zone++){
    PrHitZone* zone = m_ftHitManager->zone(Zone);
    for(PrHits::iterator itH = zone->hits().begin();zone->hits().end()!=itH;++itH){
      PrHit *hit = *itH;
      if(hit==nullptr) continue;
      tuple->column("Event",odin->eventNumber());
      tuple->column("run",odin->runNumber());
      tuple->column("Hit_LHCbID",hit->id().ftID());
      tuple->column("Hit_Xat0",hit->x());
      tuple->column("Hit_Zat0",hit->z());
      tuple->column("Hit_YMin",hit->yMin());
      tuple->column("Hit_YMax",hit->yMax());
      tuple->column("Hit_dxDy",hit->dxDy());
      tuple->column("Hit_dzDy",(hit->z(1000.)-hit->z(0.))/1000);
      tuple->column("Hit_isX",hit->isX());
      tuple->column("Hit_zone",hit->zone());
      tuple->column("Hit_charge",hit->charge());
      tuple->column("Hit_w",hit->w());
      tuple->column("Hit_werr",hit->werr());
      tuple->column("Hit_planeCode",hit->planeCode());
      tuple->column("Hit_size",hit->size());
      tuple->write();
    }
  }
}

void PrClustersResidual::TrackStudy(){
  //ODIN for event
  LHCb::ODIN* odin=getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  //Linker for the MCHit
  LinkedFrom<LHCb::MCHit,LHCb::MCParticle> myMCHitLink( evtSvc(), msgSvc(), LHCb::MCParticleLocation::Default + "2MC" + "FT" + "Hits" );
  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myFTCluster2MCHitLink ( evtSvc(),msgSvc(), LHCb::FTClusterLocation::Default + "2MCHits");
  //Get the linker for the generation of the track to study (the one after the linker)
  //Get the McParticles
  LHCb::MCParticles* mcParts = getIfExists<LHCb::MCParticles> ( LHCb::MCParticleLocation::Default );
  //Get track informations
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );
  //If you Run the cheated Seeding or the normal seeding or the forward , you don't have to decode it again:
 
  debug()<<"Initialize"<<endmsg;
  //----------------
  //-----Find number of Primary vertices for the event
  //---------------
  LHCb::MCVertices* mcVert = getIfExists<LHCb::MCVertices>(LHCb::MCVertexLocation::Default );
  if(  mcVert == nullptr ){
    error() << "Could not find MCVertices at " << LHCb::MCParticleLocation::Default << endmsg;
    return;
  }
  debug()<<"Loading Primary Vertices"<<endmsg;
  unsigned int nPrim = 0; //number of primary vertices
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
  debug()<<"Loading Primary Vertices done"<<endmsg;
  //store the number of primary vertices in tuple
  //-----------
  //For each MCParticle we want to store the Cluster Properties + MCParticle informations. Some Farrays containing the tracks which are reconstructed when we create the track object directly from the Hits which are associated. You need to post study the ntuple with an appropriate MACRO or TSelector.//
  //-----------
  
  //Loop on MCParticle -> 1 entry per MCParticle . ODIN Event number?
  debug()<<"Will loop on all the MCParticle"<<endmsg;
  for( LHCb::MCParticles::const_iterator iPart = mcParts->begin(); iPart != mcParts->end(); ++iPart){
    Tuples::Tuple tuple = GaudiTupleAlg::nTuple("TrackStudy","Events");
    //Running the cheated seeding here
    const LHCb::MCParticle* mcPart = *iPart;
    if(m_onlyHasT && !trackInfo.hasT(mcPart)) continue; //Keep Only tracks for which hasT is true! (IMPORTANT)
    tuple->column("event",(double)odin->eventNumber());
    tuple->column("run",(double)odin->runNumber());
    tuple->column("nPV",(double)nPrim);
    //Get the MCParticle from the iterator
    
    if(mcPart->p()<0.) {
      debug()<<"MCParticle has momentum less than 0"<<endmsg;
      continue;
    }
    if(trackInfo.hasT(mcPart)){
      //Do CheatedSeeding and store track reconstructed
      int totHits =0;
      std::vector<int> firedXLayers(24,0);
      std::vector<int> firedStereoLayers(24,0);
      std::vector<int> firedLayers(24,0);
      std::vector<PrHit*> track; //will contains all the hits having contribution from MCHit pointing to same MCParticle
      track.reserve(100);
      std::vector<LHCb::MCHit*> assoc_MCHit; //associated MCHit pointing to same MCParticle
      assoc_MCHit.reserve(100);
      debug()<<"Will loop on all the layers"<<endmsg;
      //loop on all the PrHit content
      std::vector<int> firedLayers12(12,0);
      for(int iZone = 0; iZone < 24; ++iZone){
        PrHitZone* zone = m_ftHitManager->zone( iZone );
        // -- loop over all hits in a zone
        debug()<<"Will loop on all the Hit on zone    "<<zone<<endmsg;
        for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH){
          PrHit* hit = *itH;
          LHCb::MCParticle* mcPart1 = myClusterLink.first( hit->id().ftID() );
          bool found = false;
          int i = 0;
          while( mcPart1 != nullptr){
            debug()<<"Finding MCParticle associated to Hit->MCHit"<<endmsg;
            if( mcPart1 == mcPart){
              i++;
              found = true;
            }
            mcPart1 = myClusterLink.next();
          }
          if( found){
            firedLayers12[ hit->planeCode()]++;
          }
          if(i>1) debug()<<"The myClusterLinker has problems! 1ftID found more than once in the clusterLink the same MCParticle"<<endmsg; //this happens very often!
          
          //if the hit is in a X layer and we found an association with the MCParticle
          if( hit->isX() && found ){
            if( firedXLayers[iZone] == 0){
              firedXLayers[iZone]++;
            }
          }
          if( !(hit->isX()) && found ){
            if( firedStereoLayers[iZone] == 0){
              firedStereoLayers[iZone]++;
            }
          }
          if( found ){
            debug()<<"Hit Associated Found"<<endmsg;
            firedLayers[iZone]++;
            LHCb::MCHit* mcHit_fromCluster = myFTCluster2MCHitLink.first(hit->id().ftID()); //this linker is generated at the Boole Level.
            while(mcHit_fromCluster!=nullptr){
              if(mcHit_fromCluster->mcParticle()==mcPart){
                assoc_MCHit.push_back(mcHit_fromCluster);
              }
              mcHit_fromCluster = myFTCluster2MCHitLink.next();
            }
            totHits++;
            track.push_back(hit);
            if(assoc_MCHit.size()!=track.size())
              debug()<<"Number of MCHits is not conciding with number of Clusters"<<endmsg;
          }
        }
      } //This does the Job Loading the PrHits!

      
      //MCHits clones business for MCHits on PrHit
      debug()<<"For the processed MCParticle i found  "<<track.size()<<"PrHits   associated to "<<assoc_MCHit.size()<<"   MCHits"<<endmsg;
      tuple->farray("FiredLayers_Counter",firedLayers,"FiredLayers",24);
      tuple->column("CheatedSeeding_NHits",totHits);
      std::vector<double> MCHit_X,MCHit_Y,MCHit_Z;
      std::vector<double> MCHit_tx,MCHit_ty,MCHit_p;
      std::vector<double> MCHit_pathlength;
      std::vector<double> MCHit_time;
      std::vector<double> MCHit_Particle_P;
      std::vector<int> MCHit_Particle_key;
      std::vector<double> MCHit_energy;
      std::vector<double> PrHit_Xat0,PrHit_Zat0,PrHit_dxDy,PrHit_w2,PrHit_yMin,PrHit_yMax,PrHit_dzDy;
      std::vector<bool> PrHit_isX;
      std::vector<int> PrHit_zone, PrHit_planeCode, PrHit_LHCBID;
      
      //ChannelID of track hits
      std::vector<double> ChID_Fraction;
      std::vector<double> ChID_Charge;
      std::vector<double> ChID_SipmCell;
      std::vector<double> ChID_Module;
      std::vector<double> ChID_Layer;
      std::vector<double> ChID_Quarter;
      std::vector<double> ChID_Mat;
      std::vector<double> ChID_SipmID;
      std::vector<double> ChID_ID;
      
      //sort MCHit by Z
      std::stable_sort(assoc_MCHit.begin(),assoc_MCHit.end(),
                [](const LHCb::MCHit* MChit1, const LHCb::MCHit* MChit2){return MChit1->midPoint().z() < MChit2->midPoint().z();  }  );
      //remove Clones of MCHits
      if(m_removeClones){
        assoc_MCHit.erase(std::unique(assoc_MCHit.begin(),assoc_MCHit.end(),[](const LHCb::MCHit* MChit1, const LHCb::MCHit* MChit2){return MChit1->midPoint().z() == MChit2->midPoint().z() ;}),assoc_MCHit.end()); 
      }
      std::sort(track.begin(),track.end(),[](const PrHit* hit1, const PrHit* hit2){return hit1->z()<hit2->z(); } );
      if(m_removeClones){
        track.erase(std::unique(track.begin(),track.end(),[](const PrHit* hit1, const PrHit* hit2){return (hit1->z()==hit2->z() && hit1->x()==hit2->x());}),track.end());
      }
      if(assoc_MCHit.size()!=0){
        if(m_onlyHasT && trackInfo.hasT(mcPart)) m_NMCHit_inClusters+=assoc_MCHit.size();
        if(!m_onlyHasT) m_NMCHit_inClusters+=assoc_MCHit.size();
        if(trackInfo.hasT(mcPart)){
          //some print out?
        }
        for (const LHCb::MCHit* mcHit:assoc_MCHit){
          if(assoc_MCHit.size()!=track.size() &&  trackInfo.hasT( mcPart )){
            //always()<<mcHit->midPoint().X()<<"\t"<<mcHit->midPoint().Y()<<"\t"<<mcHit->midPoint().Z()<<endmsg;
          }
          MCHit_tx.push_back(mcHit->dxdz());
          MCHit_ty.push_back(mcHit->dydz());
          MCHit_p.push_back(mcHit->p());
          MCHit_pathlength.push_back(mcHit->pathLength());
          MCHit_X.push_back(mcHit->midPoint().X());
          MCHit_Y.push_back(mcHit->midPoint().Y());
          MCHit_Z.push_back(mcHit->midPoint().Z());
          MCHit_Particle_P.push_back(mcHit->mcParticle()->p());
          MCHit_time.push_back(mcHit->time());
          MCHit_Particle_key.push_back(mcHit->mcParticle()->key());
          MCHit_energy.push_back(mcHit->energy());
        }
      }
      
      if(track.size()!=0){
        if(m_onlyHasT && trackInfo.hasT(mcPart)){
          m_NClusters+= track.size();
        }
        if(!m_onlyHasT){
          m_NClusters+= track.size();
        }
        if(assoc_MCHit.size()!=track.size() && trackInfo.hasT(mcPart))
          for (const PrHit* hit:track){
            if(assoc_MCHit.size()!=track.size() && trackInfo.hasT(mcPart)){
              debug()<<hit->x()<<"\t \t "<<hit->yMax()<<"\t \t"<<hit->yMin()<<"\t \t"<<hit->z()<<"\t \t"<<hit->size()<<"\t \t"<< hit->id()<<endmsg;
            }
            LHCb::FTLiteCluster liteCluster = getLiteCluster(hit->id());
            ChID_Fraction.push_back(liteCluster.fraction());
            ChID_SipmCell.push_back(liteCluster.channelID().sipmCell());
            ChID_Module.push_back(liteCluster.channelID().module());
            ChID_Layer.push_back(liteCluster.channelID().layer());
            ChID_Quarter.push_back(liteCluster.channelID().quarter());
            ChID_Mat.push_back(liteCluster.channelID().mat());
            ChID_SipmID.push_back(liteCluster.channelID().sipmId());
            ChID_ID.push_back(liteCluster.channelID());
            ChID_Charge.push_back(liteCluster.charge());
            PrHit_LHCBID.push_back(hit->id().ftID());
            PrHit_Xat0.push_back(hit->x());
            PrHit_Zat0.push_back(hit->z());
            PrHit_dxDy.push_back(hit->dxDy());
            PrHit_planeCode.push_back(hit->planeCode());
            PrHit_zone.push_back(hit->zone());
            PrHit_isX.push_back(hit->isX());
            PrHit_yMin.push_back(hit->yMin());
            PrHit_yMax.push_back(hit->yMax());
            PrHit_w2.push_back(hit->w());
            PrHit_dzDy.push_back((hit->z(1000.)-hit->z(0.))/(1000.));
          }
      }
      if(assoc_MCHit.size()==0){
        MCHit_ty.push_back(-999999.);
        MCHit_tx.push_back(-999999.);
        MCHit_p.push_back(-999999.);
        MCHit_Particle_P.push_back(-999999.);
        MCHit_time.push_back(-999999.);
        MCHit_Particle_key.push_back(-999999.);
        MCHit_pathlength.push_back(-999999.);
        MCHit_X.push_back(-999999.);
        MCHit_Y.push_back(-999999.);
        MCHit_Z.push_back(-999999.);
        MCHit_energy.push_back(-999999.);
      }
      if(track.size()==0){
        PrHit_Xat0.push_back(-999999.);
        PrHit_Zat0.push_back(-999999.);
        PrHit_dxDy.push_back(-999999.);
        PrHit_planeCode.push_back(-999999.);
        PrHit_zone.push_back(-999999.);
        PrHit_isX.push_back(-999999.);
        PrHit_yMin.push_back(-999999.);
        PrHit_yMax.push_back(-999999.);
        PrHit_w2.push_back(-999999.);
        PrHit_dzDy.push_back(-999999.);
        PrHit_LHCBID.push_back(-999999.);
      }
      tuple->farray("MCHit_ty",MCHit_ty,"MC_ass",100);
      tuple->farray("MCHit_tx",MCHit_tx,"MC_ass",100);
      tuple->farray("MCHit_p",MCHit_p,"MC_ass",100);
      tuple->farray("MCHit_pathlength",MCHit_pathlength,"MC_ass",100);
      tuple->farray("MCHit_Assoc_X",MCHit_X ,"MC_ass",100);
      tuple->farray("MCHit_Assoc_Y",MCHit_Y ,"MC_ass",100);
      tuple->farray("MCHit_Assoc_Z",MCHit_Z ,"MC_ass",100);
      tuple->farray("MCHit_Assoc_time",MCHit_time,"MC_ass",100);
      tuple->farray("MCHit_Assoc_Particle_Key",MCHit_Particle_key,"MC_ass",100);
      tuple->column("N_MCHit_Assoc",assoc_MCHit.size());
      tuple->farray("PrHit_LHCbID",PrHit_LHCBID,"PrHit",100);
      tuple->farray("PrHit_Xat0",PrHit_Xat0,"PrHit",100);
      tuple->farray("PrHit_Zat0",PrHit_Zat0,"PrHit",100);
      tuple->farray("PrHit_dxDy",PrHit_dxDy,"PrHit",100);
      tuple->farray("PrHit_planeCode",PrHit_planeCode,"PrHit",100);
      tuple->farray("PrHit_zone",PrHit_zone,"PrHit",100);
      tuple->farray("PrHit_isX",PrHit_isX,"PrHit",100);
      tuple->farray("PrHit_yMin",PrHit_yMin,"PrHit",100);
      tuple->farray("PrHit_yMax",PrHit_yMax,"PrHit",100);
      tuple->farray("PrHit_w2",PrHit_w2,"PrHit",100);
      tuple->farray("PrHit_dzDy",PrHit_dzDy,"PrHit",100);

      tuple->farray("ChID_Fraction",ChID_Fraction,"ChID",100);
      tuple->farray("ChID_SipmCell",ChID_SipmCell,"ChID",100); 
      tuple->farray("ChID_Module",ChID_Module,"ChID",100); 
      tuple->farray("ChID_Layer",ChID_Layer,"ChID",100); 
      tuple->farray("ChID_Quarter",ChID_Quarter,"ChID",100); 
      tuple->farray("ChID_Charge",ChID_Charge,"ChID",100);
      tuple->farray("ChID_Mat",ChID_Mat,"ChID",100); 
      tuple->farray("ChID_SipmID",ChID_SipmID,"ChID",100); 
      tuple->farray("ChID_ID",ChID_ID,"ChID",100);

      tuple->column("N_PrHit_Assoc",track.size());
      debug()<<"Tuple for PRHit stored"<<endmsg;
      
      int sumLowerX = 0;
      int sumUpperX = 0;
      int sumStereo = 0;
      int sumStereoUpper =0;
      int sumStereoLower =0;      
      int sumU=0;
      int sumT1=0;
      int sumT2=0;
      int sumT3=0;
      int sumV=0;
      //book OK = false;
            
      
      int sumLowerX_noDoubleCounting=0;
      int sumUpperX_noDoubleCounting=0;
      int sumStereo_noDoubleCounting=0;
      bool T1_Ok=false;
      bool T2_Ok=false;
      bool T3_Ok=false;
      int nT1_X=0;
      int nT1_UV=0;
      int nT2_X=0;
      int nT2_UV=0;
      int nT3_X=0;
      int nT3_UV=0;

      debug()<<"Will loop on X zones in step of 2 (only upper)"<<endmsg;
      //count NLayers with at least one hit
      int nSingleLayers = 0;
      for(int i = 0;i<24;i=i+2){	
        if(firedXLayers[i]!=0)
          sumLowerX_noDoubleCounting++;
        sumLowerX+=firedXLayers[i];
        
      }
      for(int i =0;i<12;i++){
        if( firedLayers12[i] !=0) nSingleLayers++;
      }
      
      for(int i = 1;i<24;i=i+2){
        if(firedXLayers[i]!=0){
          sumUpperX_noDoubleCounting++;
        }
        sumUpperX+=firedXLayers[i];
      }
      for(int i = 0;i<24;i++){
        
        //-------------If i have an hit in the zone i  i increase the counter of 1 unit (prevent multiple counting per layer)
        if(firedXLayers[i]!=0 || firedStereoLayers[i]!=0){
          if(i==0  || i==1  || i==6   || i==7) nT1_X++;
          if(i==8  || i==9  || i==14  || i==15) nT2_X++;
          if(i==16 || i==17 || i== 22 || i==23) nT3_X++;
          if(i==2  || i==3  || i==4   || i==5) nT1_UV++;
          if(i==10 ||i==11  || i==12  || i==13) nT2_UV++;
          if(i==18 || i==19 || i==20  || i==21) nT3_UV++;
        }
        if(firedStereoLayers[i]!=0){
          sumStereo_noDoubleCounting++;
        }
        //------------
        sumStereo+=firedStereoLayers[i];
      }
      T1_Ok = (nT1_X>=1 && nT1_UV>=1);
      T2_Ok = (nT2_X>=1 && nT2_UV>=1);
      T3_Ok = (nT3_X>=1 && nT3_UV>=1);
      int TotalHits_noDoubleCounting = nT1_X + nT2_X + nT3_X + nT1_UV + nT2_UV + nT3_UV;
      bool reconstructible = T1_Ok && T2_Ok && T3_Ok;
      int MinProduct = 20;
      bool Hits_9_4and5 = (sumLowerX_noDoubleCounting * sumStereo_noDoubleCounting>=MinProduct) || (sumUpperX_noDoubleCounting*sumStereo_noDoubleCounting >=MinProduct );
      int MinProduct2 = 18;
      bool Hits_9_6and3 = (sumLowerX_noDoubleCounting * sumStereo_noDoubleCounting>=MinProduct2) || (sumUpperX_noDoubleCounting*sumStereo_noDoubleCounting >=MinProduct2 );
      int MinProduct3 = 24;
      bool Hits_10_6and4 = (sumLowerX_noDoubleCounting * sumStereo_noDoubleCounting>=MinProduct3) || (sumUpperX_noDoubleCounting*sumStereo_noDoubleCounting >=MinProduct3 );
      tuple->column("nCluster_SingleLayer",nSingleLayers);
      tuple->column("nStereo_Upper",sumStereoUpper);
      tuple->column("nStereo_Lower",sumStereoLower);
      tuple->column("nTotal_noMultiple",TotalHits_noDoubleCounting);
      tuple->column("Reconstructible_PrHit",reconstructible);
      tuple->column("Reconstructed_10Hits_6and4",Hits_10_6and4);
      tuple->column("Reconstructed_9Hits_5and4",Hits_9_4and5);
      tuple->column("Reconstructed_9Hits_3and6",Hits_9_6and3);
      tuple->column("LowerX_noMultiple",sumLowerX_noDoubleCounting);
      tuple->column("UpperX_noMultiple",sumUpperX_noDoubleCounting);
      tuple->column("Stereo_noMultiple",sumStereo_noDoubleCounting);
      tuple->column("nT1_X_noMultiple",nT1_X);
      tuple->column("nT1_UV_noMultiple",nT1_UV);
      tuple->column("nT2_X_noMultiple",nT2_X);
      tuple->column("nT2_UV_noMultiple",nT2_UV);
      tuple->column("nT3_X_noMultiple",nT3_X);
      tuple->column("nT3_UV_noMultiple",nT3_UV);
      
      
      tuple->column("nU",sumU);
      tuple->column("nV",sumV);
      tuple->column("nT1",sumT1);
      tuple->column("nT2",sumT2);
      tuple->column("nT3",sumT3);
      tuple->column("nUV",sumStereo);
      tuple->column("nXUp",sumUpperX);
      tuple->column("nxDown",sumLowerX);
      
      //Here MCHit is not binded to be associated also to a cluster , so it's before clustering!!!! Count them per MCParticle
      //Extract the MCHits of mcPart
      std::vector<LHCb::MCHit*> mcHits_MCparticle;
      mcHits_MCparticle.reserve(100);
      LHCb::MCHit* mcHit = myMCHitLink.first(mcPart);
      std::vector<double> MC_X,MC_Y,MC_Z;
      std::vector<double> MC_p;
      std::vector<double> MC_time,MC_dxdz,MC_dydz,MC_Particle_p,MC_pathLenght,MC_energy;
      std::vector<double> MC_Particle_Key;
      debug()<<"Finding all MCHit on MCParticle"<<endmsg;
      while(mcHit!=nullptr){
        if(mcHit->mcParticle()==mcPart){ //check the mcHit we are handling is the one produced by the mcParticle we are looking at
          //Extract the midPoint
          mcHits_MCparticle.push_back(mcHit);
        }
        mcHit=myMCHitLink.next();
      }
      //sorting by Z the MCHits.
      std::stable_sort(mcHits_MCparticle.begin(),mcHits_MCparticle.end() ,[](const LHCb::MCHit* MChit1, const LHCb::MCHit* MChit2){return  MChit1->midPoint().z() < MChit2->midPoint().z();});
      bool hasDuplicate = false;
      int nDifferentLayersMCHits =0;
      for(size_t i = 0; i<mcHits_MCparticle.size(); i++){
        if( i==0 ) nDifferentLayersMCHits++;
        if( i>0 && std::fabs(mcHits_MCparticle[i]->midPoint().z() - mcHits_MCparticle[i-1]->midPoint().z())>30.0)
          nDifferentLayersMCHits++;
        size_t count = 1;
        size_t limit = mcHits_MCparticle.size()-1;
        while(i<limit && mcHits_MCparticle[i]->midPoint().z() == mcHits_MCparticle[i+1]->midPoint().z()){
          count++;
          i++;
        }
        if(count >1) hasDuplicate = true;
        //if(hasDuplicate) break;
      }
      if(hasDuplicate) always()<<"MCHit in MCParticle MCHit linker has duplicates"<<endmsg;
      if(m_removeClones){
        mcHits_MCparticle.erase(std::unique(mcHits_MCparticle.begin(),mcHits_MCparticle.end(),[](const LHCb::MCHit* MChit1, const LHCb::MCHit* MChit2){return MChit1->midPoint().z() == MChit2->midPoint().z();}), mcHits_MCparticle.end());
      }
      if(mcHits_MCparticle.size()!=0){
        if(mcHits_MCparticle.size()>mcHits_MCparticle.size()) 
          debug()<<"More MCHit before gauss than after.....ERROR"<<endmsg;
        if(trackInfo.hasT(mcPart) && m_onlyHasT){
          m_NMCHit+=mcHits_MCparticle.size();
        }
        if(!m_onlyHasT){
          m_NMCHit+=mcHits_MCparticle.size();
        }
        
        
        debug()<<"MCHit pMid \t MCHit pMidY \t MCHit pMidZ \t MCHit Memory \t time \t energy \t P \t pathLength \t dxdz \t dydz"<<endmsg;
        for(LHCb::MCHit* MCHIT:mcHits_MCparticle){
          Gaudi::XYZPoint pMid = MCHIT->midPoint();
          MC_X.push_back(pMid.X());
          MC_Y.push_back(pMid.Y());
          MC_Z.push_back(pMid.Z());
          MC_p.push_back(MCHIT->p());
          MC_time.push_back(MCHIT->time());
          MC_pathLenght.push_back(MCHIT->pathLength());
          MC_Particle_p.push_back(MCHIT->mcParticle()->p());
          MC_dydz.push_back(MCHIT->dydz());
          MC_dxdz.push_back(MCHIT->dxdz());
          MC_energy.push_back(MCHIT->energy());
          MC_Particle_Key.push_back(MCHIT->mcParticle()->key());
          if (trackInfo.hasT(mcPart) && hasDuplicate){
            debug()<<pMid.x()<<"\t"<<pMid.y()<<"\t"<<pMid.z()<<"\t"<<MCHIT<<"\t"<< MCHIT->time()<<"\t"<<MCHIT->energy()<<"\t"<<"\t"<<MCHIT->p()<<"\t"<<MCHIT->pathLength()<<"\t"<<MCHIT->dxdz()<<"\t"<<MCHIT->dydz()<<endmsg;
          }
        }
      }
      if(mcHits_MCparticle.size()==0){
        MC_X.push_back(-9999999.);
        MC_Y.push_back(-9999999.);
        MC_Z.push_back(-9999999.);
        MC_pathLenght.push_back(-999999.);
        MC_p.push_back(-9999999.);
        MC_dxdz.push_back(-9999999.);
        MC_dydz.push_back(-9999999.);
        MC_Particle_p.push_back(-9999999.);
        MC_time.push_back(-9999999.);
        MC_energy.push_back(-9999999.);
      }
      //we have the vector of MCHits of the MCParticle 
      //we need also to have the vector of PrHits_X,Z,XatYMCAssoc
      tuple->column("nMCHits_singleLayer",nDifferentLayersMCHits);
      
      tuple->farray("MC_Hit_X",MC_X.begin(),MC_X.end(),"MC",100);
      tuple->farray("MC_Hit_Y",MC_Y.begin(),MC_Y.end(),"MC",100);
      tuple->farray("MC_Hit_Z",MC_Z.begin(),MC_Z.end(),"MC",100);
      tuple->farray("MC_Hit_P",MC_p.begin(),MC_p.end(),"MC",100);
      tuple->farray("MC_Hit_PathLenght",MC_pathLenght.begin(),MC_pathLenght.end(),"MC",100);
      tuple->farray("MC_Hit_Energy",MC_energy.begin(),MC_energy.end(),"MC",100);
      tuple->farray("MC_Hit_Particle_P",MC_Particle_p.begin(),MC_Particle_p.end(),"MC",100);
      tuple->farray("MC_Hit_dxdz",MC_dxdz.begin(),MC_dxdz.end(),"MC",100);
      tuple->farray("MC_Hit_dydz",MC_dydz.begin(),MC_dydz.end(),"MC",100);
      tuple->farray("MC_time",MC_time.begin(),MC_time.end(),"MC",100);
      tuple->farray("MC_Hit_Particle_Key",MC_Particle_Key.begin(),MC_Particle_Key.end(),"MC",100);
      tuple->column("Number_MCHit_size",MC_X.size());
      tuple->column("MC_Hit_hasDuplicate",hasDuplicate);
      //Compute Property of the mcParticle
      bool fullInfo = trackInfo.fullInfo(mcPart);
      tuple->column("fullInfo",fullInfo);
      bool isSeed = trackInfo.hasT( mcPart );
      tuple->column("isSeed",isSeed);
      bool isLong  = trackInfo.hasVeloAndT( mcPart );
      tuple->column("isLong",isLong);
      bool isDown  = trackInfo.hasT( mcPart ) &&  trackInfo.hasTT( mcPart );
      tuple->column("isDown",isDown);
      bool over5   = 5000. < fabs( mcPart->p() );
      tuple->column("over5",over5);
      bool trigger = 3000. < fabs( mcPart->p() ) &&  500. < fabs( mcPart->pt() );
      tuple->column("trigger",trigger);
      bool isInVelo= trackInfo.hasVelo( mcPart );
      tuple->column("isInVelo",isInVelo);
      bool isInUT  = trackInfo.hasTT( mcPart );
      tuple->column("isInUT",isInUT);
      bool isElectron = std::fabs(mcPart->particleID().pid()==11);
      tuple->column("isElectron",isElectron);
      tuple->column("accT",trackInfo.accT(mcPart));
      tuple->column("accTT",trackInfo.accTT(mcPart));
      bool strangeLong = false;
      bool strangeDown = false;
      bool fromB       = false;
      bool fromD       = false;
      bool OVTXunder100   = mcPart->originVertex()->position().z() < 100.; 
      tuple->column("OVTXunder100mm",OVTXunder100);
      tuple->column("MCParticleID",std::fabs(mcPart->particleID().pid()));
      bool eta25  = (mcPart->momentum().Eta() > 2 && mcPart->momentum().Eta() < 5);
      //double Phi = mcPart->momentum().Phi();
      tuple->column("MC_Phi",mcPart->momentum().Phi());
      tuple->column("MC_Pt",mcPart->pt());
      double pseudoRapidity = mcPart->pseudoRapidity();
      tuple->column("pseudoRapidity",pseudoRapidity);
      tuple->column("Eta_in25",eta25);
      double eta = mcPart->momentum().Eta();
      tuple->column("eta",eta);
      double P = mcPart->p();
      tuple->column("P",P);
      double Pt = std::fabs(mcPart->pt());
      tuple->column("Pt",Pt);
      const LHCb::MCVertex* vertex =0;
      vertex=mcPart->originVertex();
      double OVTX_X_own = vertex->position().x();
      double OVTX_Y_own = vertex->position().y();
      double OVTX_Z_own = vertex->position().z();
      
      tuple->column("MC_px",mcPart->momentum().px());
      tuple->column("MC_py",mcPart->momentum().py());
      tuple->column("MC_pz",mcPart->momentum().pz());
      tuple->column("MC_Ovtx_x",OVTX_X_own);
      tuple->column("MC_Ovtx_y",OVTX_Y_own);
      tuple->column("MC_Ovtx_z",OVTX_Z_own);
      tuple->column("MC_Charge",mcPart->particleID().threeCharge()/3.);
      
      tuple->column("PhysicsInterestVertex",mcPart->originVertex()->isPrimary() || mcPart->originVertex()->isDecay());
         
      //bool isInVelo    = trackInfo.hasVelo(part);
      // bool strangeLong = false ;
      // bool strangeDown = false;
      // bool fromB = false;
      // bool fromD = false;
      //bool isLong = trackInfo.hasVeloAndT( mcPart);
      //bool isDown  = trackInfo.hasT( part ) &&  trackInfo.hasTT( part );
      bool hasT = trackInfo.hasT(mcPart);
      if( 0 != mcPart->originVertex() ){
        const LHCb::MCParticle* mother =  mcPart->originVertex()->mother();
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
                strangeDown = trackInfo.hasT( mcPart ) &&  trackInfo.hasTT( mcPart );
                strangeLong = trackInfo.hasVeloAndT( mcPart );
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
      std::vector<std::pair<std::string,bool> > flags;
      flags.push_back( std::make_pair("hasT",hasT));
      flags.push_back( std::make_pair("Long",isLong));
      flags.push_back( std::make_pair("LongOver5" ,isLong && over5));
      flags.push_back( std::make_pair("Long_fromB",isLong && fromB));
      flags.push_back( std::make_pair("Long_fromBOver5",isLong && fromB && over5));
      flags.push_back( std::make_pair("Down_strange", strangeDown ));
      flags.push_back( std::make_pair("Down_strangeOver5", strangeDown && over5));
      flags.push_back( std::make_pair("Down_strange_noVelo", strangeDown && !isInVelo ));
      flags.push_back( std::make_pair("Down_strange_noVeloOver5",  strangeDown && over5 && !isInVelo));
      flags.push_back( std::make_pair("Down_strange_fromBD",  strangeDown && (fromB || fromD) ));
      flags.push_back( std::make_pair("Down_strange_fromBDOver5", strangeDown && over5 && (fromB || fromD) ) );
      flags.push_back( std::make_pair("Down_strange_noVelo_fromBD",strangeDown && !isInVelo && (fromB || fromD) ));
      flags.push_back( std::make_pair("Down_strange_noVelo_fromDBOver5", strangeDown && !isInVelo && over5 && (fromB || fromD) ));
      bool wanted = trackInfo.hasT(mcPart) && (  isLong ||
                                                 ( fromB && isLong ) ||
                                                 ( strangeDown ) ||
                                                 ( isDown && (fromB || fromD) ) );
      tuple->column("WantedChecker",wanted);
      for( int i = 0;i < (int)flags.size() ;i++){
        tuple->column(flags[i].first,flags[i].second);
      }
      //strange Long
      tuple->column("strange_Long",strangeLong);
      tuple->column("strange_Long_more5",strangeLong && over5);
      //bool isLong = isLong;
      //long>5GeV
      tuple->column("hasT",hasT);
      bool isLong_more5 = isLong && over5;
      tuple->column("isLong_more5",isLong_more5);
      //long_fromB
      bool isLong_fromB =  isLong && fromB;
      tuple->column("fromB",fromB);
      tuple->column("isLong_fromB",isLong_fromB);
      bool isLong_fromB_more5 = isLong && fromB && over5;
      tuple->column("isLong_fromB_more5",isLong_fromB_more5);
      //bool strangedown
      //Strange (UT+T)
      bool strange_UT_T = strangeDown;
      tuple->column("strange_UT_T",strange_UT_T);
      bool strange_UT_T_more5 = strangeDown&& over5;
      tuple->column("strange_UT_T_more5",strange_UT_T_more5);
      //Strange(UT+T !inVelo)
      bool strange_UT_T_noVelo = strangeDown && !isInVelo;
      tuple->column("strange_UT_T_noVelo",strange_UT_T_noVelo);
      bool strange_UT_T_noVelo_more5 = strangeDown && !isInVelo && over5;
      tuple->column("strange_UT_T_noVelo_more5",strange_UT_T_noVelo_more5);
      //Strange from D or B in UT+T
      bool strange_fromDB_UT_T = strangeDown && over5 && (fromB || fromD);
      tuple->column("strange_fromDB_UT_T",strange_fromDB_UT_T);
      bool strange_fromDB_UT_T_more5 = strangeDown && over5 && (fromD || fromB);
      tuple->column("strange_fromDB_UT_T",strange_fromDB_UT_T_more5);
      //Strange from D or B in UT+T not in Velo
      bool strange_fromDB_UT_T_noVelo = strangeDown && (fromD || fromB) && !isInVelo;
      tuple->column("strange_fromDB_UT_T_noVelo",strange_fromDB_UT_T_noVelo);
      bool strange_fromDB_UT_T_noVelo_more5 = strangeDown && (fromD || fromB) && !isInVelo && !over5;
      tuple->column("strange_fromDB_UT_T_noVelo_more5",strange_fromDB_UT_T_noVelo_more5);
      // 	  LHCb::FTLiteCluster liteCluster = getLiteCluster( (*itH)->id());
      // 	  Cluster_Size.push_back(litecluster.size());
      // 	  Cluster_Fraction.push_back(litecluster.fraction());
      // 	  Cluster_SipmCell.push_back(litecluster.channelID().simpCell());
      // 	  Cluster_SipmID.push_back(litecluster.channelID().sipmId());
      // 	  Cluster_Mat.push_back(litecluster.channelID().mat());
      // 	  Cluster_Module.push_back(litecluster.channelID().module());
      // 	  Cluster_Layer.push_back(litecluster.channelID().layer());
      // 	  Cluster_Quarter.push_back(litecluster.channelID().quarter());
      // 	}
      // 	//here just first linker...correct ?
      // 	//find the one closer in y?
      //   }//end loop hit  
      //this comes from Checker
      //end loop on  MCParticles
      tuple->write();
    }
  }
}
void  PrClustersResidual::ClusterResidual(){
  //return StatusCode::SUCCESS;
  //LinkedTo<LHCb::MCParticle, LHCb::Track> myForwardLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Forward);
  //LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  if(msgLevel(MSG::DEBUG)) debug()<<" Get ClusterLinker "<<endmsg;
  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
  if(msgLevel(MSG::DEBUG)) debug()<<" Get Cluster to MCHit Linker "<<endmsg;
                  
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myFTCluster2MCHitLink ( evtSvc(),msgSvc(), LHCb::FTClusterLocation::Default + "2MCHits");
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );
  if(msgLevel(MSG::DEBUG)) debug()<<" Add MCHitAndTrackStudy "<<endmsg;
  Tuples::Tuple tuple = GaudiTupleAlg::nTuple("ClusterMCHitAndTrackStudy","Events");
  for (unsigned int zone = 0; zone < m_zone; ++zone)
  {
    bool isX = false;
    bool isU = false;
    bool isV = false;
    bool isT1 = false;
    bool isT2 = false;
    bool isT3 = false;
    int layer = zone%2;
    //       T1          T2             T3
    //    0,1,6,7    8,9,14,15 ,   16,17,22,23  X-layer
    //    2,3        10,11     ,   18,19        U-layer
    //    4,5        12,13     ,   20,21        V-layer
    //   >0 <8       >=8 <16      >=16 <24
    if(zone==0 || zone == 1 || 
       zone ==6 || zone ==7 || 
       zone ==8 || zone ==9 || 
       zone == 14 || zone == 15 || 
       zone == 16 || zone == 17 || 
       zone == 23 || zone ==22){
      isX = true;
      if(zone==0 || zone ==1 || zone == 6 || zone == 7)
        isT1 = true;
      if(zone==8 || zone ==9 || zone == 14 || zone == 15)
        isT2 = true;
      if(zone==16 || zone ==17 || zone == 22 || zone == 23)
        isT3 = true;
    }
    if(zone == 2 || zone ==3 ||
       zone ==10 || zone==11 || 
       zone == 18|| zone == 19 ){
      isU = true;
      if(zone >0 && zone <8)
        isT1 = true;
      if (zone >5 && zone <16)
        isT2 = true;
      if (zone >15 && zone <24)
        isT3 = true; 
    }
    if( zone == 4 || zone ==5 || 
        zone ==12 || zone==13 ||
        zone == 20 || zone == 21 ){
      isV = true;
      if (zone >0 && zone <8)
        isT1 = true;
      if (zone >5 && zone <16)
        isT2 = true;
      if (zone >15 && zone <24)
        isT3 = true;
    }
    
    //Get all the PrHit
    if(msgLevel(MSG::DEBUG)) debug()<<" Get PrHits "<<endmsg;
                    
    HitRange range = m_ftHitManager->hits(zone);
    //Loop on all the PrHit
    for (HitRange::const_iterator iHit = range.begin(); range.end() != iHit; ++iHit) {
      //Get the MCParticle associated to the Hit, only the first Linker!
      if(msgLevel(MSG::DEBUG)) debug()<<"Getting mcParticle from ChannelID"<<endmsg;
      const LHCb::MCParticle* mcPart1 = myClusterLink.first( (*iHit)->id().ftID() );
      if( mcPart1 == nullptr ) continue; //Considering Only hits linked to first hit in MyClusterLink
      //get the MCHit associated to the Cluster itself
      
      if(msgLevel(MSG::DEBUG)) debug()<<"Getting mcHit from ChannelID"<<endmsg;
      LHCb::MCHit* mcHit = myFTCluster2MCHitLink.first( (*iHit)->id().ftID() ); 
      if(msgLevel(MSG::DEBUG)) debug()<<"Loaded"<<endmsg;
      //took first MCHit associated to Hit channelID
      std::vector<double>  MCHit_X,MCHit_Y,MCHit_Z;
      std::vector<double>  Residual_X,Residual_Z;
      std::vector<double>  SlopeX;
      std::vector<double>  SlopeY;
      std::vector<double>  pathlength;
      std::vector<double>  energy;
      std::vector<bool>    hit_Physics;
      bool isNoise = false;
      Int_t nMCHitToCluster=0;
      if(mcHit==nullptr){
        isNoise = true;
        Residual_X.push_back(-9999.);
        Residual_Z.push_back(-9999.);
        MCHit_X.push_back(-9999.);
        MCHit_Y.push_back(-9999.);
        MCHit_Z.push_back(-9999.);
        SlopeX.push_back(-999.);
        SlopeY.push_back(-999.);
        pathlength.push_back(-999.);
        energy.push_back( -999.);
        hit_Physics.push_back(false);
      }
      while(mcHit != nullptr){
        nMCHitToCluster++;
        Gaudi::XYZPoint pMid = mcHit->midPoint();
        Residual_X.push_back( (*iHit)->x(pMid.y()) - pMid.x()); //x at y - y at z
        Residual_Z.push_back( (*iHit)->z(pMid.z()) - pMid.y());
        SlopeX.push_back( mcHit->dxdz());
        SlopeY.push_back( mcHit->dydz());
        pathlength.push_back( mcHit->pathLength());
        energy.push_back( mcHit->energy());
        MCHit_X.push_back( pMid.x());
        MCHit_Y.push_back( pMid.y());
        MCHit_Z.push_back( pMid.z());
        //mcHit = myFTCluster2MCHitLink.next();
        // const LHCb::MCParticle *mcPart = mcHit->mcParticle();
        const LHCb::MCParticle *mcPart = mcHit->mcParticle();
        if(mcPart == nullptr){
          hit_Physics.push_back(false);
        }
        if(mcPart!= nullptr){
          if(msgLevel(MSG::DEBUG)) debug() <<"Getting Vertex"<<endmsg;
          const LHCb::MCVertex* vertex = mcPart->originVertex();
          if( vertex!=nullptr && (vertex->isPrimary() || vertex->isDecay())){
            hit_Physics.push_back( true );
          }else{
            hit_Physics.push_back( false);
          }
        }
        mcHit = myFTCluster2MCHitLink.next();
      }
      if(msgLevel(MSG::DEBUG)) debug()<<"Storing vectors"<<endmsg;
      tuple->column("isNoiseCluster",isNoise);
      
      tuple->farray("MCHit_Physics", hit_Physics.begin(),hit_Physics.end(),"N_MC",100);
      tuple->farray("CosSlopeX",SlopeX.begin(),SlopeX.end(),"N_MC",100);
      tuple->farray("CosSlopeY",SlopeY.begin(),SlopeY.end(),"N_MC",100);
      tuple->farray("Energy",energy.begin(),energy.end(),"N_MC",100);
      tuple->farray("PathLength",pathlength.begin(),pathlength.end(),"N_MC",100);
      tuple->farray("Residual_X",Residual_X.begin(),Residual_X.end(),"N_MC",100);
      tuple->farray("Residual_Z",Residual_Z.begin(),Residual_Z.end(),"N_MC",100);
      tuple->farray("MCHit_X",MCHit_X.begin(),MCHit_X.end(),"N_MC",100);
      tuple->farray("MCHit_Z",MCHit_Z.begin(),MCHit_Z.end(),"N_MC",100);
      tuple->farray("MCHit_Y",MCHit_Y.begin(),MCHit_Y.end(),"N_MC",100);
      //here fill the tuple with watever you want
      //get Cluster associated to the Hit
      if(msgLevel(MSG::DEBUG)) debug()<<"Getting LiteCluster from PrHit"<<endmsg;
      LHCb::FTLiteCluster litecluster = getLiteCluster( (*iHit)->id());
      tuple->column("ClusterCharge",litecluster.charge());
      tuple->column("ClusterSize",litecluster.size());
      tuple->column("ClusterFraction",litecluster.fraction());
      tuple->column("ClusterChannelID",litecluster.channelID());
      tuple->column("ClusterChannelIDSipmCell",litecluster.channelID().sipmCell());
      tuple->column("ClusterChannelIDSipmID",litecluster.channelID().sipmId());
      tuple->column("ClusterChannelIDMat",litecluster.channelID().mat());
      tuple->column("ClusterChannelIDModule",litecluster.channelID().module());
      tuple->column("ClusterChannelLayer",litecluster.channelID().layer());
      tuple->column("ClusterChannelQuarter",litecluster.channelID().quarter());
      tuple->column("isX",isX);
      tuple->column("isU",isU);
      tuple->column("isV",isV);
      tuple->column("isT1",isT1);
      tuple->column("isT2",isT2);
      tuple->column("isT3",isT3);
      tuple->column("layer",layer);
      // -- As the key of an FTCluster is its channelID, we can link LHCbID and MCParticle directly!
      // -- Caveat: Only take the first link. This might not be fully correct if high precision is needed.
      bool isLong = trackInfo.hasVeloAndT( mcPart1 );
      bool isSeed  = trackInfo.hasT( mcPart1 );
      bool accT = trackInfo.accT(mcPart1);
      bool accTT = trackInfo.accTT(mcPart1);
      //these are the information of the MCParticle!
      if(msgLevel(MSG::DEBUG)) debug()<<" Get Vertex"<<endmsg;
      const LHCb::MCVertex * mcVert = mcPart1->originVertex();
      tuple->column("MCParticle_Physics",mcVert->isDecay() || mcVert->isPrimary());
      tuple->column("MCParticleKey",mcPart1->key());
      tuple->column("MCParticlePhi",mcPart1->momentum().Phi());
      tuple->column("MCParticlePID",mcPart1->particleID().pid());
      tuple->column("MCParticleIsLong",isLong);
      tuple->column("MCParticleIsSeed",isSeed);
      tuple->column("MCParticleP",mcPart1->p());
      tuple->column("MCParticlePt",mcPart1->pt());
      tuple->column("MCParticleGamma",mcPart1->gamma());
      tuple->column("MCParticleBeta",mcPart1->beta());
      tuple->column("MCParticleVirtualMass",mcPart1->virtualMass());
      tuple->column("MCParticlePseudoRapidity",mcPart1->pseudoRapidity());
      tuple->column("MCParticleAccT",accT);
      tuple->column("MCParticleAccTT",accTT);
      tuple->column("zone",zone);
      //MCHitInfos
      tuple->column("PrHit_XatYEq0",(*iHit)->x());
      tuple->column("PrHit_ZatYEq0",(*iHit)->z());
      tuple->column("Hit_dxDy",(*iHit)->dxDy());
      tuple->column("Hit_werr",(*iHit)->werr());
      tuple->column("Hit_w",(*iHit)->w());
      tuple->column("Hit_coord",(*iHit)->coord());
      tuple->column("Hit_isUsed",(*iHit)->isUsed());
      tuple->column("Hit_yMin",(*iHit)->yMin());
      tuple->column("Hit_yMax",(*iHit)->yMax());
      tuple->column("Hit_Zone",(*iHit)->zone());
      tuple->column("Hit_dzDy_manually",((*iHit)->z(1000.)-(*iHit)->z(0.))/1000);
      //Get the list of tracks generated by the Hit i am lookign to 
      if(m_debugTracking)
      {
        LinkedTo<LHCb::MCParticle, LHCb::Track> myForwardLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Forward);
        LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed); 
        
        std::vector<const LHCb::Track*> tracksFwd = getTrack( (*iHit)->id(), LHCb::TrackLocation::Forward );
        std::vector<const LHCb::Track*> tracksSeed = getTrack( (*iHit)->id(), LHCb::TrackLocation::Seed );
        //when assocFwd is true we consider it as reconstructed
        //when assocFwd is false we hit belongs to 
        bool isUsedByFwd = false;
        bool assocFwd = false;
        double Chi2NDofFwd = -10;
        int NDoFFwd = -1;
        
        if( !tracksFwd.empty() ){
          isUsedByFwd = true;
          assocFwd = false;
          // -- If one of the tracks associated to this LHCbID is associated to the same MCParticle 
          // -- as the LHCbID is associated, then it is associated...
          // -- (aka: if LHCbID->MCParticle == LHCbID->Track->MCParticle, then the hit was "efficient")
          for(  const LHCb::Track* track : tracksFwd){
            Chi2NDofFwd = track->chi2PerDoF();
            NDoFFwd = track->nDoF();
            LHCb::MCParticle* mcPart2 = myForwardLink.first( track->key() );
            if( mcPart1 == mcPart2 ){
              assocFwd = true;
              break;
            } 
          }
        }
        double Chi2NDofSeed = -10;
        int NDoFSeed = -1;
        // bool hasT = false;
        // bool isLong = false;
        // bool isElectron = false;
        // double eta = -999.;
        // double phi = -999.;
        // bool over5;
        
        bool isUsedBySeed = false;
        bool assocSeed = false;
        if( !tracksSeed.empty() ){
          isUsedBySeed = true;
          assocSeed = false;
          for(  const LHCb::Track* track : tracksSeed){
            Chi2NDofSeed = track->chi2PerDoF();
            NDoFSeed = track->nDoF();
            //LHCb::MCParticle* mcPart2 = mySeedLink.first( track->key() ); 
            LHCb::MCParticle* mcPart2 = mySeedLink.first( track->key() );
            if( mcPart1 == mcPart2 ){
              assocSeed = true;
              break;
            }
          }
        }
        tuple->column("TrackChi2NDOFSeed",Chi2NDofSeed);
        tuple->column("TrackNDoFSeed",NDoFSeed);
        tuple->column("TrackChi2NDoFFwd",Chi2NDofFwd);
        tuple->column("TrackNDoFFwd",NDoFFwd);
        tuple->column("assocSeed",assocSeed);
        tuple->column("isUsedBySeed",isUsedBySeed);
        tuple->column("assocFwd",assocFwd);
        tuple->column("isUsedByFwd",isUsedByFwd);
      }
      //one can also think about to put here the cheated seeding and compare with the real one.
      tuple->write();
      //mcHit = myFTCluster2MCHitLink.next();  
    }//end loop hits
  }//end loop zones
}



void PrClustersResidual::HitEfficiency(){
  //To be worked on : standard Plots ?
}
      
      
      
      
      
      
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrClustersResidual::finalize() {
  if(m_onlyHasT){
    if(m_removeClones) info()<<"I removed the clones"<<endmsg;
    if(!m_removeClones) info()<<"I didn't removed the clones"<<endmsg;
    info()<<"Only For Tracks hasT"<<endmsg
	  <<" Number of MCHits in Cluster "<<m_NMCHit_inClusters<<endmsg
	  <<" Number of MCHits in MCParticle"<<m_NMCHit<<endmsg
	  <<" Number of Clusters "<<m_NClusters<<endmsg;
  }
  if(!m_onlyHasT){
    if(m_removeClones) info()<<"I removed the clones"<<endmsg;
    if(!m_removeClones) info()<<"I didn't removed the clones"<<endmsg;
     info()<<"For All tracks"<<endmsg
	  <<" Number of MCHits in Cluster "<<m_NMCHit_inClusters<<endmsg
	  <<" Number of MCHits in MCParticle"<<m_NMCHit<<endmsg
	  <<" Number of Clusters "<<m_NClusters<<endmsg;
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}



//=============================================================================
//=============================================================================
//  Get the Cluster corresponding to the LHCbID
//=============================================================================
LHCb::FTLiteCluster PrClustersResidual::getLiteCluster(const LHCb::LHCbID id)
{
  //  if( !id.isFT() ) ; 
  LHCb::FTLiteCluster cluster ;
  //  typedef FastClusterContainer<LHCb::FTLiteCluster,int> FTLiteClusters;
  FTLiteClusters* clusters = getIfExists<FTLiteClusters>( LHCb::FTLiteClusterLocation::Default );
  
  if( clusters == nullptr ){
    error() << "Could not find FTLite clusters at: " << LHCb::FTLiteClusterLocation::Default << endmsg;   
  }
  //loop over Hits
  for( FTLiteClusters::const_iterator it = clusters->begin(); it != clusters->end(); it++)
  {
    if( (*it).channelID() == id.ftID()){
      cluster = (*it);
      break;
    }
  }
  return cluster;
}


//=============================================================================
//  Get the Track(s) corresponding to the LHCbID
//=============================================================================
 std::vector<const LHCb::Track*> PrClustersResidual::getTrack(const LHCb::LHCbID id, const std::string location){
  
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
