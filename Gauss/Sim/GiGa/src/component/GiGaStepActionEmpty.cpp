/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:12:28  ibelyaev
/// the package restructurisation(II)
///
/// ===========================================================================
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

/** Implementation of class GiGaStepActionEmpty
 *
 *  @author Vanya Belyaev
 */


/// ===========================================================================
/// ===========================================================================
static const GiGaStepActionFactory<GiGaStepActionEmpty>         s_Factory;
const       IGiGaStepActionFactory&GiGaStepActionEmptyFactory = s_Factory;

/// ===========================================================================
/// ===========================================================================
GiGaStepActionEmpty::GiGaStepActionEmpty( const std::string& Name , 
                                          ISvcLocator* Loc ) 
  : GiGaStepActionBase            ( Name , Loc ) 
{};

/// ===========================================================================
/// ===========================================================================
GiGaStepActionEmpty::~GiGaStepActionEmpty(){};

/// ===========================================================================
/// ===========================================================================
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

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStepActionEmpty::finalize   () 
{ 
  ///
  Print("finalization");
  ///
  return GiGaStepActionBase::initialize() ; 
} ;

/// ===========================================================================
