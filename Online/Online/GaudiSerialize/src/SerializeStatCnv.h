// $Id: SerializeStatCnv.h,v 1.3 2010/09/02 11:59:57 frankb Exp $
//------------------------------------------------------------------------------
// Definition of class :  SerializeStatCnv
//
//  Author     : M.Frank
//
//------------------------------------------------------------------------------
#ifndef GAUDISERIALIZE_SERIALIZESTATCNV_H
#define GAUDISERIALIZE_SERIALIZESTATCNV_H 1

// Include files
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiSerialize/SerializeBaseCnv.h"

// Forward declarations
class IDataManagerSvc;
namespace LHCb { class RawEvent; }

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  class SerializeNTupleFile : public NTuple::File {
  public:
    LHCb::RawEvent* data;
    SerializeNTupleFile(const std::string name, const std::string& logName) 
      : NTuple::File(CLID_StatisticsFile,name,logName), data(0) {}
    virtual ~SerializeNTupleFile();
  };

  /** @class SerializeStatCnv SerializeStatCnv.h Serialize/SerializeStatCnv.h
   *
   * Description:
   * Base class converter for N-tuples and related classes.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API SerializeStatCnv: public SerializeBaseCnv   {
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

    /** Helper method to issue error messages.
     * @param      msg      [IN]     Text of the error message
     * @param      throw_exc[IN]     If true throw an exception of type std::runtime_error
     *
     * @return    Status code indicating success or failure.
     */
    StatusCode makeError(const std::string& msg, bool throw_exception=false)  const;

    /** Retrieve the full path of the file a given object is placed into
     * @param      pReg     [IN]    Pointer to registry entry.
     *
     * @return     Name of the file the object should be put to.
     */
    virtual const std::string topLevel(IRegistry* pReg) const;

  public:

    /** Initializing Constructor
     * @param      typ      [IN]     Concrete storage type of the converter
     * @param      clid     [IN]     Class identifier of the object
     * @param      svc      [IN]     Pointer to service locator object
     *
     * @return Reference to SerializeBaseCnv object
     */
    SerializeStatCnv(long typ, const CLID& clid, ISvcLocator* svc) 
      : SerializeBaseCnv(typ,clid,svc), m_dataMgr(0), m_log(0) {  }

    /// Standard Destructor
    virtual ~SerializeStatCnv()   {  }
  };
}      // End namespace Gaudi
#endif // GAUDISERIALIZE_SERIALIZESTATCNV_H
