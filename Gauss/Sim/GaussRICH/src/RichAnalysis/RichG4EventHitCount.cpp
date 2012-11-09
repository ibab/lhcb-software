// $Id: RichG4EventHitCount.cpp,v 1.15 2009-07-03 11:59:49 seaso Exp $
// Include files



// local
#include "RichG4EventHitCount.h"
#include "GaussRICH/RichG4Counters.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"

// local
#include "GaussRICH/RichG4Hit.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"
#include "GaussRICH/RichG4HpdReflectionFlag.h"

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
#include <math.h>
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

   m_Rich1InitSlopeRMinFull= 0.015*rad;
   m_Rich1InitSlopeXMinFull = 0.015*rad;
   m_Rich1InitSlopeXMaxFull = 0.30*rad;
   m_Rich1InitSlopeYMinFull = 0.015*rad;
   m_Rich1InitSlopeYMaxFull =  0.30*rad;

  m_Rich1InitSlopeRMin= 0.05*rad;
  m_Rich1InitSlopeXMin= 0.05*rad;
  m_Rich1InitSlopeXMax= 0.20*rad;
  m_Rich1InitSlopeYMin = 0.05*rad;
  m_Rich1InitSlopeYMax=  0.20*rad;

  //  m_MomRich1AgelMin = 3.0*GeV;
  // m_MomRich1GasMin = 3.0*GeV;

   m_MomRich1AgelMin = 10.0*GeV;
   m_MomRich1GasMin = 10.0*GeV;
   m_MomRich2GasMin = 15.0*GeV;
  //m_MomRich1AgelMin = 20.0*GeV;
  //m_MomRich1GasMin = 20.0*GeV;
  m_MinTkPtRich1FiducialRegion= 1;
  // m_MomRich2GasMin = 30.0*GeV;
  // m_MomRich2GasMin = 20.0*GeV;
  //  m_MomRich2GasMin = 20.0*GeV;

   m_Rich2InitSlopeRMinFull= 0.015*rad;
   m_Rich2InitSlopeXMinFull= 0.015*rad;
   m_Rich2InitSlopeXMaxFull= 0.20*rad;
   m_Rich2InitSlopeYMinFull = 0.015*rad;
   m_Rich2InitSlopeYMaxFull=  0.20*rad;

  m_Rich2InitSlopeRMin= 0.05*rad;
  m_Rich2InitSlopeXMin= 0.05*rad;
  m_Rich2InitSlopeXMax= 0.20*rad;
  m_Rich2InitSlopeYMin = 0.05*rad;
  m_Rich2InitSlopeYMax=  0.20*rad;
  m_MinTkPtRich2FiducialRegion= 1;
  //  m_MinC4F10HighMomCutForYield= 60.0*GeV;
  // m_MinCF4HighMomCutForYield= 60.0*GeV;
  m_MinC4F10HighMomCutForYield= 40.0*GeV;
  m_MinCF4HighMomCutForYield= 40.0*GeV;

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

bool RichG4EventHitCount::Rich1FiducialRegion
( const G4ThreeVector & aTjPos )
{

  bool InsideRich1Fiducial = false;

  bool OutsidebeamHole= false;
  bool InsideRich1X = false;
  bool InsideRich1Y = false;

  G4double xt = aTjPos.x();
  G4double yt=  aTjPos.y();
  G4double zt = aTjPos.z();
  if(zt >= Rich1FiducialUpstrZ &&
     zt <=  Rich1FiducialDnstrZ ) {
    G4double rt= pow((xt*xt + yt*yt),0.5);
    G4double bt =  Rich1FiducialUpstrBeamHoleRad+
      (zt - Rich1FiducialUpstrZ ) * Rich1FiducialBeamHoleSlope;
    // now check if it is outside beampipe hole
    if(rt > bt )  OutsidebeamHole = true;

    // now check for X

    G4double xat = Rich1FiducialUpstrEndX +
      (zt - Rich1FiducialUpstrZ)* Rich1FiducialXslope ;


    if(fabs(xt) <= xat )  InsideRich1X= true;

    // now check for Y

    G4double yat = Rich1FiducialUpstrEndY +
      (zt - Rich1FiducialUpstrZ)* Rich1FiducialYslope ;

    if( fabs(yt) <=   yat ) InsideRich1Y= true;


    if( OutsidebeamHole &&  InsideRich1X &&
        InsideRich1Y ) InsideRich1Fiducial = true;

  }



  return  InsideRich1Fiducial;

}

bool  RichG4EventHitCount::Rich1AgelFiducialRegion(const G4ThreeVector & aTjPos)
{
  bool InsideRich1AgelFiducial= false;
  bool OutsideAgelbeamHole= false;
  bool InsideRich1AgelX = false;
  bool InsideRich1AgelY = false;

  G4double xt = aTjPos.x();
  G4double yt=  aTjPos.y();
  G4double zt = aTjPos.z();
  if(zt >= Rich1AgelFiducialUpstrZ &&
     zt <=  Rich1AgelFiducialDnstrZ ) {
    G4double rt= pow((xt*xt + yt*yt),0.5);
    G4double bt =  Rich1AgelFiducialUpstrBeamHoleRad+
      (zt - Rich1AgelFiducialUpstrZ ) * Rich1AgelFiducialBeamHoleSlope;
    // now check if it is outside beampipe hole
    if(rt > bt )  OutsideAgelbeamHole = true;
    // now check for X
    G4double xat = Rich1FiducialUpstrEndX;


    if(fabs(xt) <= xat )  InsideRich1AgelX= true;

    // now check for Y

    G4double yat = Rich1FiducialUpstrEndY ;

    if( fabs(yt) <=   yat ) InsideRich1AgelY= true;

    if( OutsideAgelbeamHole &&  InsideRich1AgelX &&
        InsideRich1AgelY ) InsideRich1AgelFiducial = true;

  }

  return InsideRich1AgelFiducial;

}

bool  RichG4EventHitCount::Rich1ProdDirSelection
( const G4ThreeVector & initMom )
{
  bool InitDirOK = false;
  if(initMom.z() > 0.0 ) {
    G4double SlopeX= (initMom.x()) /( initMom.z());
    G4double SlopeY = (initMom.y())/(initMom.z());
    G4double SlopeR=pow(( SlopeX*SlopeX+SlopeY*SlopeY),0.5);

    if( ( SlopeR  >= m_Rich1InitSlopeRMin) &&
        ( fabs(SlopeX) <= m_Rich1InitSlopeXMax ) &&
        ( fabs(SlopeY) <= m_Rich1InitSlopeYMax ) ) {

      InitDirOK = true;

    }


  }



  return  InitDirOK ;

}

bool  RichG4EventHitCount::Rich1ProdDirSelectionFullAcceptance
( const G4ThreeVector & initMom )
{
  bool InitDirOK = false;
  if(initMom.z() > 0.0 ) {
    G4double SlopeX= (initMom.x()) /( initMom.z());
    G4double SlopeY = (initMom.y())/(initMom.z());
    G4double SlopeR=pow(( SlopeX*SlopeX+SlopeY*SlopeY),0.5);

    if( ( SlopeR  >= m_Rich1InitSlopeRMinFull) &&
        ( fabs(SlopeX) <= m_Rich1InitSlopeXMaxFull ) &&
        ( fabs(SlopeY) <= m_Rich1InitSlopeYMaxFull ) ) {

      InitDirOK = true;

    }


  }



  return  InitDirOK ;

}



bool  RichG4EventHitCount::Rich1AgelMomSelection
( const G4ThreeVector & InitMomA ) {

  bool AgelMomHigh = false;

  G4double MomMagnitude=  InitMomA.mag();

  if(MomMagnitude >= m_MomRich1AgelMin ) {
    AgelMomHigh=true;
  }



  return  AgelMomHigh;

}


bool  RichG4EventHitCount::Rich1GasMomSelection
( const G4ThreeVector & InitMomB )
{
  bool GasMomHigh = false;
  G4double MomMagnitude=  InitMomB.mag();

  if(MomMagnitude >= m_MomRich1GasMin ) {
    GasMomHigh= true;
  }


  return  GasMomHigh;


}

