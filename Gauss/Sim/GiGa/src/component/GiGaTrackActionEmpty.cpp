// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGaTrackActionEmpty.cpp,v 1.4 2001-07-15 20:54:36 ibelyaev Exp $ 

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


/////////////////////////////////////////////////////////////////////////////////////////////////
static const GiGaTrackActionFactory<GiGaTrackActionEmpty>          s_GiGaTrackActionEmptyFactory;
const       IGiGaTrackActionFactory& GiGaTrackActionEmptyFactory = s_GiGaTrackActionEmptyFactory;
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionEmpty::GiGaTrackActionEmpty( const std::string& Name , ISvcLocator* Loc ) 
  : GiGaTrackActionBase( Name , Loc ) 
{};
/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionEmpty::~GiGaTrackActionEmpty(){};
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionEmpty::initialize () { return GiGaTrackActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionEmpty::finalize   () { return GiGaTrackActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionEmpty::PreUserTrackingAction  ( const G4Track* /* track */ ) 
{ Print("PreUserTrackingAction() is invoked"  , StatusCode::SUCCESS , MSG::DEBUG ); };
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionEmpty::PostUserTrackingAction ( const G4Track* /* track */ )
{ Print("PostUserTrackingAction() is invoked" , StatusCode::SUCCESS , MSG::DEBUG ); };
/////////////////////////////////////////////////////////////////////////////////////////////////


