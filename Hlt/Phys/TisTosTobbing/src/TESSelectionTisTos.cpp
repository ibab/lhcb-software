// $Id: TESSelectionTisTos.cpp,v 1.4 2010-06-18 05:29:09 tskwarni Exp $
// Include files 
#include <algorithm>
#include <vector>

// from Gaudi    
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/Plane3DTypes.h"


// local
#include "TESSelectionTisTos.h"

#include "Event/HltDecReports.h"

#include "Kernel/CaloCellCode.h"

#include "HltBase/HltSelection.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TESSelectionTisTos
//
// 2009-12-03 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TESSelectionTisTos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TESSelectionTisTos::TESSelectionTisTos( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_track2calo(0)
  , m_hcalDeCal(0)
  , m_ecalDeCal(0)
  , m_muonTracks(0)
  , m_HLTmuonTracks(0)
  , m_muonsOff(false)
  , m_objectSummaries(0)
{
  declareInterface<ITriggerSelectionTisTos>(this);
  
   declareProperty("PrefixInTES",
                   m_PrefixInTES = "/Event/Strip/Phys/" ); 
   
   // if you ever use this tool for Hlt particles (after Hlt was run) change
   // OfflineMuonTrackLocation to LHCb::TrackLocation::HltMuon i.e. 'Hlt/Track/Muon' 
   declareProperty("OfflineMuonTrackLocation",
                   m_MuonTracksLocation =  LHCb::TrackLocation::Muon); 
   // do not change this one even if you call the tool in offline
   declareProperty("HltOfflineMuonTrackLocation",
                   m_HltMuonTracksLocation =   LHCb::TrackLocation::Muon);
   
   declareProperty("TOSFracVelo", m_TOSFrac[kVelo] = 0.7 );
   declareProperty("TOSFracTT",   m_TOSFrac[kTT]   = 0.7 );
   declareProperty("TOSFracOTIT", m_TOSFrac[kOTIT] = 0.7 );
   declareProperty("TOSFracMuon", m_TOSFrac[kMuon] = 0.7 );
   declareProperty("TOSFracEcal", m_TOSFrac[kEcal] = 0.01); 
   declareProperty("TOSFracHcal", m_TOSFrac[kHcal] = 0.01);
   
   declareProperty("TISFracVelo", m_TISFrac[kVelo] = 0.01 );
   declareProperty("TISFracTT",   m_TISFrac[kTT]   = 0.01 );
   declareProperty("TISFracOTIT", m_TISFrac[kOTIT] = 0.01 );
   declareProperty("TISFracMuon", m_TISFrac[kMuon] = 0.01 );
   declareProperty("TISFracEcal", m_TISFrac[kEcal] = 0.0099 ); 
   declareProperty("TISFracHcal", m_TISFrac[kHcal] = 0.0099 );
   
   declareProperty("NoCaloHypo",m_noCaloHypo = false );  
   
   for( int hitType=0; hitType!=nHitTypes; ++hitType ){
     m_offlineInput[hitType].reserve(500);
   }
   
   m_cached_SelectionNames.reserve(500);
   m_cached_decision.reserve(500);
   m_cached_tis.reserve(500);
   m_cached_tos.reserve(500);   
}


//=============================================================================
// Destructor
//=============================================================================
TESSelectionTisTos::~TESSelectionTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TESSelectionTisTos::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  IIncidentSvc*                incidentSvc(0);     
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent
  bool rethrow = false; bool oneShot = false; long priority = 0;
  incidentSvc->addListener(this,IncidentType::BeginEvent,priority,rethrow,oneShot);
  incidentSvc->release();

  m_track2calo = tool<ITrack2Calo>( "Track2Calo","Track2Calo",this);

  m_newEvent =true;
   
  setOfflineInput();
  
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " TOSFracVelo " <<  m_TOSFrac[kVelo] << " TISFracVelo " << m_TISFrac[kVelo] << endmsg;
    verbose() << " TOSFracTT   " <<  m_TOSFrac[kTT] <<   " TISFracTT   " << m_TISFrac[kTT] << endmsg;    
    verbose() << " TOSFracOTIT " <<  m_TOSFrac[kOTIT] << " TISFracOTIT " << m_TISFrac[kOTIT] << endmsg;    
    verbose() << " TOSFracMuon " <<  m_TOSFrac[kMuon] << " TISFracMuon " << m_TISFrac[kMuon] << endmsg;    
    verbose() << " TOSFracEcal " <<  m_TOSFrac[kEcal] << " TISFracEcal " << m_TISFrac[kEcal] << endmsg;
    verbose() << " TOSFracHcal " <<  m_TOSFrac[kHcal] << " TISFracHcal " << m_TISFrac[kHcal] << endmsg;
  }

  if( m_TOSFrac[kVelo]<=0.0 ){
    info()<< " TOSFracVelo " <<  m_TOSFrac[kVelo] << " TISFracVelo " << m_TISFrac[kVelo] << " thus Velo hits not used " << endmsg;
  }
  if( m_TOSFrac[kTT]<=0.0 ){
    info()<< " TOSFracTT   " <<  m_TOSFrac[kTT] <<   " TISFracTT   " << m_TISFrac[kTT] << " thus TT hits not used " << endmsg;
  }
  if( m_TOSFrac[kOTIT]<=0.0 ){
    info()<< " TOSFracOTIT " <<  m_TOSFrac[kOTIT] << " TISFracOTIT " << m_TISFrac[kOTIT] << " thus OTIT hits not used " << endmsg;
  }
  if( m_TOSFrac[kMuon]<=0.0 ){
    info()<< " TOSFracMuon " <<  m_TOSFrac[kMuon] << " TISFracMuon " << m_TISFrac[kMuon] << " thus Muon hits not used " << endmsg;
  }
  if( m_TOSFrac[kEcal]<=0.0 ){
    info()<< " TOSFracEcal " <<  m_TOSFrac[kEcal] << " TISFracEcal " << m_TISFrac[kEcal] << " thus Ecal hits not used " << endmsg;
  }
  if( m_TOSFrac[kHcal]<=0.0 ){
    info()<< " TOSFracHcal " <<  m_TOSFrac[kHcal] << " TISFracHcal " << m_TISFrac[kHcal] << " thus Hcal hits not used " << endmsg;
  }
 
  if( m_noCaloHypo ){
    warning()<< " NoCaloHypo=True thus no TisTossing of neutral particles built on top of Calo clusters " << endmsg;
  }
 
  return StatusCode::SUCCESS;

}

