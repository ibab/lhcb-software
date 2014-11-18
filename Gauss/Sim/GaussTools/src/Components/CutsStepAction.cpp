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
#include "GaudiKernel/DeclareFactoryEntries.h" 

//GaussTools
#include "GaussTools/GaussTrackInformation.h"
/// local
#include "CutsStepAction.h"

/** @file 
 *  
 *  Implementation of class CutsStepAction
 *
 *  @author Witek Pokorski
 */

// ============================================================================
/// Factory
// ============================================================================
DECLARE_TOOL_FACTORY( CutsStepAction )


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
CutsStepAction::CutsStepAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaStepActionBase ( type , name , parent )
  // tracking cuts
  , m_trcuteg (1.0*MeV), m_trcuthadr (10.0*MeV)
{
  declareProperty ("TrCutElGamma", m_trcuteg);
  declareProperty ("TrCutHadr", m_trcuthadr);
}


// ============================================================================
/// destructor
// ============================================================================
CutsStepAction::~CutsStepAction(){}


// ============================================================================
/** stepping action
 *  @see G4UserSteppingAction
 *  @param step Geant4 step
 */
// ============================================================================
void CutsStepAction::UserSteppingAction ( const G4Step* step ) 
{
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* partdef=track->GetDefinition();

  if ( partdef == G4Electron::ElectronDefinition() ||
       partdef == G4Positron::PositronDefinition() ||
       partdef == G4Gamma::GammaDefinition() )
    { 
      if (track->GetKineticEnergy() <  m_trcuteg)
        track->SetTrackStatus(fStopAndKill);
   }
  else if(track->GetKineticEnergy() < m_trcuthadr)
    track->SetTrackStatus(fStopAndKill);


}


// ============================================================================
// The END 
// ============================================================================
