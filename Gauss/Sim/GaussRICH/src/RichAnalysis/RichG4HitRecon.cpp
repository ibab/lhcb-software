// $Id: RichG4HitRecon.cpp,v 1.1 2004-02-04 13:53:00 seaso Exp $
// Include files 



// local
#include "RichG4HitRecon.h"
#include "RichG4CkvRecon.h"
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
#include "../SensDet/RichG4Hit.h"
#include "RichG4ReconFlatMirr.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4Counters.h"

// local
#include "RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HitRecon
//
// 2003-09-11 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HitRecon::RichG4HitRecon( ):  m_RichG4CkvRec (0) , 
  m_RichG4ReconFlatMirr(0)  {


  m_agelnominalrefindex = 1.03;
  m_c4f10nominalrefindex = 1.0014;
  m_chtkBetaSaturatedCut =0.9999;
  
  m_MidRich1GasZ = (C4F10ZBeginAnalysis+C4F10ZEndAnalysis)*0.5;
  
  m_MidRich1AgelZ  = (AgelZBeginAnalysis+AgelZEndAnalysis)*0.5;
  
  m_MidRich2GasZ  = (CF4ZBeginAnalysis+CF4ZEndAnalysis)*0.5;

   m_useOnlySaturatedHits = true;
   m_useMidRadiatorZEmisPt = false;

   m_RichG4ReconResult= new RichG4ReconResult();
   

}
RichG4HitRecon::~RichG4HitRecon(  ) {
}

void RichG4HitRecon::setRichG4CkvRec () 
{
  m_RichG4CkvRec = new RichG4CkvRecon();
  
  
}
void RichG4HitRecon::setRichG4FlatMirr()
{
  m_RichG4ReconFlatMirr = new RichG4ReconFlatMirr();
  
}
RichG4ReconFlatMirr*  RichG4HitRecon::getRichG4ReconFlatMirr() 
{
  return  m_RichG4ReconFlatMirr;
  
}