void TESSelectionTisTos::handle(const Incident& ) 
{

  m_hcalDeCal = 0;
  m_ecalDeCal = 0;

  m_muonTracks = 0;
  m_HLTmuonTracks =0;  
  m_muonsOff = false;

  setOfflineInput( );

  m_newEvent=true;  

  m_objectSummaries =0;
  
}

// ------------------------------------------------------------------------------------
void TESSelectionTisTos::getHltSummary()
{
  // once there is stripping decision report, we will get it from TES here
}

// ==================== internal classification of hit type for matching 
// see HitType enum
// =====================================================================
int TESSelectionTisTos::hitMatchType(const LHCbID & id)
{  
    int hitType=kNotUsed;
    if( id.isVelo() ){
      hitType=kVelo;
    } else if( id.isTT() ){
      hitType=kTT;
    } else if ( id.isOT() || id.isIT() ){
      hitType=kOTIT;
    } else if ( id.isMuon() ) {
      hitType=kMuon;
    } else if ( id.isCalo() ) {
      if( CaloCellCode::CaloNameFromNum( id.caloID().calo() ) == "Hcal" ){
        hitType=kHcal;
      } else if (  CaloCellCode::CaloNameFromNum( id.caloID().calo() ) == "Ecal" ){
        hitType=kEcal;
      }
    }
    return hitType;    
}

//=============================================================================
//    lowest level function for qualifying match between hits on trigger track
//    and list of hits from off-line objects (could be from more than one 
//    off-line track)
//============================================================================
void TESSelectionTisTos::matchIDs(const LHCb::Track & track, 
                         const ClassifiedHits idlist[],
                         double overlap[] ) 
{
 

  int nHitsAll[nHitTypes];
  int nHitsMatched[nHitTypes];  
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    nHitsAll[hitType]=0;
    nHitsMatched[hitType]=0;
    overlap[hitType]=2.0; // default value indicating not present on trigger object
  }
  
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  for( std::vector<LHCbID>::const_iterator id=ids.begin();id!=ids.end();++id){      

    int hitType=hitMatchType(*id);
    if( hitType==kNotUsed ){ continue; }
  
    nHitsAll[hitType] += 1;
    
    for( std::vector<LHCbID>::const_iterator id2=(idlist[hitType]).begin(); id2!=(idlist[hitType]).end(); ++id2){
      if( *id == *id2 ){ 
        nHitsMatched[hitType] += 1;
        break;
      }
    }
  }

  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    if( nHitsAll[hitType]>0 ){
      overlap[hitType] = double(nHitsMatched[hitType])/double(nHitsAll[hitType]);
    }
  }
}


//=============================================================================  
// final state dughters for given particle 
//=============================================================================  
std::vector<const LHCb::Particle*> TESSelectionTisTos::finalStateParticles(const LHCb::Particle* particle) 
{
  std::vector<const LHCb::Particle*> outputParticles;
  if( particle == 0 )return outputParticles;
  std::vector<const LHCb::Particle*> daughters = particle->daughtersVector();  
  if( daughters.size() >0 ){
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      std::vector<const LHCb::Particle*> daughterParticles = finalStateParticles(*p);
      outputParticles.insert(outputParticles.end(),daughterParticles.begin(),daughterParticles.end());
    }
  } else {
    outputParticles.push_back(particle);
  }
  return outputParticles;
}
 
//=============================================================================

//=============================================================================  
// TIS/TOS based on single-track list:   
//=============================================================================  
void TESSelectionTisTos::trackListTISTOS(const std::vector<Track*> & ontracks,
                                const ClassifiedHits offidlist[],
                                bool& TIS,bool& TOS) const
{
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " trackListTISTOS ENTER " << endmsg;

    TIS=false;TOS=false;

    for (std::vector<Track*>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
       if( !(*onit) )continue;      
       const Track& ontrack = *(*onit);
       double overlap[nHitTypes];
       matchIDs(ontrack,offidlist,overlap);
       if( m_muonsOff ){ overlap[kMuon]=2.0; }
       if( !TOS ){
         // effectively .and. requirement between all types
         int nonTrivial=0;         
         for( int hitType=0; hitType!=nHitTypes; ++hitType ){
           // special way of switching sub-detector off (set TOSFrac=0.0) from TOS and TIS !      
           if(  m_TOSFrac[hitType] <=0.0 ) overlap[hitType] = 2.0; 
           TOS= overlap[hitType] >= m_TOSFrac[hitType];
           if( !TOS )break;
           if( overlap[hitType]<1.1 )++nonTrivial;           
         }
         TOS = TOS && (nonTrivial>0);         
       }
       if( !TIS ){ 
         // effectively .and. requirement between all types
         for( int hitType=0; hitType!=nHitTypes; ++hitType ){
           TIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
           if( !TIS )break;
         }
       }
        // don't waste time checking more trigger-tracks if TIS and TOS already
       if( TIS && TOS ){ break; }
    }
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " trackListTISTOS EXIT " << endmsg;
}


