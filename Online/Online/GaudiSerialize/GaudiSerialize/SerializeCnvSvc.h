// $Id: SerializeCnvSvc.h,v 1.1 2008-06-17 13:23:14 frankb Exp $
//====================================================================
//	SerializeCnvSvc definition
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
#ifndef SERIALIZE_SERIALIZECNVSVC_H
#define SERIALIZE_SERIALIZECNVSVC_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiSerialize/GaudiSerialize/SerializeCnvSvc.h,v 1.1 2008-06-17 13:23:14 frankb Exp $

// Framework include files
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ClassID.h"

// Bit of a hack, but this storage type is (again) free
#define SERIALIZE_StorageType ROOT_StorageType

// Forward declarations
class IDataManagerSvc;

static const InterfaceID IID_ISerializeMgr("ISerializeCnvSvc", 1 , 0); 

// Forward declarations
class IDataManagerSvc;

/** @class SerializeCnvSvc SerializeCnvSvc.h GaudiSerialize/SerializeCnvSvc.h
  *
  * Description:
  * 
  * SerializeCnvSvc class implementation definition.
  *
  * @author Markus Frank
  * @version 1.0
  */
class SerializeCnvSvc : public ConversionSvc  {
 protected:
  typedef std::vector<DataObject*> Objects;
  IDataManagerSvc* m_dataMgr;
  Objects          m_objects;
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISerializeMgr; }

  /// Standard constructor
  SerializeCnvSvc(const std::string& name, ISvcLocator* svc);

  /// Standard destructor
  virtual ~SerializeCnvSvc();

  /// Update state of the service
  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);

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

public:

  /// ConversionSvc overload: Query interface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

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
  virtual StatusCode writeObject(DataObject* pObj, IOpaqueAddress*& refpAddr);

  /// Read existing object. Open transaction in read mode if not active
  virtual StatusCode readObject(IOpaqueAddress* pA, DataObject*& refpObj);

};
#endif  // SERIALIZE_SERIALIZECNVSVC_H
