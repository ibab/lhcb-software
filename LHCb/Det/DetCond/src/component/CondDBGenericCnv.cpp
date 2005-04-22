// $Id: CondDBGenericCnv.cpp,v 1.4 2005-04-22 14:09:31 marcocle Exp $
// Include files 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/DataObject.h"

#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ValidDataObject.h"

#include "DetCond/ICondDBAccessSvc.h"

#include "CoolKernel/IValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

// local
#include "DetCond/CondDBGenericCnv.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBGenericCnv
//
// 2004-12-03 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBGenericCnv::CondDBGenericCnv(ISvcLocator* svc,const CLID& clid):
  Converter(CONDDB_StorageType,clid,svc)
{}
//=============================================================================
// Destructor
//=============================================================================
CondDBGenericCnv::~CondDBGenericCnv() {}; 

//=========================================================================
// Initialization
//=========================================================================
StatusCode CondDBGenericCnv::initialize() {
  // Initializes the grand father
  StatusCode sc = Converter::initialize();
  
  // Query the IDetDataSvc interface of the detector data service
  sc = serviceLocator()->service("DetectorDataSvc",m_detDataSvc);
  if( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "Can't locate DetectorDataSvc" << endreq;
    return sc;
  } else {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::DEBUG << "Succesfully located DetectorDataSvc" << endreq;
  }
  // Query the ICondDBAccessSvc interface of the detector data service
  sc = serviceLocator()->service("CondDBAccessSvc",m_dbAccSvc);
  if( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "Can't locate CondDBAccessSvc" << endreq;
    return sc;
  } else {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::DEBUG << "Succesfully located CondDBAccessSvc" << endreq;
  }
  return sc;
}

//=========================================================================
// Finalization
//=========================================================================
StatusCode CondDBGenericCnv::finalize() {
  m_detDataSvc->release();
  m_dbAccSvc->release();
  return Converter::finalize();
}

//=========================================================================
// Ask the event time to the DetectorDataSvc
//=========================================================================

StatusCode CondDBGenericCnv::eventTime(TimePoint &time) const {
  if (!m_detDataSvc->validEventTime()){
    return StatusCode::FAILURE;
  }
  time = m_detDataSvc->eventTime();
  return StatusCode::SUCCESS;
}

//=========================================================================
// Set the validity of the object
//=========================================================================
void CondDBGenericCnv::setObjValidity(TimePoint &since, TimePoint &till, DataObject *pObject){
  // Set validity of created object
  IValidity* pValidity = dynamic_cast<IValidity*>(pObject);

  if ( pValidity != NULL ) { // it has a validity
  
    pValidity->setValidity(since, till);
  
  } else {
  
    // I cannot set the validity range
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::WARNING
        << "Created object (CLID = " << pObject->clID()
        << ") does not implement IValidity: cannot set validity"
        << endreq;
  }
}

//=============================================================================