bool RichG4EventHitCount::Rich2FiducialRegion
( const G4ThreeVector & aTjPos )
{

  bool InsideRich2Fiducial = false;


  bool OutsideBeamHoleR2= false;
  bool InsideRich2X = false;
  bool InsideRich2Y = false;

  G4double xt = aTjPos.x();
  G4double yt=  aTjPos.y();
  G4double zt = aTjPos.z();
  if(zt >= Rich2FiducialUpstrZ &&
     zt <=  Rich2FiducialDnstrZ ) {
    G4double rt= pow((xt*xt + yt*yt),0.5);
    G4double bt =  Rich2FiducialUpstrBeamHoleRad+
      (zt - Rich2FiducialUpstrZ ) * Rich2FiducialBeamHoleSlope;
    // now check if it is outside beampipe hole
    if(rt > bt )  OutsideBeamHoleR2 = true;

    // now check for X

    G4double xat = Rich2FiducialUpstrEndX +
      (zt - Rich2FiducialUpstrZ)* Rich2FiducialXslope ;


    if(fabs(xt) <= xat )  InsideRich2X= true;

    // now check for Y

    G4double yat = Rich2FiducialUpstrEndY +
      (zt - Rich2FiducialUpstrZ)* Rich2FiducialYslope ;

    if( fabs(yt) <=   yat ) InsideRich2Y= true;


    if( OutsideBeamHoleR2 &&  InsideRich2X &&
        InsideRich2Y ) InsideRich2Fiducial = true;

  }






  return  InsideRich2Fiducial;

}

bool  RichG4EventHitCount::Rich2GasMomSelection
( const G4ThreeVector & InitMomB )
{
  bool GasMomHigh = false;
  G4double MomMagnitude=  InitMomB.mag();

  if(MomMagnitude >= m_MomRich2GasMin ) {
    GasMomHigh=true;
  }


  return  GasMomHigh;


}

bool  RichG4EventHitCount::Rich2ProdDirSelection
( const G4ThreeVector & initMom )
{
  bool InitDirOK = false;
  if(initMom.z() > 0.0 ) {
    G4double SlopeX= (initMom.x()) /( initMom.z());
    G4double SlopeY = (initMom.y())/(initMom.z());
    G4double SlopeR =pow((SlopeX*SlopeX+SlopeY*SlopeY),0.5);

    if( ( SlopeR >= m_Rich2InitSlopeRMin) &&
        ( fabs(SlopeX) <= m_Rich2InitSlopeXMax ) &&
        ( fabs(SlopeY) <= m_Rich2InitSlopeYMax ) ) {

      InitDirOK = true;

    }

  }


  return  InitDirOK ;

}

bool  RichG4EventHitCount::Rich2ProdDirSelectionFullAcceptance
( const G4ThreeVector & initMom )
{
  bool InitDirOK = false;
  if(initMom.z() > 0.0 ) {
    G4double SlopeX= (initMom.x()) /( initMom.z());
    G4double SlopeY = (initMom.y())/(initMom.z());
    G4double SlopeR =pow((SlopeX*SlopeX+SlopeY*SlopeY),0.5);

    if( ( SlopeR >= m_Rich2InitSlopeRMinFull) &&
        ( fabs(SlopeX) <= m_Rich2InitSlopeXMaxFull ) &&
        ( fabs(SlopeY) <= m_Rich2InitSlopeYMaxFull ) ) {

      InitDirOK = true;

    }

  }


  return  InitDirOK ;

}


bool RichG4EventHitCount::Rich1TrajTraverse
(const G4Event* anEvent, int trajId, bool FullAcceptance)
{
  //  Get the track ID of all the charged particle trajectories.
  // excluding those of the photoelectrons.
  bool atrav= false;

  //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();
  }

  bool Rich1GasMomHigh = false;
  bool TrackDirInRich1Acceptance=false;
  bool TrackChAndId = false;
  bool aTravRich1Fid= false;
  bool Rich1ProjFid= false;
  int aTravRich1FiducialCount=0;
  bool  TooFewTrajPoint=false;
  bool AllOutsideRich1=false;


  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {
    G4Trajectory* CurTraj=(G4Trajectory*)
      ((*(anEvent->GetTrajectoryContainer()))[itraj]);

    G4int ctid =   CurTraj->GetTrackID();
    G4double curtkCharge=  CurTraj-> GetCharge();
    if( (curtkCharge != 0.0)  && ( trajId == (int) ctid )) {
      TrackChAndId= true;

      const G4ThreeVector & initMomentum =  CurTraj->GetInitialMomentum();

      Rich1GasMomHigh   = Rich1GasMomSelection(initMomentum);
      if(FullAcceptance) {
        TrackDirInRich1Acceptance = Rich1ProdDirSelectionFullAcceptance(initMomentum);

      }else {
        TrackDirInRich1Acceptance = Rich1ProdDirSelection(initMomentum);
      }
      
      //      G4cout<<" Rich1Traj tkid InitMom  "<<trajId<<"   "
      //      <<initMomentum<<"   GasMomOK DirOK "
      //       <<"  "<< Rich1GasMomHigh
      //      <<"   "<< TrackDirInRich1Acceptance<< G4endl;
      // the following 'if' is to save cpu time.


      if( ( Rich1GasMomHigh ) &&
          ( TrackDirInRich1Acceptance ) ){

        int NumTjPoints = CurTraj-> GetPointEntries();



        //   G4cout<<"Rich1Traj itrajID NumTrajPt =  "
        //      << trajId<<"  "<< NumTjPoints<<G4endl;
        if( NumTjPoints <= 1 ) {
          TooFewTrajPoint = true;
          //  G4cout<<" Rich1Traj itraj Toofew traj "<< itraj
          //       <<"   "<<NumTjPoints<< G4endl;

        } else {


          G4VTrajectoryPoint* aTjPointFirst =  CurTraj-> GetPoint(0);
          G4VTrajectoryPoint* aTjPointLast =  CurTraj-> GetPoint(NumTjPoints-1);
          const G4ThreeVector & aTjPosFirst=aTjPointFirst -> GetPosition() ;
          const G4ThreeVector & aTjPosLast=aTjPointLast -> GetPosition() ;
          if(  ( aTjPosFirst.z() >    Rich1FiducialDnstrZ)  ||
               ( aTjPosLast.z() < Rich1FiducialUpstrZ ) ){
            AllOutsideRich1= true;
            //  G4cout<<" Rich1Traj AllOutsiderich1 first last traj "<<
            //     aTjPosFirst.z()<<"  "<<aTjPosLast.z()
            //      <<"  "<< itraj <<G4endl;

          } else {



            for (int ipt=0 ; ipt< NumTjPoints; ipt++ ) {
              G4VTrajectoryPoint* aTjPoint =  CurTraj-> GetPoint(ipt);



              const G4ThreeVector & aTjPos=   aTjPoint-> GetPosition() ;

              bool aTravRich1Fiducial= Rich1FiducialRegion( aTjPos);

              //           G4cout<<"Rich1Traj ipt tjpos InR1Fid  "
              //      <<ipt <<"   "<<aTjPos<<" "<<aTravRich1Fiducial<<G4endl;

              if(   aTravRich1Fiducial) aTravRich1FiducialCount++;

            }
          }

        }

        if(aTravRich1FiducialCount >= m_MinTkPtRich1FiducialRegion ) {
          // at least a Min num of points  in Rich1 acceptance.
          aTravRich1Fid= true;
        } else {
          // if the track has points upstream and downstream of
          // rich1 then project into rich1 to see if it may
          // pass through rich1.
          G4ThreeVector aTjPosUps(0.0,0.0,-1000.0);
          G4ThreeVector aTjPosDns(0.0,0.0,1000000.0);
          bool upsFound= false;
          bool dnsFound= false;
          //         bool Rich1ProjFid= false;


          if( NumTjPoints >= 2 ) {
            for (int ipta=0 ; ipta< NumTjPoints; ipta++ ) {

              G4VTrajectoryPoint* aTjPointa =  CurTraj-> GetPoint(ipta);
              G4ThreeVector aTjPosa =   aTjPointa-> GetPosition() ;
              if( ( aTjPosa.z() <= Rich1FiducialUpstrZ)  &&
                  ( aTjPosa.z() > aTjPosUps.z() ))
              {
                aTjPosUps= aTjPosa;
                upsFound = true;

              }
              if( (aTjPosa.z() >= Rich1FiducialDnstrZ ) &&
                  (aTjPosa.z() < aTjPosDns.z() ))
              {
                aTjPosDns= aTjPosa;
                dnsFound= true;

              }



            }


            if(   upsFound && dnsFound ) {


              G4ThreeVector aInitPos =  aTjPosUps;
              G4ThreeVector aLastPos = aTjPosDns;

              // create a projected point in the middle of rich1.
              if(aLastPos.z() != aInitPos.z() ) {
                G4double projZ=(Rich1FiducialUpstrZ+Rich1FiducialDnstrZ)*0.5;

                G4double projSlopeX = (aLastPos.x()-aInitPos.x()) /
                  ( aLastPos.z()-aInitPos.z());
                G4double projSlopeY = (aLastPos.y()-aInitPos.y()) /
                  ( aLastPos.z()-aInitPos.z());
                G4double projX = aInitPos.x()+ projSlopeX*(projZ-aInitPos.z()) ;
                G4double projY = aInitPos.y()+ projSlopeY*(projZ-aInitPos.z()) ;
                G4ThreeVector aProjPos(projX,projY,projZ);

                Rich1ProjFid = Rich1FiducialRegion(aProjPos);

              }



            }


            if(Rich1ProjFid) {
              aTravRich1Fid= true;

            }


          }







        }






      }





    }

  }

  if( TrackChAndId ) {

    if(Rich1GasMomHigh && TrackDirInRich1Acceptance){


      // G4cout<<"Rich1Traj tkid R1FidCount Rich1Fid "
      //        <<trajId <<"   "<< aTravRich1FiducialCount<<"   "
      //         << aTravRich1Fid<<G4endl;
      //   G4cout<<"Rich1Traj tkid  Rich1ProjFid Rich1Fid "
      //         <<trajId<<"   "<<Rich1ProjFid<<"   "
      //         <<aTravRich1Fid<<"  "<<G4endl;






      if(  aTravRich1Fid ) {

        atrav =true;
      }


    }


  }





  //  G4cout<<"End of Rich1Traj ctid Rich1traverse "
  //    << trajId <<"    "<<atrav<<G4endl;

  return  atrav;

}



