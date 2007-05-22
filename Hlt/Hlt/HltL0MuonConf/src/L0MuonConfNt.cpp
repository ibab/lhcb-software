// $Id: L0MuonConfNt.cpp,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/L0MuonCandidate.h"

//#include "Event/L0MuonCandidate.h"
#include "Event/MCParticle.h"
#include "Event/MuonCoord.h"
#include "Event/MuonDigit.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Event/L0DUReport.h"
#include "Event/RecHeader.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// local
#include "L0MuonConfNt.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonConfNt
//
// 2007-02-16 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonConfNt );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonConfNt::L0MuonConfNt( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputRZTracksName" ,
                   m_inputRZTracksName  ="");
  declareProperty( "InputRZSelectedTracksName" ,
                   m_inputRZSelectedTracksName  ="");
  declareProperty( "InputTTracksName" ,
                   m_inputTTracksName  ="");
  declareProperty( "InputATTracksName" ,
                   m_inputATTracksName  ="");
  declareProperty( "InputSpaceTracksName" ,
                   m_inputSpaceTracksName  ="");
  declareProperty( "InputSpaceSelectedTracksName" ,
                   m_inputSpaceSelectedTracksName  ="");
  declareProperty( "InputMuonTracksName" ,
                   m_inputMuonTracksName  =
                   "");

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonConfNt::~L0MuonConfNt() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonConfNt::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;



  m_iPosTool  = tool<IMuonPosTool>( "MuonPosTool" );
  if(!m_iPosTool)info()<<"error retrieving the pos tool "<<endreq;
 

  m_muonTracksContainer = m_patDataStore->tracks(m_inputMuonTracksName);


  //   m_muonPad2MC=tool<IMuonPad2MCTool>("MuonPad2MCTool");

  m_tracksRZ = m_patDataStore->tracks(m_inputRZTracksName);  
  m_tracksRZSelected = &(m_hltDataStore->tracks(m_inputRZSelectedTracksName));
  m_tracksT = &(m_hltDataStore->tracks(m_inputTTracksName));
  m_tracksAT = &(m_hltDataStore->tracks(m_inputATTracksName));
  //  m_tracksRZ = m_patDataStore->tracks("pippo");

  m_tracksSpace = &(m_hltDataStore->tracks(m_inputSpaceTracksName));  
  m_tracksSpaceSelected = &(m_hltDataStore->tracks(m_inputSpaceSelectedTracksName));  
  NTuplePtr nt(ntupleSvc(), "FILE1/Trigger");
  info()<<" booking the ntuple "<<endreq; 
  StatusCode status;
  if ( !nt )  {
    nt = ntupleSvc()->book ("FILE1/Trigger",    
                            CLID_ColumnWiseTuple,"TrgMuonRecHLT");
    if ( nt )    {
      status = nt->addItem ("RZtracks", m_RZtracks,0,500);
      status = nt->addIndexedItem ("slopeR",m_RZtracks,m_RZslopeR);
      status = nt->addIndexedItem ("slopephi",m_RZtracks,m_RZslopephi);
      status = nt->addIndexedItem ("RZR",m_RZtracks,m_RZpointR);
      status = nt->addIndexedItem ("RZZ",m_RZtracks,m_RZpointZ);
      status = nt->addIndexedItem ("RZmdecay",m_RZtracks,m_RZmid);
      status = nt->addIndexedItem ("RZtruep",m_RZtracks,m_RZtruep);
      status = nt->addIndexedItem ("RZtruepid",m_RZtracks,m_RZtruepid);
      //status = nt->addIndexedItem ("RZreco",m_RZtracks,m_RZreco);
      status = nt->addIndexedItem ("RZselected",m_RZtracks,m_RZselected);
      status = nt->addIndexedItem ("RZwhosel",m_RZtracks,m_RZwhoselected);

      status = nt->addItem ("tracks", m_tracks,0,50);
      status = nt->addIndexedItem ("RZIndex",m_tracks,m_RZIndex);
      status = nt->addIndexedItem ("momentum",m_tracks,m_momentum);
      status = nt->addIndexedItem ("slopeX",m_tracks,m_slopeX);   
      status = nt->addIndexedItem ("slopeY",m_tracks,m_slopeY);
      status = nt->addIndexedItem ("X",m_tracks,m_Xpoint);
      status = nt->addIndexedItem ("Y",m_tracks,m_Ypoint);
      status = nt->addIndexedItem ("Z",m_tracks,m_Zpoint);
      status = nt->addIndexedItem ("pid",m_tracks,m_pid);
      status = nt->addIndexedItem ("mdecay",m_tracks,m_mid);
      status = nt->addIndexedItem ("truep",m_tracks,m_truep);
      status = nt->addIndexedItem ("truepid",m_tracks,m_truepid);
      status = nt->addIndexedItem ("reco",m_tracks,m_reco);
      status = nt->addIndexedItem ("spacesele",m_tracks,m_3dselected);
      status = nt->addIndexedItem ("TIndex",m_tracks,m_TIndex);
      status = nt->addIndexedItem ("ATIndex",m_tracks,m_ATIndex);

      status = nt->addItem ("L0MuonCan", m_muonL0,0,10);
      status = nt->addIndexedItem ("L0MuonPT",m_muonL0,m_l0muonPT); 
      status = nt->addIndexedItem ("L0MuonPTtrue",m_muonL0,m_l0muonPTtrue); 
      status = nt->addIndexedItem ("L0MuonTP",m_muonL0,m_l0muonPtrue); 
      status = nt->addIndexedItem ("L0MuonPid",m_muonL0,m_l0muonPid);   
      status = nt->addIndexedItem ("L0MuonMid",m_muonL0,m_l0muonMid);  
      status = nt->addIndexedItem ("L0MuonMatch",m_muonL0,m_l0muonMatch);
      status = nt->addIndexedItem ("L0MuonRZMatch",m_muonL0,m_l0muonRZMatch);
      status = nt->addIndexedItem ("L0MuonZdecay",m_muonL0,m_l0muonZdecay);

      status = nt->addIndexedItem ("L0MuonKey1",m_muonL0,m_l0muonKey1);
                                   
      status = nt->addIndexedItem ("L0MuonKey2",m_muonL0,m_l0muonKey2);
      status = nt->addIndexedItem ("L0MuonKey3",m_muonL0,m_l0muonKey3);
      status = nt->addIndexedItem ("L0MuonPid1",m_muonL0,m_l0muonPid1);
      status = nt->addIndexedItem ("L0MuonPid2",m_muonL0,m_l0muonPid2);
      status = nt->addIndexedItem ("L0MuonPid3",m_muonL0,m_l0muonPid3);
      status = nt->addIndexedItem ("L0MuonMid1",m_muonL0,m_l0muonMid1);
      status = nt->addIndexedItem ("L0MuonMid2",m_muonL0,m_l0muonMid2);
      status = nt->addIndexedItem ("L0MuonMid3",m_muonL0,m_l0muonMid3);
      status = nt->addIndexedItem ("L0MuonX1",m_muonL0,m_l0muonx1);
      status = nt->addIndexedItem ("L0MuonX2",m_muonL0,m_l0muonx2);
      status = nt->addIndexedItem ("L0MuonX3",m_muonL0,m_l0muonx3);
      //status = nt->addIndexedItem ("L0MuonX4",m_muonL0,m_l0muonx4);
      //status = nt->addIndexedItem ("L0MuonX5",m_muonL0,m_l0muonx5);
      status = nt->addIndexedItem ("L0MuonY1",m_muonL0,m_l0muony1);
      status = nt->addIndexedItem ("L0MuonY2",m_muonL0,m_l0muony2); 
      status = nt->addIndexedItem ("L0MuonY3",m_muonL0,m_l0muony3);  
      //status = nt->addIndexedItem ("L0MuonY4",m_muonL0,m_l0muony4);
      //status = nt->addIndexedItem ("L0MuonY5",m_muonL0,m_l0muony5);
      status = nt->addIndexedItem ("L0MuonReg1",m_muonL0,m_l0muonr1);  
      status = nt->addIndexedItem ("L0MuonReg2",m_muonL0,m_l0muonr2);
      status = nt->addIndexedItem ("L0MuonReg3",m_muonL0,m_l0muonr3);  
      // status = nt->addIndexedItem ("L0MuonReg4",m_muonL0,m_l0muonr4);  
      //status = nt->addIndexedItem ("L0MuonReg5",m_muonL0,m_l0muonr5);  

      status = nt->addItem ("Ttracks", m_Ttracks,0,500);
      //      status = nt->addIndexedItem ("TRZIndex",m_Ttracks,m_TRZIndex);
      status = nt->addIndexedItem ("TL0Index",m_Ttracks,m_TL0Index);
      status = nt->addIndexedItem ("Tmomentum",m_Ttracks,m_Tmomentum);  
      status = nt->addIndexedItem ("Tpt",m_Ttracks,m_Tpt);
      status = nt->addIndexedItem ("Tchi2",m_Ttracks,m_Tchi2);
      status = nt->addIndexedItem ("Tdll",m_Ttracks,m_Tdll);
      status = nt->addIndexedItem ("TslopeX",m_Ttracks,m_TslopeX);
      status = nt->addIndexedItem ("TslopeY",m_Ttracks,m_TslopeY);
      status = nt->addIndexedItem ("TX",m_Ttracks,m_TXpoint);
      status = nt->addIndexedItem ("TY",m_Ttracks,m_TYpoint);
      status = nt->addIndexedItem ("TZ",m_Ttracks,m_TZpoint);
      status = nt->addIndexedItem ("Tpid",m_Ttracks,m_Tpid);
      status = nt->addIndexedItem ("Tmdecay",m_Ttracks,m_Tmid);
      status = nt->addIndexedItem ("Ttruep",m_Ttracks,m_Ttruep);
      status = nt->addIndexedItem ("TRZMatch",m_Ttracks,m_TRZMatch);
      //   status = nt->addIndexedItem ("truepid",m_tracks,m_truepid);
      //status = nt->addIndexedItem ("reco",m_tracks,m_reco);

      status = nt->addItem ("ATtracks", m_ATtracks,0,500);
      //      status = nt->addIndexedItem ("TRZIndex",m_Ttracks,m_TARZIndex);
      status = nt->addIndexedItem ("ATL0Index",m_ATtracks,m_ATL0Index); 
      status = nt->addIndexedItem ("ATmomentum",m_ATtracks,m_ATmomentum);
      status = nt->addIndexedItem ("ATpt",m_ATtracks,m_ATpt);
      status = nt->addIndexedItem ("ATchi2",m_ATtracks,m_ATchi2);
      status = nt->addIndexedItem ("ATdll",m_ATtracks,m_ATdll);
      status = nt->addIndexedItem ("ATdllstd",m_ATtracks,m_ATdllstd);
      status = nt->addIndexedItem ("ATslopeX",m_ATtracks,m_ATslopeX);   
      status = nt->addIndexedItem ("ATslopeY",m_ATtracks,m_ATslopeY);   
      status = nt->addIndexedItem ("ATX",m_ATtracks,m_ATXpoint);
      status = nt->addIndexedItem ("ATY",m_ATtracks,m_ATYpoint);
      status = nt->addIndexedItem ("ATZ",m_ATtracks,m_ATZpoint);
      status = nt->addIndexedItem ("ATpid",m_ATtracks,m_ATpid);
      status = nt->addIndexedItem ("ATmdecay",m_ATtracks,m_ATmid);
      status = nt->addIndexedItem ("ATtruep",m_ATtracks,m_ATtruep);
      status = nt->addIndexedItem ("ATRZMatch",m_ATtracks,m_ATRZMatch);
      //   status = nt->addIndexedItem ("truepid",m_tracks,m_truepid);
      //status = nt->addIndexedItem ("reco",m_tracks,m_reco);
  
      status = nt->addItem ("MuonTracks", m_muonTracks,0,50);
      status = nt->addIndexedItem ("MuonPid",m_muonTracks,m_muonPid);
      status = nt->addIndexedItem ("MuonMid",m_muonTracks,m_muonMid);
      status = nt->addIndexedItem ("MuonMatch",m_muonTracks,m_muonMatch);
      status = nt->addIndexedItem ("MuonRZMatch",m_muonTracks,m_muonRZMatch);
      status = nt->addIndexedItem ("MuonSlopeX",m_muonTracks,m_muonSlopeX);
      status = nt->addIndexedItem ("MuonSlopeY",m_muonTracks,m_muonSlopeY);
      status = nt->addIndexedItem ("MuonX",m_muonTracks,m_muonXpoint);
      status = nt->addIndexedItem ("MuonY",m_muonTracks,m_muonYpoint); 
      //     status = nt->addIndexedItem ("MuonDecayPoint",m_muonTracks,
      //                             m_muonDecayPoint);
      status = nt->addIndexedItem ("MuonTP",m_muonTracks,m_muonTrueP);
      status = nt->addIndexedItem ("MuonSkip",m_muonTracks,m_muonSkip);
      
 

    }
    m_Track=nt  ;
      
  }
  info()<<"end execution<"<<endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonConfNt::execute() {

  debug() << "==> Execute" << endmsg;
  HltAlgorithm::beginExecute();

  LHCb::RecHeader* header = NULL;
  if ( exist<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default)){
    header = get<LHCb::RecHeader>( LHCb::RecHeaderLocation::Default );
    info() << "Run " << header->runNumber()
            << ", Event " << header->evtNumber() << endreq;
  } else {
     warning() << "No RcHeader" << endreq;
  } 




  L0DUReport* rprtL0 = get<L0DUReport>(L0DUReportLocation::Default);
  
      
