// $Id: RelyConverter.cpp,v 1.1 2005-02-09 08:49:29 marcocle Exp $
// Include files 
#include "DetCond/RelyConverter.h"

#include "DetDesc/ValidDataObject.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"

#include "DetCond/IConditionsDBCnvSvc.h"
#include "DetCond/ICondDBAccessSvc.h"

#include "GaudiKernel/TimePoint.h"
//#include "CoolKernel/IValidityKey.h"

#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IDatabase.h"
#include "RelationalCool/RelationalException.h"

#include <string>
#include <sstream>

// local

//-----------------------------------------------------------------------------
// Implementation file for class : RelyConverter
//
// 2004-12-03 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<RelyConverter> s_factoryRelyCnv;
const ICnvFactory& RelyConverterFactory = s_factoryRelyCnv;

const CLID RelyConverter::s_CLID_any = 0;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelyConverter::RelyConverter(ISvcLocator* svc):
  CondDBGenericCnv(svc,0)
{

}
//=============================================================================
// Destructor
//=============================================================================
RelyConverter::~RelyConverter() {}; 

//=========================================================================
// Initialization
//=========================================================================
StatusCode RelyConverter::initialize() {
  // Initializes the grand father
  StatusCode sc = CondDBGenericCnv::initialize();

  sc = serviceLocator()->service 
    ("DetectorPersistencySvc", m_detPersSvc, true);
  if ( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"RelyConverter");
    log << MSG::ERROR 
        << "Cannot locate IConversionSvc interface of DetectorPersistencySvc"
        << endreq;
    return sc;
  } else {
    MsgStream log(msgSvc(),"RelyConverter");
    log << MSG::DEBUG 
        << "Retrieved IConversionSvc interface of DetectorPersistencySvc"
        << endreq;
  }
  return sc;
}

//=========================================================================
// Finalization
//=========================================================================
StatusCode RelyConverter::finalize() {
  m_detPersSvc->release();
  return CondDBGenericCnv::finalize();
}

