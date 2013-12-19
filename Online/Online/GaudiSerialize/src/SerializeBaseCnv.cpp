// $Id: SerializeBaseCnv.cpp,v 1.6 2010-09-23 12:13:36 frankb Exp $
//====================================================================
//	SerializeBaseCnv implementation
//--------------------------------------------------------------------
//
//  Package    : GaudiSerialize ( The LHCb Offline System)
//
//  Description: Generic Db data converter
//
//	Author     : M.Frank
//====================================================================

// Framework include files
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IUpdateable.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiSerialize/SerializeBaseCnv.h"
#include "GAUDI_VERSION.h"

using namespace Gaudi;

#if GAUDI_VERSION == CALC_GAUDI_VERSION(21, 11)
namespace {
  union ObjectTypes {
    ObjectTypes(DataObject* p) { Object = p; }
    DataObject*                                             Object;
    KeyedContainer<KeyedObject<long>, Containers::Map>*     KeyMap;
    KeyedContainer<KeyedObject<long>, Containers::HashMap>* KeyHashMap;
    KeyedContainer<KeyedObject<long>, Containers::Array>*   KeyArray;
    StatusCode update(int flag)  {
      IUpdateable* obj = dynamic_cast<IUpdateable*>(Object);
      if ( obj ) {
        return obj->update(flag);
      }
      return StatusCode::FAILURE;
    }
  };
}
#endif

/// Standard Constructor
SerializeBaseCnv::SerializeBaseCnv(long typ, const CLID& clid, ISvcLocator* svc)
: Converter(typ, clid, svc), m_dbMgr(0)
{
}

/// Standard Destructor
SerializeBaseCnv::~SerializeBaseCnv()  {
}

/// Initialize the converter
StatusCode SerializeBaseCnv::initialize()   {
  StatusCode status = Converter::initialize();
  MsgStream log(msgSvc(),"SerializeBaseCnv");
  if ( !status.isSuccess() )   {
    log << MSG::ERROR << "Cannot initialize base class \"Converter\"" << endmsg;
    return status;
  }
  status = conversionSvc()->queryInterface(IID_ISerializeMgr, (void**)&m_dbMgr);
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Cannot connect to \"ISerializeMgr\" interface." << endmsg;
    return status;
  }
  return status;
}

/// Finalize the Db converter
StatusCode SerializeBaseCnv::finalize()   {
  if ( m_dbMgr ) m_dbMgr->release();
  m_dbMgr = 0;
  return Converter::finalize();
}

/// Db specific implementation: Create an empty transient object: Default: DataObject
StatusCode SerializeBaseCnv::createObj(IOpaqueAddress* pAddress, DataObject*& refpObj) {
  return m_dbMgr->readObject(pAddress, refpObj);
}

/// Create persistent representation of the object
StatusCode SerializeBaseCnv::createRep(DataObject* pObj, IOpaqueAddress*& refpA) {
  return m_dbMgr->writeObject(pObj, refpA);
}

/// Update the references of an updated transient object.
StatusCode SerializeBaseCnv::updateObjRefs(IOpaqueAddress* /* pA */, DataObject* pObj) {
#if GAUDI_VERSION == CALC_GAUDI_VERSION(21, 11)
  if ( pObj )    {
    ObjectTypes obj(pObj);
    const CLID id = (pObj->clID()&0xFFFF0000);
    switch(id) {
    case CLID_ObjectList:              /* ObjectList               */
      return StatusCode::SUCCESS;
    case CLID_ObjectVector:            /* ObjectVector             */
      return StatusCode::SUCCESS;
    case CLID_ObjectVector+0x0030000:  /* Keyed object map         */
      obj.KeyMap->configureDirectAccess();
      return StatusCode::SUCCESS;
    case CLID_ObjectVector+0x0040000:  /* Keyed object hashmap     */
      obj.KeyHashMap->configureDirectAccess();
      return StatusCode::SUCCESS;
    case CLID_ObjectVector+0x0050000:  /* Keyed indirection array  */
      obj.KeyArray->configureDirectAccess();
      return StatusCode::SUCCESS;
    case 0:                            /* Any other object         */
      return StatusCode::SUCCESS;
    default:
      return obj.update(0);
    }
  }
  return StatusCode::SUCCESS;
#else
  return pObj ? pObj->update() : StatusCode::FAILURE;
#endif
}

DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any)
DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any+CLID_ObjectList)
DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any+CLID_ObjectVector)

DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any + CLID_ObjectVector+0x00030000)
DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any + CLID_ObjectVector+0x00040000)
DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any + CLID_ObjectVector+0x00050000)
DECLARE_SERIALIZE_CNV_FACTORY(SerializeBaseCnv, CLID_Any + CLID(1<<31))
