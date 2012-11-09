// $Id: RichG4HistoFillSet2.cpp,v 1.11 2009-07-03 11:59:49 seaso Exp $
// Include files

// local
#include "RichG4HistoFillSet2.h"
#include "GaussRICH/RichG4Counters.h"
// GiGa
//#include "GiGa/GiGaMACROs.h"
// local
#include "GaussRICH/RichG4Hit.h"
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
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include <vector>
//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoFillSet1
//
// 2003-05-20 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

RichG4HistoFillSet2::RichG4HistoFillSet2(  ) {

}
RichG4HistoFillSet2::~RichG4HistoFillSet2(  ) {
}

void RichG4HistoFillSet2:: FillRichG4HistoSet2( const G4Event* anEvent,
                                                int NumRichColl,  
                                                const std::vector<int> & RichG4CollectionID )
{

  FillRichG4HistoSet2A();

  FillRichG4HistoSet2B();

  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  // the following line does nto work since this is not a gaudi algorithm
  // and hence svcLoc is not automatically defined.
  // Hence the line above is used instead.

  // StatusCode sc = svcLoc()->service( "HistogramSvc", CurrentHistoSvc, true );

  SmartDataPtr<IHistogram1D>hNumTotHitRich1Large(CurrentHistoSvc,
                                                 "RICHG4HISTOSET2/106");
  SmartDataPtr<IHistogram1D>hNumTotHitAgelRich1Large(CurrentHistoSvc,
                                                     "RICHG4HISTOSET2/123");
  SmartDataPtr<IHistogram1D>hNumTotHitGasRich1Large(CurrentHistoSvc,
                                                    "RICHG4HISTOSET2/153");

  SmartDataPtr<IHistogram1D>hNumTotHitRich2NonScint(CurrentHistoSvc,
                                                 "RICHG4HISTOSET2/171");
  SmartDataPtr<IHistogram1D>hNumTotHitRich2Scint(CurrentHistoSvc,
                                                 "RICHG4HISTOSET2/172");
  SmartDataPtr<IHistogram1D>hNumTotHitRich2Large(CurrentHistoSvc,
                                                 "RICHG4HISTOSET2/173");

  SmartDataPtr<IHistogram1D>hNumTotHitAgelRich1(CurrentHistoSvc,
                                                "RICHG4HISTOSET2/125");
  SmartDataPtr<IHistogram1D>hNumTotHitC4F10Rich1(CurrentHistoSvc,
                                                 "RICHG4HISTOSET2/155");
  SmartDataPtr<IHistogram1D>hNumTotHitCF4Rich2(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/175");

  SmartDataPtr<IHistogram1D>hNumHitAgelPrim(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/127");
  SmartDataPtr<IHistogram1D>hNumHitC4F10Prim(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/157");
  SmartDataPtr<IHistogram1D>hNumHitCF4Rich2Prim(CurrentHistoSvc,
                                                "RICHG4HISTOSET2/177");

  SmartDataPtr<IHistogram1D>hMomChPartRich1(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/501");
  SmartDataPtr<IHistogram1D>hMomChPartRich2(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/511");


   SmartDataPtr<IHistogram1D>hInterMediateMomChPartRich1(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/560");
  SmartDataPtr<IHistogram1D>hLowMomChPartRich1(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/561");
  SmartDataPtr<IHistogram1D>hVeryLowMomChPartRich1(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/562");
  SmartDataPtr<IHistogram1D>hBelow1MeVMomChPartRich1(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/563");


  SmartDataPtr<IHistogram1D>hIntermediateMomChPartRich2(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/570");

  SmartDataPtr<IHistogram1D>hLowMomChPartRich2(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/571");

  SmartDataPtr<IHistogram1D>hVeryLowMomChPartRich2(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/572");

 SmartDataPtr<IHistogram1D>hBelow1MeVMomChPartRich2(CurrentHistoSvc,
                                            "RICHG4HISTOSET2/573");



  SmartDataPtr<IHistogram2D>hCkvProdAgelRich1(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/600");
  SmartDataPtr<IHistogram2D>hCkvProdC4F10Rich1(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/610");
  SmartDataPtr<IHistogram2D>hCkvProdCF4Rich2(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/620");
  
  SmartDataPtr<IHistogram1D>hNumTotHitNoRadiatorRich1(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/193");
  SmartDataPtr<IHistogram1D>hNumTotHitNoRadiatorRich2(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/195");
  SmartDataPtr<IHistogram1D>hNumTotHitNoRadiator(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/197");

  RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();


  G4HCofThisEvent * HCE;

  G4int NumtotAgel=0;
  G4int Numtotc4f10= 0;
  G4int Numtotcf4=0;
  G4int NumtotnoradiatorRich1=0;
  G4int NumtotnoradiatorRich2=0;
  G4int Numtotnoradiator=0;

  G4int NumAgelPrim  = 0;
  G4int  NumC4F10Prim = 0;
  G4int NumCf4Prim= 0;

  G4int NumtotRich1=0;
  G4int NumtotRich2=0;
  G4int NumtotRich2NonScint=0;
  G4int NumtotRich2Scint=0;
  


  G4int NumRichCollection= NumRichColl;

  int Current_RichG4CollectionID=0;
  

  for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
    Current_RichG4CollectionID =RichG4CollectionID[ihcol];

    if(Current_RichG4CollectionID >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      RichG4HitsCollection* RHC=NULL;
      if(HCE){
        RHC = (RichG4HitsCollection*)(HCE->GetHC( Current_RichG4CollectionID));
      }
      if(RHC){
        G4int nHitInCurColl = RHC->entries();
        //        G4cout<<"EndEvAction      "<< nHitInCurColl
        //       <<"   are stored in RichHitCollection set   "<<ihcol<< G4endl;
        for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {

          RichG4Hit* aHit = (*RHC)[iha];

          G4int aRadiatorNum=  aHit->GetRadiatorNumber();
          // Momentum of charged track in MeV
          G4double aChTrackTotMom =  aHit->ChTrackTotMom() ;
          G4double CkvTheta=      aHit->  ThetaCkvAtProd();
          G4int ChtkId =  aHit-> GetChTrackID();
          G4int aRichDetNum =  aHit-> GetCurRichDetNum();
          G4int aPhotSource = aHit->PhotonSourceProcessInfo() ;
          
	  //  G4ThreeVector aTrackMomAtCkv    = aHit->ChTrackMomVect();
	  // G4ThreeVector aTrackPreStepPos  = aHit-> ChTrackCkvPreStepPos();


          //          cout<<" HistofillSet2 RadNum ChTkId=   "
          //     << aRadiatorNum
          //    <<"    "<<ChtkId
          //    <<"  ChtrackMom = "<<aChTrackTotMom<<  endl;

          


          if( aRichDetNum == 0 ) {

            NumtotRich1++;

          } else if ( aRichDetNum == 1 ) {

            NumtotRich2++;
            if(aPhotSource == 2 ) {
              NumtotRich2Scint++;
            }else {
              NumtotRich2NonScint++;
            }
            
            

          }



          if( RichG4AgelPhotonRadiator( aRadiatorNum) ) {
            NumtotAgel++;


            if( ChtkId <= 1 ) {
              NumAgelPrim++;
            }

            if(hCkvProdAgelRich1) hCkvProdAgelRich1->fill(aChTrackTotMom,CkvTheta*1.0);
            


          }else if(    aRadiatorNum ==  (aRMIdValues->Rich1GaseousCkvRadiatorNum())) {

            Numtotc4f10++;

            if( ChtkId <= 1  ) {
              NumC4F10Prim++;

            }
            if(hCkvProdC4F10Rich1)hCkvProdC4F10Rich1->fill(aChTrackTotMom,CkvTheta*1.0);

          }else if (aRadiatorNum == (aRMIdValues->Rich2GaseousCkvRadiatorNum()) ) {
            Numtotcf4++;

            if(  ChtkId <= 1 ) {
              NumCf4Prim++;
            }
            if(hCkvProdCF4Rich2)hCkvProdCF4Rich2->fill(aChTrackTotMom,CkvTheta*1.0);

          }else {

	    Numtotnoradiator++;
             if( aRichDetNum == 0 ) {
  	        NumtotnoradiatorRich1++;
             
            }else if ( aRichDetNum == 1 ) {
	        NumtotnoradiatorRich2++;

            }
	     //              G4cout<<"Richg4HistoFillSet2: Unknown radiator number for Hit  "
             //     <<iha <<"   in Rich Hitcoll   "<<ihcol<<G4endl;

          }



        }

      }

    }

  }


  //  G4cout<<"Number of Rich1 Hits from Agel C4F10  "
  //     <<  NumtotAgel <<"   "<<Numtotc4f10<<G4endl;
  // G4cout<<"For SinglePartGun  NumHits from Primary particle  Agel C4F10   "
  //      <<  NumAgelPrim <<"    "<<NumC4F10Prim<<G4endl;

  //  G4cout<<"Number of Rich2 Hits "<<  Numtotcf4<<G4endl;
  // G4cout<<"SinglePartGun Rich2 Numhits from primaryPart "<<NumCf4Prim
  //        <<G4endl;

  // G4cout <<" Number of hits with no radiator "<< Numtotnoradiator<<G4endl;

  if(NumtotRich1 >0 ) {
    if(hNumTotHitRich1Large)
      hNumTotHitRich1Large->fill(NumtotRich1*1.0,1.0);
  }
  if(NumtotRich1 >0 ) {
    if(hNumTotHitRich2Large)
      hNumTotHitRich2Large->fill(NumtotRich2*1.0,1.0);
  }

  if(NumtotRich2Scint> 0 ) {
    if(hNumTotHitRich2Scint)hNumTotHitRich2Scint->fill(NumtotRich2Scint*1.0, 1.0);
    
  }
  if(NumtotRich2NonScint>0 ) {
    if(hNumTotHitRich2NonScint  )  hNumTotHitRich2NonScint->fill(NumtotRich2NonScint*1.0,1.0);
  }
  

  if( NumtotAgel > 0 )  {


    if(hNumTotHitAgelRich1) hNumTotHitAgelRich1->fill(NumtotAgel*1.0, 1.0);
    if(hNumTotHitAgelRich1Large)
      hNumTotHitAgelRich1Large->fill(NumtotAgel*1.0, 1.0);
  }


  if(Numtotc4f10 > 0 ){

    if(hNumTotHitC4F10Rich1) hNumTotHitC4F10Rich1->fill(Numtotc4f10*1.0,1.0);

    if(hNumTotHitGasRich1Large)
      hNumTotHitGasRich1Large->fill(Numtotc4f10*1.0,1.0);
  }
  if(   Numtotcf4 > 0 ){

    if(hNumTotHitCF4Rich2) hNumTotHitCF4Rich2->fill( Numtotcf4*1.0,1.0);
  }

  if( NumAgelPrim > 0 ){

    if(hNumHitAgelPrim) hNumHitAgelPrim->fill(NumAgelPrim*1.0,1.0);
  }

  if( NumC4F10Prim > 0 )  {

    if(hNumHitC4F10Prim)hNumHitC4F10Prim->
                          fill( NumC4F10Prim*1.0,1.0);
  }

  if( NumCf4Prim > 0 ){

    if(hNumHitCF4Rich2Prim) hNumHitCF4Rich2Prim->
                              fill( NumCf4Prim*1.0,1.0);

  }
  if( NumtotnoradiatorRich1 > 0 ) {
     if(  hNumTotHitNoRadiatorRich1) hNumTotHitNoRadiatorRich1->
				    fill(NumtotnoradiatorRich1*1.0,1.0);
  }
  if(NumtotnoradiatorRich2 > 0 ) {
     if(  hNumTotHitNoRadiatorRich2) hNumTotHitNoRadiatorRich2->
				    fill(NumtotnoradiatorRich2*1.0,1.0);
  }
  if( Numtotnoradiator > 0 ) {
     if(  hNumTotHitNoRadiator) hNumTotHitNoRadiator->
				    fill(Numtotnoradiator*1.0,1.0);
  }
  // Now loop through the trajectory container.


  //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();

  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {
    G4Trajectory* CurTraj=(G4Trajectory*)
      ((*(anEvent->GetTrajectoryContainer()))[itraj]);

    G4int curtid =   CurTraj->GetTrackID();


    G4double TrackMomAtCkv=-100;

    G4int aHitRefTrackId;
    bool TrajMadeHitRich1 =false;
    bool TrajMadeHitRich2 =false;

    G4int ihcola=0;


    if( curtid >= 0 ) {




      // Now loop through the hits to see if any hit has this trackId stored .

      while ( (!( TrajMadeHitRich1 ) && (ihcola==0 || ihcola == 1)) ||
              (!( TrajMadeHitRich2 ) && (ihcola==2 || ihcola == 3)) ) {
        Current_RichG4CollectionID =RichG4CollectionID[ihcola];

        if(Current_RichG4CollectionID >=0 ) {
          HCE = anEvent->GetHCofThisEvent();
          RichG4HitsCollection* RHC=NULL;
          if(HCE){
            RHC = (RichG4HitsCollection*)(HCE->GetHC( Current_RichG4CollectionID));
          }
          if(RHC){
            G4int nHitInCurColl = RHC->entries();
            //        G4cout<<"EndEvAction      "<< nHitInCurColl
            //       <<"   are stored in RichHitCollection set   "<<ihcol<< G4endl;
            G4int iha=0;

            while( !( TrajMadeHitRich1 ) &&  !( TrajMadeHitRich2 )
                   && ( iha< nHitInCurColl )  ) {

              RichG4Hit* aHit = (*RHC)[iha];
              aHitRefTrackId=   aHit-> GetChTrackID();
              G4int aRDetNum = aHit->  GetCurRichDetNum();
              TrackMomAtCkv =  aHit->  ChTrackTotMom();

              if( curtid ==  aHitRefTrackId && aRDetNum==0 ) TrajMadeHitRich1= true;
              if( curtid ==  aHitRefTrackId && aRDetNum==1 ) TrajMadeHitRich2= true;
              iha++;

            }

          }
        }
        ihcola++;

      }



    }



    if(   TrajMadeHitRich1 &&  TrackMomAtCkv > 0.0 ) {
      if( hMomChPartRich1) hMomChPartRich1->fill( TrackMomAtCkv,1.0);
      if( hInterMediateMomChPartRich1)hInterMediateMomChPartRich1->fill( TrackMomAtCkv,1.0);
      if(hLowMomChPartRich1)hLowMomChPartRich1->fill(TrackMomAtCkv,1.0);
      if(hVeryLowMomChPartRich1)hVeryLowMomChPartRich1->fill(TrackMomAtCkv,1.0);
      if(hBelow1MeVMomChPartRich1)hBelow1MeVMomChPartRich1->fill(TrackMomAtCkv,1.0);

    }

    if(   TrajMadeHitRich2 &&  TrackMomAtCkv > 0.0 ) {
      if( hMomChPartRich2) hMomChPartRich2->fill( TrackMomAtCkv,1.0);
      if(hIntermediateMomChPartRich2)hIntermediateMomChPartRich2->fill(TrackMomAtCkv,1.0);
      if(hLowMomChPartRich2)hLowMomChPartRich2->fill(TrackMomAtCkv,1.0);
      if(hVeryLowMomChPartRich2)hVeryLowMomChPartRich2->fill(TrackMomAtCkv,1.0);
      if(hBelow1MeVMomChPartRich2)hBelow1MeVMomChPartRich2->fill(TrackMomAtCkv,1.0);
    }


  } // end of trajectory loop







  }



}

void RichG4HistoFillSet2:: FillRichG4HistoSet2A()
{

  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  SmartDataPtr<IHistogram1D>hNumTotHitAgelWithRlySat(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/128");

  SmartDataPtr<IHistogram1D>hNumTotHitAgelSat(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/129");

  SmartDataPtr<IHistogram1D>hNumTotHitAgelSatNoRefl(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/130");

  SmartDataPtr<IHistogram1D>hNumTotHitC4F10Sat(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/159");

  SmartDataPtr<IHistogram1D>hNumTotHitC4F10SatNoRefl(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/160");

  SmartDataPtr<IHistogram1D>hNumTotHitC4F10SatNoReflHighMom(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/158");


  SmartDataPtr<IHistogram1D>hNumTotHitCF4Sat(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/179");;

  SmartDataPtr<IHistogram1D>hNumTotHitCF4SatNoReflNoScint(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/180");
  SmartDataPtr<IHistogram1D>hNumTotHitCF4SatNoRefl(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/181");
  SmartDataPtr<IHistogram1D>hNumTotHitCF4SatScint(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/182");

  SmartDataPtr<IHistogram1D>hNumTotHitCF4SatNoReflNoScintHighMom(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/178");

  RichG4Counters* aRichCounter = RichG4Counters::getInstance();

  const std::vector<int> & NumRich1GasSatHit =
    aRichCounter->NumHitSaturatedPerTrackRich1Gas();

  const std::vector<int> & NumRich1GasSatHitNoRefl =
    aRichCounter->NumHitSaturatedPerTrackRich1GasNoHpdRefl();

  const std::vector<int> & NumRich1GasSatHitNoReflHighMom =
    aRichCounter->NumHitSaturatedPerTrackRich1GasNoHpdReflHighMom();

  const std::vector<int> & NumRich1AgelSatHit =
    aRichCounter->NumHitSaturatedPerTrackRich1Agel();

  const std::vector<int> & NumRich1AgelSatHitNoRefl =
    aRichCounter->NumHitSaturatedPerTrackRich1AgelNoHpdRefl();

  const std::vector<int> & NumRich1AgelSatWithRlyHit =
    aRichCounter->NumHitSaturatedPerTrackRich1WithRlyAgel();

  const std::vector<int> & NumRich2GasSatHit =
    aRichCounter->NumHitSaturatedPerTrackRich2Gas();

  const std::vector<int> & NumRich2GasSatHitNoReflNoScint =
    aRichCounter->NumHitSaturatedPerTrackRich2GasNoHpdReflNoScint();

  const std::vector<int> & NumRich2GasSatHitScint =
    aRichCounter->NumHitSaturatedPerTrackRich2GasScint();
  

  const std::vector<int> & NumRich2GasSatHitNoRefl =
    aRichCounter->NumHitSaturatedPerTrackRich2GasNoHpdRefl();

 const std::vector<int> & NumRich2GasSatHitNoReflNoScintHighMom =
    aRichCounter->NumHitSaturatedPerTrackRich2GasNoHpdReflNoScintHighMom();

  int NumSatTrajRich1Gas =  (int) NumRich1GasSatHit.size();
  int  NumSatTrajRich1Agel = (int) NumRich1AgelSatHit.size();
  //int  NumSatTrajRich1WithRlyAgel = (int) NumRich1AgelSatWithRlyHit.size();
  int NumSatTrajRich2Gas =  (int) NumRich2GasSatHit.size();

  // cout<< "Fill histo2A : NumHitSatRich1Gas NumHitSatAgel "
  //      <<NumSatTrajRich1Gas<<"    "<< NumSatTrajRich1Agel
  //    <<endl;
//   G4cout<<"Fill Histo2A : NumHitSatRich2 "
//         << NumSatTrajRich2Gas<<G4endl;


  for(int ihtra=0; ihtra < NumSatTrajRich1Gas; ++ihtra ) {

    int nhita =  NumRich1GasSatHit[ihtra];
    if( nhita > 0) {
      if(hNumTotHitC4F10Sat) hNumTotHitC4F10Sat->fill(nhita,1.0);
    }
    int nhita1 =  NumRich1GasSatHitNoRefl[ihtra];
    if( nhita > 0) {
      if(hNumTotHitC4F10SatNoRefl) hNumTotHitC4F10SatNoRefl->fill(nhita1,1.0);
    }

    int nhita4 =  NumRich1GasSatHitNoReflHighMom[ihtra];
    if( nhita4 > 0) {
      if(hNumTotHitC4F10SatNoReflHighMom) hNumTotHitC4F10SatNoReflHighMom->
            fill(nhita4,1.0);
    }

  }


  for(int ihtrb=0; ihtrb< NumSatTrajRich1Agel; ++ihtrb){
    int nhitb= NumRich1AgelSatHit[ihtrb];
    if(nhitb >0 ) {
      if(hNumTotHitAgelSat) hNumTotHitAgelSat->fill(nhitb,1.0);
    }
    int nhitb1= NumRich1AgelSatWithRlyHit[ihtrb];
    if(nhitb1 >0 ) {
      if(hNumTotHitAgelWithRlySat) hNumTotHitAgelWithRlySat->fill(nhitb1,1.0);
    }
    int nhitb2= NumRich1AgelSatHitNoRefl[ihtrb];
    if(nhitb1 >0 ) {
      if(hNumTotHitAgelSatNoRefl) hNumTotHitAgelSatNoRefl->fill(nhitb2,1.0);
    }


  }


  for(int ihtrc=0; ihtrc < NumSatTrajRich2Gas; ++ihtrc ) {

    int nhitc =  NumRich2GasSatHit[ihtrc];
    if( nhitc > 0) {
      if(hNumTotHitCF4Sat) hNumTotHitCF4Sat->fill(nhitc,1.0);
    }

    int nhitc1 =  NumRich2GasSatHitNoRefl[ihtrc];
    if( nhitc1 > 0) {
      if(hNumTotHitCF4SatNoRefl) hNumTotHitCF4SatNoRefl->fill(nhitc1,1.0);
    }
    int nhitc2 =  NumRich2GasSatHitNoReflNoScint[ihtrc];
    if( nhitc2 > 0) {
      if(hNumTotHitCF4SatNoReflNoScint) hNumTotHitCF4SatNoReflNoScint->fill(nhitc2,1.0);
    }
    int nhitc3 =  NumRich2GasSatHitScint[ihtrc];
    if( nhitc3 > 0) {
      if(hNumTotHitCF4SatScint) hNumTotHitCF4SatScint->fill(nhitc3,1.0);
    }

    int nhitc4 =  NumRich2GasSatHitNoReflNoScintHighMom[ihtrc];
    if( nhitc4 > 0) {
      if(hNumTotHitCF4SatNoReflNoScintHighMom) hNumTotHitCF4SatNoReflNoScintHighMom->
                  fill(nhitc4,1.0);
    }
    
  }


  //=============================================================================
}

void RichG4HistoFillSet2:: FillRichG4HistoSet2B( )
{

  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();


  SmartDataPtr<IHistogram1D>hNumTotHitAgelFullAcceptSat(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/329");

  SmartDataPtr<IHistogram1D>hNumTotHitC4F10FullAcceptSatHighMom(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/358");
  SmartDataPtr<IHistogram1D>hNumTotHitC4F10FullAcceptSat(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/359");

  SmartDataPtr<IHistogram1D>hNumTotHitCF4FullAcceptSatNonScint(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/377");
  SmartDataPtr<IHistogram1D>hNumTotHitCF4FullAcceptSatNonScintHighMom(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/378");
  SmartDataPtr<IHistogram1D>hNumTotHitCF4FullAcceptSat(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/379");

  SmartDataPtr<IHistogram2D>hNumHitVsAngAgelFullAcceptSat(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/322");

  SmartDataPtr<IHistogram2D>hNumHitVsAngC4F10FullAcceptSat(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/352");

  SmartDataPtr<IHistogram2D>hNumHitVsAngCF4FullAcceptSat(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/372");


  RichG4Counters* aRichCounter = RichG4Counters::getInstance();


  const std::vector<int> & NumRich1GasFullAcceptSatHit =
    aRichCounter->NumHitFullAcceptSatPerTrackR1Gas();
  const std::vector<int> & NumRich1AgelFullAcceptSatHit =
    aRichCounter->NumHitFullAcceptSatPerTrackR1Agel() ;
  const std::vector<int> & NumRich2GasFullAcceptSatHit =
    aRichCounter->NumHitFullAcceptSatPerTrackR2Gas();

  const std::vector<int> NumRich1GasHighMomFullAcceptSatHit =
    aRichCounter-> NumHitFullAcceptSatPerTrackHighMomR1Gas() ;
  const std::vector<int> NumHitFullAcceptSatPerTrackNoScintR2Gas =
    aRichCounter-> NumHitFullAcceptSatPerTrackNoScintR2Gas();
  const std::vector<int> NumHitFullAcceptSatPerTrackNoScintHighMomR2Gas=
    aRichCounter-> NumHitFullAcceptSatPerTrackNoScintHighMomR2Gas();
  

  
  const std::vector<G4ThreeVector> & NumRich1GasFullAcceptTrackMom =
    aRichCounter->TrackMomFullAcceptRich1Gas();
  const std::vector<G4ThreeVector> & NumRich1AgelFullAcceptTrackMom =
    aRichCounter->TrackMomFullAcceptRich1Agel() ;
  const std::vector<G4ThreeVector> & NumRich2GasFullAcceptTrackMom =
    aRichCounter->TrackMomFullAcceptRich2Gas();




  int NumSatTrajFullAcceptRich1Gas =  (int) NumRich1GasFullAcceptSatHit.size();
  int NumSatTrajFullAccceptRich1Agel = (int) NumRich1AgelFullAcceptSatHit.size();
  int NumSatTrajFullAcceptRich2Gas =  (int) NumRich2GasFullAcceptSatHit.size();




  for(int ihtra=0; ihtra < NumSatTrajFullAcceptRich1Gas; ++ihtra ) {

    int nhita =  NumRich1GasFullAcceptSatHit[ihtra];
    if( nhita > 0) {
      if(hNumTotHitC4F10FullAcceptSat )hNumTotHitC4F10FullAcceptSat ->fill(nhita,1.0);
      G4ThreeVector r1gasTrkMom=NumRich1GasFullAcceptTrackMom[ihtra];


      double trackTheta1= r1gasTrkMom.theta();


      if( trackTheta1>0.01) {  
        if(hNumHitVsAngC4F10FullAcceptSat) hNumHitVsAngC4F10FullAcceptSat->fill(trackTheta1,nhita);
      
      
      if(hNumTotHitC4F10FullAcceptSatHighMom) {
        if(NumRich1GasHighMomFullAcceptSatHit[ihtra] > 0 ) {  
          hNumTotHitC4F10FullAcceptSatHighMom->fill(NumRich1GasHighMomFullAcceptSatHit[ihtra]);
        }
        
      }
      }
      
      
      

      
    }
    
    
    
      
    
  }
  
  
  



  for(int ihtrb=0; ihtrb<NumSatTrajFullAccceptRich1Agel ; ++ihtrb){
    int nhitb= NumRich1AgelFullAcceptSatHit[ihtrb];
      
    if(nhitb >0 ) {
      G4ThreeVector r1agelTrkMom=NumRich1AgelFullAcceptTrackMom[ihtrb];


      double trackThetaagel= r1agelTrkMom.theta();
      if(trackThetaagel > 0.01) {

        if( hNumTotHitAgelFullAcceptSat)hNumTotHitAgelFullAcceptSat->fill(nhitb,1.0);
        if(hNumHitVsAngAgelFullAcceptSat) hNumHitVsAngAgelFullAcceptSat->fill(trackThetaagel,nhitb);
      }
      
      
    }
    
    
  }
  
  

  for(int ihtrc=0; ihtrc <  NumSatTrajFullAcceptRich2Gas ; ++ihtrc ) {

    int nhitc = NumRich2GasFullAcceptSatHit[ihtrc];
    int nhitcc= NumHitFullAcceptSatPerTrackNoScintR2Gas[ihtrc];
    
    if( nhitc > 0) {
      if(hNumTotHitCF4FullAcceptSat ) hNumTotHitCF4FullAcceptSat->fill(nhitc,1.0);
      G4ThreeVector r2gasTrkMom=NumRich2GasFullAcceptTrackMom[ihtrc];


      double trackTheta2= r2gasTrkMom.theta();


      
      // if(hNumHitVsAngCF4FullAcceptSat) hNumHitVsAngCF4FullAcceptSat->fill(trackTheta2,nhitc);
      if(trackTheta2 > 0.01) {  
        if(hNumHitVsAngCF4FullAcceptSat) hNumHitVsAngCF4FullAcceptSat->fill(trackTheta2,nhitcc);
      
      
      if(hNumTotHitCF4FullAcceptSatNonScint)hNumTotHitCF4FullAcceptSatNonScint->
        fill( nhitcc );
      if(hNumTotHitCF4FullAcceptSatNonScintHighMom ){ 
        if(NumHitFullAcceptSatPerTrackNoScintHighMomR2Gas[ihtrc] > 0 ) {
          
        hNumTotHitCF4FullAcceptSatNonScintHighMom->
          fill(NumHitFullAcceptSatPerTrackNoScintHighMomR2Gas[ihtrc]);
        }
        
        
        
      }
      
      }
      
      
      
      
    }
    
    
  }
  
  
  


  //=============================================================================
}

G4bool RichG4HistoFillSet2::RichG4AgelPhotonRadiator(G4int aRadNum) 
{
  G4bool aflAg=false;
  
 RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

 if(aRMIdValues -> IsRich1AerogelAnyTileRad(  aRadNum) ) {
   aflAg=true;
 }
 
 return aflAg;  
}
