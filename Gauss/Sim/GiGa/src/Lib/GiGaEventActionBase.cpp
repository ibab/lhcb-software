// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/Lib/GiGaEventActionBase.cpp,v 1.1 2001-04-23 08:52:03 ibelyaev Exp $ 

// GiGa 
#include "GiGa/GiGaEventActionBase.h"


//////////////////////////////////////////////////////////////////////////////////////////
GiGaEventActionBase::GiGaEventActionBase( const std::string& nick , ISvcLocator* svc )
  : GiGaBase          ( nick , svc ) 
{};
/// virtual destructor ///////////////////////////////////////////////////////////////////
GiGaEventActionBase::~GiGaEventActionBase(){};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaEventActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
/////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaEventActionBase::finalize() { return GiGaBase::finalize();  };
/////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaEventActionBase::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaEventAction::interfaceID() == iid ) { *ppI = static_cast<IGiGaEventAction*>        ( this ) ; } 
  else                                            { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
//////////////////////////////////////////////////////////////////////////////////////////
void GiGaEventActionBase::BeginOfEventAction ( const G4Event* /* event */ ) {};
//////////////////////////////////////////////////////////////////////////////////////////
void GiGaEventActionBase::EndOfEventAction   ( const G4Event* /* event */ ) {};
//////////////////////////////////////////////////////////////////////////////////////////