//=========================================================================
// Create the transient representation
//=========================================================================
StatusCode RelyConverter::createObj (IOpaqueAddress* pAddress,
                                  DataObject *&pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
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
StatusCode RelyConverter::updateObj (IOpaqueAddress* pAddress,
                                  DataObject* pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
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
      log << MSG::INFO << "Be careful, because ValidDataObject::update() is not really implemented" << endmsg;
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
StatusCode RelyConverter::createRep (DataObject* pObject,
                                  IOpaqueAddress*& pAddress)
{
  MsgStream log(msgSvc(),"RelyConverter");
  log << MSG::WARNING << "createRep() not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Update the persistent representation
//=========================================================================
StatusCode RelyConverter::updateRep (IOpaqueAddress* pAddress,
                                  DataObject* pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
  log << MSG::WARNING << "updateRep() not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Create an object by delegation
//=========================================================================
StatusCode RelyConverter::i_delegatedCreation(IOpaqueAddress* pAddress,
                                           DataObject *&pObject,TimePoint &since,TimePoint &till){
  StatusCode sc;

  MsgStream log(msgSvc(),"RelyConverter");

  TimePoint now;
  sc = eventTime(now);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Cannot create DataObject: event time undefined"
        << endmsg;
    return sc;
  }

  try {

    cool::IFolderPtr folder = m_dbAccSvc->database()->getFolder(pAddress->par()[0]);    
    cool::IObjectPtr object = folder->findObject(m_condDBCnvSvc->timeToValKey(now));

    //    long storage_type = object->payloadValue<long>("storage_type");
    long storage_type = getStorageType(folder->description());
    if (storage_type <= 0) {
      log << MSG::ERROR <<
        "Folder description does not contain a valid storage type: " << endmsg;
      log << MSG::ERROR <<
        "desc = \"" << folder->description() << "\"" << endmsg;
      return StatusCode::FAILURE;
    }

    since = m_condDBCnvSvc->valKeyToTime(object->since());
    till  = m_condDBCnvSvc->valKeyToTime(object->till());

    log << MSG::DEBUG << "delegate to DetectorPersistencySvc" << endmsg;

    // Create temporary address for the relevant type and classID 
    IOpaqueAddress *tmpAddress;
    const std::string par[2] = { object->payloadValue<std::string>("data"), 
                                 pAddress->par()[1]};
    sc = dynamic_cast<IConversionSvc*>(m_condDBCnvSvc)->addressCreator()
      ->createAddress( storage_type,pAddress->clID() , par, 0, tmpAddress );
    if (sc.isFailure()){
      log << MSG::ERROR 
          << "Persistency service could not create a new address" << endreq;
      return sc;
    }

    if (sc.isFailure()) return sc;
    tmpAddress->addRef();
    if ( pAddress->registry() ){
      log << MSG::DEBUG << "register tmpAddress to registry " << pAddress->registry()->identifier()
          << endmsg;
    } else {
      log << MSG::WARNING << "the address does not have a registry" << endmsg;
    }
    tmpAddress->setRegistry(pAddress->registry());
    if (tmpAddress->registry()) {
      log << MSG::DEBUG << "tmpAddress registered to registry " << tmpAddress->registry()->identifier()
          << endmsg;
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
    
  } catch ( cool::RelationalFolderNotFound &e) {
    log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  } catch (cool::RelationalObjectNotFound &e) {
    log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  }

  
  log << MSG::DEBUG << "New object successfully created" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================


pool::AttributeListSpecification RelyConverter::m_attlist;

const pool::AttributeListSpecification & RelyConverter::attrListSpec() {
  static bool first = true;
  if (first) {
    //    MsgStream log(msgSvc(),"RelyConverter");
    //    log << MSG::DEBUG << "Preparing the AttributeListSpecification" << endmsg;
    std::cout << "Preparing the AttributeListSpecification" << std::endl;
    m_attlist.push_back<long>("storage_type");
    m_attlist.push_back<std::string>("data");
    first = false;
  }
  return m_attlist;
}

//=============================================================================

long RelyConverter::getStorageType(const std::string &desc){
  // the description string should contain a substring of the form (regexp)
  // "< *storage_type *= *[0-9]+ *>"

  const char delimiter_begin = '<';
  const char delimiter_end = '>';
  const char delimiter_sep = '=';
  const std::string delimiter_keyword = "storage_type";

  std::string::size_type pos_start;
  std::string::size_type pos_end;
  std::string::size_type pos_max;
  
  pos_start = pos_end = pos_max = desc.size();

  std::string::size_type tmp_pos = 0;

  while ( pos_start == pos_max || pos_end <= pos_start ){    
    // find the next occurrence of '<'
    tmp_pos = desc.find(delimiter_begin,tmp_pos);
    if (tmp_pos >= pos_max) break; // not found

    // skip spaces
    ++tmp_pos;
    while ( desc[tmp_pos] == ' ' || desc[tmp_pos] == '\t' ||  desc[tmp_pos] == '\n' ) ++tmp_pos;
    
    // check for the keyword
    if (desc.compare(tmp_pos, delimiter_keyword.size(), delimiter_keyword) != 0) continue;

    // skip spaces
    tmp_pos += delimiter_keyword.size();
    while ( desc[tmp_pos] == ' ' || desc[tmp_pos] == '\t' ||  desc[tmp_pos] == '\n' ) ++tmp_pos;
  
    // check for the separator
    if ( desc[tmp_pos] != delimiter_sep ) continue;
  
    // skip spaces
    ++tmp_pos;
    while ( desc[tmp_pos] == ' ' || desc[tmp_pos] == '\t' ||  desc[tmp_pos] == '\n' ) ++tmp_pos;
  
    // here should start the "int"
    pos_start = tmp_pos;
    
    // "count" the digits
    while ( desc[tmp_pos] >= '0' && desc[tmp_pos] <= '9' ) ++tmp_pos;
    
    // here should be just after the "int"
    pos_end = tmp_pos;
    
    if ( pos_start == pos_end ) continue; // no number found
    
    // skip spaces
    while ( desc[tmp_pos] == ' ' || desc[tmp_pos] == '\t' ||  desc[tmp_pos] == '\n' ) ++tmp_pos;

    // check for the delimiter_end
    if ( desc[tmp_pos] != delimiter_end ) {
      // force another loop even if the number was found;
      pos_start = pos_end = pos_max;
      continue;
    }
  }
  
  if ( pos_start == pos_max || pos_end <= pos_start ) { // not found
    return -1;
  }

  std::istringstream i(desc.substr(pos_start, pos_end-pos_start));
  
  long st;
  i >> st;
  
  return st;
}
