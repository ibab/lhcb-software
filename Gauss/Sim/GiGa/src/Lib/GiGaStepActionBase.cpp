// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/Lib/GiGaStepActionBase.cpp,v 1.1 2001-04-23 08:52:03 ibelyaev Exp $ 

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


