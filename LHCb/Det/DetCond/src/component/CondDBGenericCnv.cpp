// $Id: CondDBGenericCnv.cpp,v 1.3 2005-02-09 08:49:29 marcocle Exp $
// Include files 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/TimePoint.h"
#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/IConditionsDBCnvSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/DataObject.h"

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
{

}
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

  sc = serviceLocator()->service 
    ( "ConditionsDBCnvSvc", m_condDBCnvSvc );
  if( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "Can't locate ConditionsDBCnvSvc" << endreq;
    return sc;
  } else {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::DEBUG << "Succesfully located ConditionDBCnvSvc" << endreq;
  }
  // Query the IDetDataSvc interface of the detector data service
  sc = serviceLocator()->service 
    ("DetectorDataSvc",m_detDataSvc);
  if( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "Can't locate DetectorDataSvc" << endreq;
    return sc;
  } else {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::DEBUG << "Succesfully located DetectorDataSvc" << endreq;
  }
  // Query the ICondDBAccessSvc interface of the detector data service
  sc = serviceLocator()->service 
    ("CondDBAccessSvc",m_dbAccSvc);
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
  m_condDBCnvSvc->release();
  m_detDataSvc->release();
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
// Ask the global TAG to the ConditionsDBCnvSvc
//=========================================================================

//const std::string &CondDBGenericCnv::globalTag() const {
const std::string &CondDBGenericCnv::globalTag() {
 return m_condDBCnvSvc->globalTag();
}

//=========================================================================
// Set the validity of the object
//=========================================================================
void CondDBGenericCnv::setObjValidity(TimePoint &since,
                                      TimePoint &till,
                                      DataObject *pObject){
  // Set validity of created object
  IValidity* pValidity = dynamic_cast<IValidity*>(pObject);
  if ( 0 == pValidity ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::WARNING
        << "Created object (CLID = " << pObject->clID() << ") does not implement IValidity: cannot set validity"
        << endreq;
  } else {
    pValidity->setValidity ( since, till );
  }
}
//=========================================================================
// Check the validity of an object
//=========================================================================
CondDBGenericCnv::Object_Updatability_t CondDBGenericCnv::checkUpdatability(DataObject* pObject){
  MsgStream log(msgSvc(),"CondDBGenericCnv");
  log << MSG::DEBUG << "Method checkUpdatability starting" << endreq;

  if( pObject == 0) {
    log << MSG::ERROR << "No object to update!" << endmsg;
    return FAILURE;
  }
  StatusCode sc;
    
  TimePoint now;
  sc = eventTime(now);
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot update DataObject: event time undefined" << endmsg;
    return FAILURE;
  }

  IValidity *pValObj = dynamic_cast<IValidity*>(pObject);
  if (pValObj == 0){
    log << MSG::WARNING << "Object does not implement IValidity, cannot update" << endmsg;
    return UPTODATE;
  }
  
  if (pValObj->isValid(now)) {
    log << MSG::DEBUG << "Object is still valid, no update needed" << endmsg;
    return UPTODATE;
  }

  // Now I'm sure that the object can and has to be updated
  return NONVALID;
}


//=============================================================================
