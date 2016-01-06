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
CondDBGenericCnv::~CondDBGenericCnv() = default;

//=========================================================================
// Initialization
//=========================================================================
StatusCode CondDBGenericCnv::initialize() {
  // Initializes the grand father
  StatusCode sc = Converter::initialize();

  // Query the IDetDataSvc interface of the detector data service
  m_detDataSvc = serviceLocator()->service("DetectorDataSvc");
  if( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "Can't locate DetectorDataSvc" << endmsg;
    return sc;
  } else {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Succesfully located DetectorDataSvc" << endmsg;
  }
  // Get a pointer to the CondDBReader (implemented by the conversion service)
  m_condDBReader.reset( conversionSvc().get() );
  if ( !m_condDBReader ) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "The conversion service does not implement ICondDBReader!" << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

//=========================================================================
// Finalization
//=========================================================================
StatusCode CondDBGenericCnv::finalize() {
  m_detDataSvc.reset();
  m_condDBReader.reset();
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
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG
          << "Created object (CLID = " << pObject->clID()
          << ") does not implement IValidity: cannot set validity"
          << endmsg;
  }
}

//=========================================================================
//  get an object from the conditions database
//=========================================================================
StatusCode CondDBGenericCnv::getObject (const std::string &path, const cool::ChannelId &channel,
                                        ICondDBReader::DataPtr &data,
                                        std::string &descr, Gaudi::Time &since, Gaudi::Time &until) {

  Gaudi::Time now;
  StatusCode sc = eventTime(now);
  if (sc.isFailure()) {
    MsgStream log(msgSvc(),"CondDBGenericCnv");
    log << MSG::ERROR << "Cannot create DataObject: event time undefined" << endmsg;
    return sc;
  }

  return m_condDBReader->getObject(path,now,data,descr,since,until,channel);
}

//=========================================================================
//  get get the list of nodes in a folderset from the conditions database
//=========================================================================
StatusCode CondDBGenericCnv::getChildNodes(const std::string &path,std::vector<std::string> &node_names){
  MsgStream log(msgSvc(),"CondDBGenericCnv");
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Entering \"getChildNodes\"" << endmsg;

  return m_condDBReader->getChildNodes(path,node_names);
}
//=============================================================================

