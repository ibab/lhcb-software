// $Id: RichG4EventHitCount.cpp,v 1.1 2003-07-16 13:24:06 seaso Exp $
// Include files 



// local
#include "RichG4EventHitCount.h"
#include "RichG4Counters.h"
#include "RichG4AnalysisConstGauss.h"

// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "../SensDet/RichG4Hit.h"
//GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4TrajectoryPoint.hh"  
#include <vector>
#include <iterator>
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
//-----------------------------------------------------------------------------
// Implementation file for class : RichG4EventHitCount
//
// 2003-06-16 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4EventHitCount::RichG4EventHitCount(  ) {

  ChTkBetaSaturatedCut = 0.9999;
  
}
RichG4EventHitCount::~RichG4EventHitCount(  ) {
}

std::vector<G4int> RichG4EventHitCount::RichG4GetChTrajId
             (const G4Event* anEvent) 
{
  //  Get the track ID of all the charged particle trajectories.
  // excluding those of the photoelectrons.

   //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();
  }
  

  std::vector<int> aTraidVect;
  aTraidVect.reserve(n_trajectories);
  
  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {
     G4Trajectory* CurTraj=(G4Trajectory*)
                               ((*(anEvent->GetTrajectoryContainer()))[itraj]);

     G4int curtid =   CurTraj->GetTrackID();
     
     G4double curtkCharge=  CurTraj-> GetCharge();
     G4ThreeVector MomInit = CurTraj->  GetInitialMomentum();
       
     if(  curtkCharge != 0.0 ) {
   
       if( MomInit.z()  > 0.0 &&  MomInit.mag() > 1000.0 ) {

         aTraidVect.push_back(curtid) ;        
       }       
     }
  }
  return  aTraidVect;
}

