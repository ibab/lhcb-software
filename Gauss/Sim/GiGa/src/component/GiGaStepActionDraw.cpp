// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGaStepActionDraw.cpp,v 1.2 2001-04-26 21:01:17 ibelyaev Exp $ 
#include "G4Step.hh"
//
#include "G4Wrapper/VisManager.h"
//
#include "G4VVisManager.hh"
#include "G4Polyline.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
///
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaStepActionFactory.h"
// local
#include "GiGaStepActionDraw.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
static const GiGaStepActionFactory<GiGaStepActionDraw>          s_GiGaStepActionDrawFactory;
const       IGiGaStepActionFactory& GiGaStepActionDrawFactory = s_GiGaStepActionDrawFactory;
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStepActionDraw::GiGaStepActionDraw( const std::string& Name , ISvcLocator* Loc ) 
  : GiGaStepActionBase            ( Name , Loc ) 
{};
/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStepActionDraw::~GiGaStepActionDraw(){};
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepActionDraw::initialize () { return GiGaStepActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepActionDraw::finalize   () { return GiGaStepActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaStepActionDraw::UserSteppingAction( const G4Step* step )
{
  /// locate visual manager 
  G4VVisManager* visMgr  = G4Wrapper::getG4VVisManager();
  ///
  if      ( 0 == visMgr    ) { Error("Could not locate G4VVisManager* !" ) ; }
  else if ( 0 == step      ) { Error("G4Step* points to NULL!"           ) ; } 
  else 
    {
      /// Define a line segment 
      G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
      ///
      G4Colour colour = 
	( charge < 0.0 ) ? G4Colour( 1. , 0. , 0. ) : 
	( charge > 0.0 ) ? G4Colour( 0. , 0. , 1. ) : G4Colour( 0. , 1. , 0. ) ;
      ///
      G4Polyline polyline;
      G4VisAttributes attribs( colour );
      polyline.SetVisAttributes( attribs );
      polyline.append( step->GetPreStepPoint  ()->GetPosition () );
      polyline.append( step->GetPostStepPoint ()->GetPosition () );
      /// 
      visMgr -> Draw( polyline ); 
      ///
    }  
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////





