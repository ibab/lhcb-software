// Include files
#include "RelyConverter.h"

#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "DetDesc/ValidDataObject.h"

#include "CoolKernel/IObject.h"
#include "CoolKernel/IRecord.h"
#include "CoolKernel/RecordException.h"

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
DECLARE_CONVERTER_FACTORY(RelyConverter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelyConverter::RelyConverter(ISvcLocator* svc):
  CondDBGenericCnv(svc,RelyConverter::classID()),
  m_detPersSvc(NULL)
{}
//=============================================================================
// Destructor
//=============================================================================
RelyConverter::~RelyConverter() {}

//=========================================================================
// Initialization
//=========================================================================
StatusCode RelyConverter::initialize() {
  // Initializes the grand father
  StatusCode sc = CondDBGenericCnv::initialize();

  sc = serviceLocator()->service("DetectorPersistencySvc", m_detPersSvc, true);
  if ( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),"RelyConverter");
    log << MSG::ERROR << "Cannot locate IConversionSvc interface of DetectorPersistencySvc" << endmsg;
    return sc;
  } else {
    MsgStream log(msgSvc(),"RelyConverter");
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Retrieved IConversionSvc interface of DetectorPersistencySvc" << endmsg;
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
StatusCode RelyConverter::createObj (IOpaqueAddress* pAddress, DataObject *&pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "entering createObj" << endmsg;

  StatusCode sc = i_delegatedCreation(pAddress,pObject,CreateObject);
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot create the object " << pAddress->registry()->identifier() << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Fill references of the transient representation
//=========================================================================
StatusCode RelyConverter::fillObjRefs (IOpaqueAddress* pAddress, DataObject *pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "entering fillObjRefs" << endmsg;

  StatusCode sc = i_delegatedCreation(pAddress,pObject,FillObjectRefs);
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot fill object's refs" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Update transient representation from persistent one
//=========================================================================
StatusCode RelyConverter::updateObj (IOpaqueAddress* pAddress, DataObject* pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Method updateObj starting" << endmsg;

  DataObject* pNewObject; // create a new object and copy it to the old version
  StatusCode sc = i_delegatedCreation(pAddress,pNewObject,CreateObject);
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
        << endmsg;
    return StatusCode::FAILURE;
  }
  // Deep copy the new Condition into the old DataObject
  pVDO->update( *pNewVDO );

  // Delete the useless Condition
  delete pNewVDO;

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Object successfully updated" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Update references of the transient representation
//=========================================================================
StatusCode RelyConverter::updateObjRefs (IOpaqueAddress* pAddress, DataObject *pObject)
{
  MsgStream log(msgSvc(),"RelyConverter");
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "entering updateObjRefs" << endmsg;

  StatusCode sc = i_delegatedCreation(pAddress,pObject,UpdateObjectRefs);
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot update object's refs" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Create the persistent representation
//=========================================================================
StatusCode RelyConverter::createRep (DataObject* /*pObject*/, IOpaqueAddress*& /*pAddress*/)
{
  MsgStream log(msgSvc(),"RelyConverter");
  log << MSG::WARNING << "createRep() not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Update the persistent representation
//=========================================================================
StatusCode RelyConverter::updateRep (IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
{
  MsgStream log(msgSvc(),"RelyConverter");
  log << MSG::WARNING << "updateRep() not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
// Create an object by delegation
//=========================================================================
StatusCode RelyConverter::i_delegatedCreation(IOpaqueAddress* pAddress, DataObject *&pObject, Operation op){
  StatusCode sc;

  MsgStream log(msgSvc(),"RelyConverter");

  ICondDBReader::DataPtr data;
  std::string description;
  Gaudi::Time since,until;

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Entering \"i_delegatedCreation\"" << endmsg;

  std::string path = pAddress->par()[0];
  std::string data_field_name = "data";

  // Extract the COOL field name from the condition path
  std::string::size_type at_pos = path.find('@');
  if ( at_pos != path.npos ) {
    std::string::size_type slash_pos = path.rfind('/',at_pos);
    if ( slash_pos+1 < at_pos ) { // item name is not null
      data_field_name = path.substr(slash_pos+1,at_pos - (slash_pos +1));
    } // if I have "/@", I should use the default ("data")
    // always remove '@' from the path
    path = path.substr(0,slash_pos+1) +  path.substr(at_pos+1);
  }

  sc = getObject(path, pAddress->ipar()[0], data, description, since, until);
  if ( !sc.isSuccess() ) return sc;

  if ( !data ) {
    switch (op) {
    case CreateObject:
      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
        log << MSG::DEBUG << "Path points to a FolderSet: create a directory" << endmsg;

      // I hit a FolderSet!!! I handle it here (at least for the moment, since it's the only CondDB real converter)
      pObject = new DataObject();

      break;

    case FillObjectRefs:
      {
        if( UNLIKELY( log.level() <= MSG::DEBUG ) )
          log << MSG::DEBUG << "Create addresses for sub-folders" << endmsg;

        // find subnodes
        std::vector<std::string> children;

        sc = getChildNodes(path,children);
        if ( !sc.isSuccess() ) return sc;

        // add registries for the sub folders
        for ( std::vector<std::string>::iterator c = children.begin(); c != children.end(); ++c ) {

          IOpaqueAddress *childAddress;
          std::string par[2];
          // in current implementation, the child folders have a '/' in front.
          // So I need to treat in a different way the case of a parent COOL root folderset
          // to avoid thing like "//folder"
          if ( path == "/" ) {
            par[0] = *c;
          } else {
            par[0] = path + *c;
          }
          par[1] = *c;
          unsigned long ipar[2] = { 0,0 };

          if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
            log << MSG::VERBOSE << "Create address for " << par[0] << endmsg;
          sc = conversionSvc()->addressCreator()->createAddress(CONDDB_StorageType,
                                                                CLID_Catalog,
                                                                par,
                                                                ipar,
                                                                childAddress);
          if ( !sc.isSuccess() ) return sc;
          if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
            log << MSG::VERBOSE << "Address created" << endmsg;

          sc = dataManager()->registerAddress(pAddress->registry(), *c, childAddress);
          if ( !sc.isSuccess() ) return sc;
          if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
            log << MSG::VERBOSE << "Address registered" << endmsg;
        }
      }
      break;

    case UpdateObjectRefs:
      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
        log << MSG::DEBUG << "Update references not supported for FolderSet" << endmsg;
      break;
    }

    return StatusCode::SUCCESS;
  }

  long storage_type = getStorageType(path,description);
  if (storage_type <= 0) {
    log << MSG::ERROR <<
      "Folder description does not contain a valid storage type: " << endmsg;
    log << MSG::ERROR << "desc = \"" << description << "\"" << endmsg;
    return StatusCode::FAILURE;
  }

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "delegate to DetectorPersistencySvc" << endmsg;

  std::string xml_data;
  try {
    xml_data = (*data.get())[data_field_name].data<std::string>();
  } catch (cool::RecordSpecificationUnknownField &e) {
    log << MSG::ERROR << "I cannot find the data inside COOL object: " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }

  // for XML string temporary address, I need a way to know which is the originating href
  std::ostringstream src_href;
  src_href <<  "conddb:" << pAddress->par()[0] << ":" << pAddress->ipar()[0];

  // Create temporary address for the relevant type and classID
  IOpaqueAddress *tmpAddress = createTmpAddress(src_href.str(),
                                                storage_type,
                                                pAddress->par()[1],
                                                pAddress->clID(),
                                                xml_data,
                                                log,
                                                conversionSvc()->addressCreator());
  if (!tmpAddress) return StatusCode::FAILURE;

  tmpAddress->addRef();
  if ( pAddress->registry() ){
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "register tmpAddress to registry " << pAddress->registry()->identifier()
          << endmsg;
  } else {
    log << MSG::WARNING << "the address does not have a registry" << endmsg;
  }
  tmpAddress->setRegistry(pAddress->registry());
  if (tmpAddress->registry()) {
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "tmpAddress registered to registry " << tmpAddress->registry()->identifier()
          << endmsg;
  } else {
    log << MSG::WARNING << "tmpAddress not registered!" << endmsg;
  }

  switch (op) {
  case CreateObject:
    sc = m_detPersSvc->createObj ( tmpAddress, pObject );
    break;
  case FillObjectRefs:
    sc = m_detPersSvc->fillObjRefs ( tmpAddress, pObject );
    break;
  case UpdateObjectRefs:
    sc = m_detPersSvc->updateObjRefs ( tmpAddress, pObject );
    break;
  }

  tmpAddress->release();
  if ( sc.isFailure() ) {
    log << MSG::ERROR
        << "Persistency service could not create a new object" << endmsg;
    return sc;
  }

  if (op == CreateObject){
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Setting object validity" << endmsg;
    setObjValidity(since,until,pObject);

  }
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "New object successfully created" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================

long RelyConverter::getStorageType(const std::string &path, const std::string &desc){
  // the description string should contain a substring of the form (regexp)
  // "< *storage_type *= *[0-9]+ *>"

  if ( path.rfind(".xml") != path.npos ) return XML_StorageType;

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

IOpaqueAddress *RelyConverter::createTmpAddress(const std::string &src,
                                                long storageType,
                                                const std::string &name,
                                                const CLID &clId,
                                                const std::string &data,
                                                MsgStream &log,
                                                SmartIF<IAddressCreator>& creator) {

  if (storageType <= 0) {
    log << MSG::ERROR << "invalid storage type " << storageType << endmsg;
    return 0;
  }

  // Create temporary address for the relevant type and classID
  IOpaqueAddress *tmpAddress;

  // for XML string temporary address, I need a way to know which is the originating href
  const std::string par[3] = { data,
                               name,
                               src };
  unsigned long ipar[2] = { 1,0 };
  StatusCode sc = creator->createAddress(storageType, clId , par, ipar, tmpAddress);
  if (sc.isFailure()){
    log << MSG::ERROR
        << "Persistency service could not create a new address" << endmsg;
    return 0;
  }

  return tmpAddress;
}
