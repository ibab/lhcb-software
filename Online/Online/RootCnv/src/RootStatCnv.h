// $Id: RootStatCnv.h,v 1.1 2010-01-11 17:14:50 frankb Exp $
//------------------------------------------------------------------------------
// Definition of class :  RootStatCnv
//
//  Author     : M.Frank
//
//------------------------------------------------------------------------------
#ifndef ROOT_ROOTSTATCNV_H
#define ROOT_ROOTSTATCNV_H 1

// Include files
#include "RootConverter.h"

class IDataManagerSvc;

namespace Gaudi {

  /** @class RootStatCnv RootStatCnv.h Root/RootStatCnv.h
   *
   * Description:
   * NTuple directory converter class definition
   * Definition of the converter to manage the
   * directories in an database representing N-Tuples.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API RootStatCnv: public RootConverter   {
  protected:
    IDataManagerSvc* m_dataMgr;
  protected:

    /// Initialize converter object
    virtual StatusCode initialize();

    /// Finalize converter object
    virtual StatusCode finalize();


    /** Retrieve the name of the container a given object is placed into
     * @param      pReg     [IN]    Pointer to registry entry.
     *
     * @return     Name of the container the object should be put to.
     */
    virtual const std::string containerName(IRegistry* pDir)  const;

    /** Retrieve the name of the file a given object is placed into
     * @param      pReg     [IN]    Pointer to registry entry.
     *
     * @return     Name of the file the object should be put to.
     */
    virtual const std::string fileName(IRegistry* pReg) const;

    /** Retrieve the full path of the file a given object is placed into
     * @param      pReg     [IN]    Pointer to registry entry.
     *
     * @return     Name of the file the object should be put to.
     */
    virtual const std::string topLevel(IRegistry* pReg) const;

    /** Save statistics object description.
     * @param      path     [IN]    Path of file to save the description on.
     * @param      ident    [IN]    Identifier of description to be saved.
     * @param      desc     [IN]    Description
     * @param      opt      [IN]    Optional description
     * @param      guid     [IN]    Object giud of described object
     * @param      openMode [IN]    Mode of opening the file to save description
     *
     * @return    Status code indicating success or failure.
     */
    StatusCode saveDescription( const std::string&  path,
				const std::string&  ident,
				const std::string&  desc,
				const std::string&  opt,
				const CLID&         clid);

    StatusCode makeError(const std::string& msg, bool throw_exception=false)  const;

  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      clid     [IN]     Class identifier of the object
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to RootBaseCnv object
     */
    RootStatCnv(long typ, const CLID& clid, ISvcLocator* svc, RootCnvSvc* mgr);

    /// Standard Destructor
    virtual ~RootStatCnv()   {  }
  };
}
#endif // ROOT_RootStatCnv_H
