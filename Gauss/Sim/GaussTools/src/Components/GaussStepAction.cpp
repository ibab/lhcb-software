// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
///
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
///
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
//GaussTools
#include "GaussTools/GaussTrackInformation.h"
/// local
#include "GaussStepAction.h"

/** @file 
 *  
 *  Implementation of class GaussStepAction
 *
 *  @author Witek Pokorski
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GaussStepAction ) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussStepAction::GaussStepAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaStepActionBase ( type , name , parent )
  // points where a secondary was produced to be store
  , m_storePointsOfSecondaries ( false )
  // points where a secondary was produced but not a delta-electron
  , m_storeNotDeltaE ( false )
  // points where hits where generated to be stored
    , m_storeHitPoints ( false )
  // tracking cuts
    , m_trcuteg (1.0*MeV), m_trcuthadr (10.0*MeV)
{
  declareProperty ("StorePointsOfSecondaries", m_storePointsOfSecondaries);
  declareProperty ("StoreNotDeltaE", m_storeNotDeltaE);
  declareProperty ("StoreHitPoints", m_storeHitPoints);  
  declareProperty ("TrCutElGamma", m_trcuteg);
  declareProperty ("TrCutHadr", m_trcuthadr);
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GaussStepAction::~GaussStepAction(){};
// ============================================================================

// ============================================================================
/** stepping action
 *  @see G4UserSteppingAction
 *  @param step Geant4 step
 */
// ============================================================================
void GaussStepAction::UserSteppingAction ( const G4Step* step ) 
{
  G4Track* track = step->GetTrack();
  G4VUserTrackInformation* uinf = track->GetUserInformation(); 
  GaussTrackInformation*    ginf = 
    ( 0 == uinf )  ? 0 : static_cast<GaussTrackInformation*> ( uinf );

  G4ParticleDefinition* partdef=track->GetDefinition();

  /// if a hit created, append step
  if (m_storeHitPoints && ginf->createdHit()) 
    {
      ginf->setCreatedHit(false);
      ginf->setAppendStep(true);
      return;
    }
  
  /// for optical photons also the reflection/refraction step must be appended  
  else if ( step->GetPostStepPoint()->GetStepStatus() == 
            fGeomBoundary &&
            partdef == G4OpticalPhoton::OpticalPhoton()    )
    { 
      ginf->setAppendStep(true); 
      return;
    }

  //   if  there are some secondaries, the step must be appended  
  //  else if ( m_storePointsOfSecondaries && 
  //          0 != stepMgr()->GetSecondary() && 
  //         0 != stepMgr()->GetSecondary()->size   () ) 
  //  { 
  //    ginf->setAppendStep(true); 
  //    return;
  //  }

  if ( partdef == G4Electron::ElectronDefinition() ||
       partdef == G4Positron::PositronDefinition() ||
       partdef == G4Gamma::GammaDefinition() )
    { 
      if (track->GetKineticEnergy() <  m_trcuteg)
        track->SetTrackStatus(fStopAndKill);
   }
  else if(track->GetKineticEnergy() < m_trcuthadr)
    track->SetTrackStatus(fStopAndKill);


};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