//=============================================================================  
// TIS/TOS based on rec vertex list;
//=============================================================================  
void TESSelectionTisTos::vertexListTISTOS(const std::vector<RecVertex*> & onvertices,
                                 const ClassifiedHits offidlist[],
                                 bool& TIS,bool& TOS) const
{
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " vertexListTISTOS ENTER " << endmsg;

    TIS=false;TOS=false;

    for (std::vector<RecVertex*>::const_iterator oniv = onvertices.begin();oniv != onvertices.end(); ++oniv) {
         if( !(*oniv) )continue;      
         RecVertex& onvertex = *(*oniv);
         const SmartRefVector<LHCb::Track> & ontracks=onvertex.tracks();
         bool thisTIS(true),thisTOS(true),nontrivial(false);
         for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
           if( onit->target() ){
             nontrivial = true;             
             const Track& ontrack = *(*onit);
             double overlap[nHitTypes];
             matchIDs(ontrack,offidlist,overlap);
             if( m_muonsOff ){ overlap[kMuon]=2.0; }
             if( thisTOS ){
               // effectively .and. requirement between all types
               int nonTrivial=0;
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                 // special way of switching sub-detector off (set TOSFrac=0.0) from TOS and TIS !      
                 if(  m_TOSFrac[hitType] <=0.0 ) overlap[hitType] = 2.0; 
                  thisTOS= overlap[hitType] >= m_TOSFrac[hitType];
                  if( !thisTOS )break;
                  if( overlap[hitType]<1.1 )++nonTrivial;
               }
               thisTOS = thisTOS && (nonTrivial>0);
             }
             if( thisTIS ){ 
               // effectively .and. requirement between all types
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                  thisTIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
                  if( !thisTIS )break;
               }
             }
             // don't waste time checking more tracks if not-TIS and not-TOS already
             if( (!thisTIS) && (!thisTOS) ){ break; }
           }           
         }
         if( nontrivial ){           
           if( !TOS ){ TOS=thisTOS; }
           if( !TIS ){ TIS=thisTIS; }
         }         
         // don't waste time checking more trigger-vertices if TIS and TOS already
         if( TIS && TOS ){ break; }
    }
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " vertexListTISTOS EXIT " << endmsg;
}

//=============================================================================  
// TIS/TOS based on particle vertex list;
//=============================================================================  
void TESSelectionTisTos::pvertexListTISTOS(const std::vector<Vertex*> & onvertices,
                                 const ClassifiedHits offidlist[],
                                 bool& TIS,bool& TOS)
{
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " pvertexListTISTOS ENTER " << endmsg;

    TIS=false;TOS=false;

    for (std::vector<Vertex*>::const_iterator oniv = onvertices.begin();oniv != onvertices.end(); ++oniv) {
         if( !(*oniv) )continue;      
         Vertex& onvertex = *(*oniv);
         const SmartRefVector<LHCb::Particle> & ontracks=onvertex.outgoingParticles();
         bool thisTIS(true),thisTOS(true),nontrivial(false);
         for (SmartRefVector<LHCb::Particle>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
           if( onit->target() ){
             nontrivial = true;             
             std::vector<LHCb::Particle*> particle;
             particle.push_back( const_cast<Particle*>((const Particle*)(*onit)) );
             particleListTISTOS(particle,offidlist,thisTIS,thisTOS);
             // don't waste time checking more particles if not-TIS and not-TOS already
             if( (!thisTIS) && (!thisTOS) ){ break; }
           }           
         }
         if( nontrivial ){           
           if( !TOS ){ TOS=thisTOS; }
           if( !TIS ){ TIS=thisTIS; }
         }         
         // don't waste time checking more trigger-vertices if TIS and TOS already
         if( TIS && TOS ){ break; }
    }
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " pvertexListTISTOS EXIT " << endmsg;
}



