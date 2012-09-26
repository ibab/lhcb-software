// $Id: RootCnvSvc.h,v 1.7 2010-09-14 06:01:12 frankb Exp $
//====================================================================
//	RootCnvSvc definition
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIROOTCNV_GAUDIROOTCNVSVC_H
#define GAUDIROOTCNV_GAUDIROOTCNVSVC_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootCnvSvc.h,v 1.7 2010-09-14 06:01:12 frankb Exp $

// Framework include files
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ClassID.h"

// C++ include files
#include <set>

// Forward declarations
class IDataManagerSvc;
class IIncidentSvc;
class TClass;

// ROOT include files
#include "TROOT.h"
#include "TClass.h"
#include "TTree.h"
#include "TBranch.h"

//#include "RootCnv/RootPerfMonSvc.h"

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  // Forward declarations
  class IIODataManager;
  class IDataConnection;
  class RootDataConnection;
  class RootConnectionSetup;

  /** @class RootCnvSvc RootCnvSvc.h src/RootCnvSvc.h
   *
   * Description:
   *
   * RootCnvSvc class implementation definition.
   *
   *  @author  Markus Frank
   *  @version 1.0
   *  @date    20/12/2009
   */
  class GAUDI_API RootCnvSvc : public ConversionSvc  {
  protected:

    /// Reference to the I/O data manager
    Gaudi::IIODataManager*      m_ioMgr;
    /// Reference to incident service
    IIncidentSvc*               m_incidentSvc;
    /// On writing: reference to active output stream
    Gaudi::RootDataConnection*  m_current;
    /// TClass pointer to reference class
    TClass*                     m_classRefs;
    /// TClass pointer to DataObject class
    TClass*                     m_classDO;
    /// Setup structure (ref-counted) and passed to data connections
    RootConnectionSetup*        m_setup;
    /// Property: ROOT section name
    std::string                 m_currSection;

    /// Property: Flag to enable incidents on FILE_OPEN
    bool                        m_incidentEnabled;
    /// Property: Share files ? If set to YES, files will not be closed on finalize
    std::string                 m_shareFiles;
    /// Property: Records name to fire incident for file records
    std::string                 m_recordName;
    /// Property: Enable TTree IOperfStats if not empty; otherwise perf stat file name
    std::string                 m_ioPerfStats;
    /// Property: Compression-algorithm:compression-level
    std::string                 m_compression;

    /// Property: AutoFlush parameter for ROOT TTree (Number of events between auto flushes)
    int                         m_autoFlush;
    /// Property: Basket optimization parameter for ROOT TTree (total basket size)
    int                         m_basketSize;
    /// Property: Buffer size optimization parameter for ROOT TTree
    int		                m_bufferSize;
    /// Property: Split level optimization parameter for ROOT TTree
    int		                m_splitLevel;

    /// Set with bad files/tables
    std::set<std::string>       m_badFiles;

    /// Message streamer
    MsgStream*                  m_log;

    /// Helper: Get TClass for a given DataObject pointer
    TClass* getClass(DataObject* pObject);
    /// Helper: Use message streamer
    MsgStream& log() const {   return *m_log; }

  public:

    /// Standard constructor
    RootCnvSvc(const std::string& name, ISvcLocator* svc);

    /// Standard destructor
    virtual ~RootCnvSvc();

    /// Update state of the service
    virtual StatusCode updateServiceState(IOpaqueAddress* /* pAddress */)
    { return StatusCode::SUCCESS;                  }

    /** Standard way to print errors. after the printout an exception is thrown.
     * @param      msg      [IN]     Message string to be printed.
     *
     * @return     Status code returning failure.
     */
    StatusCode error(const std::string& msg);

    /** Connect the output file to the service with open mode.
     *  @param      dataset     String containig file name
     *  @param      mode        String containig opening mode
     *
     *  @return     Status code indicating success or failure.
     */
    StatusCode connectDatabase(const std::string& dataset, int mode, RootDataConnection** con);

    /// ConversionSvc overload: initialize Db service
    virtual StatusCode initialize();

    /// ConversionSvc overload: Finalize Db service
    virtual StatusCode finalize();

    /// ConversionSvc overload: Create new Converter using factory
    virtual IConverter* createConverter(long typ,
					const CLID& wanted,
					const ICnvFactory* fac);

    /// ConversionSvc overload: Load the class (dictionary) for the converter
    virtual void loadConverter(DataObject* pObj);

    /** Connect the output file to the service with open mode.
     *  @param      outputFile  String containig output file
     *  @param      openMode    String containig opening mode of the output file
     *
     *  @return     Status code indicating success or failure.
     */
    virtual StatusCode connectOutput(const std::string& outputFile,
				     const std::string& openMode);

    /** Connect the output file to the service.
     *  @param      outputFile  String containig output file
     *
     *  @return     Status code indicating success or failure.
     */
    virtual StatusCode connectOutput(const std::string& outputFile);

    /** Commit pending output.
     *  @param      outputFile  String containig output file
     *  @param      do_commit   if true commit the output and flush
     *                          eventually pending items to the database
     *                          if false, discard pending buffers.
     *                          Note: The possibility to commit or rollback
     *                          depends on the database technology used!
     *
     *  @return     Status code indicating success or failure.
     */
    virtual StatusCode commitOutput(const std::string& outputFile,
				    bool               do_commit);

    /** Disconnect from an existing data stream.
     * @param      dbName      String containing name of the database
     *
     * @return     Status code indicating success or failure.
     */
    virtual StatusCode disconnect(const std::string& dbName);

    /** IAddressCreator implementation: Address creation.
     * Create an address using the link infotmation together with
     * the triple (database name/container name/object name).
     *
     * @param refLink           Reference to abstract link information
     * @param dbName            Database name
     * @param containerName     Object container name
     * @param refpAddress       Opaque address information to retrieve object
     *
     * @return                  StatusCode indicating SUCCESS or failure
     */
    virtual StatusCode createAddress( long                 svc_type,
				      const CLID&          clid,
				      const std::string*   par,
				      const unsigned long* ip,
				      IOpaqueAddress*&     refpAddress);

    /** IAddressCreator implementation: Creates an address in string form to object form
     *  @param      svc_type    Technology identifier encapsulated
     *                          in this address.
     *  @param      clid        Class identifier of the DataObject
     *                          represented by the opaque address
     *  @param      address     Input address.
     *  @param      refpAddress Output address in string form.
     *  @return     Status code indicating success or failure.
     */
    virtual StatusCode createAddress( long svc_type,
				      const CLID& clid,
				      const std::string& refAddress,
				      IOpaqueAddress*& refpAddress) {
      return this->ConversionSvc::createAddress(svc_type,clid,refAddress,refpAddress);
    }

    /** Insert null marker for not existent transient object
     *
     * @param    path     [IN]   Path to the (null-)object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createNullRep(const std::string& path);

    /** Insert null marker for not existent transient object
     *
     * @param    path     [IN]   Path to the (null-)object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode createNullRef(const std::string& path);

    /** Convert the transient object to the requested persistent representation.
     *
     * @param    pObj     [IN]   Pointer to data object
     * @param    refpAddr [OUT]  Location to store pointer to object address.
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode i__createRep(DataObject* pObj, IOpaqueAddress*& refpAddr);


    /** Resolve the references of the converted object.
     *
     * @param    pAddr    [IN]   Pointer to object address.
     * @param    pObj     [IN]   Pointer to data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode i__fillRepRefs(IOpaqueAddress* pAddr,DataObject* pObj);

    /** Create transient object from persistent data
     *
     * @param    pAddr       [IN]   Pointer to object address.
     * @param    refpObj     [OUT]  Location to pointer to store data object
     *
     * @return Status code indicating success or failure.
     */
    virtual StatusCode i__createObj(IOpaqueAddress* pAddr, DataObject*& refpObj);

  /** Resolve the references of the created transient object.
    *
    * @param    pAddr    [IN]   Pointer to object address.
    * @param    pObj     [IN]   Pointer to data object
    *
    * @return Status code indicating success or failure.
    */
    virtual StatusCode i__fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj);
  };
}

#endif  // GAUDIROOTCNV_GAUDIROOTCNVSVC_H