bool RichG4EventHitCount::Rich1TrajTraverse
             (const G4Event* anEvent, int trajId) 
{
  //  Get the track ID of all the charged particle trajectories.
  // excluding those of the photoelectrons.
  bool atrav= false;
  G4double aZRange=50.0*mm;
  G4double aRich1MaxUpR= 200.0*mm;
  G4double aRich1MaxDnR= 500.0*mm;

      G4double MinMomAgelCut=5.0*GeV;
      //      G4double MinMomc4f10Cut=20.0*GeV;
  
  G4double aR1ZupA=ZUpsRich1Analysis-aZRange;
  G4double aR1ZupB=ZUpsRich1Analysis+aZRange;
   G4double aR1ZdnA=  ZDnsRich1Analysis-aZRange;
    G4double   aR1ZdnB= ZDnsRich1Analysis+aZRange; 
  
   //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();
  }
  

   
  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {
     G4Trajectory* CurTraj=(G4Trajectory*)
                               ((*(anEvent->GetTrajectoryContainer()))[itraj]);

     G4int ctid =   CurTraj->GetTrackID();
     G4double curtkCharge=  CurTraj-> GetCharge();

     G4ThreeVector initMomentum=  CurTraj->GetInitialMomentum();
     G4double InitTotMom=   initMomentum.mag();
        bool momHigh = true;
        if(  InitTotMom <     MinMomAgelCut) {
          
          momHigh=false;
          
        }
        
     
     if( (curtkCharge != 0.0)  && ( trajId == (int) ctid )) {
         
       int NumTjPoints = CurTraj-> GetPointEntries();
       G4ThreeVector InitPos , LastPos;
       G4double SlopeX, SlopeY;
       
       bool aSlpA=true;
       
       if(initMomentum.z() > 0.0 ) {
         
         SlopeX= (initMomentum.x()) /( initMomentum.z());
         SlopeY = (initMomentum.y())/(initMomentum.z());
         if( ( abs(SlopeX) <0.05) &&  ( abs(SlopeY) <0.05) ) {
           aSlpA=false;
           
         }

         if( ( abs(SlopeX) >0.25) && ( abs(SlopeY) >0.25)) {
           aSlpA=false;
           
         }
         
       }
       
       // G4cout<<"RichEv count slpx sply aslpA   "<< SlopeX
       //    <<"    "<< SlopeY<<"   "<< aSlpA<<G4endl;
       
     
  
       G4VTrajectoryPoint* InitPoint = CurTraj->  GetPoint(0);
       G4VTrajectoryPoint* LastPoint =  CurTraj->  GetPoint(NumTjPoints-1) ;       
       if(  InitPoint)  InitPos= InitPoint->   GetPosition() ;
       if( LastPoint )  LastPos = LastPoint ->   GetPosition();

       bool aTravRx=true;
       bool aTravRy=true;
       bool aTravRR=true;

       //       G4cout<<"NumTrajPt =  "<< NumTjPoints<<G4endl;
       
       for (int ipt=0 ; ipt< NumTjPoints; ipt++ ) {
         G4VTrajectoryPoint* aTjPoint =  CurTraj-> GetPoint(ipt);
         if (aTjPoint) {
           if( aTravRx &&  aTravRy &&  aTravRR && aSlpA) {
             
           G4ThreeVector aTjPos=   aTjPoint-> GetPosition() ;
           G4double aTjPosX= aTjPos.x();
            G4double aTjPosY= aTjPos.y();         
            G4double aTjPosZ= aTjPos.z();
            G4double  aTjPosR=0.0;
            
            if( ( aTjPosX !=0.0)  || ( aTjPosY !=0.0)){
              
              G4double  aTjPosR= pow((aTjPosX*aTjPosX+ aTjPosY*aTjPosY),0.5);
            }
            
               if( (  aTjPosZ > ZUpsRich1Analysis)  &&
                   (  aTjPosZ < ZDnsRich1Analysis)) {
                 //      G4cout<<"aTjPosR aTjPosZ  all  "<<  aTjPosR
                 //  << "   "<<aTjPosZ<<G4endl;

               }
               

              if( (  aTjPosZ > ZUpsRich1Analysis)  &&
                 (  aTjPosZ < ZUpsRich1Analysis)) {
           
                 if( aTjPosR< 50.0 ) {
                  //track in the beam pipe.
                   // G4cout<<"aTjPosR aTjPosZ  inbeampipe  "<<  aTjPosR
                   // << "   "<<aTjPosZ<<G4endl;
               
                  aTravRR= false;
                }
              }
              
             if( (aR1ZupA < aTjPosZ )  &&
                 (aR1ZupB > aTjPosZ )) {
               if( aTjPosR >  aRich1MaxUpR ) {

                 //   G4cout<<"aTjPosR aTjPosZ  upsend  "<<  aTjPosR
                 //  << "   "<<aTjPosZ<<G4endl;
               
                aTravRR= false;
 
               }
               
               
             }

             if( (aR1ZdnA < aTjPosZ )  &&
                 (aR1ZdnB >  aTjPosZ )) {
               if( aTjPosR >  aRich1MaxDnR ) {

                 //  G4cout<<"aTjPosR aTjPosZ  dnssend  "<<  aTjPosR
                 // << "   "<<aTjPosZ<<G4endl;
              
                aTravRR= false;
 
               }
               
               
             }
             

             
             //             if( (aTjPosZ > 990.0 && aTjPosZ < 2500.0) ||
             //    (aTjPosZ > 8500.0 &&  aTjPosZ<12000. ) ) {
             
             // G4cout<<"Trajectory XYZ Mom  "<<   aTjPosX
             //   <<"  "<< aTjPosY<<"   "<< aTjPosZ
             //     <<"   "<<InitTotMom<<G4endl;
              
             //  }

 
             
           }
           
           
           
         }
         
           
         
         
         
       }
       
       
       
       //       G4cout<<" Traj InitPos XYZ LastPos XYZ  "<<
       //  InitPos.x()<<"  "<<  InitPos.y()<<"   "<<InitPos.z()
       //    <<"   "<< LastPos.x()<<"    "<< LastPos.y()<<"    "<<
       //   LastPos.z()<<G4endl;

       //       G4cout<<"InitMom XYZ SlopeX SlopeY  "<< initMomentum.x()<<"   "<<
       //  initMomentum.y()<<"    "<<initMomentum.z()
       //    <<"   "<<SlopeX<<"    "<<SlopeY <<G4endl;

       //    G4cout<<"aTravRx  aTravRy     aTravRR aSlpA "<<
       //  aTravRx<<"   "<< aTravRy<<"   "<< aTravRR
       //    <<"   "<< aSlpA
       //    << "    "<< momHigh <<G4endl;
       

         if(aTravRx && aTravRy && aTravRR &&  aSlpA && momHigh ) {            
           atrav =true;
         
         }
         
     }     
     
  }
  
  return  atrav;
  
}