bool RichG4EventHitCount::Rich1AgelTrajTraverse
(const G4Event* anEvent, int trajId, bool FullAcc)
{
  bool atravAgel= false;


  //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();
  }

  bool Rich1AgelMomHigh=false;
  bool TrackDirInRich1Acceptance=false;
  bool TrackChAndId = false;
  bool aTravRich1AgelFid= false;
  bool Rich1AgelProjFid= false;
  int aTravRich1AgelFiducialCount=0;
  bool  TooFewTrajPoint=false;
  bool AllOutsideRich1Agel=false;


  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {
    G4Trajectory* CurTraj=(G4Trajectory*)
      ((*(anEvent->GetTrajectoryContainer()))[itraj]);

    G4int ctid =   CurTraj->GetTrackID();
    G4double curtkCharge=  CurTraj-> GetCharge();
    if( (curtkCharge != 0.0)  && ( trajId == (int) ctid )) {
      TrackChAndId= true;

      const G4ThreeVector & initMomentum=  CurTraj->GetInitialMomentum();

      Rich1AgelMomHigh   = Rich1AgelMomSelection(initMomentum);
      if(FullAcc) {
        TrackDirInRich1Acceptance = Rich1ProdDirSelectionFullAcceptance(initMomentum);
      }else {
        TrackDirInRich1Acceptance = Rich1ProdDirSelection(initMomentum);
      }
      
      //   G4cout<<" Rich1Traj tkid InitMom  "<<trajId<<"   "
      //    <<initMomentum<<"   AgelMomOK DirOK "
      //      <<"  "<< Rich1AgelMomHigh
      //     <<"   "<< TrackDirInRich1Acceptance<< G4endl;
      // the following 'if' is to save cpu time.


      if( ( Rich1AgelMomHigh ) &&
          ( TrackDirInRich1Acceptance ) ){

        int NumTjPoints = CurTraj-> GetPointEntries();

        if( NumTjPoints <= 1 ) {
          TooFewTrajPoint = true;
          //  G4cout<<" Rich1Traj itraj Toofew traj "<< itraj
          //       <<"   "<<NumTjPoints<< G4endl;

        } else {


          G4VTrajectoryPoint* aTjPointFirst =  CurTraj-> GetPoint(0);
          G4VTrajectoryPoint* aTjPointLast =  CurTraj-> GetPoint(NumTjPoints-1);
          const G4ThreeVector & aTjPosFirst=aTjPointFirst -> GetPosition() ;
          const G4ThreeVector & aTjPosLast=aTjPointLast -> GetPosition() ;
          if(  ( aTjPosFirst.z() >    Rich1AgelFiducialDnstrZ)  ||
               ( aTjPosLast.z() < Rich1AgelFiducialUpstrZ ) ){
            AllOutsideRich1Agel= true;
            //    G4cout<<" Rich1Traj AllOutsiderich1Agel first last traj "<<
            //     aTjPosFirst.z()<<"  "<<aTjPosLast.z()
            //      <<"  "<< itraj <<G4endl;

          } else {



            for (int ipt=0 ; ipt< NumTjPoints; ipt++ ) {

              G4VTrajectoryPoint* aTjPoint =  CurTraj-> GetPoint(ipt);
              const G4ThreeVector & aTjPos=   aTjPoint-> GetPosition() ;

              bool aTravRich1AgelFiducial= Rich1AgelFiducialRegion( aTjPos);

              //           G4cout<<"Rich1Traj ipt tjpos InR1Fid  "
              //      <<ipt <<"   "<<aTjPos<<" "<<aTravRich1Fiducial<<G4endl;

              if(   aTravRich1AgelFiducial) aTravRich1AgelFiducialCount++;

            }

          }




        }





        if(aTravRich1AgelFiducialCount >= m_MinTkPtRich1FiducialRegion ) {
          // at least a Min num of points  in Rich1 acceptance.
          aTravRich1AgelFid= true;
        } else {
          // if the track has points upstream and downstream of
          // rich1 then project into rich1Agel to see if it may
          // pass through rich1Agel.
          G4ThreeVector aTjPosUps(0.0,0.0,-1000.0);
          G4ThreeVector aTjPosDns(0.0,0.0,1000000.0);
          bool upsFound= false;
          bool dnsFound= false;
          //         bool Rich1ProjFid= false;


          if( NumTjPoints >= 2 ) {
            for (int ipta=0 ; ipta< NumTjPoints; ipta++ ) {

              G4VTrajectoryPoint* aTjPointa =  CurTraj-> GetPoint(ipta);
              const G4ThreeVector & aTjPosa =   aTjPointa-> GetPosition() ;
              if( ( aTjPosa.z() <= Rich1AgelFiducialUpstrZ)  &&
                  ( aTjPosa.z() > aTjPosUps.z() ))
              {
                aTjPosUps= aTjPosa;
                upsFound = true;

              }
              if( (aTjPosa.z() >= Rich1AgelFiducialDnstrZ ) &&
                  (aTjPosa.z() < aTjPosDns.z() ))
              {
                aTjPosDns= aTjPosa;
                dnsFound= true;

              }




            }

            if(   upsFound && dnsFound ) {


              G4ThreeVector aInitPos =  aTjPosUps;
              G4ThreeVector aLastPos = aTjPosDns;

              // create a projected point in the middle of rich1.
              if(aLastPos.z() != aInitPos.z() ) {
                G4double projZ=
                  (Rich1AgelFiducialUpstrZ+Rich1AgelFiducialDnstrZ)*0.5;

                G4double projSlopeX = (aLastPos.x()-aInitPos.x()) /
                  ( aLastPos.z()-aInitPos.z());
                G4double projSlopeY = (aLastPos.y()-aInitPos.y()) /
                  ( aLastPos.z()-aInitPos.z());
                G4double projX = aInitPos.x()+ projSlopeX*(projZ-aInitPos.z()) ;
                G4double projY = aInitPos.y()+ projSlopeY*(projZ-aInitPos.z()) ;
                G4ThreeVector aProjPos(projX,projY,projZ);

                Rich1AgelProjFid = Rich1AgelFiducialRegion(aProjPos);

              }

            }
            if(Rich1AgelProjFid) {
              aTravRich1AgelFid= true;

            }
          }
        }
      }
    }
  }



  if( TrackChAndId ) {

    if( ( Rich1AgelMomHigh ) &&
        ( TrackDirInRich1Acceptance ) ){

      //    G4cout<<"Rich1AgelTraj tkid R1AgelFidCount Rich1AgelFid "
      //       <<trajId <<"   "<< aTravRich1AgelFiducialCount<<"   "
      //       << aTravRich1AgelFid<<G4endl;
      //  G4cout<<"Rich1AgelTraj tkid  Rich1AgelProjFid Rich1AgelFid "
      //      <<trajId<<"   "<<Rich1AgelProjFid<<"   "
      //      <<aTravRich1AgelFid<<"  "<<G4endl;



      if(aTravRich1AgelFid) {

        atravAgel=true;
      }

    }

  }

  //  G4cout<<" End of Rich1AgelTraj trajid atravagel "
  //         <<trajId<<"    "<< atravAgel<<G4endl;



  return  atravAgel;
}


