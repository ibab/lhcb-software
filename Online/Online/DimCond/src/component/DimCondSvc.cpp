// $Id: DimCondSvc.cpp,v 1.1.1.1 2005-08-23 10:00:41 marcocle Exp $
// Include files

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "DetDesc/IUpdateManagerSvc.h"
#include "DetDesc/ICondDBAccessSvc.h"

// local
#include "DimCondSvc.h"

#include "DimCondCommand.h"

// Factory implementation
static SvcFactory<DimCondSvc> s_factory;
const ISvcFactory &DimCondSvcFactory = s_factory;

//-----------------------------------------------------------------------------
// Implementation file for class : DimCondSvc
//
// 2005-06-21 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DimCondSvc::DimCondSvc(const std::string &name, ISvcLocator *svcloc):
  Service(name,svcloc)
{}
//=============================================================================
// Destructor
//=============================================================================
DimCondSvc::~DimCondSvc() {};

//=========================================================================
//  
//=========================================================================
StatusCode DimCondSvc::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;
  
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Initialize" << endmsg;
  
  //#ifdef WIN32
  // std::string utgid(getenv("UTGID"));//Change to the correct sintax for windows
  //#else
  std::string utgid(getenv("UTGID"));
  //#endif 

  sc = service("UpdateManagerSvc",m_ums,true);
  if (!sc.isSuccess()){
    log << MSG::ERROR << "I couldn't find the UpdateManagerSvc" << endmsg;
    return sc;
  }
  
  sc = service("CondDBAccessSvc",m_db,true);
  if (!sc.isSuccess()){
    log << MSG::ERROR << "I couldn't find the CondDBAccessSvc" << endmsg;
    return sc;
  }

  log << MSG::INFO << "Setting up DimCond for UTGID " << utgid << endmsg;

  m_cmd = new DimCondCommand(utgid+"/CondDB",msgSvc(),m_ums,m_db);
  if ( !m_cmd ) {
    log << MSG::ERROR << "Unable to instantiate a DimCondCommand" << endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode DimCondSvc::finalize ( ) {
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Finalize" << endmsg;
  // local finalization

  if ( m_cmd ) delete m_cmd;

  m_db->release();
  m_ums->release();
  
  return Service::finalize();
}
//=============================================================================