//=============================================================================  
// TIS/TOS based on particle list;
//=============================================================================  
void TESSelectionTisTos::particleListTISTOS(const std::vector<LHCb::Particle*> & onparticles,
                                 const ClassifiedHits offidlist[],
                                 bool& TIS,bool& TOS) 
{
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS ENTER size=" << onparticles.size() << endmsg;
    TIS=false;TOS=false;

    for(std::vector<LHCb::Particle*>::const_iterator onp = onparticles.begin();onp != onparticles.end(); ++onp) {
      const Particle* onparticle = *onp;
      if( !onparticle )continue;      
      std::vector<const LHCb::Particle*> fdaughters= finalStateParticles( onparticle );
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS on-particle fs-size=" << fdaughters.size() << endmsg;
      bool thisTIS(true),thisTOS(true);
      for(std::vector<const LHCb::Particle*>::const_iterator pfp =fdaughters.begin(); pfp!=fdaughters.end(); ++pfp){
        if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs enter " << endmsg;
        const ProtoParticle* pp = (*pfp)->proto();
        if( 0!=pp ){
          Track onlineT;          
          const Track* onit=pp->track();
          if( 0!=onit ){
            onlineT.copy(*onit);
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs copied track " << endmsg;
          }
          
          // Ecal via CaloHypo
          if( ( m_TOSFrac[kEcal] > 0.0 && !m_noCaloHypo  ) ){
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS ECAL VIA CaloHypo " << endmsg;
            const SmartRefVector< LHCb::CaloHypo > &caloVec = pp->calo();
            if( caloVec.size() > 0 ){
              const LHCb::CaloHypo*   hypo  = *(caloVec.begin());
              if( hypo ){                
                LHCb::CaloCellID centerCell,centerCell1,dummyCell;      
                if( 0!=onit ){
                  if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ){
                    if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS EmCharged " << endmsg;
                     if( (hypo->clusters().begin())->target() ){    
                      centerCell  = (*(hypo->clusters().begin()))->seed();
                    }                    
                  }
                } else {
                  if( LHCb::CaloHypo::Photon == hypo->hypothesis() ){
                    if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS Photon " << endmsg;
                    if( (hypo->clusters().begin())->target() ){              
                      centerCell  = (*(hypo->clusters().begin()))->seed();
                    }                    
                  } else if (  LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
                    if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS Pi0Merged " << endmsg;
                    // Split Photons
                    const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
                    if( hypos.size()>1 ){ 
                      const LHCb::CaloHypo* g1 = *(hypos.begin() );
                      if( g1 ){              
                        if( (g1->clusters().begin())->target() )
                          centerCell  = (*(g1->clusters().begin()))->seed();
                      }            
                      const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
                      if( g2 ){
                        if( (g2->clusters().begin())->target() )              
                          centerCell1 = (*(g2->clusters().begin()))->seed();
                      }
                    }                    
                  }        
                }
      
                if( !(centerCell == dummyCell) ){
                  onlineT.addToLhcbIDs(centerCell);
                  if( !(centerCell1 == dummyCell) ){
                    onlineT.addToLhcbIDs(centerCell1);
                  }                  
                }
              }              
            }
          }
          


          // add muon hits only if needed 
          if(  ( m_TOSFrac[kMuon] > 0.0 ) && (0!=onit)  ){
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs trying for muons " << endmsg;
            const LHCb::MuonPID* muid = pp->muonPID();
            if( muid!=0 ){
              if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs muid " << endmsg;
              if( muid->IsMuon() ){
                if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs mu " << endmsg;
                const LHCb::Track*  mu=muid->muonTrack();
                if (!mu ){              
                  if( m_HLTmuonTracks==0 ){ 
                    if( exist<LHCb::Tracks>(m_HltMuonTracksLocation) ){
                      m_HLTmuonTracks = get<LHCb::Tracks>(m_HltMuonTracksLocation);
                    } else {
                      Warning(" No HLT muon tracks at " + m_HltMuonTracksLocation.value() 
                              + " thus, muon hits will be missing on trigger particles. " 
                              , StatusCode::SUCCESS, 10 ).setChecked();
                    }
                  }
                  if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs onit->key() " << onit->key() << endmsg;
                  if( m_HLTmuonTracks != 0 ){
                    mu= m_HLTmuonTracks->object(onit->key());
                  }
                }
                if( mu!=0 ){                                                                   
                  if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs muon hits " << endmsg;
                  const std::vector<LHCbID>& ids = mu->lhcbIDs();
                  for( std::vector<LHCbID>::const_iterator i=ids.begin(); i!=ids.end(); ++i){
                    onlineT.addToLhcbIDs(*i);
                  }
                }
              }
            }
          }

          double overlap[nHitTypes];
          if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs calling matchIDs " << endmsg;
          matchIDs(onlineT,offidlist,overlap);
          if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS fs out of  matchIDs " << endmsg;
          if( m_muonsOff ){ overlap[kMuon]=2.0; }
          if( thisTOS ){
            // effectively .and. requirement between all types
            int nonTrivial=0;
            for( int hitType=0; hitType!=nHitTypes; ++hitType ){
              // special way of switching sub-detector off (set TOSFrac=0.0) from TOS and TIS !      
              if(  m_TOSFrac[hitType] <=0.0 ) overlap[hitType] = 2.0; 
              thisTOS= overlap[hitType] >= m_TOSFrac[hitType];
              if( !thisTOS )break;
              if( overlap[hitType]<1.1 )++nonTrivial;  
            }
            thisTOS = thisTOS && (nonTrivial>0);
          }
          if( thisTIS ){ 
            // effectively .and. requirement between all types
            for( int hitType=0; hitType!=nHitTypes; ++hitType ){
              thisTIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
              if( !thisTIS )break;
            }
          }
          // don't waste time checking more tracks if not-TIS and not-TOS already
          if( (!thisTIS) && (!thisTOS) ){ break; }
        }
      }
      if( !TOS ){ TOS=thisTOS; }
      if( !TIS ){ TIS=thisTIS; }
      // don't waste time checking more trigger-vertices if TIS and TOS already
      if( TIS && TOS ){ break; }
    }
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " particleListTISTOS EXIT " << endmsg;
}

//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TESSelectionTisTos::setOfflineInput( )
{
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    m_offlineInput[hitType].clear();
  }

  clearCache();

}
   

