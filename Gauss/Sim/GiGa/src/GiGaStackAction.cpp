///
/// header file":
#include "GiGa/GiGaStackAction.h"

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
GiGaStackAction::GiGaStackAction( const std::string & nick       , 
				  ISvcLocator       * svc        ) 
  : G4UserStackingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };
//////////////////////////////////////////////////////////////////////////////////////////  
GiGaStackAction::~GiGaStackAction(){};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackAction::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackAction::finalize() { return GiGaBase::finalize();  };
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackAction::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if      ( IID_IGiGaStackAction == iid ) { *ppI = static_cast<IGiGaStackAction*>        ( this ) ; } 
  else                                    { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
/// serialize object for reading ///////////////////////
StreamBuffer& GiGaStackAction::serialize( StreamBuffer& S )       
{
  GiGaBase::serialize( S ) ; 
  return S;       
};  
/// serialize object for writing //////////////////////////
StreamBuffer& GiGaStackAction::serialize( StreamBuffer& S ) const 
{
  GiGaBase::serialize( S ) ; 
  return S ;
}; 
///////////////////////////////////////////////////////////













