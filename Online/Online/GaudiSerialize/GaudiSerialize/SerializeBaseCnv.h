// $Id: SerializeBaseCnv.h,v 1.1 2008-06-17 13:23:14 frankb Exp $
//====================================================================
//	SerializeBaseCnv definition
//
//	Author     : M.Frank
//====================================================================
#ifndef SERIALIZE_POOLDBBASECNV_H
#define SERIALIZE_POOLDBBASECNV_H

// Framework include files
#include "GaudiKernel/Converter.h"
#include "GaudiSerialize/SerializeCnvSvc.h"

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class SerializeBaseCnv SerializeBaseCnv.h Serialize/SerializeBaseCnv.h
   *
   * Description:
   * Definition of the generic Db data converter.
   * The generic data converter provides the infrastructure
   * of all data converters. The templated class takes two arguments:
   * The Transient and the Persistent object type.
   *
   * For a detailed description of the overridden function see the the
   * base class.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class SerializeBaseCnv : public Converter   {
  protected:
    /// Services needed for proper operation: POOL database manager
    SerializeCnvSvc* m_dbMgr;

  public:
    typedef Gaudi::PluginService::Factory3<IConverter*,
                                           long,
                                           const CLID&,
                                           ISvcLocator*> Factory;

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      clid     [IN]     Class identifier of the object
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to SerializeBaseCnv object
     */
    SerializeBaseCnv(long typ, const CLID& clid, ISvcLocator* svc);

    /// Standard Destructor
    virtual ~SerializeBaseCnv();

    /// Converter overrides: Initialize the Db converter
    virtual StatusCode initialize();

    /// Converter overrides: Finalize the Db data converter
    virtual StatusCode finalize();

    /// Retrieve the class type of the data store the converter uses.
    virtual long repSvcType() const  {    return i_repSvcType();  }

    /** Converter overrides: Create transient object from persistent data
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    refpObj     [OUT]  Location to pointer to store data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createObj(     IOpaqueAddress* pAddr,
				      DataObject*& refpObj);

    /** Converter overrides: Resolve the references
     * of the created transient object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode fillObjRefs(   IOpaqueAddress* pAddr,
				      DataObject*     pObj)
    {  return updateObjRefs(pAddr, pObj); }

    /** Converter overrides: Update transient object from persistent data
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateObj(     IOpaqueAddress* /* pAddr */,
				      DataObject*     /* pObj  */ )
    {  return StatusCode::SUCCESS; }

    /** Converter overrides: Update the references of
     * an updated transient object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateObjRefs( IOpaqueAddress* pAddr,
				      DataObject*     pObj);

    /** Converter overrides: Convert the transient object to the
     * requested representation.
     *
     * @param    pObj     [IN]   Pointer to data object
     * @param    refpAddr [OUT]  Location to store pointer to object address.
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createRep(     DataObject* pObj,
				      IOpaqueAddress*& refpAddr);

    /** Converter overrides: Resolve the references of the converted object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode fillRepRefs(   IOpaqueAddress* /* pAddr */,
				      DataObject*     /* pObj  */)
    {  return StatusCode::SUCCESS; }

    /** Converter overrides: Update the converted representation
     * of a transient object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateRep(     IOpaqueAddress* /* pAddr */,
				      DataObject*     /* pObj  */)
    {  return StatusCode::SUCCESS; }

    /** Converter overrides: Update the references of an
     * already converted object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode updateRepRefs( IOpaqueAddress* /* pAddr */,
				      DataObject*     /* pObj  */)
    {   return StatusCode::SUCCESS; }

  };
}         // End namespace Gaudi

#define DECLARE_SERIALIZE_CNV_FACTORY(x, clid) \
   DECLARE_COMPONENT_WITH_ID(x, ConverterID(SERIALIZE_StorageType, clid))


#endif    // SERIALIZE_SerializeBaseCnv_H
