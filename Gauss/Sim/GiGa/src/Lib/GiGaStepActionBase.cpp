// ============================================================================
// ============================================================================
// ============================================================================
// GiGa
#include "GiGa/GiGaStepActionBase.h"

/** implementation of class GiGaStepActionBase
 *
 *  @author Vanya Belyaev 
 */

// ============================================================================
// ============================================================================
GiGaStepActionBase::GiGaStepActionBase( const std::string& nick ,
                                        ISvcLocator* svc ) 
  : G4UserSteppingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };

// ============================================================================
// ============================================================================
GiGaStepActionBase::~GiGaStepActionBase(){};

// ============================================================================
// ============================================================================
StatusCode GiGaStepActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  Print("GiGaStepActionBase initialized successfully" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return StatusCode::SUCCESS;
}; 

// ============================================================================
// ============================================================================
StatusCode GiGaStepActionBase::finalize() 
{ 
  ///
  Print("GiGaStepActionBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return GiGaBase::finalize();  
};

// ============================================================================
// ============================================================================
StatusCode GiGaStepActionBase::queryInterface( const InterfaceID& iid , 
                                               void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if  ( IGiGaStepAction::interfaceID() == iid ) 
    { *ppI = static_cast<IGiGaStepAction*>        ( this ) ; } 
  else { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};

// ============================================================================
// ============================================================================
void GiGaStepActionBase::UserSteppingAction ( const G4Step* /* step */ ) 
{
  MsgStream log( msgSvc() , name() );
  log << MSG::DEBUG << " 'UserSteppingAction' is invoked! " << endreq ;
};

// ============================================================================
// The End 
// ============================================================================

