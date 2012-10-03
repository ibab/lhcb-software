// $Id: CherenkovG4HitRecon.cpp,v 1.12 2009-07-03 11:59:49 seaso Exp $
// Include files

// local
#include "CherenkovG4HitRecon.h"
#include "CherenkovG4CkvRecon.h"
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
#include "CkvG4Hit.h"
#include "RichG4ReconFlatMirr.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4Counters.h"
#include "CherenkovG4Counters.h"
#include "RichG4HitCoordResult.h"

// local
#include "CkvG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HitRecon
//
// 2003-09-11 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HitRecon::CherenkovG4HitRecon( ):  m_RichG4CkvRec (0) ,
                                    m_RichG4ReconFlatMirr(0)  {


  m_agelnominalrefindex = 1.03;
  m_c4f10nominalrefindex = 1.0014;
  m_chtkBetaSaturatedCut =0.9999;
  m_activateMinMomForTrackRecon=false;
  // m_minMomTracksForReconR1Gas=10000.0;
  //  m_minMomTracksForReconR1Gas=30000.0;
  // m_minMomTracksForReconR2Gas=30000.0;
  // for test use 5 GeV or above.
  m_minMomTracksForReconR1Gas=5000.0;
  m_minMomTracksForReconR2Gas=5000.0;

  m_MidRich1GasZ = (C4F10ZBeginAnalysis+C4F10ZEndAnalysis)*0.5;

  m_MidRich1AgelZ  = (AgelZBeginAnalysis+AgelZEndAnalysis)*0.5;

  m_MidRich2GasZ  = (CF4ZBeginAnalysis+CF4ZEndAnalysis)*0.5;

  m_useOnlySaturatedHits = true;
  m_useMidRadiatorZEmisPt = false;
  m_useOnlyStdRadiatorHits = true;
  m_RichG4ReconResult= new RichG4ReconResult();
  m_RichG4HitCoordResult = new RichG4HitCoordResult();
  
  m_useOnlySignalHitsInRecon=false; 

}
CherenkovG4HitRecon::~CherenkovG4HitRecon(  ) {
}

void CherenkovG4HitRecon::setRichG4CkvRec ()
{
  m_RichG4CkvRec = new CherenkovG4CkvRecon();


}
void CherenkovG4HitRecon::setRichG4FlatMirr()
{
  m_RichG4ReconFlatMirr= new RichG4ReconFlatMirr();

}
RichG4ReconFlatMirr*  CherenkovG4HitRecon::getRichG4ReconFlatMirr()
{
  return  m_RichG4ReconFlatMirr;

}
void CherenkovG4HitRecon::RichG4GetOccupancies( const G4Event* anEvent, 
                                                      int NumRichColl,
                                                const std::vector<int> & RichG4CollectionID )
{
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4HitReconlog( msgSvc,"CherenkovG4HitRecon");
  //  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();

  const std::vector<int> aNumPmtInRich =  m_RichG4CkvRec->NumPmtRich();
  // int irichdet=-1;

  G4HCofThisEvent * HCE;
  G4int NumRichCollection= NumRichColl;
  int Current_RichG4CollectionID=0;
  for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
    //if(ihcol ==0 || ihcol == 1 ){
    //  irichdet=0;
    // }else if ( ihcol ==2 || ihcol == 3 ){
    //  irichdet =1;
    // }
    // CherenkovG4HitReconlog<<MSG::VERBOSE
    //  <<"CherenkovG4HitRecon Current Rich for the hit "<<irichdet<<endmsg;
    
    Current_RichG4CollectionID =RichG4CollectionID[ihcol];
    if(Current_RichG4CollectionID >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      CkvG4HitsCollection* RHC=NULL;
      if(HCE){
        RHC = (CkvG4HitsCollection*)(HCE->
                                      GetHC( Current_RichG4CollectionID));
      }
      if(RHC ){
        G4int nHitInCurColl = RHC->entries();
        for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {
          CkvG4Hit* aHit = (*RHC)[iha];
          // in case saturated hits are needed, first
          // check if the current hit is a saturated hit.
          bool OccpSelectThisHit=true;
          
          // now to start the reconstruction.

          G4int aPixelXNum = aHit-> GetCurPixelXNum();
          G4int aPixelYNum = aHit-> GetCurPixelYNum();

          //   const G4ThreeVector & LocalHitCoord = aHit->GetLocalPos();
         // G4int aPmtNum =    aHit-> GetCurHpdNum();
          G4int aPmtNum =    aHit-> CurPmtNum();
          //  G4int aPmtModuleNum =  aHit-> CurModuleNum();

          G4int aRichDetNum = aHit->  GetCurRichDetNum();
          G4int aHitInPixelGap = aHit->CurHitInPixelGap() ;
          if(aHitInPixelGap == 1   ) {
            
            OccpSelectThisHit=false;
          }

           CherenkovG4Counters* aCherenkovG4Counters = CherenkovG4Counters::getInstance();
           aCherenkovG4Counters-> bumpRichPmtOccp(aPmtNum);
           // G4cout<<" Now added Occp "<<aPmtNum<<G4endl;
           
	         if(  aRichDetNum == 1 ) { aPmtNum -= aNumPmtInRich[0] ;  }
           //       G4int aRadiatornum= aHit->GetRadiatorNumber();
          
           //  const G4ThreeVector & LocalPhcathCoord = aHit -> GetLocalPEOriginPos();
         if( OccpSelectThisHit) {

            m_RichG4CkvRec->SetCurrentRichDetNum(aRichDetNum );
            m_RichG4CkvRec->SetCurrentPmtNum(aPmtNum);
            m_RichG4CkvRec->SetCurrentRichSector(ihcol);

            


            Gaudi::XYZPoint aLocalHitFromPixelNum = m_RichG4CkvRec->
                        GetSiHitCoordFromPixelNum(aPixelXNum,aPixelYNum);
          
            Gaudi::XYZPoint aLocalCoordInPhDetPanelPlane = m_RichG4CkvRec->
              GetCoordInPhDelPanelPlane(aLocalHitFromPixelNum);
            
            m_CherenkovG4HistoFillSet5Occp-> FillRichG4HistoSet5Coord( aHit, aLocalCoordInPhDetPanelPlane);
            

         }
         
          
        }
      }
    }
  }
  
}


