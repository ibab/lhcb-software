// $Id: OnlineDBValidatorSvc.cpp,v 1.1 2007-05-03 17:28:28 marcocle Exp $
// Include files 

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/Time.h"

#include "DetDesc/Condition.h"

// local
#include "OnlineDBValidatorSvc.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(OnlineDBValidatorSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : OnlineDBValidatorSvc
//
// 2007-05-03 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OnlineDBValidatorSvc::OnlineDBValidatorSvc( const std::string& name,
                                            ISvcLocator* svcloc ):
  Service(name,svcloc)
{
  declareProperty("MarkerPath",    m_markerPath   = "Conditions/Online/Marker" );
}
//=============================================================================
// Destructor
//=============================================================================
OnlineDBValidatorSvc::~OnlineDBValidatorSvc() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OnlineDBValidatorSvc::initialize()
{
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;
  
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Initialize" << endmsg;


  // register to the UMS
  IUpdateManagerSvc *ums;
  sc = service("UpdateManagerSvc", ums, true);
  if ( sc.isSuccess() ) {
    log << MSG::DEBUG << "Registering to UpdateManagerSvc" << endmsg;
    ums->registerCondition(this, m_markerPath,
                           &OnlineDBValidatorSvc::i_CheckMarker, m_markerCondition);
    ums->release(); // I do not need the UMS anymore
  } else {
    log << MSG::ERROR << "Unable find UpdateManagerSvc" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Callback for UMS
//=============================================================================
StatusCode OnlineDBValidatorSvc::i_CheckMarker()
{
  if (m_markerCondition->validTill() == Gaudi::Time::max()){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "The marker condition '"
        << m_markerPath << "' is BAD" << endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================

