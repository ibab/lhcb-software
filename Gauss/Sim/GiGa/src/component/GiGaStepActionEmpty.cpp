/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
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
{ return GiGaStepActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStepActionEmpty::finalize   () 
{ return GiGaStepActionBase::initialize() ; } ;

/// ===========================================================================
