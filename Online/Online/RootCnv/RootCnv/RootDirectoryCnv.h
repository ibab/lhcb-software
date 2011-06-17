// $Id: RootDirectoryCnv.h,v 1.2 2010-08-24 23:30:32 frankb Exp $
//------------------------------------------------------------------------------
// Definition of class :  RootDirectoryCnv
//--------------------------------------------------------------------
//
//  Package    : DbCnv/NTuples ( The LHCb Offline System)
//  Author     : M.Frank
//
//------------------------------------------------------------------------------
#ifndef POOLDB_POOLDBDIRECTORYCNV_H
#define POOLDB_POOLDBDIRECTORYCNV_H 1

// Include files
#include "RootCnv/RootStatCnv.h"

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class RootDirectoryCnv RootDirectoryCnv.h src/RootDirectoryCnv.h
   *
   * Description:
   * NTuple directory converter class definition
   * Definition of the converter to manage the
   * directories in an database representing N-Tuples.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API RootDirectoryCnv: public RootStatCnv  {
  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      clid     [IN]     Class identifier of the object
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to RootBaseCnv object
     */
    RootDirectoryCnv(long typ, const CLID& clid, ISvcLocator* svc, RootCnvSvc* mgr);

    /// Standard Destructor
    virtual ~RootDirectoryCnv()   {  }

    /** Converter overrides: Create transient object from persistent data
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    refpObj     [OUT]  Location to pointer to store data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createObj(IOpaqueAddress* pAddr,DataObject*& refpObj);

    /** Converter overrides: Resolve the references of the transient object.
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    pObj        [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr,DataObject* pObj);

    /** Converter overrides: Update transient object from persistent data
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    pObj        [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateObj(IOpaqueAddress* pAddr,DataObject* pObj);

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

    /** Converter overrides: Fill references of persistent object representation.
     *
     * @param    pObj        [IN]   Pointer to data object
     * @param    refpAddr    [OUT]  Location to store pointer to object address.
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj);

    /** Converter overrides: Update persistent object representation.
     *
     * @param    pObj        [IN]   Pointer to data object
     * @param    refpAddr    [OUT]  Location to store pointer to object address.
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateRep(IOpaqueAddress* pAddr,DataObject* pObj);

    /** Converter overrides: Update references of persistent object representation.
     *
     * @param    pObj        [IN]   Pointer to data object
     * @param    refpAddr    [OUT]  Location to store pointer to object address.
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateRepRefs(IOpaqueAddress* pAddr,DataObject* pObj);
  };
}
#endif // POOLDB_POOLDBDIRECTORYCNV_H