bool RichG4EventHitCount::Rich2TrajTraverse
(const G4Event* anEvent, int trajId, bool FullAcc)
{

  //  Get the track ID of all the charged particle trajectories.
  // excluding those of the photoelectrons.
  bool atrav= false;

  //get the trajectories
  G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(atrajectoryContainer){n_trajectories=atrajectoryContainer->entries();
  }

  // bool Rich2AgelMomHigh=false;
  bool Rich2GasMomHigh = false;
  bool TrackDirInRich2Acceptance=false;
  bool TrackChAndId = false;
  bool aTravRich2Fid= false;
  bool Rich2ProjFid= false;
  int aTravRich2FiducialCount=0;
  bool TooFewTrajPoint=false;

  bool AllOutsideRich2=false;


  for (int itraj=0; itraj<  n_trajectories ; itraj++ ) {

    G4Trajectory* CurTraj=(G4Trajectory*)
      ((*(anEvent->GetTrajectoryContainer()))[itraj]);

    G4int ctid =   CurTraj->GetTrackID();
    G4double curtkCharge=  CurTraj-> GetCharge();
    if( (curtkCharge != 0.0)  && ( trajId == (int) ctid ) ) {
      TrackChAndId= true;

      const G4ThreeVector & initMomentum=  CurTraj->GetInitialMomentum();

      Rich2GasMomHigh   = Rich2GasMomSelection(initMomentum);
      if(FullAcc) {
        TrackDirInRich2Acceptance = Rich2ProdDirSelectionFullAcceptance(initMomentum);
      }else {      
        TrackDirInRich2Acceptance = Rich2ProdDirSelection(initMomentum);
      }
      
      // the following 'if' is to save cpu time.

      // G4cout<<" Rich2Traj trajID initmom Rich2momOk TrackDirOk "
      //      << trajId <<"   "<<initMomentum<<"    "
      //      << Rich2GasMomHigh<<"   "<<TrackDirInRich2Acceptance
      //      << G4endl;

      if( ( Rich2GasMomHigh ) &&
          ( TrackDirInRich2Acceptance ) ){

        int NumTjPoints = CurTraj-> GetPointEntries();


        // G4cout<<"Rich2traj trajid NumTrajPt =  "
        //     <<trajId<<"   "<< NumTjPoints<<G4endl;


        if( NumTjPoints <= 1 ) {
          TooFewTrajPoint = true;
          // G4cout<<" Rich2Traj itraj Toofew trajid "<< trajId
          //       <<"   "<<NumTjPoints<< G4endl;

        } else {


          G4VTrajectoryPoint* aTjPointFirst =  CurTraj-> GetPoint(0);
          G4VTrajectoryPoint* aTjPointLast =  CurTraj-> GetPoint(NumTjPoints-1);
          const G4ThreeVector & aTjPosFirst=aTjPointFirst -> GetPosition() ;
          const G4ThreeVector & aTjPosLast=aTjPointLast -> GetPosition() ;
          if(  ( aTjPosFirst.z() >    Rich2FiducialDnstrZ)  ||
               ( aTjPosLast.z() < Rich2FiducialUpstrZ ) ){
            AllOutsideRich2= true;
            //    G4cout<<" Rich2Traj AllOutsiderich2 first last itraj "
            //      << aTjPosFirst.z()<<"  "<<aTjPosLast.z()
            //      <<"   "<< trajId <<G4endl;

          } else {




            for (int ipt=0 ; ipt< NumTjPoints; ipt++ ) {
              G4VTrajectoryPoint* aTjPoint =  CurTraj-> GetPoint(ipt);



              const G4ThreeVector & aTjPos=   aTjPoint-> GetPosition() ;

              bool aTravRich2Fiducial= Rich2FiducialRegion( aTjPos);

              // G4cout<<" Rich2Traj ipt aTjpos R2Fid "
              //      << ipt <<"   "<<aTjPos<<"   "
              //      <<aTravRich2Fiducial<<G4endl;

              if(   aTravRich2Fiducial) aTravRich2FiducialCount++;

            }

          }
        }

        if(aTravRich2FiducialCount >= m_MinTkPtRich2FiducialRegion ) {
          // at least a Min num of points  in Rich2 acceptance.
          aTravRich2Fid= true;

        } else {
          // if the track has points upstream and downstream of
          // rich2 then project into rich2 to see if it may
          // pass through rich2.
          G4ThreeVector aTjPosUps(0.0,0.0,-1000.0);
          G4ThreeVector aTjPosDns(0.0,0.0,1000000.0);
          bool upsFound= false;
          bool dnsFound= false;
          //         bool Rich2ProjFid= false;


          if( NumTjPoints >= 2 ) {
            for (int ipta=0 ; ipta< NumTjPoints; ipta++ ) {

              const G4VTrajectoryPoint* aTjPointa =  CurTraj-> GetPoint(ipta);
              const G4ThreeVector & aTjPosa =   aTjPointa-> GetPosition() ;
              if( ( aTjPosa.z() <= Rich2FiducialUpstrZ)  &&
                  ( aTjPosa.z() > aTjPosUps.z() ))
              {
                aTjPosUps= aTjPosa;
                upsFound =true;
              }


              if( (aTjPosa.z() >= Rich2FiducialDnstrZ ) &&
                  (aTjPosa.z() < aTjPosDns.z() ))
              {
                aTjPosDns= aTjPosa;
                dnsFound= true;

              }


            }

            if(   upsFound && dnsFound ) {


              G4ThreeVector aInitPos =  aTjPosUps;
              G4ThreeVector aLastPos = aTjPosDns;

              // create a projected point in the middle of rich1.
              if(aLastPos.z() != aInitPos.z() ) {
                G4double projZ=(Rich2FiducialUpstrZ+Rich2FiducialDnstrZ)*0.5;


                G4double projSlopeX = (aLastPos.x()-aInitPos.x()) /
                  ( aLastPos.z()-aInitPos.z());
                G4double projSlopeY = ( aLastPos.y()-aInitPos.y()) /
                  ( aLastPos.z()-aInitPos.z());
                G4double projX = aInitPos.x()+ projSlopeX*(projZ-aInitPos.z()) ;
                G4double projY = aInitPos.y()+ projSlopeY*(projZ-aInitPos.z()) ;
                G4ThreeVector aProjPos(projX,projY,projZ);
                Rich2ProjFid = Rich2FiducialRegion( aProjPos );

              }

            }

            if(Rich2ProjFid) {
              aTravRich2Fid= true;

            }

          }


        }


      }


    }

  }

  if( TrackChAndId ) {
    if(Rich2GasMomHigh && TrackDirInRich2Acceptance){

      // G4cout<<" Rich2Traj itraj Rich2Fidcount Rich2fid "
      //      << trajId<<"  "<<aTravRich2FiducialCount
      //      <<"   "<<aTravRich2Fid<<G4endl;

      //  G4cout<<" Rich2Traj itraj Rich2ProjFid Rich2Fid "
      //      << trajId<<"  "<<Rich2ProjFid
      //      <<"   "<<aTravRich2Fid<<G4endl;


      if(  aTravRich2Fid ) {

        atrav =true;
      }


    }





  }

  // G4cout<<"End of Rich2Traj itraj atrav "
  //     <<trajId<<"   "<<atrav <<G4endl;


  return  atrav;

}


