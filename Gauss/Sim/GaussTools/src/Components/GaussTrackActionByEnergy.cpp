// $Id: GaussTrackActionByEnergy.cpp,v 1.4 2007-01-12 15:36:42 ranjard Exp $ 
// Include files 

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
// Geant4
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
// GiGa
//#include "GiGa/GiGaMACROs.h"
// GaussTools 
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
// local
#include "GaussTrackActionByEnergy.h"
// ============================================================================

/** @file 
 *  Implementation file for class GaussTrackActionByEnergy
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
DECLARE_TOOL_FACTORY( GaussTrackActionByEnergy );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionByEnergy::GaussTrackActionByEnergy
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionZ   ( type , name , parent ) 
  //
  , m_storeByEnergy ( true    )  
  , m_threshold     ( 1 * GeV )  
  , m_rejectRICHphe ( true )
{
  declareProperty ( "StoreEnergy" , m_storeByEnergy ) ;
  declareProperty ( "Threshold"   , m_threshold     ) ;
  declareProperty ( "RejectRICHPhotoelectrons", m_rejectRICHphe );
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionByEnergy::~GaussTrackActionByEnergy() {}
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergy::PreUserTrackingAction  ( const G4Track* track ) 
{
  // no action 
  if ( !storeByEnergy()                                 ) { return ; } // RETURN
  
  if ( 0 == track || track->GetKineticEnergy() < threshold() ) { return ; }
  
  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
  if ( m_rejectRICHphe ) 
  {
    const G4VProcess* process  = track->GetCreatorProcess() ;
    if ( 0 == process ) 
    { Error ( "Pre..: G4VProcess         points to NULL!" ) ; } 
    else if ( ("RichHpdPhotoelectricProcess" == process->GetProcessName()) ||
              ( "RichPmtPhotoelectricProcess" == process->GetProcessName()) ||
              ( "TorchTBMcpPhotoElectricProcess" == process->GetProcessName())  ) 
    {
      Warning ( "Pre..: RichHpdPhotoelectricProcess or RichPmtPhotoelectricProcess particles not kept" );
      return;
    } 
  }
  
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  
  // check the validity 
  if( 0 == tr ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; return ; }
  
  // check the track information
  GaussTrackInformation*   info = trackInfo() ;
  if( 0 == info ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; return ; }

  if ( info->toBeStored() ) { return ; }
  
  mark( info ) ; 
  
};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergy::PostUserTrackingAction  
( const G4Track* /* track */ ) {} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
