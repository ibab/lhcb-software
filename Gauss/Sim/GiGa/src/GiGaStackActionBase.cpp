/// GiGa 
#include "GiGa/GiGaStackActionBase.h"


//////////////////////////////////////////////////////////////////////////////////////////
GiGaStackActionBase::GiGaStackActionBase( const std::string& nick , ISvcLocator* svc )
  : G4UserStackingAction () 
  , GiGaBase             ( nick , svc ) 
{};
/// virtual destructor ///////////////////////////////////////////////////////////////////
GiGaStackActionBase::~GiGaStackActionBase(){};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
/////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionBase::finalize() { return GiGaBase::finalize();  };
/////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionBase::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaStackAction::interfaceID() == iid ) { *ppI = static_cast<IGiGaStackAction*>        ( this ) ; } 
  else                                            { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
//////////////////////////////////////////////////////////////////////////////////////////