void RichG4EventHitCount::RichG4CountSaturatedHits(const G4Event* anEvent,  int NumRichColl,
                                                   const std::vector<int> & RichG4CollectionID )
{
  // first get all the track ids
  //  std::vector<int> TrakIDVect = RichG4GetChTrajId(anEvent);
  // Now find the number of tracks creating hits in RICH.
  std::vector<int>  TrajIdVectR1;
  std::vector<int>  TrajIdVectR1Agel;
  std::vector<int>  TrajIdVectR2;

  TrajIdVectR1.clear();
  TrajIdVectR1.reserve(100);
  TrajIdVectR1Agel.clear();
  TrajIdVectR1Agel.reserve(100);
  TrajIdVectR2.clear();
  TrajIdVectR2.reserve(100);


  RichG4RadiatorMaterialIdValues* aRMIdValues= RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

  
//   G4cout<<"  Now counting Saturated numhits in Rich1 Rich2 "<<G4endl;


  //       G4double MinMomAgelCut=5.0*GeV;
  //  G4double MinMomc4f10Cut=5.0*GeV;
  //  G4double MaxMomAgelCut=15.0*GeV;




  G4HCofThisEvent * HCE;
  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();
  //  G4cout<<" Now in RichG4CountSaturated Hits "<< G4endl;
  
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

            //G4cout<<" TRkId for rich1 hits  "<< ChtkId <<G4endl;

            bool trajAlreadyStoredR1=true;;
            bool trajtraversedRich1=false;

            bool trajAlreadyStoredR1Agel=true;;
            bool trajtraversedRich1Agel=false;

            bool trajAlreadyStoredR2=true;;
            bool trajtraversedRich2=false;



            if(TrajIdVectR1.size() > 0 ) {

              std::vector<int>::iterator p = find(TrajIdVectR1.begin(),
                                                  TrajIdVectR1.end(),
                                                  ChtkId);
              if( p == TrajIdVectR1.end() ) trajAlreadyStoredR1=false;


            } else {

              trajAlreadyStoredR1=false;

            }


            if(TrajIdVectR1Agel.size() > 0 ) {

              std::vector<int>::iterator p = find(TrajIdVectR1Agel.begin(),
                                                  TrajIdVectR1Agel.end(),
                                                  ChtkId);
              if( p == TrajIdVectR1Agel.end() ) trajAlreadyStoredR1Agel=false;


            } else {

              trajAlreadyStoredR1Agel=false;

            }

            if(TrajIdVectR2.size() > 0 ) {

              std::vector<int>::iterator p = find(TrajIdVectR2.begin(),
                                                  TrajIdVectR2.end(),
                                                  ChtkId);
              if( p == TrajIdVectR2.end() ) trajAlreadyStoredR2=false;


            } else {

              trajAlreadyStoredR2=false;

            }



            if( !( trajAlreadyStoredR1) ) {

              // Now loop through the trajectory container to see
              // this track goes through the whle length of Rich1.
              // This is done by checking if the first point of the
              // trajectory is upstream of Rich1 and the last point is
              // downstream of Rich1.


              trajtraversedRich1= Rich1TrajTraverse(anEvent, ChtkId, false );

              if( trajtraversedRich1) {

                TrajIdVectR1.push_back(ChtkId);
              }

            }

            if( !( trajAlreadyStoredR1Agel) ) {

              // Now loop through the trajectory container to see
              // this track goes through the whle length of Rich1 agel.
              // This is done by checking if the first point of the
              // trajectory i supstream of Rich1 agel and the last point is
              // downstream of Rich1 agel.


              trajtraversedRich1Agel= Rich1AgelTrajTraverse(anEvent, ChtkId , false);

              if( trajtraversedRich1Agel) {

                TrajIdVectR1Agel.push_back(ChtkId);
              }

            }


            // now for rich2
            if( !( trajAlreadyStoredR2) ) {

              // Now loop through the trajectory container to see
              // this track goes through the whle length of Rich2.
              // This is done by checking if the first point of the
              // trajectory i supstream of Rich2 and the last point is
              // downstream of Rich2.


              trajtraversedRich2= Rich2TrajTraverse(anEvent, ChtkId , false );

              if( trajtraversedRich2) {

                TrajIdVectR2.push_back(ChtkId);
              }

            }





          }  // end loop through hits.
        }


        
      }
      



    }  // end loop over collections


    //G4cout<<"TrajIdR1VectSize and  TrajIdR1AgelVectSize "
    //       << TrajIdVectR1.size()
    //       <<"     "<< TrajIdVectR1Agel.size()<<G4endl;
    // G4cout<<"TrajIdVectRich2Size =  "<< TrajIdVectR2.size()<<G4endl;

    // store the trackid info.

    aRichCounter-> setTrackIdTraverseRich1Gas( TrajIdVectR1);
    aRichCounter-> setTrackIdTraverseRich1Agel(TrajIdVectR1Agel);
    aRichCounter-> setTrackIdTraverseRich2Gas(TrajIdVectR2);


    
    int NumTrajR1=  TrajIdVectR1.size();
    int NumTrajR1Agel = TrajIdVectR1Agel.size();
    int NumTrajR2=  TrajIdVectR2.size();

    std::vector<int>TrajNumHitGasRich1( NumTrajR1,0);
    std::vector<int>TrajSatNumHitGasRich1( NumTrajR1,0);
    std::vector<int>TrajSatNumHitGasRich1NoHpdRefl( NumTrajR1,0);
    std::vector<int>TrajNumHitAgelRich1( NumTrajR1Agel,0);
    std::vector<int>TrajSatNumHitAgelRich1( NumTrajR1Agel,0);
    std::vector<int>TrajSatNumHitAgelRich1NoHpdRefl( NumTrajR1Agel,0);
    std::vector<int>TrajNumHitAgelWithRayleighRich1( NumTrajR1Agel,0);
    std::vector<int>TrajSatNumHitAgelWithRayleighRich1( NumTrajR1Agel,0);
    std::vector<int>TrajSatNumHitAgelWithRayleighRich1NoHpdRefl( NumTrajR1Agel,0);
    std::vector<int>TrajNumHitGasRich2( NumTrajR2,0);
    std::vector<int>TrajSatNumHitGasRich2( NumTrajR2,0);
    std::vector<int>TrajSatNumHitGasRich2NoHpdRefl( NumTrajR2,0);
    std::vector<int>TrajSatNumHitGasRich2Scint( NumTrajR2,0);
    std::vector<int>TrajSatNumHitGasRich2NoHpdReflNoScint( NumTrajR2,0);
    std::vector<int>TrajSatNumHitGasRich1NoHpdReflHighMom( NumTrajR1,0);
    std::vector<int>TrajSatNumHitGasRich2NoHpdReflNoScintHighMom( NumTrajR2,0);
    

    
    for (int ihcolb=0; ihcolb<NumRichCollection; ihcolb++) {

      Current_RichG4CollectionID =RichG4CollectionID[ihcolb];

      if(Current_RichG4CollectionID >=0 ) {
     
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

          G4int aPhotSource = bHit->PhotonSourceProcessInfo();
          
          //is it a reflected hit?    

	       std::vector<bool> aHpdRefl = bHit->DecodeRichHpdReflectionFlag();

	       bool areflectedInHpd= bHit->ElectronBackScatterFlag(); //plot without any bsc or refl
         for(int ii=0; ii<(int)aHpdRefl.size() ; ++ii) 
         {
           if( aHpdRefl [ii]) areflectedInHpd=true; 
	       }

          G4double aChTrackTotMom =  bHit->ChTrackTotMom() ;

          int ChtkId =  (int) (bHit-> GetChTrackID()) ;
          G4int aRadiatorNum=  bHit->GetRadiatorNumber();

          G4double ChTkPDGMass =  bHit->RichChTrackMass();

          G4double ChTkEnergy =
            pow( (ChTkPDGMass*ChTkPDGMass+ aChTrackTotMom* aChTrackTotMom),0.5);
          G4double ChTkBeta=0.0;

          if( ChTkEnergy > 0.0 ) {

            ChTkBeta = aChTrackTotMom/ChTkEnergy;

            // G4cout<<"trackId  Beta  Mom Mass  "
            //    <<ChtkId<<"   "<< ChTkBeta <<"    "
            //    <<aChTrackTotMom<<"    "
            //    <<ChTkPDGMass
            //    <<G4endl;

          } //end if

          G4int NumRayleighScat = bHit->OptPhotRayleighFlag();
          
          int it=0;
          while(it < (int) TrajIdVectR1Agel.size()) {

            if(TrajIdVectR1Agel[it] ==  ChtkId) {
              if( aRMIdValues ->  IsRich1AerogelAnyTileRad( aRadiatorNum ) ) {
                     TrajNumHitAgelWithRayleighRich1[it]++;

                  if(NumRayleighScat == 0 ) {
                     TrajNumHitAgelRich1[it]++;
                  } //end if
                  
                if( ChTkBeta >  ChTkBetaSaturatedCut) {

                  //               if(  ( aChTrackTotMom >  MinMomAgelCut)  &&
                  //    ( aChTrackTotMom <  MaxMomAgelCut ) ) {
                    TrajSatNumHitAgelWithRayleighRich1[it]++;
                  if(!areflectedInHpd) TrajSatNumHitAgelWithRayleighRich1NoHpdRefl[it]++;

                  if(NumRayleighScat == 0 ) {
                    
                    TrajSatNumHitAgelRich1[it]++;
                    if(!areflectedInHpd) TrajSatNumHitAgelRich1NoHpdRefl[it]++;
                  }
                  

                  //                 }

                }//end if

              }
              // now skip out
              it =  TrajIdVectR1Agel.size() +1;

            }//end if


            it++;

          }//end while for int

          int itr1g = 0;

          while(itr1g < (int) TrajIdVectR1.size()) {


            if(TrajIdVectR1[itr1g] ==  ChtkId) {


              if( aRadiatorNum ==  ( aRMIdValues -> Rich1GaseousCkvRadiatorNum()) ) {

                TrajNumHitGasRich1[itr1g]++;

                if( ChTkBeta >  ChTkBetaSaturatedCut) {

                  //                 if(aChTrackTotMom > MinMomc4f10Cut ) {

                  TrajSatNumHitGasRich1[itr1g]++;
                  if(!areflectedInHpd) TrajSatNumHitGasRich1NoHpdRefl[itr1g]++;
                  if(aChTrackTotMom >  m_MinC4F10HighMomCutForYield ) {
                    if(!areflectedInHpd)  TrajSatNumHitGasRich1NoHpdReflHighMom[itr1g]++;
                  }

                  //                }

                }
              }
              // now skip out
              itr1g =  TrajIdVectR1.size() +1;

            }

            itr1g++;

          } // end loop over traj ID


          //          G4cout<<" Loooping through Rich2 vect "<<G4endl;

          int it2=0;
          while(it2 < (int) TrajIdVectR2.size()) {

            if(TrajIdVectR2[it2] ==  ChtkId) {
              if(aRadiatorNum == ( aRMIdValues -> Rich2GaseousCkvRadiatorNum() )   ) {

                TrajNumHitGasRich2[it2]++;

                //             G4cout<<" Beta and BetaCut  "
                //    << ChTkBeta<<"   "<< ChTkBetaSaturatedCut<<G4endl;

                if( ChTkBeta >  ChTkBetaSaturatedCut) {
                  //              if( ChTkBeta >  0.9999 ) {

                  //                if(  aChTrackTotMom > 10000 ) {

                  TrajSatNumHitGasRich2[it2]++;
                  if(!areflectedInHpd) TrajSatNumHitGasRich2NoHpdRefl[it2]++;
                  if((!areflectedInHpd) && (aPhotSource != 2 ))TrajSatNumHitGasRich2NoHpdReflNoScint[it2]++;
                  if(aPhotSource == 2 )TrajSatNumHitGasRich2Scint[it2]++;
                   if(aChTrackTotMom > m_MinCF4HighMomCutForYield ) {
                     
                    if(( !areflectedInHpd) && (aPhotSource != 2)) TrajSatNumHitGasRich2NoHpdReflNoScintHighMom[it2]++; 
                  }
                 
                  //  }

                }



              }





              it2 =  TrajIdVectR2.size() +1;

            }




            it2++;

          } // end loop over TrajIdVectR2.


        }  // end loop over hits in a coll.

        
      }
      
      
      }
      


    }   //end loop over collections



    aRichCounter-> setNumHitPerTrackRich1Gas(TrajNumHitGasRich1);
    aRichCounter-> setNumHitSaturatedPerTrackRich1Gas(TrajSatNumHitGasRich1);
    aRichCounter-> setNumHitSaturatedPerTrackRich1GasNoHpdRefl(TrajSatNumHitGasRich1NoHpdRefl);
    aRichCounter-> setNumHitPerTrackRich1Agel( TrajNumHitAgelRich1);
    aRichCounter-> setNumHitPerTrackRich1WithRlyAgel( TrajNumHitAgelWithRayleighRich1);
    aRichCounter->
      setNumHitSaturatedPerTrackRich1Agel(TrajSatNumHitAgelRich1);
    aRichCounter->
      setNumHitSaturatedPerTrackRich1AgelNoHpdRefl(TrajSatNumHitAgelRich1NoHpdRefl);
    aRichCounter->
      setNumHitSaturatedPerTrackRich1WithRlyAgel(TrajSatNumHitAgelWithRayleighRich1);
    aRichCounter->
      setNumHitSaturatedPerTrackRich1WithRlyAgelNoHpdRefl(TrajSatNumHitAgelWithRayleighRich1NoHpdRefl);
    aRichCounter-> setNumHitPerTrackRich2Gas(TrajNumHitGasRich2);
    aRichCounter-> setNumHitSaturatedPerTrackRich2Gas(TrajSatNumHitGasRich2);
    aRichCounter-> setNumHitSaturatedPerTrackRich2GasNoHpdRefl(TrajSatNumHitGasRich2NoHpdRefl);
    aRichCounter-> setNumHitSaturatedPerTrackRich2GasNoHpdReflNoScint(TrajSatNumHitGasRich2NoHpdReflNoScint);
    aRichCounter-> setNumHitSaturatedPerTrackRich2GasScint(TrajSatNumHitGasRich2Scint);
    aRichCounter->  setNumHitSaturatedPerTrackRich1GasNoHpdReflHighMom( TrajSatNumHitGasRich1NoHpdReflHighMom );
    aRichCounter->  setNumHitSaturatedPerTrackRich2GasNoHpdReflNoScintHighMom(TrajSatNumHitGasRich2NoHpdReflNoScintHighMom);
    

    // now to test the procedure
    //         for(int i=0; i< (int) TrajIdVectR1.size(); i++ ) {

    //         G4cout<<"TrackInList Rich1Gas SatTkHitNum AlltkHitnum =   "
    //          <<i<<"   "<<TrajSatNumHitGasRich1[i]
    //         <<"     "<<  TrajNumHitGasRich1[i]<<G4endl;
    //         }
    //     for(int ig=0; ig< (int) TrajIdVectR1Agel.size(); ig++ ) {
    //        G4cout<<"TrackInList Rich1Agel SatTkHitNum AlltkHitnum =   "
    //              <<ig<<"   "<< TrajSatNumHitAgelRich1[ig]
    //              <<"    "<< TrajNumHitAgelRich1[ig]<<G4endl;
    //
    //          }

    // for test

    //       G4cout<<"Num Sat traj Rich2 "
    //   <<  aRichCounter->NumHitSaturatedPerTrackRich2Gas().size() <<G4endl;
    //
    //              for(int ia=0; ia< (int)  TrajSatNumHitGasRich2.size(); ia++ ) {
    //
    //                G4cout<<  "TrackInListR2    Rich2GasSatHitNum  "
    //       <<ia<<"    "<< TrajSatNumHitGasRich2[ia]<<G4endl;


    //  }





  }



  //   G4cout<<"End of   RichG4CountSaturatedHits " <<G4endl;

};

