// $Id: GiGaEventActionDraw.cpp,v 1.8 2002-04-25 13:02:04 ibelyaev Exp $
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.7  2002/04/09 17:16:50  ibelyaev
///  printout polishing
///
/// Revision 1.6  2001/08/12 15:42:51  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.5  2001/07/27 17:03:19  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:26  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
// Geant4 
#include "G4VVisManager.hh"
#include "G4Event.hh"
// GiGa 
#include "GiGa/GiGaEventActionFactory.h"
// local
#include "GiGaEventActionDraw.h"

/** @file 
 *
 *  implementation of class GiGaEventActionDraw 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
// factories
// ============================================================================
IMPLEMENT_GiGaEventAction( GiGaEventActionDraw );

// ============================================================================
// ============================================================================
GiGaEventActionDraw::GiGaEventActionDraw
( const std::string& Name ,
  ISvcLocator* Loc ) 
  : GiGaEventActionBase( Name , Loc ) 
{};

// ============================================================================
// ============================================================================
GiGaEventActionDraw::~GiGaEventActionDraw(){};

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionDraw::initialize () 
{ 
  StatusCode sc = GiGaEventActionBase::initialize() ; 
  if( sc.isFailure() )
    { return Error("Could not initialize the base class ",sc); }
  ///
  Print("initialized successfully");
  return StatusCode::SUCCESS;
};

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionDraw::finalize   () 
{ 
  Print("finalization");
  return GiGaEventActionBase::finalize () ; 
} ;

// ============================================================================
// ============================================================================
void GiGaEventActionDraw::EndOfEventAction( const G4Event* event ) 
{
  // locate visual manager 
  G4VVisManager* visMgr  = G4VVisManager::GetConcreteInstance();
  ///
  if      ( 0 == visMgr    ) { Error("Could not locate G4VVisManager* !" ) ; }
  else if ( 0 == event     ) { Error("G4Event* points to NULL!"          ) ; } 
  else    { event->Draw(); }
  ///
};

// ============================================================================
// The End 
// ============================================================================
