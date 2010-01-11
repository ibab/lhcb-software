// $Id: RootDatabaseCnv.h,v 1.1 2010-01-11 17:14:49 frankb Exp $
//====================================================================
//  Statistics file converter class definition
//
//  Author     : M.Frank
//
//====================================================================
#ifndef ROOT_ROOTDATABASECNV_H
#define ROOT_ROOTDATABASECNV_H 1

// Include files
#include "RootDirectoryCnv.h"

namespace Gaudi {

  /** @class RootDatabaseCnv RootDatabaseCnv.h Root/RootDatabaseCnv.h
   *
   * Statistics file converter class definition
   *
   * Description:
   * Definition of the converter to open Database file.
   * Wether this is a real file or only a connection to
   * a database depends on the underlying engine.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API RootDatabaseCnv: public RootDirectoryCnv {
  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to RootDatabaseCnv object
     */
    RootDatabaseCnv(long typ, const CLID&, ISvcLocator* svc, RootCnvSvc* mgr);

    /// Standard destructor
    virtual ~RootDatabaseCnv();

    /** Retrieve the name of the container a given object is placed into
     * @param      pReg     [IN]    Pointer to registry entry.
     *
     * @return     Name of the container the object should be put to.
     */
    virtual const std::string containerName(IRegistry* /* pReg */)  const  {
      return "<local>";
    }

    /** Converter overrides: Create transient object from persistent data
     *
     * @param    refpAddress [IN]   Pointer to object address.
     * @param    refpObject  [OUT]  Location to pointer to store data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& refpObj);
  };
}
#endif    // ROOT_ROOTDATABASECNV_H
