// $Id: GiGaEventActionDraw.cpp,v 1.1 2002-09-26 18:10:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "GiGaEventActionDraw.h"
// G4 
#include "G4VVisManager.hh"
#include "G4Event.hh"

/** @file 
 *
 *  implementation of class GiGaEventActionDraw 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
// factories
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaEventActionDraw );
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
GiGaEventActionDraw::GiGaEventActionDraw 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaEventActionBase( type , name , parent  ) 
{};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaEventActionDraw::~GiGaEventActionDraw(){};
// ============================================================================

// ============================================================================
// G4 
// ============================================================================
void GiGaEventActionDraw::EndOfEventAction( const G4Event* event ) 
{
  // locate visual manager 
  G4VVisManager* visMgr  = G4VVisManager::GetConcreteInstance();
  //
  if      ( 0 == visMgr    ) { Error("Could not locate G4VVisManager* !" ) ; }
  else if ( 0 == event     ) { Error("G4Event* points to NULL!"          ) ; } 
  else    { event->Draw(); }
  ///
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