bool RichG4EventHitCount::Rich2TrajTraverse
             (const G4Event* anEvent, int trajId) 
{
  //  Get the track ID of all the charged particle trajectories.
  // excluding those of the photoelectrons.
  bool atrav= false;
  G4double InitMomCut=50.0*GeV;
  
   //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();
  }
  

   
  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {
     G4Trajectory* CurTraj=(G4Trajectory*)
                               ((*(anEvent->GetTrajectoryContainer()))[itraj]);

     G4int ctid =   CurTraj->GetTrackID();
     G4double curtkCharge=  CurTraj-> GetCharge();

     G4ThreeVector initMomentum=  CurTraj->GetInitialMomentum();
     G4double initTotMom= initMomentum.mag();

     //     G4cout<<"chtk init Mom and cut  "<<initTotMom
     //   <<"   "<< InitMomCut <<G4endl;
     
     if( initTotMom  > InitMomCut ) {
       
     
     if( (curtkCharge != 0.0)  && ( trajId == (int) ctid )) {
         
       int NumTjPoints = CurTraj-> GetPointEntries();
       G4ThreeVector InitPos , LastPos;
       G4double SlopeX, SlopeY;
       
       if(initMomentum.z() > 0.0 ) {
         
         SlopeX= (initMomentum.x()) /( initMomentum.z());
         SlopeY = (initMomentum.y())/(initMomentum.z());
       }
  
     
  
       G4VTrajectoryPoint* InitPoint = CurTraj->  GetPoint(0);
       G4VTrajectoryPoint* LastPoint =  CurTraj->  GetPoint(NumTjPoints-1) ;       
       if(  InitPoint)  InitPos= InitPoint->   GetPosition() ;
       if( LastPoint )  LastPos = LastPoint ->   GetPosition();

       bool aTravRx=true;
       bool aTravRy=true;
       
       for (int ipt=0 ; ipt< NumTjPoints; ipt++ ) {
         G4VTrajectoryPoint* aTjPoint =  CurTraj-> GetPoint(ipt);
         if (aTjPoint) {
           G4ThreeVector aTjPos=   aTjPoint-> GetPosition() ;
           G4double aTjPosX= aTjPos.x();
            G4double aTjPosY= aTjPos.y();         
            G4double aTjPosZ= aTjPos.z();         
             
           if( aTjPosZ > ZUpsRich2Analysis && 
               aTjPosZ <   ZDnsRich2Analysis) {
             G4double CurSx=  aTjPosX/ aTjPosZ;
             G4double CurSy=  aTjPosY/ aTjPosZ;

             if(( abs(aTjPosX) < 5.0 ) || (abs(aTjPosY) < 5.0 ) ) {
               //track in the beam pipe.
               
                aTravRx= false;
             }             

             if(abs(CurSx)< 0.02 || abs(CurSx) > 0.20) {
               aTravRx= false;
               
             }
             if(abs(CurSy)< 0.02 || abs(CurSy) > 0.20) {
               aTravRy= false;
               
             }

             
             
           }
           
           
           
         }
         
         
       }
       
       
       //       G4cout<<" Traj InitPos XYZ LastPos XYZ  "<<
       //  InitPos.x()<<"  "<<  InitPos.y()<<"   "<<InitPos.z()
       //    <<"   "<< LastPos.x()<<"    "<< LastPos.y()<<"    "<<
       //   LastPos.z()<<G4endl;

       //       G4cout<<"InitMom XYZ SlopeX SlopeY  "<< initMomentum.x()<<"   "<<
       //  initMomentum.y()<<"    "<<initMomentum.z()
       //    <<"   "<<SlopeX<<"    "<<SlopeY <<G4endl;
       


       if(  InitPos.z() < ZUpsRich2Analysis && 
            LastPos.z() >  ZDnsRich2Analysis ) {
         //         if( LastPos.x() < 9000.0 ) {
           
         //   if(aTravRx && aTravRy ) {
           
         if( ( ( abs(SlopeX) > 0.02) && ( abs(SlopeX) < 0.120))  
             || ((abs(SlopeY) > 0.02) && (abs(SlopeY) < 0.120))) {
                      
            
           atrav =true;

         }
         //         }
         
         
         //       }
         
         
       }
       
       
       
       
     }
     
     
     }
     
  }
  
  

  
  return  atrav;
  
}

  

