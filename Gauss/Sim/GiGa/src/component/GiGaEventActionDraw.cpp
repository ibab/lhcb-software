/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
// Geant4 
#include "G4VVisManager.hh"
#include "G4Event.hh"
// GiGa 
#include "GiGa/GiGaEventActionFactory.h"
// local
#include "GiGaEventActionDraw.h"

/** implementationof class GiGaEventActionDraw 
 * 
 *  @author Vanya Belyaev
 */

/// ===========================================================================
/// ===========================================================================
static const GiGaEventActionFactory<GiGaEventActionDraw>         s_Factory;
const       IGiGaEventActionFactory&GiGaEventActionDrawFactory = s_Factory;

/// ===========================================================================
/// ===========================================================================
GiGaEventActionDraw::GiGaEventActionDraw( const std::string& Name ,
                                          ISvcLocator* Loc ) 
  : GiGaEventActionBase( Name , Loc ) 
{};

/// ===========================================================================
/// ===========================================================================
GiGaEventActionDraw::~GiGaEventActionDraw(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionDraw::initialize () 
{ return GiGaEventActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionDraw::finalize   () 
{ return GiGaEventActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionDraw::EndOfEventAction( const G4Event* event ) 
{
  /// locate visual manager 
  G4VVisManager* visMgr  = G4VVisManager::GetConcreteInstance();
  ///
  if      ( 0 == visMgr    ) { Error("Could not locate G4VVisManager* !" ) ; }
  else if ( 0 == event     ) { Error("G4Event* points to NULL!"          ) ; } 
  else 
    {
      event->Draw();
    }
  ///
};

/// ===========================================================================
