// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
///
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
///
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

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
DECLARE_TOOL_FACTORY( GaussStepAction )
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
  // points where hits where generated to be stored
  , m_storeHitPoints ( false )
  , m_maxoptsteps(100) 
{
  declareProperty ("StoreHitPoints", m_storeHitPoints);  
  declareProperty ("MaxOptPhotonSteps", m_maxoptsteps);
}


// ============================================================================
/// destructor
// ============================================================================
GaussStepAction::~GaussStepAction(){}


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
  GaussTrackInformation* ginf = (GaussTrackInformation*) uinf;
  
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
      if(track->GetCurrentStepNumber() > m_maxoptsteps)
        {
          // kill the photon which is making too many reflections
          track->SetTrackStatus(fStopAndKill);
        }
      else
        {
          ginf->setAppendStep(true); 
        }
      return;
    }
}


// ============================================================================
// The END 
// ============================================================================
