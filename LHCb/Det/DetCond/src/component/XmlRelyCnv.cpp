// $Id: XmlRelyCnv.cpp,v 1.1 2004-12-08 17:19:17 marcocle Exp $
// Include files 
#include "DetCond/XmlRelyCnv.h"
#include "DetDesc/ValidDataObject.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "DetCond/IConditionsDBCnvSvc.h"
#include "GaudiKernel/TimePoint.h"

// local

//-----------------------------------------------------------------------------
// Implementation file for class : XmlRelyCnv
//
// 2004-12-03 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlRelyCnv> s_factoryXmlRely;
const ICnvFactory& XmlRelyCnvFactory = s_factoryXmlRely;

const CLID XmlRelyCnv::s_CLID_any = 0;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlRelyCnv::XmlRelyCnv(ISvcLocator* svc):
  CondDBGenericCnv(svc,0)
{

}
//=============================================================================
// Destructor
//=============================================================================
XmlRelyCnv::~XmlRelyCnv() {}; 

//=========================================================================
// Initialization
//=========================================================================
StatusCode XmlRelyCnv::initialize() {
  // Initializes the grand father
  StatusCode sc = CondDBGenericCnv::initialize();

  sc = serviceLocator()->service 
    ("DetectorPersistencySvc", m_detPersSvc, true);
  if ( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"XmlRelyCnv");
    log << MSG::ERROR 
        << "Cannot locate IConversionSvc interface of DetectorPersistencySvc"
        << endreq;
    return sc;
  } else {
    MsgStream log(msgSvc(),"XmlRelyCnv");
    log << MSG::DEBUG 
        << "Retrieved IConversionSvc interface of DetectorPersistencySvc"
        << endreq;
  }
  return sc;
}

//=========================================================================
// Finalization
//=========================================================================
StatusCode XmlRelyCnv::finalize() {
  m_detPersSvc->release();
  return CondDBGenericCnv::finalize();
}

//=========================================================================
// Create the transient representation
//=========================================================================
StatusCode XmlRelyCnv::createObj (IOpaqueAddress* pAddress,
                                  DataObject *&pObject)
{
  MsgStream log(msgSvc(),"XmlRelyCnv");
  log << MSG::DEBUG << "entering createObj" << endmsg;

  TimePoint since;
  TimePoint till;
  StatusCode sc = i_delegatedCreation(pAddress,pObject,since,till);
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot create the object" << endmsg;
    return sc;
  }

  log << MSG::DEBUG << "Setting object validity" << endreq;
  setObjValidity(since,till,pObject);

  return StatusCode::SUCCESS;
}

//=========================================================================
// Update transient representation from persistent one
//=========================================================================
StatusCode XmlRelyCnv::updateObj (IOpaqueAddress* pAddress,
                                  DataObject* pObject)
{
  MsgStream log(msgSvc(),"XmlRelyCnv");
  log << MSG::DEBUG << "Method updateObj starting" << endreq;
  switch(checkUpdatability(pObject)){
  case FAILURE : return StatusCode::FAILURE;
  case UPTODATE: return StatusCode::SUCCESS;
  case NONVALID: // update the object
    { // start a new scope to correctly handle new object destruction 
      
      DataObject* pNewObject; // create a new object and copy it to the old version
      TimePoint since;
      TimePoint till;
      StatusCode sc = i_delegatedCreation(pAddress,pNewObject,since,till);
      if (sc.isFailure()){
        log << MSG::ERROR << "Cannot create the new object" << endmsg;
        return sc;
      }

      // do the real update
      //
      //   Since DataObject::operator= operator is not virtual, dynamic cast first!
      //   Overloaded virtual method Condition::update() must be properly defined!
      //   The memory pointed to by the old pointer must contain the new object
      //                                            Andrea Valassi

      ValidDataObject* pVDO = dynamic_cast<ValidDataObject*>(pObject);
      ValidDataObject* pNewVDO = dynamic_cast<ValidDataObject*>(pNewObject);
      if ( 0 == pVDO || 0 == pNewVDO ) {
        log << MSG::ERROR
            << "Cannot update objects other than ValidDataObject: " 
            << "update() must be defined!"
            << endreq;
        return StatusCode::FAILURE;
      }
      // Deep copy the new Condition into the old DataObject
      log << MSG::INFO << "Be careful, because ValidDataObject::update() ins not really implemented" << endmsg;
      pVDO->update( *pNewVDO );  
      // Delete the useless Condition
      delete pNewVDO;

      setObjValidity(since,till,pObject);
    }
  }
  
  log << MSG::DEBUG << "Object successfully updated" << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Create the persistent representation
//=========================================================================
StatusCode XmlRelyCnv::createRep (DataObject* pObject,
                                  IOpaqueAddress*& pAddress)
{
  MsgStream log(msgSvc(),"XmlRelyCnv");
  log << MSG::WARNING << "createRep() not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Update the persistent representation
//=========================================================================
StatusCode XmlRelyCnv::updateRep (IOpaqueAddress* pAddress,
                                  DataObject* pObject)
{
  MsgStream log(msgSvc(),"XmlRelyCnv");
  log << MSG::WARNING << "updateRep() not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Create an object by delegation
//=========================================================================
StatusCode XmlRelyCnv::i_delegatedCreation(IOpaqueAddress* pAddress,
                                           DataObject *&pObject,TimePoint &since,TimePoint &till){
  StatusCode sc;

  // first try to get the object (it fails if the event time is not defined)
  std::string stringData;
  sc = getCondDBObject(since,till,stringData,pAddress->par()[0]);
  if (sc.isFailure()){
    return sc;
  }
  
  MsgStream log(msgSvc(),"XmlRelyCnv");

  std::string description;
  sc = getCondDBFolder(description,pAddress->par()[0]);
  if (sc.isFailure()){
    log << MSG::ERROR << "Could not get folder description for '" <<
      pAddress->par()[0] << "'" << endmsg;
    return sc;
  }

  long storage_type;
  sc  = m_condDBCnvSvc->decodeDescription( description,storage_type);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not decode folder description" << endreq;
    return sc;
  }

  log << MSG::DEBUG << "delegate to DetectorPersistencySvc" << endmsg;
  // Create temporary address for the relevant type and classID 
  IOpaqueAddress *tmpAddress;
  const std::string par[2] = { stringData, 
                               pAddress->par()[1]};
  sc = dynamic_cast<IConversionSvc*>(m_condDBCnvSvc)->
    addressCreator()->createAddress
    ( storage_type,pAddress->clID() , par, 0, tmpAddress );
  if (sc.isFailure()){
     log << MSG::ERROR 
         << "Persistency service could not create a new address" << endreq;
    return sc;
  }

  if (sc.isFailure()) return sc;
  tmpAddress->addRef();
  if ( pAddress->registry() ){
    log << MSG::DEBUG << "register tmpAddress to registry " << pAddress->registry()->identifier() << endmsg;
  } else {
    log << MSG::WARNING << "the address does not have a registry" << endmsg;
  }
  tmpAddress->setRegistry(pAddress->registry());
  if (tmpAddress->registry()) {
    log << MSG::DEBUG << "tmpAddress registered to registry " << tmpAddress->registry()->identifier() << endmsg;
  } else {
    log << MSG::WARNING << "tmpAddress not registered!" << endmsg;
  }
  
  sc = m_detPersSvc->createObj ( tmpAddress, pObject );
  tmpAddress->release();
  if ( sc.isFailure() ) {
    log << MSG::ERROR 
        << "Persistency service could not create a new object" << endreq;
    return sc;
  }

  log << MSG::DEBUG << "New object successfully created" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
