///
/// header file":
#include "GiGa/GiGaTrackAction.h"

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
GiGaTrackAction::GiGaTrackAction( const std::string & nick       , 
				  ISvcLocator       * svc        ) 
  : G4UserTrackingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };
//////////////////////////////////////////////////////////////////////////////////////////  
GiGaTrackAction::~GiGaTrackAction(){};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackAction::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackAction::finalize() { return GiGaBase::finalize();  };
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackAction::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if      ( IID_IGiGaTrackAction == iid ) { *ppI = static_cast<IGiGaTrackAction*>        ( this ) ; } 
  else                                    { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
/// serialize object for reading ///////////////////////
StreamBuffer& GiGaTrackAction::serialize( StreamBuffer& S )       
{
  GiGaBase::serialize( S ) ; 
  return S;       
};  
/// serialize object for writing //////////////////////////
StreamBuffer& GiGaTrackAction::serialize( StreamBuffer& S ) const 
{
  GiGaBase::serialize( S ) ; 
  return S ;
}; 
///////////////////////////////////////////////////////////