//  m_L0=0;
  if(!rprtL0->decision()) {  
    m_Track->write();
    return StatusCode::SUCCESS;
  }




  if((!rprtL0->channelDecision( 11))&&(!rprtL0->channelDecision( 13))){
    m_Track->write();
    return StatusCode::SUCCESS;
  }
   
  
  //RZ tracks
  
   info()<<" qui "<<endreq;
    debug()<<" tracce "<<m_tracksRZ->size()<<endreq;
  Tracks::const_iterator pIRZ;  
  Tracks::const_iterator pRZSele;
  info()<<" ciao"<<endreq;
  LinkedTo< MCParticle, Track>
    myLinkRZ( eventSvc(), msgSvc(),m_inputRZTracksName );
  
  
  for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
    Track* thisTrack = (*pIRZ);
    if(thisTrack->checkFlag( Track::Invalid ))continue;
    if(thisTrack->checkFlag( Track::Backward ))continue;
    if(m_RZtracks<m_RZtracks->range().distance()){
      State* stato=NULL;
      stato=&(thisTrack->firstState());
      m_RZslopeR[m_RZtracks]=stato->tx();
      int zone = thisTrack->specific();
      double trackPhi  = (zone * M_PI/4.) - 3.*M_PI/8.;
      //double sectorSize= 45.*(M_PI/180.);
      m_RZslopephi[m_RZtracks]=trackPhi;
      m_RZpointR[m_RZtracks]=stato->x();
      m_RZpointZ[m_RZtracks]=stato->z();  
      MCParticle* pp=NULL;
      pp = myLinkRZ.first(thisTrack->key());
      int pid=0;
      int mid=0;
      float truep=0;
      if(pp!=NULL){
        pid=pp->particleID().pid();
        const MCVertex *origin= pp->originVertex();
        truep=sqrt(pow(pp->momentum().z(),2)+pow(pp->momentum().y(),2)+
                   pow(pp->momentum().x(),2));
        if(origin){
          const  MCParticle* mother=origin->mother();
          if(mother)mid=mother->particleID().pid();
        }
        //  if(abs(pid)==13)m_RZreco[m_RZtracks]=reconstructableMuon(pp);
      }
     
      bool selected=false;
      int whosel=0;
      
      info()<<" tracce sele "<<m_tracksRZSelected->size()<<endreq;
      for(pRZSele=m_tracksRZSelected->begin();
          pRZSele<m_tracksRZSelected->end();pRZSele++){
        info()<<*pRZSele<<" "<<thisTrack<<endreq;
        if(*pRZSele==thisTrack){   
          info()<<" queste e' sle "<<endreq;
          selected=true;;
          if((*pRZSele)->checkFlag(Track::L0Candidate)){
            whosel=1;
          }else whosel=2;
          
          
        }
      }
      
      m_RZtruepid[m_RZtracks]=pid;
      m_RZmid[m_RZtracks]=mid;
      m_RZtruep[m_RZtracks]=truep; 
      m_RZselected[m_RZtracks]=0;
      if(selected)  m_RZselected[m_RZtracks]=1;
      m_RZwhoselected[m_RZtracks]=whosel;
      info()<<" sle ected n "<<selected<<" "<< 
        m_RZselected[m_RZtracks]<<endreq;
      m_RZtracks++;
      
    }
    
  }
  
  LinkedTo< MCParticle, Track>
    myLink( eventSvc(), msgSvc(),LHCb::TrackLocation::HltVelo);
  info()<<" track number "<< m_tracksSpace->size()<<endreq;
  
  Hlt::TrackContainer::const_iterator pItr;
  for ( pItr = m_tracksSpace->begin(); m_tracksSpace->end() != pItr;
        pItr++ ) {
    // info()<<"dentro"<<endreq;
    
    Track* thisTrack = (*pItr);
    if(thisTrack->checkFlag( Track::Invalid ))continue;
    if(thisTrack->checkFlag( Track::Backward ))continue;
     MCParticle* pp=NULL;
     pp = myLink.first(thisTrack->key());
     float truep=0;
     long pid=0;
     long mid=0;
     bool reco=false;
     bool selected3d=false;

     // now check if track has been selected by 3d Matching
     Track* p_selected=NULL;
     bool selected=false;
     info()<<" selected tracks "<< m_tracksSpaceSelected->size()<<endreq;
     Hlt::TrackContainer::const_iterator pMatched;
     for(pMatched=m_tracksSpaceSelected->begin();pMatched<m_tracksSpaceSelected->end();
         pMatched++){
       info()<<" qui fuori "<<thisTrack<<endreq;
       const SmartRefVector< Track > ance=((*pMatched)->ancestors());
       info()<<" qui fuori "<<thisTrack<<" "<<ance.size()<<endreq;
       for(SmartRefVector< Track >::const_iterator iance=ance.begin();
           iance<ance.end();iance++){
         const Track* ppConvert=(*iance);
         info()<<" qui dentro "<<ppConvert<<" "<<thisTrack<<endreq;

         if(ppConvert==thisTrack){
          selected3d=true;
          p_selected=(*pMatched);
          info()<<" pointer "<< p_selected<<endreq;
          break;
         }
       }
       
       if(selected3d)break;
     }

 
  

     const SmartRefVector< Track > anceRZ=((*pItr)->ancestors());
     int indexRZ=0;
     bool foundRZ=false;
     for(SmartRefVector< Track >::const_iterator iance=anceRZ.begin();
         iance<anceRZ.end();iance++){
       const Track* ppConvert=(*iance);
       debug()<<" RZ qui dentro "<<ppConvert<<" "<<thisTrack<<endreq;
       //  indexRZ++;
       indexRZ=0;
       
       for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
         Track* pRZTrack = (*pIRZ);
         if(pRZTrack->checkFlag( Track::Invalid ))continue;
         if(pRZTrack->checkFlag( Track::Backward ))continue;

         if(pRZTrack== ppConvert){
           foundRZ=true;
           break;
         }
         indexRZ++;
       }
       break;
     }
     if(!foundRZ) indexRZ=-1;

     if(pp!=NULL){
       pid=pp->particleID().pid();
       truep=sqrt(pow(pp->momentum().z(),2)+pow(pp->momentum().y(),2)+
                  pow(pp->momentum().x(),2));
       const MCVertex *origin= pp->originVertex();
       //info()<<"origin "<<origin<<endreq;
       //       if(abs(pid)==13)reco=reconstructableMuon(pp);

       if(origin){
        const MCParticle* mother=origin->mother();
        if(mother)mid=mother->particleID().pid();
       }
     }
     //now the index of the T track linked
     int indexT=-1;
     int indexAT=-1;
     bool foundT=false;
     bool foundAT=false;
     
     if(selected3d){
       const SmartRefVector< Track > anceRZ=((p_selected)->ancestors());
       indexT=0;
       //  bool foundT=false;
       for(SmartRefVector< Track >::const_iterator iance=anceRZ.begin();
           iance<anceRZ.end();iance++){
         const Track* ppConvert=(*iance);
         info()<<" T qui dentro "<<ppConvert<<" "<<thisTrack<<endreq;
       //  indexRZ++;
         indexT=0;
         
         for(pIRZ=m_tracksT->begin();pIRZ<m_tracksT->end();pIRZ++){
           Track* pRZTrack = (*pIRZ);
           if(pRZTrack->checkFlag( Track::Invalid ))continue;
           if(pRZTrack->checkFlag( Track::Backward ))continue;
           //   info()<<" T Track poi "<<pRZTrack<<endreq;
           
           if(pRZTrack== ppConvert){
             foundT=true;
             //  info()<<"index T found "<<indexT<<endreq;
             
             break;
           }
           indexT++;
         }
         if(foundT)break;
       }
       if(!foundT) indexT=-1;
       //int indexAT=0;
       // bool foundAT=false;
      
       if(!foundT){
         const SmartRefVector< Track > anceRZ=((p_selected)->ancestors());
         //    int indexAT=0;
         //bool foundAT=false;
         for(SmartRefVector< Track >::const_iterator iance=anceRZ.begin();
             iance<anceRZ.end();iance++){
           const Track* ppConvert=(*iance);
           info()<<" T qui dentro "<<ppConvert<<" "<<thisTrack<<endreq;
           //  indexRZ++;
           indexAT=0;
           
           for(pIRZ=m_tracksAT->begin();pIRZ<m_tracksAT->end();pIRZ++){
             Track* pRZTrack = (*pIRZ);
             if(pRZTrack->checkFlag( Track::Invalid ))continue;
             if(pRZTrack->checkFlag( Track::Backward ))continue;
             //   info()<<" T Track poi "<<pRZTrack<<endreq;
             
             if(pRZTrack== ppConvert){
               foundAT=true;
               //  info()<<"index T found "<<indexT<<endreq;
               
               break;
             }
             indexAT++;
           }
           if(foundAT)break;
         }         
       }       
       //  if(foundT)info()<<" e T "<<indexT<<endreq;
       
     }
     
     
     if(m_tracks<m_tracks->range().distance()){
         State* stato=NULL;
       stato=&(thisTrack->firstState());   
       m_slopeX[m_tracks]=stato->slopes().x()/stato->slopes().z();
       m_slopeY[m_tracks]=stato->slopes().y()/stato->slopes().z();
       m_Xpoint[m_tracks]= stato->x();
       m_Ypoint[m_tracks]= stato->y();
       m_Zpoint[m_tracks]= stato->z();
       //       m_impact[m_tracks]=ipmin;
       m_RZIndex[m_tracks]=indexRZ;
       //    m_pid[m_tracks]=13;
       m_mid[m_tracks]=mid;
       m_truepid[m_tracks]=pid;
       m_truep[m_tracks]=truep;
       m_3dselected[m_tracks]=0;
       m_TIndex[m_tracks]=-1;
       m_ATIndex[m_tracks]=-1;       
       if(selected3d){
           m_3dselected[m_tracks]=1;
           if(foundT)m_TIndex[m_tracks]=indexT;
           if(foundAT)m_ATIndex[m_tracks]=indexAT;
           
       }
       if(p_selected){
         m_momentum[m_tracks]=p_selected->p()*p_selected->charge();
         
       }
       
       m_tracks++;
       
     }
     
  }
  





  //now the L0Muon candidates
  std::vector<L0MuonCandidate*>::const_iterator itL0;
  L0MuonCandidates* cand=
    get<L0MuonCandidates>(L0MuonCandidateLocation::Default);
  
  LinkedTo<LHCb::MCParticle,LHCb::MuonCoord> 
    LinkMuon(eventSvc(),msgSvc(),
             LHCb::MuonCoordLocation::MuonCoords);
  
  
  for (itL0=cand->begin();itL0!=cand->end();itL0++){
    if(m_muonL0<m_muonL0->range().distance()){
      MCParticle* pp1[3]={NULL,NULL,NULL};
      long mid=0;
      long pid=0;
      float zdecay=-1000;	      
      float truept=0;
      float truep=0;      
      long key1[3]={-1,-1,-1};
      long pid1[3]={0,0,0};
      long mid1[3]={0,0,0};
      
      double x1[3];
      double  dx1, dy1, dz1;
      double y1[3];
      double z1[3];
      
      long r[3];
      
      /*       info()<<"candidate c"<<(*itL0)->pad(0)<<endmsg;
               info()<<"candidate c"<<(*itL0)->pad(1)<<endmsg;
               info()<<"candidate c"<<(*itL0)->pad(2)<<endmsg;
               info()<<"candidate c"<<(*itL0)->pad(3)<<endmsg;
               info()<<"candidate c"<<(*itL0)->pad(4)<<endmsg;*/
      
      for(int ss=0;ss<3;ss++){
        
        //          if(((*itL0)->pad(ss))){
        //get coords
        debug()<< "qui "<<endmsg;
        std::vector<MuonTileID> list_of_tile= (*itL0)->muonTileIDs(ss);
        MuonTileID tile=*(list_of_tile.begin());
        r[ss]=tile.region();            
        m_iPosTool-> calcTilePos(tile,x1[ss], dx1,y1[ss], dy1,z1[ss], dz1);
        pp1[ss]=LinkMuon.first(tile);
        
        //            searchNature(tile,pp1[ss]);
        if(pp1[ss]!=NULL){
          debug()<<"station 1 "<<
            pp1[ss]->particleID().pid()<<" "<<
            pp1[ss]->key()<<  endmsg;
          pid1[ss]= pp1[ss]->particleID().pid();
          const MCVertex *origin= pp1[ss]->originVertex();   
          zdecay=origin->position().z();
          const MCParticle* mother=origin->mother();
          if(mother!=NULL){
            mid1[ss]=mother->particleID().pid();
          }
          key1[ss]=  pp1[ss]->key();
        }
        // }
      }
      MCParticle* ppL0=NULL;
      debug()<<" pre link "<<endreq;
      const MCParticle* mother=NULL;
      
      linkMCToL0Muon(*itL0,ppL0);
      debug()<<" dopo link "<<ppL0<<endreq;
        
      if(ppL0!=NULL){
        bool de=isReconstructable(ppL0);
        info()<<"is muon reconstructable ? "<<de<<endreq;
        

        //            long mid=0;
        const MCVertex *origin= ppL0->originVertex();
        truept=ppL0->pt();
        truep=ppL0->p();        
        mother=origin->mother();
        if(mother!=NULL){
          mid=mother->particleID().pid();
        }
        pid=ppL0->particleID().pid();
        //key=pp->key();
        //  int counter=-1;
        m_l0muonMatch[m_muonL0]=-1;
        /*          bool foundMatch=false;
                    for ( pItr = m_tracksSpace->begin(); m_tracksSpace->end() != pItr;
                    pItr++ ) {
                    // info()<<"dentro"<<endreq;
            Track* thisTrack = (*pItr);
            if(thisTrack->checkFlag( Track::Invalid ))continue;
            if(thisTrack->checkFlag( Track::Backward ))continue;
            counter++;
            MCParticle* target=NULL;            target = myLink.first(thisTrack->key());
            if(target==ppL0){
            m_l0muonMatch[m_muonL0]=counter;
              foundMatch=true;
              break;
              }
              }
              counter=-1;
              if(!foundMatch&&mother!=NULL){
              for ( pItr = m_tracksSpace->begin();
                  m_tracksSpace->end() != pItr;
                  pItr++ ) {
              // info()<<"dentro"<<endreq;
              
              Track* thisTrack = (*pItr);
              if(thisTrack->checkFlag( Track::Invalid ))continue;
              if(thisTrack->checkFlag( Track::Backward ))continue;
              counter++;
              MCParticle* target=NULL;
              target = myLink.first(thisTrack->key());
              if(target==mother){
                m_l0muonMatch[m_muonL0]=counter;
                foundMatch=false;
                break;
              }
            }
          }
          
          */          

        int counterRZ=-1;
        m_l0muonRZMatch[m_muonL0]=-1;
        bool foundMatchRZ=false;
        
        for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
          Track* thisTrack = (*pIRZ);
          if(thisTrack->checkFlag( Track::Invalid ))continue;
          if(thisTrack->checkFlag( Track::Backward ))continue;
          counterRZ++;
          MCParticle* target=NULL;
          target = myLinkRZ.first(thisTrack->key());
          if(target==ppL0){
            m_l0muonRZMatch[m_muonL0]=counterRZ;
            foundMatchRZ=true;
            break;
          }
        }
        
        if(!foundMatchRZ&&mother!=NULL){
          for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
            
            // info()<<"dentro"<<endreq;
            
            Track* thisTrack = (*pIRZ);
            if(thisTrack->checkFlag( Track::Invalid ))continue;
            if(thisTrack->checkFlag( Track::Backward ))continue;
            counterRZ++;
            MCParticle* target=NULL;
            target = myLinkRZ.first(thisTrack->key());
            if(target==mother){
              m_l0muonRZMatch[m_muonL0]=counterRZ;
              foundMatchRZ=true;
              break;
            }
          }
        }
      }
      debug()<<"sto pr scrivere "<<endreq;
      
      
      debug()<<"sto pr scrivere "<<endreq;
      m_l0muonPT[m_muonL0]=(*itL0)->pt();
      m_l0muonPTtrue[m_muonL0]=truept;
      m_l0muonPtrue[m_muonL0]=truep;
      m_l0muonPid[m_muonL0]=pid;      
      m_l0muonMid[m_muonL0]=mid;
      m_l0muonZdecay[m_muonL0]=zdecay;      
      m_l0muonKey1[m_muonL0]=key1[0];
      m_l0muonKey2[m_muonL0]=key1[1];   
      m_l0muonKey3[m_muonL0]=key1[2];
      m_l0muonPid1[m_muonL0]=pid1[0];  
      m_l0muonPid2[m_muonL0]=pid1[1];
      m_l0muonPid3[m_muonL0]=pid1[2];
      m_l0muonMid1[m_muonL0]=mid1[0];
      m_l0muonMid2[m_muonL0]=mid1[1];
      m_l0muonMid3[m_muonL0]=mid1[2];
      
      m_l0muonx1[m_muonL0]=x1[0];
      m_l0muonx2[m_muonL0]=x1[1];
      m_l0muonx3[m_muonL0]=x1[2];
      m_l0muony1[m_muonL0]=y1[0];
      m_l0muony2[m_muonL0]=y1[1];
      m_l0muony3[m_muonL0]=y1[2];
      m_l0muonr1[m_muonL0]=r[0];
      m_l0muonr2[m_muonL0]=r[1];
      m_l0muonr3[m_muonL0]=r[2];
      
      m_muonL0++;
        
    
      
    }    
  }
  //now the T tracks built from L0Muon


  Tracks::const_iterator pIT;
  info()<<" ciao"<<endreq;
  LinkedTo< MCParticle, Track>
    myLinkT( eventSvc(), msgSvc(),"Hlt/Tracks/ConfirmedL0Muon" );
  if(myLinkT.notFound())info()<<" link not found why ?"<<endreq;  
  
  for(pIT=m_tracksT->begin();pIT<m_tracksT->end();pIT++){
    Track* thisTrack = (*pIT);
    info()<<" a new track "<<endreq;
    //    if(thisTrack->checkFlag( Track::Invalid ))continue;
    //if(thisTrack->checkFlag( Track::Backward ))continue;
    if(m_Ttracks<m_Ttracks->range().distance()){
      State* stato=NULL;
      stato=&(thisTrack->firstState());
      float xpoint=stato->x();
      float ypoint=stato->y();
      float zpoint=stato->z();
      float xslope=stato->slopes().x()/stato->slopes().z();
      float yslope=stato->slopes().y()/stato->slopes().z();
      float mom=thisTrack->p();
      float mompt=thisTrack->pt();
      float chi=thisTrack->chi2();
      MCParticle* pp=NULL;
      pp = myLinkT.first(thisTrack->key());
      info()<<thisTrack->key()<<" "<<pp<<endreq;
      float truep=0;
      long pid=0;
      long mid=0;
      const MCParticle* mother=NULL;
      
      if(pp!=NULL){
        pid=pp->particleID().pid();
        truep=sqrt(pow(pp->momentum().z(),2)+pow(pp->momentum().y(),2)+
                   pow(pp->momentum().x(),2));
        const MCVertex *origin= pp->originVertex();
        //info()<<"origin "<<origin<<endreq;
        // if(abs(pid)==13)reco=reconstructableMuon(pp);
        
        if(origin){
           mother=origin->mother();
          if(mother)mid=mother->particleID().pid();
        }
      }
      L0MuonCandidate* muon=NULL;
      
      linkToL0Muon(thisTrack,muon);
      int counter=0;
      bool found=false;
      
      if(muon!=NULL){
        for (itL0=cand->begin();itL0!=cand->end();itL0++){
          if(*itL0==muon){
            found=true;
            
            break;
          }
          
          counter++;
          
        }
        
            
      }



      int counterRZ=-1;
      m_TRZMatch[m_Ttracks]=-1;
      bool foundMatchRZ=false;
      if(pp!=NULL){
        
        for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
          Track* thisTrack = (*pIRZ);
          if(thisTrack->checkFlag( Track::Invalid ))continue;
          if(thisTrack->checkFlag( Track::Backward ))continue;
          counterRZ++;
          MCParticle* target=NULL;
          target = myLinkRZ.first(thisTrack->key());
          if(target==pp){
            m_TRZMatch[m_Ttracks]=counterRZ;
            foundMatchRZ=true;
            break;
          }
        }
        
        if(!foundMatchRZ&&mother!=NULL){
          for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
            
            // info()<<"dentro"<<endreq;
            
            Track* thisTrack = (*pIRZ);
            if(thisTrack->checkFlag( Track::Invalid ))continue;
            if(thisTrack->checkFlag( Track::Backward ))continue;
            counterRZ++;
            MCParticle* target=NULL;
            target = myLinkRZ.first(thisTrack->key());
            if(target==mother){
              m_TRZMatch[m_Ttracks]=counterRZ;
              foundMatchRZ=true;
              break;
            }
          }
        }
      }
      
    
    
      debug()<<"sto pr scrivere "<<endreq;
    
      
      double dist=calcDLL(thisTrack,muon);
      info()<<" dist "<<dist;
      
      info()<<"x  pi "<<xpoint<<" " <<pid<<endreq;
      m_TslopeX[m_Ttracks]=xslope;
      m_TslopeY[m_Ttracks]=yslope;
      m_TXpoint[m_Ttracks]=xpoint;
      m_TYpoint[m_Ttracks]=ypoint;
      m_TZpoint[m_Ttracks]=zpoint;
      m_Tchi2[m_Ttracks]=chi;
      m_Tdll[m_Ttracks]=dist;
      m_Tmomentum[m_Ttracks]=mom;
      m_Tpt[m_Ttracks]=mompt;
      m_Tpid[m_Ttracks]=pid;
      m_Tmid[m_Ttracks]=mid;
      m_Ttruep[m_Ttracks]=truep;
      m_TL0Index[m_Ttracks]=-1;
      if(found)m_TL0Index[m_Ttracks]=counter;
      
      m_Ttracks++;
      
    }
    
  }
  
  //now the T tracks built from MuonTracks


  //Tracks::const_iterator pIT;
  info()<<" ciao"<<endreq;
  LinkedTo< MCParticle, Track>
    myLinkAT( eventSvc(), msgSvc(),"Hlt/Tracks/ConfirmedTMuon" );
  if(myLinkAT.notFound())info()<<" link not found why ?"<<endreq;  
  //error()<<" size "<<m_tracksAT->size()<<endreq;
  for(pIT=m_tracksAT->begin();pIT<m_tracksAT->end();pIT++){
    Track* thisTrack = (*pIT);
    //    error()<<" a new track "<<endreq;
    //    if(thisTrack->checkFlag( Track::Invalid ))continue;
    //if(thisTrack->checkFlag( Track::Backward ))continue;
    if(m_ATtracks<m_ATtracks->range().distance()){
      State* stato=NULL;
      stato=&(thisTrack->firstState());
      float xpoint=stato->x();
      float ypoint=stato->y();
      float zpoint=stato->z();
      float xslope=stato->slopes().x()/stato->slopes().z();
      float yslope=stato->slopes().y()/stato->slopes().z();
      float mom=thisTrack->p();
      float mompt=thisTrack->pt();
      float chi=thisTrack->chi2();
      MCParticle* pp=NULL;
      pp = myLinkAT.first(thisTrack->key());
      info()<<thisTrack->key()<<" "<<pp<<endreq;
      float truep=0;
      long pid=0;
      long mid=0;
      const MCParticle* mother=NULL;
      
      if(pp!=NULL){
        pid=pp->particleID().pid();
        truep=sqrt(pow(pp->momentum().z(),2)+pow(pp->momentum().y(),2)+
                   pow(pp->momentum().x(),2));
        const MCVertex *origin= pp->originVertex();
        //info()<<"origin "<<origin<<endreq;
        // if(abs(pid)==13)reco=reconstructableMuon(pp);
        
        if(origin){
           mother=origin->mother();
          if(mother)mid=mother->particleID().pid();
        }
      }
      Track* muon=NULL;
      
      linkToMuon(thisTrack,muon);
      int counter=0;
      bool found=false;
      
      if(muon!=NULL){
        Tracks::const_iterator itMuonTrack;
        for(itMuonTrack = m_muonTracksContainer->begin();
            itMuonTrack < m_muonTracksContainer->end();itMuonTrack++){
          if(*itMuonTrack==muon){
            found=true;            
            break;
          }          
          counter++; 
        }
        
      }
      int counterRZ=-1;
      m_ATRZMatch[m_ATtracks]=-1;
      bool foundMatchRZ=false;
      if(pp!=NULL){
        
        for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
          Track* thisTrack = (*pIRZ);
          if(thisTrack->checkFlag( Track::Invalid ))continue;
          if(thisTrack->checkFlag( Track::Backward ))continue;
          counterRZ++;
          MCParticle* target=NULL;
          target = myLinkRZ.first(thisTrack->key());
          if(target==pp){
            m_ATRZMatch[m_ATtracks]=counterRZ;
            foundMatchRZ=true;
            break;
          }
        }
        
        if(!foundMatchRZ&&mother!=NULL){
          for(pIRZ=m_tracksRZ->begin();pIRZ<m_tracksRZ->end();pIRZ++){
            
            // info()<<"dentro"<<endreq;
            
            Track* thisTrack = (*pIRZ);
            if(thisTrack->checkFlag( Track::Invalid ))continue;
            if(thisTrack->checkFlag( Track::Backward ))continue;
            counterRZ++;
            MCParticle* target=NULL;
            target = myLinkRZ.first(thisTrack->key());
            if(target==mother){
              m_ATRZMatch[m_ATtracks]=counterRZ;
              foundMatchRZ=true;
              break;
            }
          }
        }
      }
      
    
    
      debug()<<"sto pr scrivere "<<endreq;
    
      //      double dist;      
      double dist=calcDLL(thisTrack,muon);
      double diststd=calcDLLstd(thisTrack,muon);
      info()<<" dist "<<dist;
      
      info()<<"x  pi "<<xpoint<<" " <<pid<<endreq;
      m_ATslopeX[m_ATtracks]=xslope;
      m_ATslopeY[m_ATtracks]=yslope;
      m_ATXpoint[m_ATtracks]=xpoint;
      m_ATYpoint[m_ATtracks]=ypoint;
      m_ATZpoint[m_ATtracks]=zpoint;
      m_ATchi2[m_ATtracks]=chi;
      m_ATdll[m_ATtracks]=dist;
      m_ATdllstd[m_ATtracks]=diststd;
      m_ATmomentum[m_ATtracks]=mom;
      m_ATpt[m_ATtracks]=mompt;
      m_ATpid[m_ATtracks]=pid;
      m_ATmid[m_ATtracks]=mid;
      m_ATtruep[m_ATtracks]=truep;
      m_ATL0Index[m_ATtracks]=-1;
      if(found)m_ATL0Index[m_ATtracks]=counter;
      
      m_ATtracks++;
      
    }
    
  }


  
  
  //muon tracks
  Tracks::const_iterator itMuonTrack;
  for(itMuonTrack = m_muonTracksContainer->begin();
      itMuonTrack < m_muonTracksContainer->end();itMuonTrack++){
    
    if(m_muonTracks<m_muonTracks->range().distance()){

      //      bool skipL0=false;
      
      MCParticle* ppMuon=NULL;
      linkMCToMuon(*itMuonTrack,ppMuon);
      if(ppMuon!=NULL){
        debug()<<" found the match "<<
          ppMuon->particleID().pid()<<endreq;
        m_muonPid[m_muonTracks]=ppMuon->particleID().pid();
        m_muonTrueP[m_muonTracks]=ppMuon->p();
        const MCVertex *origin= ppMuon->originVertex();
        const MCParticle* mother=origin->mother();
        //m_muonDecayPoint[m_muonTracks]=origin->position().z();
       
        if(mother!=NULL){
          m_muonMid[m_muonTracks]=mother->particleID().pid();
          
        }
      }
      if(ppMuon!=NULL){
               //            long mid=0;
        const MCVertex *origin= ppMuon->originVertex();
        const MCParticle* mother=origin->mother();


        int counter=-1;
        m_muonMatch[m_muonTracks]=-1;
        bool foundMatch=false;
        for ( pItr = m_tracksSpace->begin(); m_tracksSpace->end() != pItr;
              pItr++ ) {
          // info()<<"dentro"<<endreq;
          
          Track* thisTrack = (*pItr);
          if(thisTrack->checkFlag( Track::Invalid ))continue;
          if(thisTrack->checkFlag( Track::Backward ))continue;
          counter++;
          MCParticle* target=NULL;
          target = myLink.first(thisTrack->key());
          if(target==ppMuon){
            m_muonMatch[m_muonTracks]=counter;
            foundMatch=true;
            break;
          }
        }
        counter=-1;
        if(!foundMatch&&mother!=NULL){
          for ( pItr = m_tracksSpace->begin();
                m_tracksSpace->end() != pItr;
                pItr++ ) {
            // info()<<"dentro"<<endreq;
            
            Track* thisTrack = (*pItr);
            if(thisTrack->checkFlag( Track::Invalid ))continue;
            if(thisTrack->checkFlag( Track::Backward ))continue;
            counter++;
            MCParticle* target=NULL;
            target = myLink.first(thisTrack->key());
            if(target==mother){
              m_muonMatch[m_muonTracks]=counter;
              
              foundMatch=true;
              break;
            }
          }
        }  

        int counterRZ=-1;
        m_muonRZMatch[m_muonTracks]=-1;
        bool foundMatchRZ=false;
        for ( pItr = m_tracksRZ->begin(); m_tracksRZ->end() != pItr;
              pItr++ ) {
          // info()<<"dentro"<<endreq;
          
          Track* thisTrack = (*pItr);
          if(thisTrack->checkFlag( Track::Invalid ))continue;
          if(thisTrack->checkFlag( Track::Backward ))continue;
          counterRZ++;
          MCParticle* target=NULL;
          target = myLinkRZ.first(thisTrack->key());
          if(target==ppMuon){
            m_muonRZMatch[m_muonTracks]=counterRZ;
            
            foundMatchRZ=true;
            break;
          }
        }
        counterRZ=-1;
        if(!foundMatchRZ&&mother!=NULL){
          for ( pItr = m_tracksRZ->begin();
                m_tracksRZ->end() != pItr;
                pItr++ ) {
            // info()<<"dentro"<<endreq;
            
            Track* thisTrack = (*pItr);
            if(thisTrack->checkFlag( Track::Invalid ))continue;
            if(thisTrack->checkFlag( Track::Backward ))continue;
            counterRZ++;
            MCParticle* target=NULL;
            target = myLinkRZ.first(thisTrack->key());
            if(target==mother){   
              m_muonMatch[m_muonTracks]=counterRZ;  
              foundMatchRZ=true;
              break;
            }
            
          }
        }  
      }
      
             
      double xM2,xM3,yM2,yM3,zM2,zM3,dx,dy,dz;     
      int n=(*itMuonTrack)->nLHCbIDs();
      std::vector< LHCb::LHCbID >    lh=(*itMuonTrack)->lhcbIDs();
      std::vector<LHCb::LHCbID>::iterator ii;
      unsigned int m=0;
      MuonTileID tileM[4];
      //     MuonTileID tileM3;
         //MuonTileID tileM4;
         //MuonTileID tileM5;
         //unsigned int mm=0;
      
      for(ii=lh.begin();ii<lh.end();ii++){
        if(ii->isMuon()){
             if(m<4) tileM[m]=ii->muonID();
             
             m++;
        }
      }
      
      m_iPosTool->calcTilePos(tileM[0],xM2,dx,yM2,dy,zM2,dz);
      m_iPosTool->calcTilePos(tileM[1],xM3,dx,yM3,dy,zM3,dz); 

      bool skipL0=false;
      
      for (itL0=cand->begin();itL0!=cand->end();itL0++){
        std::vector<MuonTileID> list_of_tile= (*itL0)->muonTileIDs(2);
        MuonTileID tile=*(list_of_tile.begin());
        if(tile==tileM[1])skipL0=true;
        
      }
      
         

      info()<<" per  salvare "<<endreq;  
          

      m_muonXpoint[m_muonTracks]=xM2;
      m_muonYpoint[m_muonTracks]=yM2;
      m_muonSlopeX[m_muonTracks]=(xM3-xM2)/(16400.0-15200.);
      m_muonSlopeY[m_muonTracks]=(yM3-yM2)/(16400.0-15200.);
      m_muonSkip[m_muonTracks]=0;
      if(skipL0)m_muonSkip[m_muonTracks]=1;
      
      m_muonTracks++;
      
    }
  }
  
     
     


  m_Track->write();

  HltAlgorithm::endExecute();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonConfNt::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
