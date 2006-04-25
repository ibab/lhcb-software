// $Id: CondDBGenericCnv.cpp,v 1.11 2006-04-25 17:20:20 marcocle Exp $
// Include files 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/DataObject.h"

#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ValidDataObject.h"

#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/Exception.h"

// local
#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/ICondDBCnvSvc.h"
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
  // Get a pointer to the CondDBCnvSvc
  sc = conversionSvc()->queryInterface(ICondDBCnvSvc::interfaceID(),(void**)&m_condDBCnvSvc);
  if ( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "The conversion service does not implement ICondDBCnvSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  return sc;
}

//=========================================================================
// Finalization
//=========================================================================
StatusCode CondDBGenericCnv::finalize() {
  m_detDataSvc->release();
  m_condDBCnvSvc->release();
  return Converter::finalize();
}

//=========================================================================
// Ask the event time to the DetectorDataSvc
//=========================================================================

StatusCode CondDBGenericCnv::eventTime(Gaudi::Time &time) const {
  if (!m_detDataSvc->validEventTime()){
    return StatusCode::FAILURE;
  }
  time = m_detDataSvc->eventTime();
  return StatusCode::SUCCESS;
}

//=========================================================================
// Set the validity of the object
//=========================================================================
void CondDBGenericCnv::setObjValidity(Gaudi::Time &since, Gaudi::Time &till, DataObject *pObject){
  // Set validity of created object
  IValidity* pValidity = dynamic_cast<IValidity*>(pObject);

  if ( pValidity != NULL ) { // it has a validity
  
    pValidity->setValidity(since, till);
  
  } else {
  
    // I cannot set the validity range
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::DEBUG
        << "Created object (CLID = " << pObject->clID()
        << ") does not implement IValidity: cannot set validity"
        << endreq;
  }
}

//=========================================================================
//  get an object from the conditions database
//=========================================================================
StatusCode CondDBGenericCnv::getObject (const std::string &path, const cool::ChannelId &channel,
                                        boost::shared_ptr<coral::AttributeList> &data,
                                        std::string &descr, Gaudi::Time &since, Gaudi::Time &until) {

  MsgStream log(msgSvc(),"CondDBGenericCnv");

  Gaudi::Time now;
  StatusCode sc = eventTime(now);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Cannot create DataObject: event time undefined" << endmsg;
    return sc;
  }

  bool found_object = false;
  
  for ( std::vector<ICondDBAccessSvc*>::iterator accSvc = m_condDBCnvSvc->accessServices().begin();
        accSvc !=  m_condDBCnvSvc->accessServices().end() && ! found_object ; ++accSvc ) {
    found_object = (*accSvc)->getObject(path,now,data,descr,since,until,channel).isSuccess();
  }
  return (found_object) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBGenericCnv::getChildNodes(const std::string &path,std::vector<std::string> &node_names){
  bool found_folderset = false;
  
  MsgStream log(msgSvc(),"CondDBGenericCnv");
  log << MSG::DEBUG << "Entering \"getChildNodes\"" << endmsg;

  for ( std::vector<ICondDBAccessSvc*>::iterator accSvc = m_condDBCnvSvc->accessServices().begin();
        accSvc !=  m_condDBCnvSvc->accessServices().end() && ! found_folderset ; ++accSvc ) {
    found_folderset = (*accSvc)->getChildNodes(path,node_names).isSuccess();
  }
  return (found_folderset) ? StatusCode::SUCCESS : StatusCode::FAILURE; 
}
//=============================================================================

