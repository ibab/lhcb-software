// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/GiGaStepActionBase.cpp,v 1.2 2001-03-18 14:56:15 ibelyaev Exp $ 

// GiGa
#include "GiGa/GiGaStepActionBase.h"
//

////////////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStepActionBase::GiGaStepActionBase( const std::string& nick , ISvcLocator* svc ) 
  : G4UserSteppingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };
////////////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStepActionBase::~GiGaStepActionBase(){};
////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepActionBase::finalize() { return GiGaBase::finalize();  };
/////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepActionBase::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if  ( IGiGaStepAction::interfaceID() == iid ) { *ppI = static_cast<IGiGaStepAction*>        ( this ) ; } 
  else                                          { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaStepActionBase::UserSteppingAction ( const G4Step* /* step */ ) {};
/////////////////////////////////////////////////////////////////////////////////////////////////////////