StatusCode L0MuonConfNt::linkMCToL0Muon(L0MuonCandidate* muon,MCParticle*& pp)
{


  LinkedTo<LHCb::MCParticle,LHCb::MuonCoord> 
    LinkMuon(eventSvc(),msgSvc(),
             LHCb::MuonCoordLocation::MuonCoords);
  

  pp=NULL;
  MCParticle* p0=NULL;
  MCParticle* p1=NULL;
  MCParticle* p2=NULL;
  //  MCParticle* p3=NULL;
  // MCParticle* p4=NULL;
  std::vector<MuonTileID> list_of_tile= (muon)->muonTileIDs(0);
  MuonTileID tileM1=*(list_of_tile.begin());
  list_of_tile= (muon)->muonTileIDs(1);
  MuonTileID tileM2=*(list_of_tile.begin());
  list_of_tile= (muon)->muonTileIDs(2);
  MuonTileID tileM3=*(list_of_tile.begin());
  
  
  p0=LinkMuon.first(tileM1);
  p1=LinkMuon.first(tileM2);
  p2=LinkMuon.first(tileM3);
  
 if(p1==p2&&p1!=NULL){
    pp=p1;
    return StatusCode::SUCCESS;
    
  }
  
  
  if(pp!=NULL)return StatusCode::SUCCESS;
  return StatusCode::FAILURE;


}

