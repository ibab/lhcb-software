// $Id: RootConverter.h,v 1.2 2010-01-11 17:13:39 frankb Exp $
//====================================================================
//	RootBaseCnv definition
//
//	Author     : M.Frank
//====================================================================
#ifndef RootCnv_RootConverter_H
#define RootCnv_RootConverter_H

// Framework include files
#include "GaudiKernel/Converter.h"
#include "RootCnvSvc.h"

namespace Gaudi {

  /** @class RootConverter RootConverter.h Root/RootConverter.h
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
  class RootConverter : public Converter   {
  protected:
    /// Services needed for proper operation
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
      virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& refpObj) {
	return m_dbMgr->i__createObj(pAddr, refpObj);
      }

      /// Resolve the references of the created transient object.
      virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj) {
	return m_dbMgr->i__fillObjRefs(pAddr, pObj);
      }

      /** Converter overrides: Convert the transient object to the 
       * requested representation.
       *
       * @param    pObj     [IN]   Pointer to data object
       * @param    refpAddr [OUT]  Location to store pointer to object address.
       *
       * @return Status code indicating success or failure.
       */
      virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& refpAddr) {
	return m_dbMgr->i__createRep(pObj, refpAddr);
      }

      /// Resolve the references of the created transient object.
      virtual StatusCode fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj) {
	return m_dbMgr->i__fillRepRefs(pAddr, pObj);
      }

  };
}

#endif    // RootCnv_RootConverter_H