void CherenkovG4HitRecon::RichG4ReconstructCherenkovAngle( const G4Event* anEvent, 
                                                      int NumRichColl,
                                                      const std::vector<int> & RichG4CollectionID )
{


  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4HitReconlog( msgSvc,"CherenkovG4HitRecon");
  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();


  const std::vector<int> & TkIdVectRich1Gas =
    aRichCounter -> TrackIdTraverseRich1Gas();
  const std::vector<int> & TkIdVectRich1Agel =
    aRichCounter -> TrackIdTraverseRich1Agel();
  const std::vector<int> & TkIdVectRich2Gas =
    aRichCounter -> TrackIdTraverseRich2Gas();

  int NumTkIdRich1Agel = TkIdVectRich1Agel.size();
  int NumTkIdRich1Gas  = TkIdVectRich1Gas.size();
  int NumTkIdRich2Gas =  TkIdVectRich2Gas.size();
 //  CherenkovG4HitReconlog<<MSG::INFO<<" NumTrkid rich1 rich2 "<<NumTkIdRich1Gas <<"  "
 //                       <<NumTkIdRich2Gas<<endreq;
  RichG4RadiatorMaterialIdValues* aRMIdValues =
      RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
  

  const std::vector<int> aNumPmtInRich =  m_RichG4CkvRec->NumPmtRich();
  //  int irichdet=-1;



  G4HCofThisEvent * HCE;
  G4int NumRichCollection= NumRichColl;
  int Current_RichG4CollectionID=0;
  for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
    // if(ihcol ==0 || ihcol == 1 ){
    //  irichdet=0;
    //  }else if ( ihcol ==2 || ihcol == 3 ){
    //  irichdet =1;
    //    }

    Current_RichG4CollectionID =RichG4CollectionID[ihcol];
    if(Current_RichG4CollectionID >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      CkvG4HitsCollection* RHC=NULL;
      if(HCE){
        RHC = (CkvG4HitsCollection*)(HCE->
                                      GetHC( Current_RichG4CollectionID));
      }
      if(RHC ){
        G4int nHitInCurColl = RHC->entries();
        for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {
          CkvG4Hit* aHit = (*RHC)[iha];
          // in case saturated hits are needed, first
          // check if the current hit is a saturated hit.

          // now to start the reconstruction.

          G4int aPixelXNum = aHit-> GetCurPixelXNum();
          G4int aPixelYNum = aHit-> GetCurPixelYNum();

          const G4ThreeVector & LocalHitCoord = aHit->GetLocalPos();
         // G4int aPmtNum =    aHit-> GetCurHpdNum();
          G4int aPmtNum =    aHit-> CurPmtNum();
          // G4int aPmtModuleNum =  aHit-> CurModuleNum();

          G4int aRichDetNum = aHit->  GetCurRichDetNum();
          G4int aHitInPixelGap = aHit->CurHitInPixelGap() ;
          //G4int aHitInPixelGap = 0;
          
          
           //         G4cout<<" RichG4Recon Pmt LocalCoord Pixel Num GapFlag"<< LocalHitCoord <<"  "
           //     <<aPixelXNum<<"  "<< aPixelYNum<<"   "<<aHitInPixelGap <<G4endl;
          //  G4cout<<" RichG4Recon DetNum Pmt num Module num "<<aRichDetNum<< "   "
         //       <<aPmtNum <<"  "<<aPmtModuleNum<<G4endl;
            
	  // now to accomodate the new pmt numbering scheme.
	         if(  aRichDetNum == 1 ) { aPmtNum -= aNumPmtInRich[0] ;  }
          

          G4int aPrimaryMirrCopyInfo = aHit->Mirror1PhotonDetectorCopyNum();
          G4int aSecMirrCopyInfo = aHit->Mirror2PhotonDetectorCopyNum();
        //       G4cout<<" RichHitrecon hit info: richdet PrimMirrCopyInfo  SecMirrorcopyinfo "<< aRichDetNum<<" "
        //     	<<aPrimaryMirrCopyInfo<<"  " <<  aSecMirrCopyInfo<<G4endl;

          G4int adfact = 100;
          G4int aSecMirrCopyNum = aSecMirrCopyInfo- (((G4int) (aSecMirrCopyInfo/adfact))*adfact); 
          // for test true coords 

        //  const G4ThreeVector trueLocalPeOrigin =  aHit->GetLocalPEOriginPos();


        //  const G4ThreeVector trueGlobalPeOrigin= aHit->GetGlobalPEOriginPos();
          
       //   const G4ThreeVector trueReflPtOnMirror1=  aHit-> Mirror1PhotonReflPosition();
          
       //   const G4ThreeVector trueReflPtOnMirror2=  aHit-> Mirror2PhotonReflPosition();

           // G4cout<<" True Local PeOrigin "<<trueLocalPeOrigin <<G4endl;
          //  G4cout<<" TrueGlocal Peorigin "<<trueGlobalPeOrigin<<G4endl;
         //  G4cout<<" trueReflPtOnMirror1 "<<trueReflPtOnMirror1<<G4endl;
         //  G4cout<<" trueReflPtOnMirror2 "<<trueReflPtOnMirror2<<G4endl;

          //end test

          const G4ThreeVector & EmissPt =  aHit->GetPhotEmisPt();
          const G4ThreeVector & TkMom = aHit->ChTrackMomVect() ;
          G4int aRadiatornum= aHit->GetRadiatorNumber();
          G4double ChTkPDGMass =  aHit->RichChTrackMass();
          G4double aChTrackTotMom =  aHit->ChTrackTotMom() ;
          const G4ThreeVector & aChTrackPreStepPos = aHit->ChTrackCkvPreStepPos();
          const G4ThreeVector & aChTrackPostStepPos = aHit->ChTrackCkvPostStepPos();
          G4int aPhotRayleighFlag=  aHit->OptPhotRayleighFlag();
          //   const G4ThreeVector & LocalPhcathCoord = aHit -> GetLocalPEOriginPos();
          const G4ThreeVector & PhotonAerogelExitPosition =
            aHit ->OptPhotAgelExitPos();
          // const G4int  asource= aHit-> PhotonSourceProcessInfo();
          // const G4int aReflFlag= aHit->RichHpdPhotonReflectionFlag();
         
          G4int aPhotSource = aHit->PhotonSourceProcessInfo();
          // std::vector<bool> aPmtRefl = aHit->DecodeRichHpdReflectionFlag();
           bool areflectedInPmt= aHit->ElectronBackScatterFlag(); //plot without any bsc or refl

           
          G4double ChTkEnergy =
            pow( (ChTkPDGMass*ChTkPDGMass+ aChTrackTotMom* aChTrackTotMom),0.5);
          G4double ChTkBeta=0.0;
          if( ChTkEnergy > 0.0 ) {

            ChTkBeta = aChTrackTotMom/ChTkEnergy;
          }
          int ChtkId =  (int) (aHit-> GetChTrackID()) ;


          const G4ThreeVector & GlobalPhcathCoord =   aHit->GetGlobalPEOriginPos();
          const G4ThreeVector & GlobalQwExtCoord  =   
                                aHit ->HpdQuartzWindowExtSurfPhotIncidentPosition ();

          //G4cout<<" G4HitRecon Local PeOrigin "<< CurLocalPeOrigin <<G4endl;
          // G4cout<<" G4HitRecon Global Peorigin "<<GlobalPhcathCoord <<G4endl;
          //  G4cout<<" G4HitRecon Global Qw ext point " <<GlobalQwExtCoord <<G4endl;
          

          // in case saturated hits are needed, first
          // check if the current hit is a saturated hit.

          bool SelectThisHit = false;

          
          if(!(m_useOnlySaturatedHits) ) {
            
        	    if( ! m_useOnlyStdRadiatorHits ) {
               SelectThisHit= true;
              }else {
                if( ( aRadiatornum == (aRMIdValues-> Rich1GaseousCkvRadiatorNum() )   ) || 
                    ( aRadiatornum ==  (aRMIdValues-> Rich2GaseousCkvRadiatorNum() )   ) || 
                    (  aRMIdValues -> IsRich1AerogelAnyTileRad(aRadiatornum)) ) {

                SelectThisHit= true;
                
                }
                
              }
              

          }else {

          //  CherenkovG4HitReconlog<<MSG::INFO<<" Now recon only sat  hits "<<NumTkIdRich1Gas<<"  "<<NumTkIdRich2Gas<< endreq;
           
            // first for agel saturated  hits
            int itagel=0;

            while ( (aRMIdValues-> IsRich1AerogelAnyTileRad(aRadiatornum))  && (itagel < NumTkIdRich1Agel) ) {

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

            while (( aRadiatornum == (aRMIdValues-> Rich1GaseousCkvRadiatorNum() )) && (itr1s < NumTkIdRich1Gas) ) {
              if( TkIdVectRich1Gas[itr1s] ==  ChtkId ) {

                if( ChTkBeta >   m_chtkBetaSaturatedCut ) {

                  
                  SelectThisHit= true;
                  if(m_activateMinMomForTrackRecon ) {
                    if( aChTrackTotMom <  m_minMomTracksForReconR1Gas )  SelectThisHit= false;
                    
                  }

                }

                // skip out
                itr1s = NumTkIdRich1Gas+1;
              }


              itr1s++;


            }

            // now  for rich2gas saturated  hits
            int itr2s=0;

            while ((aRadiatornum == (aRMIdValues-> Rich2GaseousCkvRadiatorNum() ))  && (itr2s < NumTkIdRich2Gas) ) {
               
           //   CherenkovG4HitReconlog<<MSG::INFO<<" Now recon Rich2 sat  hits "<<NumTkIdRich2Gas
           //                         <<"  "<<itr2s<<"   "<<  TkIdVectRich2Gas[itr2s] <<"  "<< ChtkId<<endreq;
           //   CherenkovG4HitReconlog<<MSG::INFO<<" Recon rich2 chtk Mom mass "<<aChTrackTotMom<<
           //       "     "<<ChTkPDGMass<<endreq;


              if( TkIdVectRich2Gas[itr2s] ==  ChtkId ) {

                if( ChTkBeta >   m_chtkBetaSaturatedCut ) {


                  
                  SelectThisHit= true;

                  if(m_activateMinMomForTrackRecon ) {
                    if( aChTrackTotMom <  m_minMomTracksForReconR2Gas )  SelectThisHit= false;
                    
                  }


                }

                // skip out
                itr2s = NumTkIdRich2Gas+1;
              }


              itr2s++;


            }



          } //  end of all test for sat hits.

          // perform reconstruction only for those hits which went through both primary and
          // secondary mirrors.
          
          if( aPrimaryMirrCopyInfo < 0 || aSecMirrCopyInfo < 0 ) {
            SelectThisHit=false;
            
          }


          // if(asource !=1 ) SelectThisHit=false;
         //  if(aReflFlag !=0 ) SelectThisHit=false;
          if(aHitInPixelGap == 1   ) {
            
            SelectThisHit=false;
          }
          if(m_useOnlySignalHitsInRecon) {
              if(areflectedInPmt || (aPhotSource == 2 ) ) SelectThisHit =false;
          }
          
        //  CherenkovG4HitReconlog<<MSG::INFO<< "Selected Hit  Pixelgapval "<< SelectThisHit <<"   "<<aHitInPixelGap <<endreq;
          

          if(  SelectThisHit ) {
             //  aHit->Print();


             // CherenkovG4HitReconlog<<MSG::INFO<<
            //  "RichG4Hit selected hitnum  RichDetNum collectionNum pmtModule pmt "<<iha<<"   "<<aRichDetNum<<"   "
            //                                  << ihcol<<"   "<<aPmtModuleNum<<"   "
            //                                  <<aPmtNum<<endreq;

            // CherenkovG4HitReconlog<<MSG::INFO<<" Sidet Pixel X Y num are "
            //                  <<aPixelXNum<<"   "<<aPixelYNum<<endreq;


          //    CherenkovG4HitReconlog<<MSG::INFO<<" Local ph cath coord is "
         //                    << LocalPhcathCoord.x()<<"    "
         //                    <<  LocalPhcathCoord.y()<<"    "
         //                    <<  LocalPhcathCoord.z()<<endreq;
         //

            // now convert to clhep like param and store then in
            // RichG4CkvRec.

       //      CherenkovG4HitReconlog<<MSG::INFO<<" EmisPt is "
      //                        <<EmissPt.x()<<"   "
      //                        <<EmissPt.y()<<"   "
      //                        <<EmissPt.z()<<endreq;
     //
            //  CherenkovG4HitReconlog<<MSG::INFO<<" Rich1 Mid radiator for 0 1 2 rad is "
            //                  << m_MidRich1AgelZ<<"  "
            //                  <<   m_MidRich1GasZ<<"   "
            //                  <<  m_MidRich2GasZ<<endreq;

            // CherenkovG4HitReconlog<<MSG::INFO<<"TkMom is  "
            //                 <<TkMom.x()<<"   "
            //                <<TkMom.y()<<"   "
            //                 <<TkMom.z()<<endreq;




            m_RichG4CkvRec->SetCurrentTkMom (  TkMom.x(),  TkMom.y(),
                                               TkMom.z());
            m_RichG4CkvRec->SetCurrentRichDetNum(aRichDetNum );
            m_RichG4CkvRec->SetCurrentPmtNum(aPmtNum);
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

            Gaudi::XYZPoint EmisPtUseTrueEmissPt =
              Gaudi::XYZPoint(EmissPt.x(),EmissPt.y(),EmissPt.z()  );

            Gaudi::XYZPoint EmisPtUseAgelExit =  Gaudi::XYZPoint(EmissPt.x(),
                                                       EmissPt.y(),EmissPt.z() );


            // double emisptReconZ =  EmissPt.z();
            double emisptReconX=
              (aChTrackPreStepPos.x()+aChTrackPostStepPos.x())*.5;
            double emisptReconY=
              (aChTrackPreStepPos.y()+aChTrackPostStepPos.y())*.5;
            double emisptReconZ=
              (aChTrackPreStepPos.z()+aChTrackPostStepPos.z())*.5;

            // double testmidx = (aChTrackPreStepPos.x()+aChTrackPostStepPos.x())*.5;
            //  double testmidy = (aChTrackPreStepPos.y()+aChTrackPostStepPos.y())*.5;
            // now to get the mid point xy of the radiator.


            double aXdiff = aChTrackPostStepPos.x()-aChTrackPreStepPos.x();
            double aZDiff = aChTrackPostStepPos.z()- aChTrackPreStepPos.z();
            double aZdiffWrtRich1MidZ = 0.0;
            double aZdiffWrtRich2MidZ = 0.0;
            double aslpLocalX = 0.0;
            double aMidRadX = aChTrackPreStepPos.x();

            double aYdiff = aChTrackPostStepPos.y()-aChTrackPreStepPos.y();
            double aslpLocalY = 0.0;
            double aMidRadY = aChTrackPreStepPos.y();

            if ( aZDiff != 0.0 ) {
              aslpLocalX = aXdiff/aZDiff;
              aslpLocalY =aYdiff/aZDiff; 
            }

            if(aRadiatornum ==   (aRMIdValues-> Rich1GaseousCkvRadiatorNum() ) ){
                aZdiffWrtRich1MidZ= m_MidRich1GasZ- aChTrackPreStepPos.z();
                aMidRadX =  aChTrackPreStepPos.x() + aslpLocalX *  aZdiffWrtRich1MidZ;
                aMidRadY = aChTrackPreStepPos.y() +  aslpLocalY * aZdiffWrtRich1MidZ;
            } else if ( aRadiatornum ==  (aRMIdValues-> Rich2GaseousCkvRadiatorNum() ) ) {
                
               aZdiffWrtRich2MidZ=  m_MidRich2GasZ- aChTrackPreStepPos.z();
               aMidRadX =  aChTrackPreStepPos.x() + aslpLocalX *  aZdiffWrtRich2MidZ;
               aMidRadY = aChTrackPreStepPos.y() +  aslpLocalY * aZdiffWrtRich2MidZ;
              
              }
             
            
            
            // bool setMidPtRadiator = true;
            //  bool setMidPtRadiator = false;

            // bool  setMidPtRadiator =   m_useMidRadiatorZEmisPt;



            if( aRMIdValues -> IsRich1AerogelAnyTileRad(aRadiatornum)  ) {
                emisptReconZ =  m_MidRich1AgelZ;

              if( PhotonAerogelExitPosition.z() > 0.0 ) {

                EmisPtUseAgelExit = PhotonAerogelExitPosition;
                // CherenkovG4HitReconlog<<MSG::INFO<<
                //  " Agel Exit pos "<<EmisPtUseAgelExit.x()
                //      <<"   " <<EmisPtUseAgelExit.y()
                //      <<"    "<< EmisPtUseAgelExit.z()<<endreq;




                // EmisPtUseAgelExit  =  m_RichG4CkvRec->
                // getPhotAgelExitZ(EmissPt.x(),EmissPt.y(),EmissPt.z(),
                // aHit );

              }

            }else if( aRadiatornum ==( aRMIdValues ->  Rich1GaseousCkvRadiatorNum() )) {
              emisptReconZ =   m_MidRich1GasZ;
              emisptReconX  =  aMidRadX;
              emisptReconY  =  aMidRadY;
              
	      //  G4cout<<"Rich1 emis pt prestepZ post stepz midradiatorZ trueZ "<< aChTrackPreStepPos.z()
	      //     <<"   "<<aChTrackPostStepPos.z()<<" "<< m_MidRich1GasZ<<"  "<<EmissPt.z()<<G4endl;
	      //  G4cout<<"Rich1emis X true X Y trueY "<<  emisptReconX <<"  "<<testmidx<<"  "
              //      << emisptReconY<<"  "<<testmidy<<G4endl;
              
              
            } else if ( aRadiatornum ==  ( aRMIdValues ->  Rich2GaseousCkvRadiatorNum() ) ) {
              emisptReconZ = m_MidRich2GasZ;
              emisptReconX  =  aMidRadX;
              emisptReconY  =  aMidRadY;

	      //  G4cout<<"Rich2 emis pt prestepZ post stepz midradiatorZ trueZ "<< aChTrackPreStepPos.z()
              //      <<"   "<<aChTrackPostStepPos.z()<<" "<<m_MidRich2GasZ<<"  "<<EmissPt.z()<<G4endl;
	      //  G4cout<<"Rich1emis X true X Y trueY "<<  emisptReconX <<"  "<<testmidx<<"  "
              //      << emisptReconY<<"  "<<testmidy<<G4endl;

            }

            Gaudi::XYZPoint EmisPtUseMidPtRadiatorZ =
              Gaudi::XYZPoint(EmissPt.x(),EmissPt.y(),emisptReconZ   );

            Gaudi::XYZPoint EmisPtUseMidPtRadiator =
              Gaudi::XYZPoint(emisptReconX,emisptReconY,emisptReconZ   );

            // now to get the detection point.
            // For reflecting  the coord wrt the flat mirror.
            RichG4ReconFlatMirr* aFlatMirr = m_RichG4ReconFlatMirr;

            // first get the point on the photodetector  and then perform the
            // reflection.


            // First get the pixel local coord on sidet from the
            // pixel x y num (column row num) or as the true  localhit coord.
            // Now reconstruct the hit coord onto the hpd Ph cathode plane.
            // option 1: D1

            Gaudi::XYZPoint aLocalHitFromPixelNum =
              m_RichG4CkvRec->
              GetSiHitCoordFromPixelNum(aPixelXNum,aPixelYNum);
            Gaudi::XYZPoint aHitOnQwFromPixelNum= m_RichG4CkvRec->
              ReconPhCoordFromLocalCoord( aLocalHitFromPixelNum);

            Gaudi::XYZPoint aDetPointFromPixelNum =  aFlatMirr->
              FlatMirrorReflect(aHitOnQwFromPixelNum ,aFlatMirrtype);

            // option2: D2

            Gaudi::XYZPoint aTrueLocalHit( LocalHitCoord.x(),
                                      LocalHitCoord.y(), LocalHitCoord.z());
            Gaudi::XYZPoint aHitOnQwFromTrueLocalHit= m_RichG4CkvRec->
              ReconPhCoordFromLocalCoord( aTrueLocalHit );

            Gaudi::XYZPoint aDetPointFromTrueLocalHit =  aFlatMirr->
              FlatMirrorReflect( aHitOnQwFromTrueLocalHit ,aFlatMirrtype);

            // option3: D3

            Gaudi::XYZPoint  aHitOnQwFromGlobalPhCathode (GlobalPhcathCoord.x(),
                                                     GlobalPhcathCoord.y(),
                                                     GlobalPhcathCoord.z());

            Gaudi::XYZPoint aDetPointFromGlobalPhCathode =  aFlatMirr->
              FlatMirrorReflect( aHitOnQwFromGlobalPhCathode,aFlatMirrtype);


            // option4: D4
      	    Gaudi::XYZPoint  aHitOnQwFromGlobalQwExtCoord( GlobalQwExtCoord.x(),
                                                           GlobalQwExtCoord.y(),
                                                           GlobalQwExtCoord.z());
            Gaudi::XYZPoint aDetPointFromGlobalQwExtCoord = aFlatMirr ->
              FlatMirrorReflect(aHitOnQwFromGlobalQwExtCoord,aFlatMirrtype); 


            // test printout

            //  G4cout<<" det pont PmtQw D1XYZ" << aHitOnQwFromPixelNum.x()<<"  "
            //      <<aHitOnQwFromPixelNum.y()<<"  "<<aHitOnQwFromPixelNum.z()<<G4endl;

           //   G4cout<<" det pont at PMTQW D2XYZ "<< aHitOnQwFromTrueLocalHit.x()<<"   "
           //       << aHitOnQwFromTrueLocalHit.y()<<"  "<< aHitOnQwFromTrueLocalHit.z()<<G4endl;
           //   G4cout<<" det pont at PMTQW D3XYZ "<<GlobalPhcathCoord.x() <<"   "
           //       << GlobalPhcathCoord.y()<<"  "<< GlobalPhcathCoord.z()<<G4endl;
            

            // end of test printout

            // test to plot the hit coord

            m_RichG4HitCoordResult->setDetPtFromPixelNum(aDetPointFromPixelNum );
            m_RichG4HitCoordResult->setDetPtInPhDetFromTrueLocalHit(aDetPointFromTrueLocalHit);
            m_RichG4HitCoordResult->setDetPtInPhDetFromGlobalPhCathode(aDetPointFromGlobalPhCathode);
            const Gaudi::Transform3D aPhDetGlobalToLocal = m_RichG4CkvRec->
                                                            getCurPhDetTrans(ihcol)->
                                                            PhDetGlobalToLocal();
            
            Gaudi::XYZPoint aDetPointFromPixelNumInPhDet = aPhDetGlobalToLocal*aDetPointFromPixelNum;
            
            //Gaudi::XYZPoint aDetPointFromTrueLocalHitInPhDet = aPhDetGlobalToLocal*aDetPointFromTrueLocalHit;
            Gaudi::XYZPoint  aDetPointFromGlobalPhCathodeInPhDet = aPhDetGlobalToLocal*aDetPointFromGlobalPhCathode;
            m_RichG4HitCoordResult->setDetPtInPhDetFromPixelNum(aDetPointFromPixelNumInPhDet);
            m_RichG4HitCoordResult->setDetPtInPhDetFromTrueLocalHit(aDetPointFromGlobalPhCathodeInPhDet);
            m_RichG4HitCoordResult->setDetPtInPhDetFromGlobalPhCathode(aDetPointFromGlobalPhCathodeInPhDet);
            m_RichG4HitCoordResult->setradiatorNum( aRadiatornum);
            

            // end of test to plot the hit coord

            //  Now there are three options for getting the detection point
            //  and two options for getting the emission point. In aerogel
            // there is an additional option for using the exit point from aerogel as
            // emission point.
            // the options for detection point are:
            // (D1) from pixelnum,
            // (D2) from the true hit coordinate,
            // (D3) from the photocathode hit coord.
            // (D4) from the Qw external point as hit coord.
            // The options for the emission point are:
            //  (E1) the true emission point,
            //   (E2) mid point of the radiator Z.
            //   (E3) Aerogel exit point.
            //   (E4) mid point of the radiator.

            Gaudi::XYZPoint aReflPointD1E1= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD2E1= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD3E1= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD3E2= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD1E2= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD3E4= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD1E4= Gaudi::XYZPoint (0.0,0.0,0.0);


            Gaudi::XYZPoint aReflPointD1E3= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD2E3= Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD3E3= Gaudi::XYZPoint (0.0,0.0,0.0);

            Gaudi::XYZPoint aReflPointD4E1 = Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD4E2 = Gaudi::XYZPoint (0.0,0.0,0.0);
            Gaudi::XYZPoint aReflPointD4E4 = Gaudi::XYZPoint (0.0,0.0,0.0);

            m_RichG4ReconResult->InitReconResult();

            m_RichG4ReconResult-> setradiatorForCkv(aRadiatornum);


            if( ( aRadiatornum ==  ( aRMIdValues ->  Rich1GaseousCkvRadiatorNum() ) )  ||  
                ( aRadiatornum == ( aRMIdValues ->  Rich2GaseousCkvRadiatorNum() ) )) {




              aReflPointD1E1 =  m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum ,
                                                  EmisPtUseTrueEmissPt, 
                               aHitOnQwFromPixelNum , aRichDetNum, aSecMirrCopyNum );




              aReflPointD2E1 =  m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror( aDetPointFromTrueLocalHit  ,
                                                 EmisPtUseTrueEmissPt, aHitOnQwFromTrueLocalHit,
                                                 aRichDetNum, aSecMirrCopyNum  );


              aReflPointD3E1 =m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode  ,
                                                  EmisPtUseTrueEmissPt,aHitOnQwFromGlobalPhCathode,
                                                  aRichDetNum, aSecMirrCopyNum  );


              aReflPointD3E2 =
                m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode  ,
                                                  EmisPtUseMidPtRadiatorZ,aHitOnQwFromGlobalPhCathode,
                                                  aRichDetNum, aSecMirrCopyNum  );


              aReflPointD3E4 =
                m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode  ,
                                                  EmisPtUseMidPtRadiator,aHitOnQwFromGlobalPhCathode,
                                                  aRichDetNum, aSecMirrCopyNum  );


              aReflPointD1E2 =  m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum ,
                                                EmisPtUseMidPtRadiatorZ ,aHitOnQwFromPixelNum,
                                                aRichDetNum, aSecMirrCopyNum );

              aReflPointD1E4 =  m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum ,
                                                  EmisPtUseMidPtRadiator ,aHitOnQwFromPixelNum,
                                                  aRichDetNum, aSecMirrCopyNum );

               aReflPointD4E1 =m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(   aDetPointFromGlobalQwExtCoord ,
                                                  EmisPtUseTrueEmissPt,aHitOnQwFromGlobalQwExtCoord,
                                                  aRichDetNum, aSecMirrCopyNum  );


              aReflPointD4E2 =
                m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalQwExtCoord ,
                                                  EmisPtUseMidPtRadiatorZ,aHitOnQwFromGlobalQwExtCoord,
                                                  aRichDetNum, aSecMirrCopyNum );

              aReflPointD4E4 =
                m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(   aDetPointFromGlobalQwExtCoord  ,
                                                  EmisPtUseMidPtRadiator,aHitOnQwFromGlobalQwExtCoord,
                                                  aRichDetNum, aSecMirrCopyNum  );


              // G4cout<<"Reflection point D1E1  on SphMirror "<< aReflPointD1E1.x()<<"   "
              //      <<  aReflPointD1E1.y()<<"   "<<aReflPointD1E1.z()<<G4endl;
              //  G4cout<<"Reflection point D1E2  on SphMirror "<< aReflPointD1E2.x()<<"   "
              //      <<  aReflPointD1E2.y()<<"   "<<aReflPointD1E2.z()<<G4endl;
              

              // calculate the cherenkov angle in gas radiators.
              // G4cout<<" Now for the gas radaitor ckv D1E1 "<<G4endl;
              // G4double atestCkvAngled1e1= m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD1E1,
              //                                                                 EmisPtUseTrueEmissPt);
            //G4double atestCkvAngled3e1= m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD3E1,
            //                                                                   EmisPtUseTrueEmissPt);
              //   G4double atestCkvAngled2e1= m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD2E1,
              //                                                                 EmisPtUseTrueEmissPt);

              //G4cout<<" Example refl point recon d1e1 d3e1 d2e1"<<aReflPointD1E1 <<"   "<< aReflPointD3E1 <<"   "
              //      <<aReflPointD2E1<<G4endl;
              
            //  G4cout<<"  Current example Recon Ckv Angle "<<atestCkvAngled1e1<<"   "<< atestCkvAngled3e1 <<"   "
            //        <<  atestCkvAngled2e1 <<G4endl;
              
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

              m_RichG4ReconResult->setckvAngleD4E1(
                                                   m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD4E1,
                                                                                            EmisPtUseTrueEmissPt ));
              m_RichG4ReconResult->setckvAngleD4E2(
                                                   m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD4E2,
                                                                                            EmisPtUseMidPtRadiatorZ ));
              m_RichG4ReconResult->setckvAngleD4E4(
                                                   m_RichG4CkvRec->CherenkovThetaFromReflPt(aReflPointD4E4,
                                                                                            EmisPtUseMidPtRadiator ));

            }else if( aRMIdValues-> IsRich1AerogelAnyTileRad(aRadiatornum) ) {



              aReflPointD1E3=  m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum,
                                                  EmisPtUseAgelExit,aHitOnQwFromPixelNum,aRichDetNum, aSecMirrCopyNum);


              aReflPointD2E3= m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror( aDetPointFromTrueLocalHit    ,
                                                 EmisPtUseAgelExit,aHitOnQwFromTrueLocalHit, aRichDetNum, aSecMirrCopyNum);


              aReflPointD3E3= m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(aDetPointFromGlobalPhCathode  ,
                                                EmisPtUseAgelExit,aHitOnQwFromGlobalPhCathode,aRichDetNum, aSecMirrCopyNum );




              aReflPointD3E1=m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(aDetPointFromGlobalPhCathode,
                                                EmisPtUseTrueEmissPt, aHitOnQwFromGlobalPhCathode,aRichDetNum, aSecMirrCopyNum);



              aReflPointD3E2= m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode,
                                                  EmisPtUseMidPtRadiatorZ,aHitOnQwFromGlobalPhCathode,aRichDetNum, aSecMirrCopyNum );


              aReflPointD3E4= m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromGlobalPhCathode,
                                                  EmisPtUseMidPtRadiator,aHitOnQwFromGlobalPhCathode,aRichDetNum, aSecMirrCopyNum );




              aReflPointD1E2=m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum ,
                                                  EmisPtUseMidPtRadiatorZ ,aHitOnQwFromPixelNum,aRichDetNum, aSecMirrCopyNum);


              aReflPointD1E4=m_RichG4CkvRec->
                ReconReflectionPointOnSPhMirror(  aDetPointFromPixelNum ,
                                                  EmisPtUseMidPtRadiator,aHitOnQwFromPixelNum,aRichDetNum, aSecMirrCopyNum );


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