StatusCode L0MuonConfNt::linkToL0Muon(Track* track,L0MuonCandidate*& muon)
{
  std::vector< LHCb::LHCbID >  lista=	track->lhcbIDs ();
  MuonTileID tileM2;
  MuonTileID tileM3;
  muon=NULL;
  
  std::vector<LHCbID>::iterator it;
  for(it=lista.begin();it<lista.end();it++){
    if(it->isMuon())
    {
      MuonTileID tile=it->muonID();
      if(tile.station()==1)tileM2=tile;
      if(tile.station()==2)tileM3=tile;
      
    }
       
  }
  info()<<tileM2.isValid()<<" "<<tileM3.isValid()<<endreq;
  
  if(tileM2.isValid()&&tileM3.isValid()){
    std::vector<L0MuonCandidate*>::const_iterator itL0;
    L0MuonCandidates* cand=
      get<L0MuonCandidates>(L0MuonCandidateLocation::Default);
    for (itL0=cand->begin();itL0!=cand->end();itL0++){
      std::vector<MuonTileID> list_of_tile= (*itL0)->muonTileIDs(1);
      MuonTileID l0tileM2=*(list_of_tile.begin());
      list_of_tile= (*itL0)->muonTileIDs(2);
      MuonTileID l0tileM3=*(list_of_tile.begin());
      if(tileM2==l0tileM2&&tileM3==l0tileM3){
        muon=*itL0;
        info()<<"foudn match "<<endreq;
        
        return StatusCode::SUCCESS;
        
      }
      
    }
    
  }
  return StatusCode::SUCCESS;
  

}
StatusCode L0MuonConfNt::linkToMuon(Track* track,Track*& muon)
{
  std::vector< LHCb::LHCbID >  lista=	track->lhcbIDs ();
  MuonTileID tileM2;
  MuonTileID tileM3;
  muon=NULL;
  
  std::vector<LHCbID>::iterator it;
  for(it=lista.begin();it<lista.end();it++){
    if(it->isMuon())
    {
      MuonTileID tile=it->muonID();
      if(tile.station()==1)tileM2=tile;
      if(tile.station()==2)tileM3=tile;
      
    }
       
  }
  info()<<tileM2.isValid()<<" "<<tileM3.isValid()<<endreq;
  
  if(tileM2.isValid()&&tileM3.isValid()){
    
    Tracks::iterator itMuonTrack;
    

    for(itMuonTrack = m_muonTracksContainer->begin();
        itMuonTrack < m_muonTracksContainer->end();itMuonTrack++){
      Track* p_muon=*itMuonTrack;
      
      int n=p_muon->nLHCbIDs();
      std::vector< LHCb::LHCbID >   lh=p_muon->lhcbIDs();
      std::vector<LHCb::LHCbID>::iterator ii;
      unsigned int m=0;
      MuonTileID muontileM2;
      MuonTileID muontileM3;
      
      
      for(ii=lh.begin();ii<lh.end();ii++){
        if(ii->isMuon()){
          if(m==0) muontileM2=ii->muonID();
          if(m==1) muontileM3=ii->muonID();
          m++;
        }
      }
      
      
      
      if(tileM2==muontileM2&&tileM3==muontileM3){
        muon=p_muon;
        info()<<"foudn match "<<endreq;
        
        return StatusCode::SUCCESS;
        
      }
    }
  }
  
  
  
  
  return StatusCode::SUCCESS;
  

}





