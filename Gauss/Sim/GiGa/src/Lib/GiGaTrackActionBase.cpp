// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/Lib/GiGaTrackActionBase.cpp,v 1.1 2001-04-23 08:52:03 ibelyaev Exp $ 


// GiGa
#include "GiGa/GiGaTrackActionBase.h"


//////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionBase::GiGaTrackActionBase( const std::string& nick , ISvcLocator* svc )
  : G4UserTrackingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };
//////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionBase::~GiGaTrackActionBase(){};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
///////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionBase::finalize() { return GiGaBase::finalize();  };
///////////////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaTrackActionBase::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaTrackAction::interfaceID()  == iid ) { *ppI = static_cast<IGiGaTrackAction*>        ( this ) ; } 
  else                                             { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionBase::PreUserTrackingAction  ( const G4Track* /* track */ ) {};
///////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionBase::PostUserTrackingAction ( const G4Track* /* track */ ) {};
///////////////////////////////////////////////////////////////////////////////////////////











