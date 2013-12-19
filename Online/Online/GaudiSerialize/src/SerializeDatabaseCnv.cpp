// $Id: SerializeDatabaseCnv.h,v 1.2 2010/08/24 14:03:03 frankb Exp $
//====================================================================
//  Statistics file converter class definition
//
//  Author     : M.Frank
//
//====================================================================
#ifndef GAUDISERIALIZECNV_SERIALIZEDATABASECNV_H
#define GAUDISERIALIZECNV_SERIALIZEDATABASECNV_H 1

// Framework include files
#include "SerializeDirectoryCnv.h"

/*
 *   Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class SerializeDatabaseCnv SerializeDatabaseCnv.h Serialize/SerializeDatabaseCnv.h
   *
   * Statistics file converter class definition
   *
   * Description:
   * Definition of the converter to open serialize database files.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API SerializeDatabaseCnv : public SerializeDirectoryCnv {
  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      svc      [IN]     Pointer to service locator object
     * @param      mgr      [IN]     Pointer to hosting conversion service
     *
     * @return Reference to SerializeDatabaseCnv object
     */
    SerializeDatabaseCnv(long typ, const CLID& clid, ISvcLocator* svc): SerializeDirectoryCnv(typ,clid,svc) {}

    /// Standard destructor
    virtual ~SerializeDatabaseCnv() {}

    /** Converter overrides: Create transient object from persistent data
     *
     * @param    refpAddress [IN]   Pointer to object address.
     * @param    refpObject  [OUT]  Location to pointer to store data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& refpObj);

    /** Converter overrides: Resolve the references
     * of the created transient object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj);
  };
}
#endif    // GAUDISERIALIZECNV_SERIALIZEDATABASECNV_H
// $Id: SerializeDatabaseCnv.cpp,v 1.6 2010/09/27 15:43:53 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  SerializeDatabaseCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------
// Framework include files
//#include "SerializeDatabaseCnv.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "Event/RawEvent.h"

using namespace Gaudi;
DECLARE_SERIALIZE_CNV_FACTORY(SerializeDatabaseCnv, CLID_StatisticsFile)

// Create database object
StatusCode
SerializeDatabaseCnv::createObj(IOpaqueAddress* pAddr, DataObject*& refpObj)  {
  NTuple::File* pFile = new SerializeNTupleFile("", "");
  pFile->setOpen(true);
  refpObj = pFile;
  std::cout << "Create transient NTuple file object:" << pAddr->par()[0] << std::endl;
  return StatusCode::SUCCESS;
}

// Converter overrides: Fill the references of an updated transient object.
StatusCode
SerializeDatabaseCnv::fillObjRefs(IOpaqueAddress* /* pAddr */, DataObject* pObject)
{
  std::string top = topLevel(pObject->registry())+"/RawTupleData";
  DataObject* obj = 0;
  StatusCode sc = dataProvider()->retrieveObject(top,obj);
  if ( !sc.isSuccess() ) {
    obj = new LHCb::RawEvent();
    sc = dataProvider()->registerObject(top,obj);
    if ( !sc.isSuccess() ) {
      delete obj;
      return makeError("Failed to create RAW data object in store:"+top);
    }
  }
  return StatusCode::SUCCESS;
}