double  L0MuonConfNt::calcDLL(Track* track,L0MuonCandidate* muon)
{

  double distDLL=0;
  
  //M2 and M3
  std::vector<MuonTileID> list_of_tile= (muon)->muonTileIDs(1);
  MuonTileID tileM2=*(list_of_tile.begin());
  list_of_tile= (muon)->muonTileIDs(2);
  MuonTileID tileM3=*(list_of_tile.begin());
  double x,y,z,dx,dy,dz;
  
  m_iPosTool-> calcTilePos(tileM2,x, dx,y, dy,z, dz);

  State* stato=NULL;
  stato=&(track->firstState());
  float xpoint=stato->x();
  float ypoint=stato->y();
  float zpoint=stato->z();
  float xslope=stato->slopes().x()/stato->slopes().z();
  float yslope=stato->slopes().y()/stato->slopes().z();
  float x_M2=xpoint+xslope*(z-zpoint);  
  float y_M2=ypoint+yslope*(z-zpoint);
  distDLL=((x_M2-x)/dx)*((x_M2-x)/dx)+
    ((y_M2-y)/dy)*((y_M2-y)/dy);
  debug()<<" dist M2 "<<distDLL<<" "<<x<<" "<<dx<<" "<<x_M2<<endreq;
  debug()<<" dist M2 "<<distDLL<<" "<<y<<" "<<dy<<" "<<y_M2<<endreq;
  //info()<<  
  m_iPosTool-> calcTilePos(tileM3,x, dx,y, dy,z, dz); 
  float x_M3=xpoint+xslope*(z-zpoint);  
  float y_M3=ypoint+yslope*(z-zpoint);
  distDLL+=((x_M3-x)/dx)*((x_M3-x)/dx)+
    ((y_M3-y)/dy)*((y_M3-y)/dy);
 
  debug()<<" dist M3 "<<distDLL<<endreq;

  //M4 and M5




  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  
  if ( coords==0 ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
  }
 
 
  // loop over the coords
  float dist_tempM4=100000;
  float dist_tempM5=100000;
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station(); 
    if(station==0||station==1||station==2)continue;
    if(region!=0&&(*iCoord)->uncrossed())continue;
    debug()<<station<<" "<<region<<endreq;
    
    m_iPosTool-> calcTilePos( (*iCoord)->key(),x, dx,y, dy,z, dz); 
    float dist_temp=0;
    float x_Mi=xpoint+xslope*(z-zpoint);  
    float y_Mi=ypoint+yslope*(z-zpoint);
    dist_temp=((x_Mi-x)/dx)*((x_Mi-x)/dx)+
      ((y_Mi-y)/dy)*((y_Mi-y)/dy);
    
    if(station==3){
      if(float(dist_temp)<float(dist_tempM4))dist_tempM4=dist_temp;  
    }
    
    if(station==4){
      if(float(dist_temp)<float(dist_tempM5))dist_tempM5=dist_temp;  
    }
    
 
    
  }
  
  distDLL+=dist_tempM4;
  debug()<<" dist M4 "<<distDLL<<endreq;
  distDLL+=dist_tempM5;
  debug()<<" dist M5 "<<distDLL<<endreq;
  return distDLL;
  

}

