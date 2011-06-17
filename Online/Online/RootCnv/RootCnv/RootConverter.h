// $Id: RootConverter.h,v 1.5 2010-08-24 14:03:03 frankb Exp $
//====================================================================
//	RootConverter class definition
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIROOTCNV_ROOTCONVERTER_H
#define GAUDIROOTCNV_ROOTCONVERTER_H

// Framework include files
#include "GaudiKernel/Converter.h"
#include "RootCnv/RootCnvSvc.h"

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class RootConverter RootConverter.h Root/RootConverter.h
   *
   * Description:
   * Definition of the ROOT data converter.
   * The generic data converter provides the infrastructure 
   * of all data converters. All actions are delegated to
   * the corresponding conversion service.
   *
   * For a detailed description of the overridden function see the the
   * base class.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API RootConverter : public Converter   {
  protected:

    /// Conversion service needed for proper operation to forward requests
    RootCnvSvc* m_dbMgr;

  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      clid     [IN]     Class identifier of the object
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to RootConverter object
     */
    RootConverter(long typ, const CLID& clid, ISvcLocator* svc, RootCnvSvc* mgr)
      : Converter(typ, clid, svc), m_dbMgr(mgr) {}
      
      /// Standard Destructor
      virtual ~RootConverter() {}

      /// Retrieve the class type of the data store the converter uses.
      virtual long repSvcType() const  {    return i_repSvcType();  }

      /** Converter overrides: Create transient object from persistent data
       *
       * @param    pAddr       [IN]   Pointer to object address.
       * @param    refpObj     [OUT]  Location to pointer to store data object
       *
       * @return Status code indicating success or failure.
       */
      virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& refpObj) 
      {	return m_dbMgr->i__createObj(pAddr, refpObj);      }

      /** Resolve the references of the created transient object.
       *
       * @param    pAddr    [IN]   Pointer to object address.
       * @param    pObj     [IN]   Pointer to data object
       *
       * @return Status code indicating success or failure.
       */
      virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj) 
      {	return m_dbMgr->i__fillObjRefs(pAddr, pObj);       }

      /** Converter overrides: Convert the transient object to the 
       * requested representation.
       *
       * @param    pObj     [IN]   Pointer to data object
       * @param    refpAddr [OUT]  Location to store pointer to object address.
       *
       * @return Status code indicating success or failure.
       */
      virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& refpAddr) 
      {	return m_dbMgr->i__createRep(pObj, refpAddr);      }

      /** Resolve the references of the created transient object.
       *
       * @param    pAddr    [IN]   Pointer to object address.
       * @param    pObj     [IN]   Pointer to data object
       *
       * @return Status code indicating success or failure.
       */
      virtual StatusCode fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj) 
      {	return m_dbMgr->i__fillRepRefs(pAddr, pObj);       }
  };
}

#endif    // GAUDIROOTCNV_ROOTCONVERTER_H
