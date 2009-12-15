// $Id: RootTreeCnvSvc.h,v 1.1 2009-12-15 15:37:25 frankb Exp $
//====================================================================
//	RootTreeCnvSvc definition
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIROOT_GAUDIROOTCNVSVC_H
#define GAUDIROOT_GAUDIROOTCNVSVC_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootTreeCnvSvc.h,v 1.1 2009-12-15 15:37:25 frankb Exp $

// Framework include files
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ClassID.h"

// Forward declarations
class IDataManagerSvc;
class IIncidentSvc;
class TClass;

namespace Gaudi {
  class IIODataManagerSvc;
  class IDataConnection;
  class RootDataConnection;
}

namespace Gaudi {
  /** @class RootTreeCnvSvc RootTreeCnvSvc.h src/RootTreeCnvSvc.h
   *
   * Description:
   * 
   * RootTreeCnvSvc class implementation definition.
   *
   *  @author  Markus Frank
   *  @version 1.0
   *  @date    20/12/2009
   */
  class RootTreeCnvSvc : public ConversionSvc  {
  protected:
    typedef std::vector<DataObject*> Objects;
    /// Services needed for proper operation: Data Manager
    IDataManagerSvc*            m_dataMgr;
    /// Reference to the I/O data manager
    Gaudi::IIODataManager*      m_ioMgr;
    /// Reference to incident service
    IIncidentSvc*               m_incidentSvc;
    /// On writing: reference to active output stream
    Gaudi::RootDataConnection*  m_current;
    /// Flag to enable incidents on FILE_OPEN
    bool                        m_incidentEnabled;
    /// Share files ? If set to YES, files will not be closed on finalize
    std::string                 m_shareFiles;
    /// Objects to be saved to data file
    Objects                     m_objects;

    TClass*                     m_classRefs;
    TClass*                     m_classDO;
  public:
    /// Standard constructor
    RootTreeCnvSvc(const std::string& name, ISvcLocator* svc);

    /// Standard destructor
    virtual ~RootTreeCnvSvc() {}

    /// Update state of the service
    virtual StatusCode updateServiceState(IOpaqueAddress* /* pAddress */) 
    { return StatusCode::SUCCESS;                  }

    /** Standard way to print errors. after the printout an exception is thrown.
     * @param      msg      [IN]     Message string to be printed.
     *
     * @return     Status code returning failure.
     */
    StatusCode error(const std::string& msg);

    /** Connect to data provider service. Re-connects to data manager service.
     *  @param      pDataSvc    New data provider object.
     *  @return     Status code indicating success or failure.
     */
    StatusCode setDataProvider(IDataProviderSvc* pDataSvc);

    /// Connect the output file to the service with open mode.
    StatusCode connectDatabase(const std::string& dataset, int mode, RootDataConnection** con);

  public:
    /// ConversionSvc overload: initialize Db service
    virtual StatusCode initialize();

    /// ConversionSvc overload: Finalize Db service
    virtual StatusCode finalize();

    /// ConversionSvc overload: Create new Converter using factory
    virtual IConverter* createConverter(long typ, 
					const CLID& wanted, 
					const ICnvFactory* fac);

    /// ConversionSvc overload: Load the class (dictionary) for the converter 
    virtual void loadConverter(DataObject*);

    /** Connect the output file to the service with open mode.
     *  @param      outputFile  String containig output file
     *  @param      openMode    String containig opening mode of the output file
     *  @return     Status code indicating success or failure.
     */
    virtual StatusCode connectOutput(const std::string& outputFile,
				     const std::string& openMode);

    /** Connect the output file to the service.
     *  @param      outputFile  String containig output file
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
     * @param refLink        Reference to abstract link information
     * @param dbName         Database name
     * @param containerName  Object container name
     * @param refpAddress    Opaque address information to retrieve object
     * @return               StatusCode indicating SUCCESS or failure
     */
    virtual StatusCode createAddress( long                 svc_type,
				      const CLID&          clid,
				      const std::string*   par, 
				      const unsigned long* ip,
				      IOpaqueAddress*&     refpAddress);
  
    /// Mark an object for write given an object reference
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& refpAddr);

    /// Resolve the references of the converted object.
    virtual StatusCode fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject);

    /// Read existing object. Open transaction in read mode if not active
    virtual StatusCode createObj(IOpaqueAddress* pA, DataObject*& refpObj);

    /// Resolve the references of the created transient object.
    virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  };
}

#endif  // GAUDIROOT_GAUDIROOTCNVSVC_H