//    hit list input ---------------------------------------------------------------
void TESSelectionTisTos::addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist, ClassifiedHits hitidlist[] )
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with LHCbID list ENTER " << endmsg;

  bool modified(false);
  
  for( std::vector<LHCbID>::const_iterator id=hitlist.begin();id!=hitlist.end();++id){

    int hitType=hitMatchType(*id);
    if( hitType==kNotUsed ){ continue; }

    if( std::find((hitidlist[hitType]).begin(),(hitidlist[hitType]).end(),*id)
        != (hitidlist[hitType]).end() ){ continue;}

    (hitidlist[hitType]).push_back(*id);
    modified = true;
    
  }
  if( modified )clearCache();

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with LHCbID list EXIT " << endmsg;

}

//    calo hit list input ---------------------------------------------------------------
void TESSelectionTisTos::addToOfflineInput( const std::vector<LHCb::CaloCellID> & hitlist, ClassifiedHits hitidlist[] )
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with CaloCellID list ENTER " << endmsg;

  bool modified(false);
  
  for( std::vector<CaloCellID>::const_iterator id=hitlist.begin();id!=hitlist.end();++id){

    int hitType=hitMatchType(*id);
    if( hitType==kNotUsed ){ continue; }

    if( std::find((hitidlist[hitType]).begin(),(hitidlist[hitType]).end(),*id)
        != (hitidlist[hitType]).end() ){ continue;}

    (hitidlist[hitType]).push_back(*id);
    modified = true;
    
  }
  if( modified )clearCache();

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with CaloCellID list EXIT " << endmsg;

}

//    Track input ---------------------------------------------------------------
void TESSelectionTisTos::addToOfflineInput( const LHCb::Track & track, ClassifiedHits hitidlist[]) 
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Track ENTER " << endmsg;


  //   add hits saved directly on the track ------------------------------------------------------------
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  addToOfflineInput( ids, hitidlist);


    
  // ------------------ add expected Hcal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed 
  if(  m_TOSFrac[kHcal] > 0.0 ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Track PROJECT TO HCAL " << endmsg;
    if( m_track2calo != 0 ){
      if( m_hcalDeCal==0 ){ m_hcalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal ); }
      if( m_hcalDeCal != 0 ){

        // do not project through the magnet 
        if( track.closestState( m_hcalDeCal->plane( CaloPlane::ShowerMax ) ).z() > 5000.0 ){
          if( m_track2calo->match( &track, DeCalorimeterLocation::Hcal) ){ 
            if( m_track2calo->isValid() ){ // is matched cellid valid
              LHCb::CaloCellID centerCell  = m_track2calo->caloCellID();
              std::vector<LHCb::CaloCellID> cells3x3;
              cells3x3.push_back(centerCell);
              const std::vector<LHCb::CaloCellID> & neighbors = m_hcalDeCal->neighborCells( centerCell );
              cells3x3.insert( cells3x3.end(), neighbors.begin(),  neighbors.end() );      
              addToOfflineInput( cells3x3, hitidlist );
            }
          }
        }
        
      }
    }
  }
  // ------------------ add expected Ecal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed 
  if( m_TOSFrac[kEcal] > 0.0 ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Track PROJECT TO ECAL " << endmsg;
    if( m_track2calo != 0 ){
      if( m_ecalDeCal==0 ){ m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); }
      if( m_ecalDeCal != 0 ){

        // do not project through the magnet 
        if( track.closestState( m_ecalDeCal->plane( CaloPlane::ShowerMax ) ).z() > 5000.0 ){
          if( m_track2calo->match( &track, DeCalorimeterLocation::Ecal) ){ // play with 3rd parameter?
            if( m_track2calo->isValid() ){ // is matched cellid valid
              LHCb::CaloCellID centerCell  = m_track2calo->caloCellID();
              std::vector<LHCb::CaloCellID> cells3x3;
              cells3x3.push_back(centerCell);
              const std::vector<LHCb::CaloCellID> & neighbors = m_ecalDeCal->neighborCells( centerCell );
              cells3x3.insert( cells3x3.end(), neighbors.begin(),  neighbors.end() );      
              addToOfflineInput( cells3x3, hitidlist );
            }
          }
        }
        
      }
    }
  }
  
   
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Track END " << endmsg;
 

}

