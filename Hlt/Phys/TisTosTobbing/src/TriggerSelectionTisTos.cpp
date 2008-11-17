// $Id: TriggerSelectionTisTos.cpp,v 1.12 2008-11-17 16:26:34 tskwarni Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/Plane3DTypes.h"


// local
#include "TriggerSelectionTisTos.h"

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

#include "Kernel/CaloCellCode.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerSelectionTisTos
//
// 2007-08-06 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerSelectionTisTos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerSelectionTisTos::TriggerSelectionTisTos( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITriggerSelectionTisTos>(this);

  declareProperty("HltDecReportsLocation",
                  m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default); 
  declareProperty("HltSelReportsLocation",
                  m_HltSelReportsLocation = LHCb::HltSelReportsLocation::Default);
 
  declareProperty("OfflineMuonTrackLocation",
                  m_MuonTracksLocation = LHCb::TrackLocation::Muon);
 
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

  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    m_offlineInput[hitType].reserve(500);
  }

  m_track2calo = 0;
  
  m_cached_SelectionNames.reserve(500);
  m_cached_decision.reserve(500);
  m_cached_tis.reserve(500);
  m_cached_tos.reserve(500);
  
 
}


//=============================================================================
// Destructor
//=============================================================================
TriggerSelectionTisTos::~TriggerSelectionTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerSelectionTisTos::initialize() {
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
  
  if( m_TOSFrac[kVelo]>1.0 && m_TISFrac[kVelo]<0.0 ){
    info()<< " TOSFracVelo " <<  m_TOSFrac[kVelo] << " TISFracVelo " << m_TISFrac[kVelo] << " thus Velo hits not used " << endmsg;
  }
  if( m_TOSFrac[kTT]>1.0 && m_TISFrac[kTT]<0.0 ){
    info()<< " TOSFracTT   " <<  m_TOSFrac[kTT] <<   " TISFracTT   " << m_TISFrac[kTT] << " thus TT hits not used " << endmsg;
  }
  if( m_TOSFrac[kOTIT]>1.0 && m_TISFrac[kOTIT]<0.0 ){
    info()<< " TOSFracOTIT " <<  m_TOSFrac[kOTIT] << " TISFracOTIT " << m_TISFrac[kOTIT] << " thus OTIT hits not used " << endmsg;
  }
  if( m_TOSFrac[kMuon]>1.0 && m_TISFrac[kMuon]<0.0 ){
    info()<< " TOSFracMuon " <<  m_TOSFrac[kMuon] << " TISFracMuon " << m_TISFrac[kMuon] << " thus Muon hits not used " << endmsg;
  }
  if( m_TOSFrac[kEcal]>1.0 && m_TISFrac[kEcal]<0.0 ){
    info()<< " TOSFracEcal " <<  m_TOSFrac[kEcal] << " TISFracEcal " << m_TISFrac[kEcal] << " thus Ecal hits not used " << endmsg;
  }
  if( m_TOSFrac[kHcal]>1.0 && m_TISFrac[kHcal]<0.0 ){
    info()<< " TOSFracHcal " <<  m_TOSFrac[kHcal] << " TISFracHcal " << m_TISFrac[kHcal] << " thus Hcal hits not used " << endmsg;
  }
 
  return StatusCode::SUCCESS;

}


void TriggerSelectionTisTos::handle(const Incident& ) 
{
  m_hltDecReports=0;
  m_hltSelReports=0;

  m_hcalDeCal = 0;
  m_ecalDeCal = 0;

  m_muonTracks = 0;
  m_muonsOff = false;

  m_hltDecReports = 0;
  m_hltSelReports = 0;

  setOfflineInput( );

  m_newEvent=true;  
  
}


// ------------------------------------------------------------------------------------
void TriggerSelectionTisTos::getHltSummary()
{
  if( !m_hltDecReports ){
    if( exist<HltDecReports>(m_HltDecReportsLocation) ){    
      m_hltDecReports = get<HltDecReports>(m_HltDecReportsLocation);
    } else {
      Error( " No HltDecReports at "+m_HltDecReportsLocation.value(), StatusCode::FAILURE, 20 );
      m_hltDecReports =0;
    }    
  }  
  if( !m_hltSelReports ){
    if( exist<HltSelReports>(m_HltSelReportsLocation) ){    
      m_hltSelReports = get<HltSelReports>(m_HltSelReportsLocation);
    } else {
      Error( " No HltSelReports at "+m_HltSelReportsLocation.value(), StatusCode::FAILURE, 20 );
      m_hltSelReports =0;
    }    
  }
}