void RichG4HitRecon::RichG4ReconstructCherenkovAngle(
 const G4Event* anEvent,  int NumRichColl, 
 std::vector<int> RichG4CollectionID ) 
{


  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4HitReconlog( msgSvc,"RichG4HitRecon");
  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();

  //  RichG4HitReconlog<<MSG::INFO<<
  //  "Now reconstruting ckv angle"<<endreq;
   
   std::vector<int> TkIdVectRich1Gas = 
                  aRichCounter -> TrackIdTraverseRich1Gas();
   std::vector<int> TkIdVectRich1Agel = 
                  aRichCounter -> TrackIdTraverseRich1Agel();
   std::vector<int> TkIdVectRich2Gas = 
                  aRichCounter -> TrackIdTraverseRich2Gas();

   int NumTkIdRich1Agel = TkIdVectRich1Agel.size();
   int NumTkIdRich1Gas  = TkIdVectRich1Gas.size();
   int NumTkIdRich2Gas =  TkIdVectRich2Gas.size();
   
  int irichdet=-1;
 
 
   G4HCofThisEvent * HCE;
   G4int NumRichCollection= NumRichColl;
     int Current_RichG4CollectionID=0;
      for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
        if(ihcol ==0 || ihcol == 1 ){
          irichdet=0;
        }else if ( ihcol ==2 || ihcol == 3 ){
          irichdet =1;
          
        }
        
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

            // in case saturated hits are needed, first
            // check if the current hit is a saturated hit.

            
            // now to start the reconstruction.

            G4int aPixelXNum = aHit-> GetCurPixelXNum();            
            G4int aPixelYNum = aHit-> GetCurPixelYNum();
            
            G4ThreeVector LocalHitCoord = aHit->GetLocalPos();
            G4int anHpdNum =    aHit-> GetCurHpdNum();
            G4int aRichDetNum = aHit->  GetCurRichDetNum();
            G4ThreeVector EmissPt =  aHit-> GetPhotEmisPt();
            G4ThreeVector TkMom = aHit->ChTrackMomVect() ;
            G4int aRadiatornum= aHit->GetRadiatorNumber();
            G4double ChTkPDGMass =  aHit->RichChTrackMass();
            G4double aChTrackTotMom =  aHit->ChTrackTotMom() ;
            G4ThreeVector aChTrackPreStepPos = aHit->ChTrackCkvPreStepPos();
            G4ThreeVector aChTrackPostStepPos = aHit->ChTrackCkvPostStepPos();
            G4int aPhotRayleighFlag=  aHit->OptPhotRayleighFlag();
            G4ThreeVector LocalPhcathCoord = aHit -> GetLocalPEOriginPos();
            G4ThreeVector PhotonAerogelExitPosition = 
                                   aHit ->OptPhotAgelExitPos();
            
            
            G4double ChTkEnergy = 
            pow( (ChTkPDGMass*ChTkPDGMass+ aChTrackTotMom* aChTrackTotMom),0.5);
            G4double ChTkBeta=0.0;
            if( ChTkEnergy > 0.0 ) {
            
            ChTkBeta = aChTrackTotMom/ChTkEnergy;
            }
           int ChtkId =  (int) (aHit-> GetChTrackID()) ;
         
            
            G4ThreeVector GlobalPhcathCoord =   aHit->GetGlobalPEOriginPos();

            // in case saturated hits are needed, first
            // check if the current hit is a saturated hit.

            bool SelectThisHit = false;
            if(!(m_useOnlySaturatedHits) ) {
              SelectThisHit= true;
              
            }else {

              // first for agel saturated  hits
                 int itagel=0;

                 while (aRadiatornum == 0 && itagel < NumTkIdRich1Agel ) {

                   if( TkIdVectRich1Agel[itagel] ==  ChtkId ) {
                    // select only those are not rayleigh scattered.

                     if(aPhotRayleighFlag == 0 ) {
                       
                      if( ChTkBeta >   m_chtkBetaSaturatedCut ) {
                        SelectThisHit= true;
                     
                      }
                     }
                     
                      // skip out 
                      itagel = NumTkIdRich1Agel+1;
                   }
                   
                   
                   itagel++;
                   
                   
                 }
                 
              // now  for rich1gas saturated  hits
                 int itr1s=0;

                 while (aRadiatornum == 1 && itr1s < NumTkIdRich1Gas ) {
                   if( TkIdVectRich1Gas[itr1s] ==  ChtkId ) {
                    
                     if( ChTkBeta >   m_chtkBetaSaturatedCut ) {
                       SelectThisHit= true;
                     
                      }
                  
                      // skip out 
                      itr1s = NumTkIdRich1Gas+1;
                   }
                   
                   
                   itr1s++;
                   
                   
                 }
                 
               // now  for rich2gas saturated  hits
                 int itr2s=0;

                 while (aRadiatornum == 2 && itr2s < NumTkIdRich2Gas ) {
                   if( TkIdVectRich2Gas[itr2s] ==  ChtkId ) {
                    
                     if( ChTkBeta >   m_chtkBetaSaturatedCut ) {
                       SelectThisHit= true;
                     
                      }
                  
                      // skip out 
                      itr2s = NumTkIdRich2Gas+1;
                   }
                   
                   
                   itr2s++;
                   
                   
                 }               
                
                
                
            } //  end of all test for sat hits. 

            if(  SelectThisHit ) {

             //             RichG4HitReconlog<<MSG::INFO<<
            //  "For the RichG4Hit hitnum  RichDet collection hpd "
            //                  <<iha<<"   "<<aRichDetNum<<"   "
            //                  << ihcol<<"   "<< anHpdNum
            //   <<endreq;             

            // RichG4HitReconlog<<MSG::INFO<<" Sidet Pixel X Y num are "
            //                  <<aPixelXNum<<"   "<<aPixelYNum<<endreq;
             

            // RichG4HitReconlog<<MSG::INFO<<" Local ph cath coord is "
            //                 << LocalPhcathCoord.x()<<"    "
            //                 <<  LocalPhcathCoord.y()<<"    "
            //                 <<  LocalPhcathCoord.z()<<endreq; 
          
             
            // now convert to clhep like param and store then in
            // RichG4CkvRec.

            // RichG4HitReconlog<<MSG::INFO<<" EmisPt is " 
            //                  <<EmissPt.x()<<"   "
            //                  <<EmissPt.y()<<"   "
            //                  <<EmissPt.z()<<endreq;

            //  RichG4HitReconlog<<MSG::INFO<<" Rich1 Mid radiator for 0 1 2 rad is "
            //                  << m_MidRich1AgelZ<<"  "
            //                  <<   m_MidRich1GasZ<<"   "
            //                  <<  m_MidRich2GasZ<<endreq;

             // RichG4HitReconlog<<MSG::INFO<<"TkMom is  "
             //                 <<TkMom.x()<<"   "
             //                <<TkMom.y()<<"   "
             //                 <<TkMom.z()<<endreq;


             

             m_RichG4CkvRec->SetCurrentTkMom (  TkMom.x(),  TkMom.y(),
                                 TkMom.z());
             m_RichG4CkvRec->SetCurrentRichDetNum(aRichDetNum );
             m_RichG4CkvRec->SetCurrentHpdNum(anHpdNum);
             m_RichG4CkvRec->SetCurrentRichSector(ihcol);
             m_RichG4CkvRec->
             SetChTrackPreStepPosition( aChTrackPreStepPos.x(),
                                        aChTrackPreStepPos.y(),
                                        aChTrackPreStepPos.z());
             
             m_RichG4CkvRec->
               SetChTrackPostStepPosition(aChTrackPostStepPos.x(),
                                          aChTrackPostStepPos.y(),
                                          aChTrackPostStepPos.z());
             
          
              // Now store the flat mirror plane type.
             int aFlatMirrtype= ihcol;             
             m_RichG4CkvRec->SetCurrentFlatMirrorType(aFlatMirrtype);

             // end of storage in RichG4CkvRec.

             // now to get the appropriate emission point.

             HepPoint3D EmisPtUseTrueEmissPt =
                HepPoint3D(EmissPt.x(),EmissPt.y(),EmissPt.z()  );

             HepPoint3D EmisPtUseAgelExit =  HepPoint3D(EmissPt.x(),
                                           EmissPt.y(),EmissPt.z() );

             
             // double emisptReconZ =  EmissPt.z();
             double emisptReconX= 
                   (aChTrackPreStepPos.x()+aChTrackPostStepPos.x())*.5;
             double emisptReconY= 
                   (aChTrackPreStepPos.y()+aChTrackPostStepPos.y())*.5;
             double emisptReconZ= 
                   (aChTrackPreStepPos.z()+aChTrackPostStepPos.z())*.5;
             
             // bool setMidPtRadiator = true;
             //  bool setMidPtRadiator = false;

             // bool  setMidPtRadiator =   m_useMidRadiatorZEmisPt;
             

               
             if( aRadiatornum == 0 ) {
               // emisptReconZ =  m_MidRich1AgelZ;

               if( PhotonAerogelExitPosition.z() > 0.0 ) {
                 
                 EmisPtUseAgelExit = PhotonAerogelExitPosition;
                 // RichG4HitReconlog<<MSG::INFO<<
                 //  " Agel Exit pos "<<EmisPtUseAgelExit.x()
                 //      <<"   " <<EmisPtUseAgelExit.y()
                 //      <<"    "<< EmisPtUseAgelExit.z()<<endreq;
                 
                 


                 // EmisPtUseAgelExit  =  m_RichG4CkvRec->
                 // getPhotAgelExitZ(EmissPt.x(),EmissPt.y(),EmissPt.z(),
                 // aHit );
               
               }
               
             }else if( aRadiatornum == 1 ) {
               //  emisptReconZ =   m_MidRich1GasZ;
               
             } else if ( aRadiatornum == 2 ) {
               //  emisptReconZ = m_MidRich2GasZ;
               
             }

             HepPoint3D EmisPtUseMidPtRadiatorZ =  
                HepPoint3D(EmissPt.x(),EmissPt.y(),emisptReconZ   );

             HepPoint3D EmisPtUseMidPtRadiator =  
                HepPoint3D(emisptReconX,emisptReconY,emisptReconZ   );
             
             // now to get the detection point.
             // For reflecting  the coord wrt the flat mirror.
             RichG4ReconFlatMirr* aFlatMirr = m_RichG4ReconFlatMirr;

             // first get the point on the photodetector  and then perform the
             // reflection.


             // First get the pixel local coord on sidet from the
             // pixel x y num (column row num) or as the true  localhit coord.
            // Now reconstruct the hit coord onto the hpd Ph cathode plane.
             // option 1
              
              HepPoint3D aLocalHitFromPixelNum = 
                m_RichG4CkvRec-> 
                GetSiHitCoordFromPixelNum(aPixelXNum,aPixelYNum);             
              HepPoint3D aHitOnQwFromPixelNum= m_RichG4CkvRec->
                 ReconPhCoordFromLocalCoord( aLocalHitFromPixelNum);

              HepPoint3D aDetPointFromPixelNum =  aFlatMirr->
               FlatMirrorReflect(aHitOnQwFromPixelNum ,aFlatMirrtype);

              // option2

             HepPoint3D aTrueLocalHit( LocalHitCoord.x(),
                                   LocalHitCoord.y(), LocalHitCoord.z());
             HepPoint3D aHitOnQwFromTrueLocalHit= m_RichG4CkvRec->
                 ReconPhCoordFromLocalCoord( aTrueLocalHit );

            HepPoint3D aDetPointFromTrueLocalHit =  aFlatMirr->
               FlatMirrorReflect( aHitOnQwFromTrueLocalHit ,aFlatMirrtype);

             // option3

             HepPoint3D  aHitOnQwFromGlobalPhCathode (GlobalPhcathCoord.x(),
                                   GlobalPhcathCoord.y(),
                                   GlobalPhcathCoord.z());
             
            HepPoint3D aDetPointFromGlobalPhCathode =  aFlatMirr->
               FlatMirrorReflect( aHitOnQwFromGlobalPhCathode,aFlatMirrtype);

            //  Now there are three options for getting the detection point
            //  and two options for getting the emission point. In aerogel
            // there is an additional option for using the exit point from aerogel as
            // emission point.                               
            // the options for detection point are: 
            // (D1) from pixelnum, 
            // (D2) from the true hit coordinate, 
            // (D3) from the photocathode hit coord.
            // The options for the emission point are:
            //  (E1) the true emission point,
            //   (E2) mid point of the radiator Z.
            //   (E3) Aerogel exit point.
            //   (E4) mid point of the radiator.

            HepPoint3D aReflPointD1E1= HepPoint3D (0.0,0.0,0.0);            
            HepPoint3D aReflPointD2E1= HepPoint3D (0.0,0.0,0.0);
            HepPoint3D aReflPointD3E1= HepPoint3D (0.0,0.0,0.0);
            HepPoint3D aReflPointD3E2= HepPoint3D (0.0,0.0,0.0);
            HepPoint3D aReflPointD1E2= HepPoint3D (0.0,0.0,0.0);            
            HepPoint3D aReflPointD3E4= HepPoint3D (0.0,0.0,0.0);
            HepPoint3D aReflPointD1E4= HepPoint3D (0.0,0.0,0.0);            


            HepPoint3D aReflPointD1E3= HepPoint3D (0.0,0.0,0.0);
            HepPoint3D aReflPointD2E3= HepPoint3D (0.0,0.0,0.0);
            HepPoint3D aReflPointD3E3= HepPoint3D (0.0,0.0,0.0);


            m_RichG4ReconResult->InitReconResult();
            
            m_RichG4ReconResult-> setradiatorForCkv(aRadiatornum);
            
            if( aRadiatornum == 1 ||  aRadiatornum == 2 ) {
              
   
            aReflPointD1E1 =  m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum , 
                                               EmisPtUseTrueEmissPt );

            aReflPointD2E1 =  m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror( aDetPointFromTrueLocalHit  , 
                                                EmisPtUseTrueEmissPt );

             aReflPointD3E1 =m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode  ,
                                              EmisPtUseTrueEmissPt );

             aReflPointD3E2 = 
             m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode  ,
                                               EmisPtUseMidPtRadiatorZ );

             aReflPointD3E4 = 
             m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode  ,
                                               EmisPtUseMidPtRadiator );

             aReflPointD1E2 =  m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum , 
                                               EmisPtUseMidPtRadiatorZ );
             aReflPointD1E4 =  m_RichG4CkvRec->
             ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum , 
                                               EmisPtUseMidPtRadiator );


            // calculate the cherenkov angle in gas radiators.
             m_RichG4ReconResult->setckvAngleD1E1(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD1E1,
                               EmisPtUseTrueEmissPt ));

             m_RichG4ReconResult->setckvAngleD2E1(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD2E1,
                            EmisPtUseTrueEmissPt ));
             m_RichG4ReconResult->setckvAngleD3E1(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD3E1,
                              EmisPtUseTrueEmissPt ));

             m_RichG4ReconResult->setckvAngleD3E2(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD3E2,
                          EmisPtUseMidPtRadiatorZ ));

             m_RichG4ReconResult->setckvAngleD3E4(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD3E4,
                          EmisPtUseMidPtRadiator ));

             m_RichG4ReconResult->setckvAngleD1E2(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD1E2 ,
                          EmisPtUseMidPtRadiatorZ ));

             m_RichG4ReconResult->setckvAngleD1E4(
              m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD1E4 ,
                          EmisPtUseMidPtRadiator ));
           
            }else if(  aRadiatornum == 0 ) {
            
               
             aReflPointD1E3=  m_RichG4CkvRec->
               ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum,
                  EmisPtUseAgelExit);

             aReflPointD2E3= m_RichG4CkvRec->
               ReconReflectionPointOnSPhMirror( aDetPointFromTrueLocalHit    ,
                  EmisPtUseAgelExit);
             aReflPointD3E3= m_RichG4CkvRec->
               ReconReflectionPointOnSPhMirror(aDetPointFromGlobalPhCathode  ,
                  EmisPtUseAgelExit);

               
              aReflPointD3E1=m_RichG4CkvRec->
               ReconReflectionPointOnSPhMirror(aDetPointFromGlobalPhCathode,
               EmisPtUseTrueEmissPt);

              aReflPointD3E2= m_RichG4CkvRec->
              ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode,
                                                EmisPtUseMidPtRadiatorZ );
              aReflPointD3E4= m_RichG4CkvRec->
              ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode,
                                                EmisPtUseMidPtRadiator );

              aReflPointD1E2=m_RichG4CkvRec->
               ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum , 
                                               EmisPtUseMidPtRadiatorZ );
              aReflPointD1E4=m_RichG4CkvRec->
               ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum , 
                                               EmisPtUseMidPtRadiator );

            // calculate the cherenkov angle in aerogel

             m_RichG4ReconResult->setckvAngleD1E3(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD1E3,
                     EmisPtUseAgelExit ));
             m_RichG4ReconResult->setckvAngleD2E3(            
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD2E3,
                       EmisPtUseAgelExit));
             m_RichG4ReconResult->setckvAngleD3E3(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD3E3,
                      EmisPtUseAgelExit ));

             m_RichG4ReconResult->setckvAngleD3E1(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD3E1,
                     EmisPtUseTrueEmissPt ));

             m_RichG4ReconResult->setckvAngleD3E2(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD3E2 ,
                   EmisPtUseMidPtRadiatorZ ));
             m_RichG4ReconResult->setckvAngleD3E4(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD3E4 ,
                   EmisPtUseMidPtRadiator ));

             m_RichG4ReconResult->setckvAngleD1E2(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD1E2,
                   EmisPtUseMidPtRadiatorZ ));
             m_RichG4ReconResult->setckvAngleD1E4(
                   m_RichG4CkvRec->CherenkovThetaInAerogel(aReflPointD1E4,
                   EmisPtUseMidPtRadiator ));
            
            }
            
            

            if(m_RichG4HistoFillSet4Ckv) {
              //            RichG4HitReconlog << MSG::INFO 
              //  << " Now filling Ckv Angle histo "
              //                <<  aCkvAngle<< endreq;

              m_RichG4HistoFillSet4Ckv->
                FillRichG4HistoSet4(aHit,m_RichG4ReconResult );
               
            }
            
            
            }
            
          }
        }
       }
      }
      
}


//=============================================================================
