// $Id: RootStatCnv.h,v 1.3 2010-09-02 11:59:57 frankb Exp $
//------------------------------------------------------------------------------
// Definition of class :  RootStatCnv
//
//  Author     : M.Frank
//
//------------------------------------------------------------------------------
#ifndef ROOT_ROOTSTATCNV_H
#define ROOT_ROOTSTATCNV_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "RootCnv/RootConverter.h"

// Forward declarations
class IDataManagerSvc;

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class RootStatCnv RootStatCnv.h Root/RootStatCnv.h
   *
   * Description:
   * Base class converter for N-tuples and related classes.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API RootStatCnv: public RootConverter   {
  protected:
    /// Reference to data manager service to manipulate the TES
    IDataManagerSvc* m_dataMgr;
    /// Reference to logger object
    MsgStream*       m_log;

    /// Helper to use mesage logger 
    MsgStream& log() const { return *m_log; }

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

    /** Helper method to issue error messages.
     * @param      msg      [IN]     Text of the error message
     * @param      throw_exc[IN]     If true throw an exception of type std::runtime_error
     *
     * @return    Status code indicating success or failure.
     */
    StatusCode makeError(const std::string& msg, bool throw_exception=false)  const;

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