void RichG4EventHitCount::RichG4CountSaturatedHits(
      const G4Event* anEvent,  int NumRichColl,
      std::vector<int> RichG4CollectionID ) 
    {
 // first get all the track ids
      //  std::vector<int> TrakIDVect = RichG4GetChTrajId(anEvent);
  // Now find the number of tracks creating hits in RICH.
      std::vector<int>  TrajIdVect;
      std::vector<int>  TrajIdVectR2;
      TrajIdVect.clear();
      TrajIdVect.reserve(100);
      TrajIdVectR2.clear();
      TrajIdVectR2.reserve(100);
       G4double MinMomAgelCut=5.0*GeV;
      G4double MinMomc4f10Cut=5.0*GeV;
       G4double MaxMomAgelCut=15.0*GeV;
     
      
    G4HCofThisEvent * HCE;
    RichG4Counters* aRichCounter =  RichG4Counters::getInstance();
    if(aRichCounter ) {
    
      G4int NumRichCollection= NumRichColl;
  
     int Current_RichG4CollectionID=0;
      for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
      Current_RichG4CollectionID =RichG4CollectionID[ihcol];
       if(Current_RichG4CollectionID >=0 ) {
        HCE = anEvent->GetHCofThisEvent();
        RichG4HitsCollection* RHC=NULL;
        if(HCE){
          RHC = (RichG4HitsCollection*)(HCE->
                                        GetHC( Current_RichG4CollectionID));
        }
        if(RHC){
          G4int nHitInCurColl = RHC->entries();
          for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {

          RichG4Hit* aHit = (*RHC)[iha];
          int ChtkId =  (int) (aHit-> GetChTrackID());

          //          G4cout<<" TRkId for rich1 hits  "<< ChtkId <<G4endl;

          bool trajAlreadyStored=true;;
          bool trajtraversedRich1=false;

          bool trajAlreadyStoredR2=true;;
          bool trajtraversedRich2=false;
          
          if(TrajIdVect.size() > 0 ) {
            
           std::vector<int>::iterator p = find(TrajIdVect.begin(),
                                                TrajIdVect.end(), 
                                                ChtkId);
           if( p == TrajIdVect.end() ) trajAlreadyStored=false;
           

          } else {

             trajAlreadyStored=false;          
            
          } 

          if(TrajIdVectR2.size() > 0 ) {
            
           std::vector<int>::iterator p = find(TrajIdVectR2.begin(),
                                                TrajIdVectR2.end(), 
                                                ChtkId);
           if( p == TrajIdVectR2.end() ) trajAlreadyStoredR2=false;
           

          } else {

             trajAlreadyStoredR2=false;          
            
          } 



          if( !( trajAlreadyStored) ) {

            // Now loop through the trajectory container to see
            // this track goes through the whle length of Rich1.
            // This is done by checking if the first point of the
            // trajectory i supstream of Rich1 and the last point is
            // downstream of Rich1.
            
            
            trajtraversedRich1= Rich1TrajTraverse(anEvent, ChtkId );
            
             if( trajtraversedRich1) {
                
                TrajIdVect.push_back(ChtkId);
              }
             
          }

          // now for rich2           
          if( !( trajAlreadyStoredR2) ) {

            // Now loop through the trajectory container to see
            // this track goes through the whle length of Rich2.
            // This is done by checking if the first point of the
            // trajectory i supstream of Rich2 and the last point is
            // downstream of Rich2.
            
            
            trajtraversedRich2= Rich2TrajTraverse(anEvent, ChtkId );
            
             if( trajtraversedRich2) {
                
                TrajIdVectR2.push_back(ChtkId);
              }
             
          }
          


          
          
          }  // end loop through hits.         
        }
        
        
        
       }
       
       
       
      }  // end loop over collections


             G4cout<<"TrajIdVectSize =  "<< TrajIdVect.size()<<G4endl;
             G4cout<<"TrajIdVectRich2Size =  "<< TrajIdVectR2.size()<<G4endl;
       //     G4cout<<"TrajIdVectSize before erase "<< TrajIdVect.size()<<G4endl;
      
      //       for (int ihcola=0; ihcola<NumRichCollection; ihcola++) {
            
      //    Current_RichG4CollectionID =RichG4CollectionID[ihcola];
      //   HCE = anEvent->GetHCofThisEvent();
      //   RichG4HitsCollection* RHCA=NULL;
      //   if(HCE){
      //    RHCA = (RichG4HitsCollection*)(HCE->
      //                       GetHC( Current_RichG4CollectionID));
      //   }
      //  if(RHCA){
      //    G4int nHitInCurCollb = RHCA->entries();
      //     for (G4int ihab=0; ihab<nHitInCurCollb ; ihab++ ) {

      //          RichG4Hit* abHit = (*RHCA)[ihab];
      //    int aPeId = (int) (abHit->GetPETrackID()) ;

      //          vector<int>::iterator q = find(TrajIdVect.begin(),
      //                       TrajIdVect.end(),aPeId);
      //    if(q != TrajIdVect.end()) TrajIdVect.erase(q) ;
          
           
      //    }
      //     
      //  }
       
      //  } //end loop over collections
 

      //      G4cout<<"TrajIdVectSize after erase "<< TrajIdVect.size()<<G4endl;

    
      int NumTraj=  TrajIdVect.size();
      
      std::vector<int>TrajNumHitGasRich1( NumTraj,0);      
      std::vector<int>TrajSatNumHitGasRich1( NumTraj,0);      
      std::vector<int>TrajNumHitAgelRich1( NumTraj,0);      
      std::vector<int>TrajSatNumHitAgelRich1( NumTraj,0);      


      int NumTrajR2=  TrajIdVectR2.size();
      std::vector<int>TrajNumHitGasRich2( NumTrajR2,0);      
      std::vector<int>TrajSatNumHitGasRich2( NumTrajR2,0);      
      
       for (int ihcolb=0; ihcolb<NumRichCollection; ihcolb++) {
            
       Current_RichG4CollectionID =RichG4CollectionID[ihcolb];
        HCE = anEvent->GetHCofThisEvent();
        RichG4HitsCollection* RHCB=NULL;
        if(HCE){
          RHCB = (RichG4HitsCollection*)(HCE->
                             GetHC( Current_RichG4CollectionID));
        }
       if(RHCB){
          G4int nHitInCurCollb = RHCB->entries();
          for (G4int ihb=0; ihb<nHitInCurCollb ; ihb++ ) {

          RichG4Hit* bHit = (*RHCB)[ihb];

          G4double aChTrackTotMom =  bHit->ChTrackTotMom() ;
          int ChtkId =  (int) (bHit-> GetChTrackID()) ;
          G4int aRadiatorNum=  bHit->GetRadiatorNumber();
          
          G4double ChTkPDGMass =  bHit->RichChTrackMass();
          
          G4double ChTkEnergy = 
            pow( (ChTkPDGMass*ChTkPDGMass+ aChTrackTotMom* aChTrackTotMom),0.5);
          G4double ChTkBeta=0.0;
          
          if( ChTkEnergy > 0.0 ) {
            
            ChTkBeta = aChTrackTotMom/ChTkEnergy;
            //             G4cout<<"trackId  Beta  Mom Mass  "
            //    <<ChtkId<<"   "<< ChTkBeta <<"    "
            //    <<aChTrackTotMom<<"    "
            //    <<ChTkPDGMass
            //    <<G4endl;
             
          }
          
                   
          int it=0;
          while(it < (int) TrajIdVect.size()) {
            
            if(TrajIdVect[it] ==  ChtkId) {
              if(aRadiatorNum == 0 ) {
                
              TrajNumHitAgelRich1[it]++;
            
              if( ChTkBeta >  ChTkBetaSaturatedCut) {

               if(  ( aChTrackTotMom >  MinMomAgelCut)  &&  
                    ( aChTrackTotMom <  MaxMomAgelCut ) ) {
                  
                  TrajSatNumHitAgelRich1[it]++;

                 }
                  
              }
              
              }else if( aRadiatorNum == 1 ) {
              
              TrajNumHitGasRich1[it]++;

              if( ChTkBeta >  ChTkBetaSaturatedCut) {

                 if(aChTrackTotMom > MinMomc4f10Cut ) {

                  TrajSatNumHitGasRich1[it]++;
                }
                  
              }
              

              
              }
              
              
              
              
              it =  TrajIdVect.size() +1;              
              
            }
          
            it++;
            
          } // end loop over traj ID


          //          G4cout<<" Loooping through Rich2 vect "<<G4endl;
          
          int it2=0;
          while(it2 < (int) TrajIdVectR2.size()) {
            
            if(TrajIdVectR2[it2] ==  ChtkId) {
              if(aRadiatorNum == 2 ) {
                
              TrajNumHitGasRich2[it2]++;
  
              //              G4cout<<" Beta and BetaCut  "
              //    << ChTkBeta<<"   "<< ChTkBetaSaturatedCut<<G4endl;

             if( ChTkBeta >  ChTkBetaSaturatedCut) {
              //              if( ChTkBeta >  0.9999 ) {

                //                if(  aChTrackTotMom > 10000 ) {
                  
                  TrajSatNumHitGasRich2[it2]++;                
                  //  }
                  
              }
              
              
              
              }
              
              
              

              
              it2 =  TrajIdVectR2.size() +1;              
              
            }
            
            
              
          
            it2++;
            
          } // end loop over TrajIdVectR2.
          
          
          }  // end loop over hits in a coll.
          
          
       }
       
       
       
       }   //end loop over collections       



       aRichCounter-> setNumHitPerTrackRich1Gas(TrajNumHitGasRich1);
       aRichCounter-> setNumHitSaturatedPerTrackRich1Gas(TrajSatNumHitGasRich1);
        aRichCounter-> setNumHitPerTrackRich1Agel( TrajNumHitAgelRich1);
         aRichCounter-> 
           setNumHitSaturatedPerTrackRich1Agel(TrajSatNumHitAgelRich1);
       aRichCounter-> setNumHitPerTrackRich2Gas(TrajNumHitGasRich2);
       aRichCounter-> setNumHitSaturatedPerTrackRich2Gas(TrajSatNumHitGasRich2);

       // for test 
       
       //       G4cout<<"Num Sat traj Rich2 "
       //   <<  aRichCounter->NumHitSaturatedPerTrackRich2Gas().size() <<G4endl;
       
         // now to test the procedure         
         //        for(int i=0; i< (int) TrajIdVect.size(); i++ ) {
 
           //           G4cout<<"TrackInList GasSatHitNum AgelSatHitNum=   "
           //    <<i<<"  "<<TrajSatNumHitGasRich1[i]<<"   "
           //    << TrajSatNumHitAgelRich1[i]
           //    <<"     TotHitPerTrackin Gas Agel =  "
           //    <<  TrajNumHitGasRich1[i]<<"    "
           //    << TrajNumHitAgelRich1[i]<<G4endl;
         //        }
           
       //       for(int ia=0; ia< (int)  TrajSatNumHitGasRich2.size(); ia++ ) {

       //         G4cout<<  "TrackInListR2    Rich2GasSatHitNum  "
       //      <<ia<<"    "<< TrajSatNumHitGasRich2[ia]<<G4endl;
         
         
       //  }
       
       
       
           
       
    }
    

    //    G4cout<<"End of   RichG4CountSaturatedHits " <<G4endl;
    
    };
  
  
   
