// $Id: GiGaEventActionEmpty.cpp,v 1.1 2002-09-26 18:10:51 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// Revision 1.7  2002/04/25 13:02:04  ibelyaev
//  small update
//
// ============================================================================
// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "GiGaEventActionEmpty.h"

/** @file 
 *
 * implementation of class GiGaEventActionEmpty
 * 
 *  @author Vanya Belyaev 
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaEventActionEmpty );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaEventActionBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaEventActionEmpty::GiGaEventActionEmpty
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaEventActionBase( type , name , parent ) 
{};
// ============================================================================

// ============================================================================
/// Desctructor 
// ============================================================================
GiGaEventActionEmpty::~GiGaEventActionEmpty(){};
// ============================================================================

// ============================================================================
/// G4 
// ============================================================================
void GiGaEventActionEmpty::BeginOfEventAction ( const G4Event* /* event */ ) 
{ Print("'BeginOfEventAction' method is invoked"); };
// ============================================================================

// ============================================================================
// G4
// ============================================================================
void GiGaEventActionEmpty::EndOfEventAction   ( const G4Event* /* event */ ) 
{ Print("'EndOfEventAction' method is invoked"); };
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