void RichG4EventHitCount::RichG4CountSaturatedHitsFullAcc(const G4Event* anEvent,  int NumRichColl,
                                                   const std::vector<int> & RichG4CollectionID )
{
  std::vector<int>  TrajIdVectR1FullAcc;
  std::vector<int>  TrajIdVectR1AgelFullAcc;
  std::vector<int>  TrajIdVectR2FullAcc;
  std::vector<G4ThreeVector> TrajIdVectMomR1FullAcc;
  std::vector<G4ThreeVector> TrajIdVectMomAgelFullAcc;
 std::vector<G4ThreeVector> TrajIdVectMomR2FullAcc;

  TrajIdVectR1FullAcc.clear();
  TrajIdVectR1FullAcc.reserve(100);
  TrajIdVectR1AgelFullAcc.clear();
  TrajIdVectR1AgelFullAcc.reserve(100);
  TrajIdVectR2FullAcc.clear();
  TrajIdVectR2FullAcc.reserve(100);
  TrajIdVectMomR1FullAcc.clear();
  TrajIdVectMomR1FullAcc.reserve(100);
  TrajIdVectMomAgelFullAcc.clear();
  TrajIdVectMomAgelFullAcc.reserve(100);
  TrajIdVectMomR2FullAcc.clear();
  TrajIdVectMomR2FullAcc.reserve(100);
  
  RichG4RadiatorMaterialIdValues* aRMIdValues= RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

  
//   G4cout<<"  Now counting Saturated numhits in Rich1 Rich2 with full acceptance "<<G4endl;
  G4HCofThisEvent * HCE;
  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();
  //  G4cout<<" Now in RichG4CountSaturated Hits "<< G4endl;
  
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
            G4ThreeVector ChMomVect = aHit-> ChTrackMomVect() ;
            
            //G4cout<<" TRkId for rich1 hits  "<< ChtkId <<G4endl;
             bool trajAlreadyStoredR1FullAcc=true;;
             bool trajtraversedRich1FullAcc=false;

            bool trajAlreadyStoredR1AgelFullAcc=true;;
            bool trajtraversedRich1AgelFullAcc=false;

            bool trajAlreadyStoredR2FullAcc=true;;
            bool trajtraversedRich2FullAcc=false;
           
            if(TrajIdVectR1FullAcc.size() > 0 ) {

              std::vector<int>::iterator p = find(TrajIdVectR1FullAcc.begin(),
                                                  TrajIdVectR1FullAcc.end(),
                                                  ChtkId);
              if( p == TrajIdVectR1FullAcc.end() ) trajAlreadyStoredR1FullAcc=false;


            } else {

              trajAlreadyStoredR1FullAcc=false;

            }


            if(TrajIdVectR1AgelFullAcc.size() > 0 ) {

              std::vector<int>::iterator p = find(TrajIdVectR1AgelFullAcc.begin(),
                                                  TrajIdVectR1AgelFullAcc.end(),
                                                  ChtkId);
              if( p == TrajIdVectR1AgelFullAcc.end() ) trajAlreadyStoredR1AgelFullAcc=false;


            } else {

              trajAlreadyStoredR1AgelFullAcc=false;

            }

            if(TrajIdVectR2FullAcc.size() > 0 ) {

              std::vector<int>::iterator p = find(TrajIdVectR2FullAcc.begin(),
                                                  TrajIdVectR2FullAcc.end(),
                                                  ChtkId);
              if( p == TrajIdVectR2FullAcc.end() ) trajAlreadyStoredR2FullAcc=false;


            } else {

              trajAlreadyStoredR2FullAcc=false;

            }

            if( !( trajAlreadyStoredR1FullAcc) ) {

              // Now loop through the trajectory container to see
              // this track goes through the whle length of Rich1.
              // This is done by checking if the first point of the
              // trajectory is upstream of Rich1 and the last point is
              // downstream of Rich1.


              trajtraversedRich1FullAcc= Rich1TrajTraverse(anEvent, ChtkId, true );

              if( trajtraversedRich1FullAcc) {

                TrajIdVectR1FullAcc.push_back(ChtkId);
                TrajIdVectMomR1FullAcc.push_back(ChMomVect);
                

              }

            }

            if( !( trajAlreadyStoredR1AgelFullAcc) ) {

              // Now loop through the trajectory container to see
              // this track goes through the whle length of Rich1 agel.
              // This is done by checking if the first point of the
              // trajectory i supstream of Rich1 agel and the last point is
              // downstream of Rich1 agel.


              trajtraversedRich1AgelFullAcc= Rich1AgelTrajTraverse(anEvent, ChtkId , true);

              if( trajtraversedRich1AgelFullAcc) {

                TrajIdVectR1AgelFullAcc.push_back(ChtkId);
                TrajIdVectMomAgelFullAcc.push_back(ChMomVect);
                
              }

            }


            // now for rich2
            if( !( trajAlreadyStoredR2FullAcc) ) {

              // Now loop through the trajectory container to see
              // this track goes through the whle length of Rich2.
              // This is done by checking if the first point of the
              // trajectory i supstream of Rich2 and the last point is
              // downstream of Rich2.


              trajtraversedRich2FullAcc= Rich2TrajTraverse(anEvent, ChtkId , true );

              if( trajtraversedRich2FullAcc) {

                TrajIdVectR2FullAcc.push_back(ChtkId);
                TrajIdVectMomR2FullAcc.push_back(ChMomVect);
                
              }

            }





          }// endloop through hits
          
        }
        
      }
      
    }// end loop through the hit collections
    
    aRichCounter-> setTrackIdFullAcceptRich1Gas ( TrajIdVectR1FullAcc);
    aRichCounter-> setTrackIdFullAcceptRich1Agel (TrajIdVectR1AgelFullAcc);
    aRichCounter-> setTrackIdFullAcceptRich2Gas (TrajIdVectR2FullAcc);
    aRichCounter-> setTrackMomFullAcceptRich1Gas(TrajIdVectMomR1FullAcc);
    aRichCounter-> setTrackMomFullAcceptRich1Agel(TrajIdVectMomAgelFullAcc);
    aRichCounter-> setTrackMomFullAcceptRich2Gas( TrajIdVectMomR2FullAcc);
    

    
    int NumTrajR1FullAcc=  TrajIdVectR1FullAcc.size();
    int NumTrajR1AgelFullAcc = TrajIdVectR1AgelFullAcc.size();
    int NumTrajR2FullAcc=  TrajIdVectR2FullAcc.size();

    std::vector<int>TrajSatNumHitFullAccAgelRich1( NumTrajR1AgelFullAcc,0);
 
    std::vector<int>TrajSatNumHitFullAccGasRich1( NumTrajR1FullAcc,0);
    std::vector<int>TrajSatNumHitFullAccGasRich1NoHpdReflHighMom( NumTrajR1FullAcc,0);

    std::vector<int>TrajSatNumHitFullAccGasRich2( NumTrajR2FullAcc,0);
    std::vector<int>TrajSatNumHitFullAccGasRich2NoScint( NumTrajR2FullAcc,0);
    std::vector<int>TrajSatNumHitFullAccGasRich2NoScintHighMom( NumTrajR2FullAcc,0);

    


    for (int ihcolb=0; ihcolb<NumRichCollection; ihcolb++) {

      Current_RichG4CollectionID =RichG4CollectionID[ihcolb];

      if(Current_RichG4CollectionID >=0 ) {
     
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

          G4int aPhotSource = bHit->PhotonSourceProcessInfo();
          
          //is it a reflected hit?    

	       std::vector<bool> aHpdRefl = bHit->DecodeRichHpdReflectionFlag();

	       bool areflectedInHpd= bHit->ElectronBackScatterFlag(); //plot without any bsc or refl
         for(int ii=0; ii<(int)aHpdRefl.size() ; ++ii) 
         {
           if( aHpdRefl [ii]) areflectedInHpd=true; 
	       }

          G4double aChTrackTotMom =  bHit->ChTrackTotMom() ;

          int ChtkId =  (int) (bHit-> GetChTrackID()) ;
          G4int aRadiatorNum=  bHit->GetRadiatorNumber();

          G4double ChTkPDGMass =  bHit->RichChTrackMass();

          G4double ChTkEnergy =
            pow( (ChTkPDGMass*ChTkPDGMass+ aChTrackTotMom* aChTrackTotMom),0.5);
          G4double ChTkBeta=0.0;

          if( ChTkEnergy > 0.0 ) {

            ChTkBeta = aChTrackTotMom/ChTkEnergy;

            // G4cout<<"trackId  Beta  Mom Mass  "
            //    <<ChtkId<<"   "<< ChTkBeta <<"    "
            //    <<aChTrackTotMom<<"    "
            //    <<ChTkPDGMass
            //    <<G4endl;

          } //end if

          G4int NumRayleighScat = bHit->OptPhotRayleighFlag();
          
          int it=0;
          while(it < (int) TrajIdVectR1AgelFullAcc.size()) {

            if(TrajIdVectR1AgelFullAcc[it] ==  ChtkId) {
              if( aRMIdValues ->  IsRich1AerogelAnyTileRad( aRadiatorNum ) ) {
                //if(NumRayleighScat == 0 ) {
                //     TrajNumHitAgelRich1FullAcc[it]++;
                //  } //end if
                  
                if( ChTkBeta >  ChTkBetaSaturatedCut) {

                  //               if(  ( aChTrackTotMom >  MinMomAgelCut)  &&
                  //    ( aChTrackTotMom <  MaxMomAgelCut ) ) {
                    
                  if(!areflectedInHpd) {
                   if(NumRayleighScat == 0 ) {
                    if(!areflectedInHpd)TrajSatNumHitFullAccAgelRich1 [it]++;
                   }
                   
                  }
                  
                  
                  
                  //                 }

                }//end if chtkbeta                
              } // end if aerogel test
              
              
              // now skip out
              it =  TrajIdVectR1AgelFullAcc.size() +1;

            }//end if


            it++;

          }//end while for int

          int itr1g = 0;

          while(itr1g < (int) TrajIdVectR1FullAcc.size()) {


            if(TrajIdVectR1FullAcc[itr1g] ==  ChtkId) {


              if( aRadiatorNum ==  ( aRMIdValues -> Rich1GaseousCkvRadiatorNum()) ) {

                if( ChTkBeta >  ChTkBetaSaturatedCut) {

                  //                 if(aChTrackTotMom > MinMomc4f10Cut ) {

                  //TrajSatNumHitGasRich1[itr1g]++;

                  if(!areflectedInHpd)TrajSatNumHitFullAccGasRich1 [itr1g]++;

                  if(aChTrackTotMom >  m_MinC4F10HighMomCutForYield ) {
                    if(!areflectedInHpd)  TrajSatNumHitFullAccGasRich1NoHpdReflHighMom[itr1g]++;
                  }

                  //                }

                }
              }
              // now skip out
              itr1g =  TrajIdVectR1FullAcc.size() +1;

            }

            itr1g++;

          } // end loop over traj ID


          //          G4cout<<" Loooping through Rich2 vect "<<G4endl;

          int it2=0;
          while(it2 < (int) TrajIdVectR2FullAcc.size()) {

            if(TrajIdVectR2FullAcc[it2] ==  ChtkId) {
              if(aRadiatorNum == ( aRMIdValues -> Rich2GaseousCkvRadiatorNum() )   ) {

                //TrajNumHitGasRich2FullAcc[it2]++;

                //             G4cout<<" Beta and BetaCut  "
                //    << ChTkBeta<<"   "<< ChTkBetaSaturatedCut<<G4endl;

                if( ChTkBeta >  ChTkBetaSaturatedCut) {
                  //              if( ChTkBeta >  0.9999 ) {

                  //                if(  aChTrackTotMom > 10000 ) {

                  
                  if(!areflectedInHpd) TrajSatNumHitFullAccGasRich2[it2]++;
                  if((!areflectedInHpd) && (aPhotSource != 2 ))TrajSatNumHitFullAccGasRich2NoScint[it2]++;
                   if(aChTrackTotMom > m_MinCF4HighMomCutForYield ) {
                    if(( !areflectedInHpd) && (aPhotSource != 2)) TrajSatNumHitFullAccGasRich2NoScintHighMom[it2]++; 
                  }
                 
                  //  }

                }



              }





              it2 =  TrajIdVectR2FullAcc.size() +1;

            }




            it2++;

          } // end loop over TrajIdVectR2.


        }  // end loop over hits in a coll.

        
      }}
      
      


    }   //end loop over collections

    aRichCounter->setNumHitFullAcceptSatPerTrackR1Agel(TrajSatNumHitFullAccAgelRich1);
    aRichCounter->setNumHitFullAcceptSatPerTrackR1Gas (TrajSatNumHitFullAccGasRich1);
    aRichCounter->setNumHitFullAcceptSatPerTrackR2Gas(TrajSatNumHitFullAccGasRich2);
    
    aRichCounter->setNumHitFullAcceptSatPerTrackHighMomR1Gas(TrajSatNumHitFullAccGasRich1NoHpdReflHighMom);
    aRichCounter->setNumHitFullAcceptSatPerTrackNoScintR2Gas(TrajSatNumHitFullAccGasRich2NoScint);
    aRichCounter->setNumHitFullAcceptSatPerTrackNoScintHighMomR2Gas(TrajSatNumHitFullAccGasRich2NoScintHighMom);
    

  }// end test richcounter existance
  
  
  
}



