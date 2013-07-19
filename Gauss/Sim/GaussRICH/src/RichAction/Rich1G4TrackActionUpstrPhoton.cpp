// $Id: Rich1G4TrackActionUpstrPhoton.cpp,v 1.4 2007-01-12 15:32:02 ranjard Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h" 

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// Geant4
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4OpticalPhoton.hh"
#include "globals.hh"
#include <math.h>
#include "G4VProcess.hh"

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

// GiGa
#include "GiGa/GiGaTrajectory.h"

// local
#include "GaussRICH/Rich1G4TrackActionUpstrPhoton.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Rich1G4TrackActionUpstrPhoton
//
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory moved to Factories.cpp
//DECLARE_TOOL_FACTORY( Rich1G4TrackActionUpstrPhoton );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich1G4TrackActionUpstrPhoton::Rich1G4TrackActionUpstrPhoton
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaTrackActionBase( type , name , parent ) {  }


//=============================================================================
// Destructor
//=============================================================================
Rich1G4TrackActionUpstrPhoton::~Rich1G4TrackActionUpstrPhoton(){ }

//=============================================================================
// PreUserTrackingAction
//=============================================================================
void Rich1G4TrackActionUpstrPhoton::PreUserTrackingAction
( const G4Track* aTrack ) {

  if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!

  const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

    //    G4double CurPhotonOrigZcoord= aTrack->GetVertexPosition().z();
    G4double CurPhotonZcoord= aTrack->GetPosition().z();

    // test by SE
    //    G4cout<<" Cur phot Zorgin = "<<CurPhotonOrigZcoord<<G4endl;
    // G4cout<<" Cur phot XYZ orgin = "<< aTrack->GetVertexPosition() <<G4endl;
    // G4cout<<" Cur phot XYZ pos =  "<< aTrack->GetPosition() <<G4endl;


    // end of test by SE
    // Kill the photons created upstream of Aerogel.
    // following change made in July 2003 since origin seems to
    // be not set at this stage, hence  it is 0,0,0 always.
    //    if( CurPhotonOrigZcoord >= ZUpsRich1Analysis &&
    //    CurPhotonOrigZcoord <=  AgelZBeginAnalysis ) {
    if( CurPhotonZcoord >= ZUpsRich1Analysis &&
        CurPhotonZcoord <  AgelZBeginAnalysis ) {

      trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);

      //        G4cout<<"Photon Killed upstr end of rich1 Z= "
      //      <<CurPhotonZcoord<<G4endl;

    }
    // Kill photons in Rich1 which are produced
    // downstream of the Exitwall.

    //    if( CurPhotonOrigZcoord >=  Rich1ExitWallBeginZAnalysis &&
    //   CurPhotonOrigZcoord <=  ZDnsRich1Analysis ) {
    if( CurPhotonZcoord >=  Rich1ExitWallBeginZAnalysis &&
        CurPhotonZcoord <=  ZDnsRich1Analysis ) {

      trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);
      //       G4cout<<"Photon Killed downstr end of rich1 Z= "
      //      <<CurPhotonZcoord<<G4endl;


    }


  }

}

//=============================================================================
// PostUserTrackingAction
//=============================================================================
void Rich1G4TrackActionUpstrPhoton::PostUserTrackingAction( const G4Track* /*aTrack*/ )
{ }