//    Proto-particle input -----------------------------------------------------------------------
void TESSelectionTisTos::addToOfflineInput( const LHCb::ProtoParticle & protoParticle,
                                      ClassifiedHits hitidlist[] ) 
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle ENTER " << endmsg;


  const Track* t=protoParticle.track();


  // Ecal via CaloHypo
  if( m_TOSFrac[kEcal] > 0.0 && !m_noCaloHypo ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle ECAL VIA CaloHypo " << endmsg;
    const SmartRefVector< LHCb::CaloHypo > &caloVec = protoParticle.calo();
    if( caloVec.size() > 0 ){
      const LHCb::CaloHypo*   hypo  = *(caloVec.begin());
      if( hypo !=0 ){

        LHCb::CaloCellID centerCell,centerCell1,dummyCell;      
        if( 0!=t ){
          if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ){
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle EmCharged " << endmsg;
            if( (hypo->clusters().begin())->target() ){              
              centerCell  = (*(hypo->clusters().begin()))->seed();
              // info() << " EmCharged " << centerCell << endmsg;          
            }
          }
        } else {
          if( LHCb::CaloHypo::Photon == hypo->hypothesis() ){
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle Photon " << endmsg;
            if( (hypo->clusters().begin())->target() ){              
              centerCell  = (*(hypo->clusters().begin()))->seed();
              //    info() << " Photon " << centerCell << endmsg;          
            }            
          } else if (  LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle Pi0Merged " << endmsg;
            // Split Photons
            const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
            if( hypos.size()>1 ){ 
              const LHCb::CaloHypo* g1 = *(hypos.begin() );
              if( g1 ){              
                if( (g1->clusters().begin())->target() )
                  centerCell  = (*(g1->clusters().begin()))->seed();
              }            
              const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
              if( g2 ){
                if( (g2->clusters().begin())->target() )              
                  centerCell1 = (*(g2->clusters().begin()))->seed();
                // info() << " Pi0Merged " << centerCell << centerCell1 << endmsg;          
              }
            }
          }        
        }
      
      
        if( !(centerCell == dummyCell) ){
        
          if( m_ecalDeCal==0 ){ m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); }
          std::vector<LHCb::CaloCellID> cells3x3;
          cells3x3.push_back(centerCell);
          const std::vector<LHCb::CaloCellID> & neighbors = m_ecalDeCal->neighborCells( centerCell );
          cells3x3.insert( cells3x3.end(), neighbors.begin(),  neighbors.end() );      
          if( !(centerCell1 == dummyCell) ){
            cells3x3.push_back(centerCell1);
            const std::vector<LHCb::CaloCellID> & neighbors = m_ecalDeCal->neighborCells( centerCell1 );
            cells3x3.insert( cells3x3.end(), neighbors.begin(),  neighbors.end() );      
          }
          addToOfflineInput( cells3x3, hitidlist );
        }
      }
    }
  }




 if( 0!=t ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle TRACK " << endmsg;
    // add hits from track itself (possibly projected hcal and ecal hits too)
    addToOfflineInput(*t,hitidlist);
    // add muon hits only if needed 
    if( m_TOSFrac[kMuon]>0.0  ){
      const LHCb::MuonPID* muid = protoParticle.muonPID();
      if( muid!=0 ){
        if( muid->IsMuon() ){
          const LHCb::Track*  mu=muid->muonTrack();
          if (!mu ){              
            if( m_muonTracks==0 ){ 
              if( exist<LHCb::Tracks>(m_MuonTracksLocation) ){
                m_muonTracks = get<LHCb::Tracks>(m_MuonTracksLocation);
                m_muonsOff = (m_muonTracks==0);          
              } else {
                m_muonsOff = true;          
                Warning(" No offline muon tracks at "+m_MuonTracksLocation.value()+
                        " thus, muon hits will be ignored on trigger tracks. " , StatusCode::SUCCESS, 10 ).setChecked();
              }
            }
            if( m_muonTracks != 0 ){
              mu= m_muonTracks->object(t->key());
            }
          }
          if( mu!=0 ){                                                                   
            if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with ProtoParticle TRACK-MUON " << endmsg;
            const std::vector<LHCbID>& ids = mu->lhcbIDs();
            addToOfflineInput( ids, hitidlist);
          }
        }
      }
    }
 }
}




//    Particle input; for composite particles loop over daughters will be executed ------------------------------
void TESSelectionTisTos::addToOfflineInput( const LHCb::Particle & particle, ClassifiedHits hitidlist[]  )
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Particle ENTER " << endmsg;
  const SmartRefVector<LHCb::Particle> & daughters = particle.daughters();  
  if( daughters.size() >0 ){
    for(SmartRefVector<LHCb::Particle>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Particle DAUGHTER " << endmsg;
      if( *p ){
        const LHCb::Particle & part = *(*p);
        addToOfflineInput(part,hitidlist);
      }
    }
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Particle PROTOPARTICLE " << endmsg;
      addToOfflineInput(*pp,hitidlist);
    } else {
        Error("Particle passed as signal has no daughters and ProtoParticle is not accessible; TisTossing is not possible",
              StatusCode::SUCCESS, 10 ).setChecked();
    }    
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToOfflineInput with Particle EXIT " << endmsg;
}

//=============================================================================
// -------------- outputs:
//=============================================================================

  //
  //    decision =  was trigger satisfied ? (this is independent of offline input passed to this tool!)
  //    tis      =  Trigger Independent of Signal  
  //                (trigger selection was satisified independently of the offline input passed to this tool)
  //    tos      =  Trigger On Signal  
  //                (the offline input passed to this tool was sufficient to satisfy this trigger selection)
  //   
  //    comments:
  //         if decision==false then tis=false and tos=false 
  //
  //         tis, tos are not mutually exclusive ( tis==true and tos=true is possible, TOE in TriggerSource terminology)
  //
  //         TOB (Trigger On Both ) =  decision && !tis && !tos  
  //           


