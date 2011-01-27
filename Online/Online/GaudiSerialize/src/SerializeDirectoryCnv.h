// $Id: SerializeDirectoryCnv.h,v 1.2 2010/08/24 23:30:32 frankb Exp $
//------------------------------------------------------------------------------
// Definition of class :  SerializeDirectoryCnv
//--------------------------------------------------------------------
//
//  Package    : DbCnv/NTuples ( The Gaudi Offline System)
//  Author     : M.Frank
//
//------------------------------------------------------------------------------
#ifndef GAUDISERIALIZE_SERIALIZEDIRECTORYCNV_H
#define GAUDISERIALIZE_SERIALIZEDIRECTORYCNV_H 1

// Include files
#include "SerializeStatCnv.h"

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class SerializeDirectoryCnv SerializeDirectoryCnv.h src/SerializeDirectoryCnv.h
   *
   * Description:
   * NTuple directory converter class definition
   * Definition of the converter to manage the
   * directories in an database representing N-Tuples.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API SerializeDirectoryCnv: public SerializeStatCnv  {
  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      clid     [IN]     Class identifier of the object
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to SerializeBaseCnv object
     */
    SerializeDirectoryCnv(long typ, const CLID& clid, ISvcLocator* svc) : SerializeStatCnv(typ, clid, svc) {}

    /// Standard Destructor
    virtual ~SerializeDirectoryCnv()   {  }

    /** Converter overrides: Create transient object from persistent data
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    refpObj     [OUT]  Location to pointer to store data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createObj(IOpaqueAddress* pAddr,DataObject*& refpObj);

    /** Converter overrides: Update the references of
     * an updated transient object.
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    pObj        [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateObjRefs(IOpaqueAddress* pAddr,DataObject* pObj);

    /** Converter overrides: Convert the transient object to the
     * requested representation.
     *
     * @param    pObj        [IN]   Pointer to data object
     * @param    refpAddr    [OUT]  Location to store pointer to object address.
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createRep(DataObject* pObj,IOpaqueAddress*& refpAddr);
  };
}
#endif // GAUDISERIALIZE_SERIALIZEDIRECTORYCNV_H
