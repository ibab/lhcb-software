// $Id: GiGaTrackActionEmpty.cpp,v 1.10 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
///
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
///
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaTrackActionFactory.h"
/// local
#include "GiGaTrackActionEmpty.h"

/** @file
 * 
 * Implementation of the class GiGaTrackActionEmpty
 *  
 *  @author Vanya Belyaev
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaTrackAction( GiGaTrackActionEmpty );

// ============================================================================
// ============================================================================
GiGaTrackActionEmpty::GiGaTrackActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaTrackActionBase( Name , Loc ) 
{};

// ============================================================================
// ============================================================================
GiGaTrackActionEmpty::~GiGaTrackActionEmpty(){};

// ============================================================================
// ============================================================================
StatusCode GiGaTrackActionEmpty::initialize () 
{ 
  /// initialize the base class 
  StatusCode sc = GiGaTrackActionBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc);}
  ///  
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
// ============================================================================
StatusCode GiGaTrackActionEmpty::finalize   () 
{ 
  ///  
  Print("initialized succesfully");
  ///
  return GiGaTrackActionBase::initialize() ; 
} ;

// ============================================================================
// ============================================================================
void 
GiGaTrackActionEmpty::PreUserTrackingAction  ( const G4Track* /* track */ ) 
{ Print("PreUserTrackingAction() is invoked"  , 
        StatusCode::SUCCESS , MSG::DEBUG ); };

// ============================================================================
// ============================================================================
void 
GiGaTrackActionEmpty::PostUserTrackingAction ( const G4Track* /* track */ )
{ Print("PostUserTrackingAction() is invoked" , 
        StatusCode::SUCCESS , MSG::DEBUG ); };

// ============================================================================