// ------------ single selection summary TisTos
void TESSelectionTisTos::selectionTisTos( const std::string & selectionName,  
                     					      bool & decision, bool & tis, bool & tos )
{
  if( findInCache( selectionName, decision, tis, tos ) )return;

  decision=false; tis=false; tos=false;

  std::string path;  
  if( selectionName.find('/') == 0 ){
    path = selectionName;
  } else {
    path = m_PrefixInTES.value() + selectionName;
  }
  
  if( exist<Particles>(path) || exist<Particles>(path+"/Particles") ){
    
    Particles* cParticles;
    if( exist<Particles>(path) ){      
      cParticles =  get<Particles>(path);  
    } else {
      cParticles = get<Particles>(path+"/Particles");
    }    
    std::vector<Particle*> particles;    
    for( Particles::iterator icand =  cParticles->begin(); icand !=  cParticles->end(); icand++ ) {
      particles.push_back(*icand);
    }
    decision = (0!=particles.size());    
    if( decision > 0 ){
      particleListTISTOS( particles,m_offlineInput,tis,tos);        
    }

  } else if( exist<Vertices>(path) || exist<Vertices>(path+"/Vertices")  ){

    Vertices* cVertices;    
    if( exist<Vertices>(path) ){      
      cVertices =  get<Vertices>(path);  
    } else {
      cVertices = get<Vertices>(path+"/Vertices");
    }    
    std::vector<Vertex*> vtxs;    
    for( Vertices::iterator icand =  cVertices->begin(); icand !=  cVertices->end(); icand++ ) {
      vtxs.push_back(*icand);
    } 
    decision = (0!=vtxs.size());    
    if( decision > 0 ){
      pvertexListTISTOS( vtxs,m_offlineInput,tis,tos);        
    }
    
  } else if( exist<RecVertex::Container>(path) ){ 
    
    RecVertex::Container* cVertices =  get<RecVertex::Container>(path);    
    std::vector<RecVertex*> vertices;
    for( RecVertex::Container::iterator icand =  cVertices->begin(); icand !=  cVertices->end(); icand++ ) {
      vertices.push_back(*icand);
    }
    decision = (0!=vertices.size());    
    if( decision ){
      vertexListTISTOS( vertices,m_offlineInput,tis,tos);        
    }
    
  } else if( exist<Tracks>(path) ){
    
    Tracks* cTracks =  get<Tracks>(path);    
    std::vector<Track*> tracks;
    for( Tracks::iterator icand =  cTracks->begin(); icand !=  cTracks->end(); icand++ ) {
      tracks.push_back(*icand);
    }
    decision = (0!=tracks.size());    
    if( decision ){
      trackListTISTOS( tracks,m_offlineInput,tis,tos);        
    }
    
  }

  storeInCache(selectionName,decision,tis,tos);
}

// ------------ multiple selections summary TisTos
void TESSelectionTisTos::selectionTisTos( const std::vector< std::string > & selectionNames,
                                bool & decision, bool & tis, bool & tos ,
                                bool selectionOR)
{
  if( selectionOR ){
    //            OR between various trigger selections
    decision=false; tis=false; tos=false;
    for( std::vector< std::string >::const_iterator pselnam=selectionNames.begin();
         pselnam!=selectionNames.end();
         ++pselnam){
      bool decision_thisSel,tis_thisSel,tos_thisSel;
      selectionTisTos( *pselnam,decision_thisSel,tis_thisSel,tos_thisSel);
      decision = decision || decision_thisSel;
      tis = tis || tis_thisSel;
      tos = tos || tos_thisSel;
      if( tis && tos && decision )break; 
    }
  } else {
    //            AND between various trigger selections
    decision=true; tis=true; tos=true;
    for( std::vector< std::string >::const_iterator pselnam=selectionNames.begin();
         pselnam!=selectionNames.end();
         ++pselnam){
      bool decision_thisSel,tis_thisSel,tos_thisSel;
      selectionTisTos( *pselnam,decision_thisSel,tis_thisSel,tos_thisSel);
      decision = decision && decision_thisSel;
      tis = tis && tis_thisSel;
      tos = tos && tos_thisSel;
      if( !(decision||tis||tos) )break; 
    }
  } 
}

// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
std::vector<LHCb::LHCbID> TESSelectionTisTos::offlineLHCbIDs() 
{
  std::vector<LHCb::LHCbID> flatHitList;
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    if( m_offlineInput[hitType].size()>0 ){
      flatHitList.insert(flatHitList.end(),m_offlineInput[hitType].begin(),m_offlineInput[hitType].end());
    }
  }
  return flatHitList;
}

// ------------  additional functionality:  lists of object summaries for tracks/vertices/particles from trigger selections
//               satisfying TIS, TOS requirements

