// $Id: GiGaStepActionEmpty.cpp,v 1.8 2002-04-25 13:02:05 ibelyaev Exp $ 
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
/// GiGa 
#include "GiGa/GiGaStepActionFactory.h"
/// local
#include "GiGaStepActionEmpty.h"

/** @file 
 *  
 *  Implementation of class GiGaStepActionEmpty
 *
 *  @author Vanya Belyaev
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaStepAction( GiGaStepActionEmpty ) ;

// ============================================================================
// ============================================================================
GiGaStepActionEmpty::GiGaStepActionEmpty( const std::string& Name , 
                                          ISvcLocator* Loc ) 
  : GiGaStepActionBase            ( Name , Loc ) 
{};

// ============================================================================
// ============================================================================
GiGaStepActionEmpty::~GiGaStepActionEmpty(){};

// ============================================================================
// ============================================================================
StatusCode GiGaStepActionEmpty::initialize () 
{ 
  /// initialize the base class 
  StatusCode sc = GiGaStepActionBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc); }
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
// ============================================================================
StatusCode GiGaStepActionEmpty::finalize   () 
{ 
  Print("finalization");
  return GiGaStepActionBase::finalize() ; 
};

// ============================================================================
// ============================================================================
void GiGaStepActionEmpty::UserSteppingAction ( const G4Step* ) 
{ Print(" UserSteppingAction!"); }

// ============================================================================
// The END 
// ============================================================================