double  L0MuonConfNt::calcDLL(Track* track,Track* p_muon)
{

  double distDLL=0;
  
  //M2 and M3

  int n=p_muon->nLHCbIDs();
  std::vector< LHCb::LHCbID >   lh=p_muon->lhcbIDs();
  std::vector<LHCb::LHCbID>::iterator ii;
  unsigned int m=0;
  MuonTileID muontileM2;
  MuonTileID muontileM3;
  MuonTileID muontileM4;
  MuonTileID muontileM5;
      
      
  for(ii=lh.begin();ii<lh.end();ii++){
    if(ii->isMuon()){
      if(m==0) muontileM2=ii->muonID();
      if(m==1) muontileM3=ii->muonID();
      if(m==2) muontileM4=ii->muonID();
      if(m==3) muontileM5=ii->muonID();
      m++;
    }
  }
  
  double x,y,z,dx,dy,dz;
  
  m_iPosTool-> calcTilePos(muontileM2,x, dx,y, dy,z, dz);

  State* stato=NULL;
  stato=&(track->firstState());
  float xpoint=stato->x();
  float ypoint=stato->y();
  float zpoint=stato->z();
  float xslope=stato->slopes().x()/stato->slopes().z();
  float yslope=stato->slopes().y()/stato->slopes().z();
  float x_M2=xpoint+xslope*(z-zpoint);  
  float y_M2=ypoint+yslope*(z-zpoint);
  distDLL=((x_M2-x)/dx)*((x_M2-x)/dx)+
    ((y_M2-y)/dy)*((y_M2-y)/dy);
  debug()<<" dist M2 "<<distDLL<<" "<<x<<" "<<dx<<" "<<x_M2<<endreq;
  debug()<<" dist M2 "<<distDLL<<" "<<y<<" "<<dy<<" "<<y_M2<<endreq;
  //info()<<  
  m_iPosTool-> calcTilePos(muontileM3,x, dx,y, dy,z, dz); 
  float x_M3=xpoint+xslope*(z-zpoint);  
  float y_M3=ypoint+yslope*(z-zpoint);
  distDLL+=((x_M3-x)/dx)*((x_M3-x)/dx)+
    ((y_M3-y)/dy)*((y_M3-y)/dy);
  m_iPosTool-> calcTilePos(muontileM4,x, dx,y, dy,z, dz); 
  float x_M4=xpoint+xslope*(z-zpoint);  
  float y_M4=ypoint+yslope*(z-zpoint);
  distDLL+=((x_M4-x)/dx)*((x_M4-x)/dx)+
    ((y_M4-y)/dy)*((y_M4-y)/dy);

  m_iPosTool-> calcTilePos(muontileM5,x, dx,y, dy,z, dz); 
  float x_M5=xpoint+xslope*(z-zpoint);  
  float y_M5=ypoint+yslope*(z-zpoint);
  distDLL+=((x_M5-x)/dx)*((x_M5-x)/dx)+
    ((y_M5-y)/dy)*((y_M5-y)/dy);

  return distDLL;
  

}

