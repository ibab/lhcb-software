/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:12:29  ibelyaev
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
// GiGa 
#include "GiGa/GiGaTrackActionFactory.h"
/// local
#include "GiGaTrackActionEmpty.h"

/** Implementationof class GiGaTrackActionEmpty
 *  
 *  @author Vanya Belyaev
 */

/// ===========================================================================
/// ===========================================================================
static const GiGaTrackActionFactory<GiGaTrackActionEmpty>         s_Factory;
const       IGiGaTrackActionFactory&GiGaTrackActionEmptyFactory = s_Factory;

/// ===========================================================================
/// ===========================================================================
GiGaTrackActionEmpty::GiGaTrackActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaTrackActionBase( Name , Loc ) 
{};

/// ===========================================================================
/// ===========================================================================
GiGaTrackActionEmpty::~GiGaTrackActionEmpty(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaTrackActionEmpty::initialize () 
{ return GiGaTrackActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaTrackActionEmpty::finalize   () 
{ return GiGaTrackActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
void 
GiGaTrackActionEmpty::PreUserTrackingAction  ( const G4Track* /* track */ ) 
{ Print("PreUserTrackingAction() is invoked"  , 
        StatusCode::SUCCESS , MSG::DEBUG ); };

/// ===========================================================================
/// ===========================================================================
void 
GiGaTrackActionEmpty::PostUserTrackingAction ( const G4Track* /* track */ )
{ Print("PostUserTrackingAction() is invoked" , 
        StatusCode::SUCCESS , MSG::DEBUG ); };

/// ===========================================================================


