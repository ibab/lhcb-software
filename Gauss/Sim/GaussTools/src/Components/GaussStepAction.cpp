// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
///
#include "G4Step.hh"
//#include "G4TouchableHistory.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4LogicalVolume.hh"
#include "G4OpticalPhoton.hh"
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
{
  declareProperty ("StorePointsOfSecondaries", m_storePointsOfSecondaries);
  declareProperty ("StoreNotDeltaE", m_storeNotDeltaE);
  declareProperty ("StoreHitPoints", m_storeHitPoints);  
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
  G4VUserTrackInformation* uinf = step->GetTrack()->GetUserInformation(); 
  GaussTrackInformation*    ginf = 
    ( 0 == uinf )  ? 0 : dynamic_cast<GaussTrackInformation*> ( uinf );

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
            step->GetTrack()->GetDefinition        () == 
            G4OpticalPhoton::OpticalPhoton         ()    )
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
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