// ==================== internal classification of hit type for matching 
// see HitType enum
// =====================================================================
int TriggerSelectionTisTos::hitMatchType(const LHCbID & id)
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
void TriggerSelectionTisTos::matchIDs(const std::vector< LHCb::LHCbID > & ids,
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

	
// ------------------------------------------------------------------------------------
void TriggerSelectionTisTos::hosTISTOS(const LHCb::HltObjectSummary & hos,
                                         const ClassifiedHits offidlist[],
                                         bool& TIS,bool& TOS) const
{ 
  const SmartRefVector< LHCb::HltObjectSummary > & sub = hos.substructure();
  
  if( hos.summarizedObjectCLID() == 1 ){
    // this is summary of trigger selection 
    //     calculate OR among substructure
    TIS=false; TOS=false;
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      bool thisTIS,thisTOS;
      hosTISTOS( *(ihos->target()), offidlist, thisTIS, thisTOS);
      TIS = TIS || thisTIS;
      TOS = TOS || thisTOS;
      if( TIS && TOS )break; // don't waste time if tis && tos already
    }
  } else if( sub.size() ){    
    // this is summary of reco object with substructure
    //     calculate AND among substructure
    TIS=true; TOS=true;
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      bool thisTIS,thisTOS;
      hosTISTOS( *(ihos->target()), offidlist, thisTIS, thisTOS);
      TIS = TIS && thisTIS; 
      TOS = TOS && thisTOS; 
      // don't waste time checking more tracks if not-TIS and not-TOS already
      if( (!TIS) && (!TOS) ){ break; }
    }    
  } else {
    // this is summary of reco object with no-substructure
    // calculate AND among hit types
    double overlap[nHitTypes];
    matchIDs( hos.lhcbIDs(),offidlist,overlap);
    if( m_muonsOff ){ overlap[kMuon]=2.0; }
    TIS=true;TOS=true;
    // effectively .and. requirement between all types
    int nonTrivial=0;
    for( int hitType=0; hitType!=nHitTypes; ++hitType ){
      TOS= overlap[hitType] >= m_TOSFrac[hitType];
      if( !TOS )break;
      if( overlap[hitType]<1.1 )++nonTrivial;  
    }
    TOS = TOS && (nonTrivial>0);    
    // effectively .and. requirement between all types
    for( int hitType=0; hitType!=nHitTypes; ++hitType ){
      TIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
      if( !TIS )break;
    }
  }  
}



//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TriggerSelectionTisTos::setOfflineInput( )
{
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    m_offlineInput[hitType].clear();
  }

  clearCache();

}
   

//    hit list input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist, ClassifiedHits hitidlist[] )
{
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
}

//    calo hit list input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const std::vector<LHCb::CaloCellID> & hitlist, ClassifiedHits hitidlist[] )
{
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
}

