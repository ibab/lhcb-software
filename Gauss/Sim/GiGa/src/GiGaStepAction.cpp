///
/// header file":
#include "GiGa/GiGaStepAction.h"

/// 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDetectorElement.h"

///
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/System.h"
///
///
#include "GiGa/GiGaException.h"


///
GiGaStepAction::GiGaStepAction( const std::string & nick       , 
				  ISvcLocator       * svc        ) 
  : G4UserSteppingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };
//////////////////////////////////////////////////////////////////////////////////////////  
GiGaStepAction::~GiGaStepAction(){};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepAction::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepAction::finalize() { return GiGaBase::finalize();  };
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaStepAction::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if      ( IID_IGiGaStepAction == iid ) { *ppI = static_cast<IGiGaStepAction*>        ( this ) ; } 
  else                                    { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
/// serialize object for reading ///////////////////////
StreamBuffer& GiGaStepAction::serialize( StreamBuffer& S )       
{
  GiGaBase::serialize( S ) ; 
  return S;       
};  
/// serialize object for writing //////////////////////////
StreamBuffer& GiGaStepAction::serialize( StreamBuffer& S ) const 
{
  GiGaBase::serialize( S ) ; 
  return S ;
}; 
///////////////////////////////////////////////////////////













