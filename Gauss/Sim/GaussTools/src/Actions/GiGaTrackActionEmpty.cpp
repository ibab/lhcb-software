// $Id: GiGaTrackActionEmpty.cpp,v 1.1 2002-09-26 18:10:54 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.11  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
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
#include "GiGa/GiGaMACROs.h"
/// local
#include "GiGaTrackActionEmpty.h"

/** @file
 * 
 * Implementation of the class GiGaTrackActionEmpty
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaTrackActionEmpty );
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
GiGaTrackActionEmpty::GiGaTrackActionEmpty
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ) {};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaTrackActionEmpty::~GiGaTrackActionEmpty(){};
// ============================================================================

// ============================================================================
/// G4 
// ============================================================================
void 
GiGaTrackActionEmpty::PreUserTrackingAction  ( const G4Track* /* track */ ) 
{ Print("PreUserTrackingAction() is invoked"  , 
        StatusCode::SUCCESS                   , MSG::VERBOSE ); };
// ============================================================================

// ============================================================================
/// G4 
// ============================================================================
void 
GiGaTrackActionEmpty::PostUserTrackingAction ( const G4Track* /* track */ )
{ Print("PostUserTrackingAction() is invoked" , 
        StatusCode::SUCCESS                   , MSG::VERBOSE ); };
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


