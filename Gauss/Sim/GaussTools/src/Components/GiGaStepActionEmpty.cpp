// $Id: GiGaStepActionEmpty.cpp,v 1.1 2002-12-12 15:19:32 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:53  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.9  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// Revision 1.8  2002/04/25 13:02:05  ibelyaev
//  small update
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
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// local
#include "GiGaStepActionEmpty.h"

/** @file 
 *  
 *  Implementation of class GiGaStepActionEmpty
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaStepActionEmpty ) ;
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
GiGaStepActionEmpty::GiGaStepActionEmpty
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaStepActionBase ( type , name , parent ) {};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaStepActionEmpty::~GiGaStepActionEmpty(){};
// ============================================================================

// ============================================================================
/** stepping action
 *  @see G4UserSteppingAction
 *  @param step Geant4 step
 */
// ============================================================================
void GiGaStepActionEmpty::UserSteppingAction ( const G4Step* ) {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
