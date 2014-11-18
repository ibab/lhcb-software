// $Id: GiGaEventActionDraw.cpp,v 1.2 2007-01-12 15:36:45 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaEventActionDraw )

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
{}


// ============================================================================
// destructor 
// ============================================================================
GiGaEventActionDraw::~GiGaEventActionDraw(){}


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
}


// ============================================================================
// The End 
// ============================================================================