//    Track input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::Track & track, ClassifiedHits hitidlist[]) 
{

  //   add hits saved directly on the track ------------------------------------------------------------
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  addToOfflineInput( ids, hitidlist);


    
  // ------------------ add expected Hcal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed 
  if( ( m_TOSFrac[kHcal] > 0.0 ) && ( m_TISFrac[kHcal] < 1.0 ) ){
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
  if( ( m_TOSFrac[kEcal] > 0.0 ) && ( m_TISFrac[kEcal] < 1.0 ) ){
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
  
  

}

//    Proto-particle input -----------------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::ProtoParticle & protoParticle,
                                      ClassifiedHits hitidlist[] ) 
{

  const Track* t=protoParticle.track();


  // Ecal via CaloHypo
  if( ( m_TOSFrac[kEcal] > 0.0 ) && ( m_TISFrac[kEcal] < 1.0 ) ){
    const SmartRefVector< LHCb::CaloHypo > &caloVec = protoParticle.calo();
    if( caloVec.size() > 0 ){
      const LHCb::CaloHypo*   hypo  = *(caloVec.begin());
      if( hypo !=0 ){

        LHCb::CaloCellID centerCell,centerCell1,dummyCell;      
        if( 0!=t ){
          if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ){
            if( (hypo->clusters().begin())->target() ){              
              centerCell  = (*(hypo->clusters().begin()))->seed();
              // info() << " EmCharged " << centerCell << endmsg;          
            }
          }
        } else {
          if( LHCb::CaloHypo::Photon == hypo->hypothesis() ){
            if( (hypo->clusters().begin())->target() ){              
              centerCell  = (*(hypo->clusters().begin()))->seed();
              //    info() << " Photon " << centerCell << endmsg;          
            }            
          } else if (  LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
            // Split Photons
            const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
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
    // add hits from track itself (possibly projected hcal and ecal hits too)
    addToOfflineInput(*t,hitidlist);
    // add muon hits only if needed 
    if( ( m_TOSFrac[kMuon] > 0.0 ) && ( m_TISFrac[kMuon] < 1.0 ) ){
      if( m_muonTracks==0 ){ 
        if( exist<LHCb::Tracks>(m_MuonTracksLocation) ){
          m_muonTracks = get<LHCb::Tracks>(m_MuonTracksLocation);
          m_muonsOff = (m_muonTracks==0);          
        } else {
          m_muonsOff = true;          
          Warning(" No offline muon tracks at "+m_MuonTracksLocation.value()+
                  " thus, muon hits will be ignored on trigger tracks. " , StatusCode::SUCCESS, 10 );
        }
      }
      if( m_muonTracks != 0 ){
        const LHCb::MuonPID* muid = protoParticle.muonPID();
        if( muid!=0 ){
          if( muid->IsMuon() ){
            const Track* mu= m_muonTracks->object(t->key());
            if( mu!=0 ){                                                                   
              const std::vector<LHCbID>& ids = mu->lhcbIDs();
              addToOfflineInput( ids, hitidlist);
            }
          }
        }
      }
    }
  }

}



//    Particle input; for composite particles loop over daughters will be executed ------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::Particle & particle, ClassifiedHits hitidlist[]  )
{
  const SmartRefVector<LHCb::Particle> & daughters = particle.daughters();  
  if( daughters.size() >0 ){
    for(SmartRefVector<LHCb::Particle>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      const LHCb::Particle & part = *(*p);
      addToOfflineInput(part,hitidlist);
    }
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      addToOfflineInput(*pp,hitidlist);
    }
  }
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
void TriggerSelectionTisTos::selectionTisTos( const std::string & selectionName,  
                     					      bool & decision, bool & tis, bool & tos )
{
  if( findInCache( selectionName, decision, tis, tos ) )return;

  decision=false; tis=false; tos=false;
  getHltSummary();

  // get decision from HltDecReports if can find it
  if( m_hltDecReports ){
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );
    if( rep )decision = rep->decision();
  }
  
  if( !m_hltSelReports ){ storeInCache(selectionName,decision,tis,tos); return;}
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel ){ storeInCache(selectionName,decision,tis,tos); return;}
  // presence in HltSelReports indicates that decision was positive
  // it is possible that decision was not stored in HltDecReports if this was an intermediate step, thus
  //   overwrite whatever we got from HltDecReports
  decision=true;

  // classify the decision
  hosTISTOS( *sel, m_offlineInput,tis,tos);
  storeInCache(selectionName,decision,tis,tos);


}

// ------------ multiple selections summary TisTos
void TriggerSelectionTisTos::selectionTisTos( const std::vector< std::string > & selectionNames,
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
      if( tis && tos )break; // don't waste time if tis && tos already
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
      if( !decision )break; // don't waste time if failed trigger already (tis,tos are false too!)      
    }
  } 
}

// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
std::vector<LHCb::LHCbID> TriggerSelectionTisTos::offlineLHCbIDs() 
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

std::vector<const LHCb::HltObjectSummary*> TriggerSelectionTisTos::hltSelectionObjectSummaries( const std::string & selectionName,
                                                                                                unsigned int tisRequirement,
                                                                                                unsigned int tosRequirement)
{
  std::vector<const LHCb::HltObjectSummary*> matchedObjectSummaries;

  getHltSummary();
  if( !m_hltSelReports )return matchedObjectSummaries;
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel )return matchedObjectSummaries;  

  const SmartRefVector< LHCb::HltObjectSummary > & sub = sel->substructure();
  for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
    const LHCb::HltObjectSummary* hos = ihos->target();    
    bool tis,tos;
    hosTISTOS( *hos, m_offlineInput,tis,tos);
    if(    ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
        && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
      matchedObjectSummaries.push_back( hos );
    }    
  }
  return matchedObjectSummaries;
}

