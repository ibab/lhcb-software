// ============================================================================
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// Geant4 
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
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaTrajectory.h"
/// local
#include "Rich1G4TrackActionUpstrPhoton.h"
#include "RichG4AnalysisConstGauss.h"
///
// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( Rich1G4TrackActionUpstrPhoton );
// ============================================================================

// ============================================================================
// ============================================================================
Rich1G4TrackActionUpstrPhoton::Rich1G4TrackActionUpstrPhoton
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ) { ; }
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
Rich1G4TrackActionUpstrPhoton::~Rich1G4TrackActionUpstrPhoton(){; }

void Rich1G4TrackActionUpstrPhoton::PreUserTrackingAction  
       ( const G4Track* aTrack ) {

  if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!

  const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
    
    G4double CurPhotonOrigZcoord= aTrack->GetVertexPosition().z();

    // Kill the photons created upstream of Aerogel.

    if( CurPhotonOrigZcoord >= ZUpsRich1Analysis && 
        CurPhotonOrigZcoord <=  AgelZBeginAnalysis ) {
      
       trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);

      
    }
    
    
  }
  
}



void Rich1G4TrackActionUpstrPhoton::PostUserTrackingAction 
   ( const G4Track* aTrack ) 
{;} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================