std::vector<const LHCb::HltObjectSummary*> TESSelectionTisTos::hltSelectionObjectSummaries(
                      const std::string & selectionName,
                      unsigned int tisRequirement,
                      unsigned int tosRequirement)
{
  std::vector<const LHCb::HltObjectSummary*> matchedObjectSummaries;

  if( !m_objectSummaries ){
    if( exist<HltObjectSummary::Container>("/Event/Hlt/TESSelectionTisTosStore") ){
      m_objectSummaries =  get<HltObjectSummary::Container>("/Event/Hlt/TESSelectionTisTosStore");
    } else  {      
      m_objectSummaries = new HltObjectSummary::Container();
      put( m_objectSummaries,"/Event/Hlt/TESSelectionTisTosStore");
    }      
  }  

  std::string path;  
  if( selectionName.find('/') == 0 ){
    path = selectionName;
  } else {
    path = m_PrefixInTES.value() + selectionName;
  }
  
  if( exist<Tracks>(path) ){
    
    Tracks* sel =  get<Tracks>(path);    
    if( !sel )return matchedObjectSummaries;
    if( !(sel->size()) )return matchedObjectSummaries;
    for( Tracks::iterator icand =  sel->begin(); icand !=  sel->end(); icand++ ) {
      Track* object= *icand; 
      std::vector<Track*> tracks; tracks.push_back(object); // vector of size 1      
      bool tis,tos;
      trackListTISTOS(tracks,m_offlineInput,tis,tos);
      if(    ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
             && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
        HltObjectSummary* hos(0);
        for( HltObjectSummarys::iterator ppHos=m_objectSummaries->begin();
             ppHos!=m_objectSummaries->end();++ppHos){
          HltObjectSummary* pHos(*ppHos);    
          if( pHos->summarizedObjectCLID() == object->clID() ){
            if( pHos->summarizedObject() == object ){
              hos = pHos;
            }
          }
        }
        if( !hos ){          
          // do not create full object summary; just a pointer back to the original object
          hos = new HltObjectSummary();
          m_objectSummaries->push_back(hos);
          hos->setSummarizedObjectCLID( object->clID() );
          hos->setSummarizedObject(object);
          hos->setLhcbIDs( object->lhcbIDs() );
        }        
        matchedObjectSummaries.push_back( hos );
      }
    }

  } else if( exist<RecVertex::Container>(path) ){

    RecVertex::Container* sel =  get<RecVertex::Container>(path);    
    if( !sel )return matchedObjectSummaries;
    if( !(sel->size()) )return matchedObjectSummaries;
    for( RecVertex::Container::iterator icand =  sel->begin(); icand !=  sel->end(); icand++ ) {
      RecVertex* object= *icand; 
      std::vector<RecVertex*> vertices; vertices.push_back(object); // vector of size 1      
      bool tis,tos;
      vertexListTISTOS(vertices,m_offlineInput,tis,tos);
      if(    ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
             && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
        HltObjectSummary* hos(0);
        for( HltObjectSummarys::iterator ppHos=m_objectSummaries->begin();
             ppHos!=m_objectSummaries->end();++ppHos){
          HltObjectSummary* pHos(*ppHos);    
          if( pHos->summarizedObjectCLID() == object->clID() ){
            if( pHos->summarizedObject() == object ){
              hos = pHos;
            }
          }
        }
        if( !hos ){          
          // do not create full object summary; just a pointer back to the original object
          hos = new HltObjectSummary();
          m_objectSummaries->push_back(hos);
          hos->setSummarizedObjectCLID( object->clID() );
          hos->setSummarizedObject(object);
        }        
        matchedObjectSummaries.push_back( hos );
      }
    }


  } else if( exist<Particles>(path) ||  exist<Particles>(path+"/Particles") ){
    
    Particles* sel;
    if( exist<Particles>(path) ){      
      sel =  get<Particles>(path);
    } else {
      sel =  get<Particles>(path+"/Particles" );
    }    
    if( !sel )return matchedObjectSummaries;
    if( !(sel->size()) )return matchedObjectSummaries;
    for( Particles::iterator icand =  sel->begin(); icand !=  sel->end(); icand++ ) {
      Particle* object= *icand; 
      std::vector<Particle*> particles; particles.push_back(object); // vector of size 1      
      bool tis,tos;
      particleListTISTOS(particles,m_offlineInput,tis,tos);
      if(    ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
             && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
        HltObjectSummary* hos(0);
        for( HltObjectSummarys::iterator ppHos=m_objectSummaries->begin();
             ppHos!=m_objectSummaries->end();++ppHos){
          HltObjectSummary* pHos(*ppHos);    
          if( pHos->summarizedObjectCLID() == object->clID() ){
            if( pHos->summarizedObject() == object ){
              hos = pHos;
            }
          }
        }
        if( !hos ){          
          // do not create full object summary; just a pointer back to the original object
          hos = new HltObjectSummary();
          m_objectSummaries->push_back(hos);
          hos->setSummarizedObjectCLID( object->clID() );
          hos->setSummarizedObject(object);
        }        
        matchedObjectSummaries.push_back( hos );
      }
    }
    
  } else if( exist<Vertices>(path) ||  exist<Vertices>(path+"/Vertices") ){
    
    Vertices* sel;
    if( exist<Vertices>(path) ){      
      sel =  get<Vertices>(path);
    } else {
      sel =  get<Vertices>(path+"/Vertices" );
    }    
    if( !sel )return matchedObjectSummaries;
    if( !(sel->size()) )return matchedObjectSummaries;
    for( Vertices::iterator icand =  sel->begin(); icand !=  sel->end(); icand++ ) {
      Vertex* object= *icand; 
      std::vector<Vertex*> vtxs; vtxs.push_back(object); // vector of size 1      
      bool tis,tos;
      pvertexListTISTOS(vtxs,m_offlineInput,tis,tos);
      if(    ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
             && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
        HltObjectSummary* hos(0);
        for( HltObjectSummarys::iterator ppHos=m_objectSummaries->begin();
             ppHos!=m_objectSummaries->end();++ppHos){
          HltObjectSummary* pHos(*ppHos);    
          if( pHos->summarizedObjectCLID() == object->clID() ){
            if( pHos->summarizedObject() == object ){
              hos = pHos;
            }
          }
        }
        if( !hos ){          
          // do not create full object summary; just a pointer back to the original object
          hos = new HltObjectSummary();
          m_objectSummaries->push_back(hos);
          hos->setSummarizedObjectCLID( object->clID() );
          hos->setSummarizedObject(object);
        }        
        matchedObjectSummaries.push_back( hos );
      }
    }
    
  }

  
  return matchedObjectSummaries;
}