void RichG4EventHitCount::RichG4CountAndClassifyHits(
      const G4Event* anEvent,  int NumRichColl,
      std::vector<int> RichG4CollectionID ) 
{

    G4HCofThisEvent * HCE;
    RichG4Counters* aRichCounter =  RichG4Counters::getInstance();
    if(aRichCounter ) {
      
      G4cout<<"Begin  Rich hit count "<< G4endl;
      
      
     G4int NumRichCollection= NumRichColl;
  
     int Current_RichG4CollectionID=0;
  
     for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
      Current_RichG4CollectionID =RichG4CollectionID[ihcol];
    
      if(Current_RichG4CollectionID >=0 ) {
        HCE = anEvent->GetHCofThisEvent();
        RichG4HitsCollection* RHC=NULL;
        if(HCE){
          RHC = (RichG4HitsCollection*)(HCE->
                           GetHC( Current_RichG4CollectionID));
        }
        if(RHC){
          G4int nHitInCurColl = RHC->entries();
          //        G4cout<<"EndEvAction      "<< nHitInCurColl
          //     <<"   are stored in RichHitCollection set   "
          //      <<ihcol<< G4endl;

          for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {

          RichG4Hit* aHit = (*RHC)[iha];

          G4int aRadiatorNum=  aHit->GetRadiatorNumber();
          // Momentum of charged track in MeV
          G4double aChTrackTotMom =  aHit->ChTrackTotMom() ;
          G4double CkvTheta=      aHit->  ThetaCkvAtProd();
          G4int ChtkId =  aHit-> GetChTrackID();
          G4double ChTkPDGMass =  aHit->RichChTrackMass();
          
          G4double ChTkEnergy = 
            pow( (ChTkPDGMass*ChTkPDGMass+ aChTrackTotMom* aChTrackTotMom),0.5);
          G4double ChTkBeta=0.0;
          
          if( ChTkEnergy > 0.0 ) {
            
            ChTkBeta = aChTrackTotMom/ChTkEnergy;
          }
          
          //    cout<<" RichEventHitCount RadNum ChTkId=   "
          //    << aRadiatorNum 
          //    <<"    "<<ChtkId 
          //    <<"  ChtrackMom = "<<aChTrackTotMom<<  endl;
          if(ihcol == 0 || ihcol == 1 ) {
              
            aRichCounter->bumpNumHitTotRich1All();
            
          }

          if(ihcol == 2 || ihcol == 2 ) {
              
            aRichCounter->bumpNumHitTotRich2All();
            
          }

          
          if(   aRadiatorNum == 1 ){
  
            aRichCounter->bumpNumHitTotRich1Gas();

            if(  ChtkId <= 1 ) {
            
              aRichCounter->bumpNumHitPartGunPrimaryPartRich1Gas();
              
            }
            
          }else if(  aRadiatorNum == 0 ) {
          
            aRichCounter->bumpNumHitTotRich1Agel();            
          
            if(  ChtkId <= 1 ) {

              aRichCounter->bumpNumHitPartGunPrimaryPartRich1Agel();
              
            }
            
          } else if (  aRadiatorNum == 2 ) {

            aRichCounter->bumpNumHitTotRich2Gas();

            if(  ChtkId <= 1 ) {
            
              aRichCounter->bumpNumHitPartGunPrimaryPartRich2Gas();
              
            }
            

            
          }
          
          
          
          
          
          }
          
          
          
        }
        
      }
      
     }
     
    }

      G4cout<<"End  Rich hit count "<< G4endl;

     
}


    





      
//=============================================================================