double  L0MuonConfNt::calcDLLstd(Track* track,Track* p_muon)
{

  double distDLL=0;
  
  //M2 and M3

  int n=p_muon->nLHCbIDs();
  std::vector< LHCb::LHCbID >   lh=p_muon->lhcbIDs();
  std::vector<LHCb::LHCbID>::iterator ii;
  unsigned int m=0;
  MuonTileID muontileM2;
  MuonTileID muontileM3;
  MuonTileID muontileM4;
  MuonTileID muontileM5;
      
      
  for(ii=lh.begin();ii<lh.end();ii++){
    if(ii->isMuon()){
      if(m==0) muontileM2=ii->muonID();
      if(m==1) muontileM3=ii->muonID();
      if(m==2) muontileM4=ii->muonID();
      if(m==3) muontileM5=ii->muonID();
      m++;
    }
  }
  
  double x,y,z,dx,dy,dz;
  
  m_iPosTool-> calcTilePos(muontileM2,x, dx,y, dy,z, dz);

  State* stato=NULL;
  stato=&(track->firstState());
  float xpoint=stato->x();
  float ypoint=stato->y();
  float zpoint=stato->z();
  float xslope=stato->slopes().x()/stato->slopes().z();
  float yslope=stato->slopes().y()/stato->slopes().z();
  float x_M2=xpoint+xslope*(z-zpoint);  
  float y_M2=ypoint+yslope*(z-zpoint);
  distDLL=((x_M2-x)/dx)*((x_M2-x)/dx)+
    ((y_M2-y)/dy)*((y_M2-y)/dy);
  debug()<<" dist M2 "<<distDLL<<" "<<x<<" "<<dx<<" "<<x_M2<<endreq;
  debug()<<" dist M2 "<<distDLL<<" "<<y<<" "<<dy<<" "<<y_M2<<endreq;
  //info()<<  
  m_iPosTool-> calcTilePos(muontileM3,x, dx,y, dy,z, dz); 
  float x_M3=xpoint+xslope*(z-zpoint);  
  float y_M3=ypoint+yslope*(z-zpoint);
  distDLL+=((x_M3-x)/dx)*((x_M3-x)/dx)+
    ((y_M3-y)/dy)*((y_M3-y)/dy);
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  
  if ( coords==0 ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
  }
 
 
  // loop over the coords
  float dist_tempM4=100000;
  float dist_tempM5=100000;
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station(); 
    if(station==0||station==1||station==2)continue;
    if(region!=0&&(*iCoord)->uncrossed())continue;
    debug()<<station<<" "<<region<<endreq;
    
    m_iPosTool-> calcTilePos( (*iCoord)->key(),x, dx,y, dy,z, dz); 
    float dist_temp=0;
    float x_Mi=xpoint+xslope*(z-zpoint);  
    float y_Mi=ypoint+yslope*(z-zpoint);
    dist_temp=((x_Mi-x)/dx)*((x_Mi-x)/dx)+
      ((y_Mi-y)/dy)*((y_Mi-y)/dy);
    
    if(station==3){
      if(float(dist_temp)<float(dist_tempM4))dist_tempM4=dist_temp;  
    }
    
    if(station==4){
      if(float(dist_temp)<float(dist_tempM5))dist_tempM5=dist_temp;  
    }
    
 
    
  }
  
  distDLL+=dist_tempM4;
  debug()<<" dist M4 "<<distDLL<<endreq;
  distDLL+=dist_tempM5;
  debug()<<" dist M5 "<<distDLL<<endreq;

  return distDLL;
  

}

