// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGaEventActionDraw.cpp,v 1.3 2001-07-15 20:54:32 ibelyaev Exp $ 

// Geant4 
#include "G4VVisManager.hh"
#include "G4Event.hh"
// GiGa 
#include "GiGa/GiGaEventActionFactory.h"
// local
#include "GiGaEventActionDraw.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
static const GiGaEventActionFactory<GiGaEventActionDraw>          s_GiGaEventActionDrawFactory;
const       IGiGaEventActionFactory& GiGaEventActionDrawFactory = s_GiGaEventActionDrawFactory;
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaEventActionDraw::GiGaEventActionDraw( const std::string& Name , ISvcLocator* Loc ) 
  : GiGaEventActionBase( Name , Loc ) 
{};
/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaEventActionDraw::~GiGaEventActionDraw(){};
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaEventActionDraw::initialize () { return GiGaEventActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaEventActionDraw::finalize   () { return GiGaEventActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////