void RichG4EventHitCount::RichG4CountAndClassifyHits( const G4Event* anEvent,  int NumRichColl,
                                                      const std::vector<int> & RichG4CollectionID )
{


  
  RichG4RadiatorMaterialIdValues* aRMIdValues= RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

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
	  //                  G4cout<<"EndEvAction      "<< nHitInCurColl
          //     <<"   are stored in RichHitCollection set   "
          //      <<ihcol<< G4endl;

          for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {

            RichG4Hit* aHit = (*RHC)[iha];

	    std::vector<bool> aHpdRefl = aHit->DecodeRichHpdReflectionFlag();
	    bool areflectedInHpd= false;
            for(int ii=0; ii<(int)aHpdRefl.size() ; ++ii) {
              if( aHpdRefl [ii]) areflectedInHpd=true; 
	    }
            G4int aBackScatFlag = aHit->ElectronBackScatterFlag() ;
            RichG4HpdReflectionFlag* aRichG4HpdReflectionFlag= 
                 RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();

	    std::vector<bool> aBVect = aHit-> DecodeRichHpdReflectionFlag();
            bool aHpdQwPcRefl=false;
            if(  aBVect [aRichG4HpdReflectionFlag->HpdQwPCRefl()]){
              aHpdQwPcRefl=true;  
	    }         
            
            bool aHpdChroRefl=false;
            if(  aBVect [aRichG4HpdReflectionFlag->  HpdChromiumRefl()]){
              aHpdChroRefl=true;  
	    }         
            bool aHpdSiDetRefl=false;

            if(  aBVect [aRichG4HpdReflectionFlag->  HpdSiliconRefl()]){
              aHpdSiDetRefl=true;  
	    }           
            bool aHpdKovDetRefl=false;

            if(  aBVect [aRichG4HpdReflectionFlag->  HpdKovarRefl()]){
              aHpdKovDetRefl=true;  
	    }           
            bool aHpdKapDetRefl=false;

            if(  aBVect [aRichG4HpdReflectionFlag->  HpdKaptonRefl()]){
              aHpdKapDetRefl=true;  
	    }         

            G4int aRadiatorNum=  aHit->GetRadiatorNumber();
            // Momentum of charged track in MeV
            G4double aChTrackTotMom =  aHit->ChTrackTotMom() ;
            // G4double CkvTheta=      aHit->  ThetaCkvAtProd();
            G4int ChtkId =  aHit-> GetChTrackID();
            G4double ChTkPDGMass =  aHit->RichChTrackMass();

            G4int aPhotonSource= aHit->PhotonSourceProcessInfo();
            
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
              if(  areflectedInHpd) aRichCounter-> bumpNumHitTotRich1HpdRefl();
            }

            if(ihcol == 2 || ihcol == 3 ) {

              aRichCounter->bumpNumHitTotRich2All();
              if(  areflectedInHpd) aRichCounter-> bumpNumHitTotRich2HpdRefl();

            }


            if(   aRadiatorNum == ( aRMIdValues -> Rich1GaseousCkvRadiatorNum () ) ){

              aRichCounter->bumpNumHitTotRich1Gas();

              if(aPhotonSource == 1   ) aRichCounter->bumpNumHitTotRich1GasCherenkovProc();
              
	      // now for the hpd reflection, backscattering etc.
	      if(  areflectedInHpd) {                
                 aRichCounter->bumpNumHitTotRich1GasHpdRefl();                               
	      }
              if(aBackScatFlag >0 ) {
		aRichCounter->bumpNumHitTotRich1GasHpdBackScat();
	      }

              if(aHpdQwPcRefl ) {
          	aRichCounter->bumpNumHitTotRich1GasHpdQWPCRefl();
	      }
              if( aHpdChroRefl ) {
                aRichCounter-> bumpNumHitTotRich1GasHpdChromRefl();
	      }
              if(aHpdSiDetRefl){
		aRichCounter-> bumpNumHitTotRich1GasHpdSiliconRefl();
	      }
              if(aHpdKovDetRefl){
		aRichCounter-> bumpNumHitTotRich1GasHpdKovarRefl();
	      }
              if(aHpdKapDetRefl){
		aRichCounter-> bumpNumHitTotRich1GasHpdKaptonRefl();
	      }


              if(  ChtkId <= 1 ) {

                aRichCounter->bumpNumHitPartGunPrimaryPartRich1Gas();

              }

            }else if(aRMIdValues -> IsRich1AerogelAnyTileRad (aRadiatorNum)) {

              aRichCounter->bumpNumHitTotRich1Agel();
              if(aPhotonSource == 1   ) aRichCounter->bumpNumHitTotRich1AgelCherenkovProc();

              if(aHpdKapDetRefl){
            		aRichCounter->bumpNumHitTotRich1AerogelHpdKaptonRefl();
	      }
              if(  ChtkId <= 1 ) {

                aRichCounter->bumpNumHitPartGunPrimaryPartRich1Agel();

              }

            } else if (  aRadiatorNum == (aRMIdValues -> Rich2GaseousCkvRadiatorNum()) ) {

              aRichCounter->bumpNumHitTotRich2Gas();
              if(aPhotonSource == 1   ) { 
                    aRichCounter->bumpNumHitTotRich2GasCherenkovProc();
              }else if(aPhotonSource == 2   ){aRichCounter->bumpNumHitTotRich2GasScintProc(); 
              }
              
            
              

           	      if(  areflectedInHpd) {                
                   aRichCounter->bumpNumHitTotRich2GasHpdRefl();                               
	                }
                  if(aBackScatFlag >0 ) {
              		  aRichCounter->bumpNumHitTotRich2GasHpdBackScat();
	                }

                 if(aHpdQwPcRefl ) {
                	aRichCounter->bumpNumHitTotRich2GasHpdQWPCRefl();
                 }
                if( aHpdChroRefl ) {
                  aRichCounter-> bumpNumHitTotRich2GasHpdChromRefl();
	              }
                if(aHpdSiDetRefl){
	              	aRichCounter-> bumpNumHitTotRich2GasHpdSiliconRefl();
	              }
                if(aHpdKovDetRefl){
	             	aRichCounter-> bumpNumHitTotRich2GasHpdKovarRefl();
	              }
                if(aHpdKapDetRefl){
             		aRichCounter-> bumpNumHitTotRich2GasHpdKaptonRefl();
	              }



              if(  ChtkId <= 1 ) {

                aRichCounter->bumpNumHitPartGunPrimaryPartRich2Gas();
                
              }
              
              
              

            }else if (aRadiatorNum == ( aRMIdValues ->  RichFilterGenericCkvRadiatorNum() ) ) {
      	      aRichCounter-> bumpNumHitTotRich1FilterGeneric();

            }else if ( aRadiatorNum == ( aRMIdValues -> RichFilterD263CkvRadiatorNum() ) ) {
         	      aRichCounter->bumpNumHitTotRich1FilterD263();

            }else if ( aRadiatorNum == ( aRMIdValues -> Rich1GasQWindowCkvRadiatorNum()) ) {
          	      aRichCounter->bumpNumHitTotRich1GasQw();
            }else if ( aRadiatorNum == ( aRMIdValues -> Rich2GasQWindowCkvRadiatorNum()) ) {
	                aRichCounter->bumpNumHitTotRich2GasQw();
	          }else if ( aRadiatorNum == ( aRMIdValues -> RichHpdQuartzWindowCkvRadiatorNum()) ) {
              
              if(ihcol == 0 || ihcol == 1 ) {
	               aRichCounter-> bumpNumHitTotRich1HpdQw();
	            }else if (ihcol == 2 || ihcol == 3 ) {
	               aRichCounter-> bumpNumHitTotRich2HpdQw();
              } 

             } else {
   

       	      aRichCounter->bumpNumHitTotNoRadiator();
             
            }
          }



        }

      }

    }

  }




}

//=============================================================================