StatusCode L0MuonConfNt::linkMCToMuon(Track* muon,MCParticle*& pp)
{


  LinkedTo<LHCb::MCParticle,LHCb::MuonCoord> 
    LinkMuon(eventSvc(),msgSvc(),
             LHCb::MuonCoordLocation::MuonCoords);
  

  pp=NULL;
  MCParticle* p0=NULL;
  MCParticle* p1=NULL;
  MCParticle* p2=NULL;
  //  MCParticle* p3=NULL;
  // MCParticle* p4=NULL;
  int n=muon->nLHCbIDs();
  std::vector< LHCb::LHCbID >   lh=muon->lhcbIDs();
  std::vector<LHCb::LHCbID>::iterator ii;
  unsigned int m=0;
  MuonTileID tileM2;
  MuonTileID tileM3;

  
  for(ii=lh.begin();ii<lh.end();ii++){
    if(ii->isMuon()){
      if(m==0) tileM2=ii->muonID();
      if(m==1) tileM3=ii->muonID();
      m++;
    }
  }


  p1=LinkMuon.first(tileM2);
  p2=LinkMuon.first(tileM3);
  
  if(p1==p2&&p1!=NULL){
    pp=p1;
    return StatusCode::SUCCESS;
    
  }
  
  
  if(pp!=NULL)return StatusCode::SUCCESS;
  return StatusCode::FAILURE;


}






/**
 *   checks wether MCParticle has 1 x and u hit in each T station
 *
 */
bool L0MuonConfNt::isReconstructable( const LHCb::MCParticle* mcpart )
{
  bool decision = false;
  bool hasT1x  = false;
  bool hasT1u  = false;
  bool hasT2x  = false;
  bool hasT2u  = false;
  bool hasT3x  = false;
  bool hasT3u  = false;
  
  //loop OTTimes
  const LHCb::OTTimes*
ottimes=get<LHCb::OTTimes>(LHCb::OTTimeLocation::Default);
  LinkedTo<LHCb::MCParticle> ottimeasso 
(evtSvc(),msgSvc(),LHCb::OTTimeLocation::Default);
  


 LinkedTo<LHCb::MCParticle> lhcbIdLink( evtSvc(), msgSvc(), "Pat/LHCbID");



  if( NULL!=mcpart ){
    // loop on ottimes
    for( LHCb::OTTimes::const_iterator iot=ottimes->begin(); 
         ottimes->end()!=iot; ++iot ){
      // get association
      const LHCb::MCParticle* part = ottimeasso.first(*iot);
      const LHCb::MCParticle* mcpart_ot = NULL;
      double weight = 0.;
      while( NULL != part ){
        if( ottimeasso.weight() > weight ){
          weight = ottimeasso.weight(); mcpart_ot = part; 
        }
        part = ottimeasso.next();
      }// while association
      if( NULL!=mcpart_ot ) {
        if( mcpart==mcpart_ot ){ 
          // Retrieve  channel id
          const LHCb::OTChannelID otchannelid = (*iot)->channel();
          info()<<" ot cha id "<<LHCbID(otchannelid)<<endreq;

          MCParticle* part=NULL;
          part = lhcbIdLink.first( LHCbID(otchannelid).lhcbID() );
          while ( 0 != part ) {  
            info()<<" list of part pointer "<<part<<endreq;
            
            part = lhcbIdLink.next();
          }
          
          
          int station = otchannelid.station();
          int layer = otchannelid.layer();
          
          if( station == 1 ){
            if( layer == 0
                || layer == 3 )  hasT1x = true;
            if( layer == 1
                || layer == 2 )  hasT1u = true;
          }
          if( station == 2 ){
            if( layer == 0
                || layer == 3 )  hasT2x = true;
            if( layer == 1
                || layer == 2 )  hasT2u = true;
          }
          if( station == 3 ){
            if( layer == 0
                || layer == 3 )  hasT3x = true;
            if( layer == 1
                || layer == 2 )  hasT3u = true;
          }//station ==3

        }//if ottimes and mcpart equal
      }//if mcpart != 0
    }//loop ottimes
  }//if mcpart
  
            
   // get STClusters and its linkers
  const LHCb::STClusters* 
stclusters=get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
  LinkedTo<LHCb::MCParticle> 
itdigitasso(evtSvc(),msgSvc(),LHCb::STClusterLocation::ITClusters);
  
  if( NULL!=mcpart ){
    // loop over STClusters
    for( LHCb::STClusters::const_iterator iclu=stclusters->begin(); 
stclusters->end()!=iclu ; ++iclu ){
      // get mcpart associated to this 
      const LHCb::MCParticle* part = itdigitasso.first(*iclu);
      const LHCb::MCParticle* mcpart_st = NULL;
      double weight = 0.;
      while( NULL != part ){
        if( itdigitasso.weight() > weight ){
          weight = itdigitasso.weight(); 
          mcpart_st = part; 
        }
        part = itdigitasso.next();
      }// while association
      if( NULL!=mcpart_st ){
        if( mcpart==mcpart_st ){
          // retrieve STChannelID
          const LHCb::STChannelID stchannelid = (*iclu)->channelID();
          info()<<" st cha id "<<LHCbID(stchannelid)<<endreq;

          int station = stchannelid.station();
          int layer = stchannelid.layer();
          
          if( station == 1 ){
            if( layer == 1
                || layer == 4 )  hasT1x = true;
            if( layer == 2
                || layer == 3 )  hasT1u = true;
          }
          if( station == 2 ){
            if( layer == 1
                || layer == 4 )  hasT2x = true;
            if( layer == 2
                || layer == 3 )  hasT2u = true;
          }
          if( station == 3 ){
            if( layer == 1
                || layer == 4 )  hasT3x = true;
            if( layer == 2
                || layer == 3 )  hasT3u = true;
          }//station ==3

        }//if ottimes and mcpart equal
      }//if mcpart != 0
    }//loop ottimes
  }//if mcpart

  if( hasT1x 
      && hasT1u 
      && hasT2x 
      && hasT2u 
      && hasT3x 
      && hasT3u ) decision = true;
  
  return decision;
}


