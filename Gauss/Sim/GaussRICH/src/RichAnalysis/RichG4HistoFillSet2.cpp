// $Id: RichG4HistoFillSet2.cpp,v 1.3 2004-02-10 14:24:10 jonesc Exp $
// Include files

// local
#include "RichG4HistoFillSet2.h"
#include "RichG4Counters.h"
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
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "RichG4SvcLocator.h"
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

  FillRichG4HistoSet2A( anEvent,NumRichColl, RichG4CollectionID );

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


  G4HCofThisEvent * HCE;

  G4int NumtotAgel=0;
  G4int Numtotc4f10= 0;
  G4int Numtotcf4=0;

  G4int NumAgelPrim  = 0;
  G4int  NumC4F10Prim = 0;
  G4int NumCf4Prim= 0;

  G4int NumtotRich1=0;
  G4int NumtotRich2=0;



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

          //          cout<<" HistofillSet2 RadNum ChTkId=   "
          //     << aRadiatorNum
          //    <<"    "<<ChtkId
          //    <<"  ChtrackMom = "<<aChTrackTotMom<<  endl;

          if( aRichDetNum == 0 ) {

            NumtotRich1++;

          } else if ( aRichDetNum == 1 ) {

            NumtotRich2++;

          }



          if(  aRadiatorNum == 0 ) {
            NumtotAgel++;


            if( ChtkId <= 1 ) {
              NumAgelPrim++;
            }


          }else if( aRadiatorNum == 1 ) {

            Numtotc4f10++;

            if( ChtkId <= 1  ) {
              NumC4F10Prim++;

            }

          }else if (aRadiatorNum == 2 ) {
            Numtotcf4++;

            if(  ChtkId <= 1 ) {
              NumCf4Prim++;


            }


          }else {

            G4cout<<"Richg4HistoFillSet2: Unknown radiator number for Hit  "
                  <<iha <<"   in Rich Hitcoll   "<<ihcol<<G4endl;

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

  if(NumtotRich1 >0 ) {
    if(hNumTotHitRich1Large)
      hNumTotHitRich1Large->fill(NumtotRich1*1.0,1.0);
  }
  if(NumtotRich1 >0 ) {
    if(hNumTotHitRich2Large)
      hNumTotHitRich2Large->fill(NumtotRich2*1.0,1.0);
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

    }

    if(   TrajMadeHitRich2 &&  TrackMomAtCkv > 0.0 ) {
      if( hMomChPartRich2) hMomChPartRich2->fill( TrackMomAtCkv,1.0);

    }


  } // end of trajectory loop







  }



}

void RichG4HistoFillSet2:: FillRichG4HistoSet2A( const G4Event* anEvent,
                                                 int NumRichColl,  
                                                 const std::vector<int> & RichG4CollectionID )
{

  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();


  SmartDataPtr<IHistogram1D>hNumTotHitAgelSat(CurrentHistoSvc,
                                              "RICHG4HISTOSET2/129");

  SmartDataPtr<IHistogram1D>hNumTotHitC4F10Sat(CurrentHistoSvc,
                                               "RICHG4HISTOSET2/159");

  SmartDataPtr<IHistogram1D>hNumTotHitCF4Sat(CurrentHistoSvc,
                                             "RICHG4HISTOSET2/179");


  RichG4Counters* aRichCounter = RichG4Counters::getInstance();

  const std::vector<int> & NumRich1GasSatHit =
    aRichCounter->NumHitSaturatedPerTrackRich1Gas();

  const std::vector<int> & NumRich1AgelSatHit =
    aRichCounter->NumHitSaturatedPerTrackRich1Agel();

  const std::vector<int> & NumRich2GasSatHit =
    aRichCounter->NumHitSaturatedPerTrackRich2Gas();

  int NumSatTrajRich1Gas =  (int) NumRich1GasSatHit.size();
  int  NumSatTrajRich1Agel = (int) NumRich1AgelSatHit.size();
  int NumSatTrajRich2Gas =  (int) NumRich2GasSatHit.size();

  // cout<< "Fill histo2A : NumHitSatRich1Gas NumHitSatAgel "
  //      <<NumSatTrajRich1Gas<<"    "<< NumSatTrajRich1Agel
  //    <<endl;
  G4cout<<"Fill Histo2A : NumHitSatRich2 "
        << NumSatTrajRich2Gas<<G4endl;


  for(int ihtra=0; ihtra < NumSatTrajRich1Gas; ++ihtra ) {

    int nhita =  NumRich1GasSatHit[ihtra];
    if( nhita > 0) {
      if(hNumTotHitC4F10Sat) hNumTotHitC4F10Sat->fill(nhita,1.0);
    }

  }

  for(int ihtrb=0; ihtrb< NumSatTrajRich1Agel; ++ihtrb){
    int nhitb= NumRich1AgelSatHit[ihtrb];
    if(nhitb >0 ) {
      if(hNumTotHitAgelSat) hNumTotHitAgelSat->fill(nhitb,1.0);
    }

  }

  for(int ihtrc=0; ihtrc < NumSatTrajRich2Gas; ++ihtrc ) {

    int nhitc =  NumRich2GasSatHit[ihtrc];
    if( nhitc > 0) {
      if(hNumTotHitCF4Sat) hNumTotHitCF4Sat->fill(nhitc,1.0);
    }

  }


  //=============================================================================
}
