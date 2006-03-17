// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawDataCnvSvc.h,v 1.2 2006-03-17 17:23:56 frankb Exp $
//	====================================================================
//  RawDataCnvSvc.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_RAWDATACNVSVC_H
#define MDF_RAWDATACNVSVC_H

#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/StreamBuffer.h"
#include <map>

// Forward declarations
class IDataManagerSvc;

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {  

  // Forward declarations
  class RawDataAddress;
  class RawEvent;
  class RawBank;

  /** @class RawDataCnvSvc RawDataCnvSvc.h  MDF/RawDataCnvSvc.h
    *
    * Conversion service for the online data.
    * 
    * @author  M.Frank
    * @version 1.0
    * @date    01/01/2005
    */
  class RawDataCnvSvc : public ConversionSvc  {
  protected:
    typedef  std::map<std::string, void*>  FileMap;
    FileMap::const_iterator  m_current;
    FileMap       m_fileMap;
    bool          m_wrFlag;
    /// Compression algorithm identifier
    int           m_compress;
    /// Flag to create checksum
    int           m_genChecksum;
    /// Streambuffer to hold uncompressed data
    StreamBuffer  m_data;
    /// Streambuffer to hold compressed data
    StreamBuffer  m_tmp;

    /// Services needed for proper operation: Data Manager
    IDataManagerSvc* m_dataMgr;

    /// Helper to print errors and return bad status
    StatusCode error(const std::string& msg)  const;

    /// Open MDF file
    virtual void* openIO(const std::string& fname, const std::string&  mode) const;

    /// Close MDF file
    virtual StatusCode closeIO(void* ioDesc) const;

    /// Commit output to buffer manager
    virtual StatusCode commitDescriptors(void* ioDesc);

    /// Commit Raw Bank output to stream/buffer
    virtual StatusCode commitRawBanks(void* ioDesc);

    /// Read raw banks
    virtual StatusCode readRawBanks(RawDataAddress* pAddr, RawEvent* evt);

    /// Allocate data space for output
    virtual char* const getDataSpace(void* ioDesc, size_t len);

    /// Declare event to data space
    virtual StatusCode writeDataSpace(void* ioDesc, 
                                      size_t len, 
                                      long long trNumber, 
                                      unsigned int trMask[4],
                                      int evType, 
                                      int hdrType);

    /// Write data block to stream
    virtual StatusCode streamWrite(void* iodesc, void* ptr, size_t len);

  public:
    /// Initializing constructor
    /**  @param   nam         [IN]  Name of the service
      *  @param   typ         [IN]  Storage type if different from MDF_StorageType
      *  @param   loc         [IN]  Pointer to the service locator object
      *  @return Initialized reference to service object
      */
    RawDataCnvSvc(const std::string& nam, ISvcLocator* loc, long typ);

    /// Initializing constructor
    /**  @param   nam         [IN]  Name of the service
      *  @param   loc         [IN]  Pointer to the service locator object
      *  @return Initialized reference to service object
      */
    RawDataCnvSvc(const std::string& nam, ISvcLocator* loc);

    /// Standard destructor      
    virtual ~RawDataCnvSvc()  {}

    /// Service initialization
    virtual StatusCode RawDataCnvSvc::initialize();

    /// Service finalization
    virtual StatusCode finalize();

    /// Concrete class type
    virtual const CLID& objType() const;

    /// Object creation callback
    /**  @param   pAddr      [IN]  Reference to opaque object address
      *  @param   refpObj   [OUT]  Reference to store pointer to created object
      *
      *  @return Status code indicating success or failure
      */
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& refpObj);

    /// Callback for reference processing (misused to attach leaves)
    /**  @param   pAddr      [IN]  Reference to opaque object address
      *  @param   refpObj   [OUT]  Reference to store pointer to created object
      *
      *  @return Status code indicating success or failure
      */
    virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj);

    /// Connect the output file to the service with open mode.
    virtual StatusCode connectOutput(const std::string& name, const std::string& mode);

    /// Connect the input file to the service with READ mode
    virtual StatusCode connectInput(const std::string& fname, void*& iodesc);

    /// Commit pending output.
    virtual StatusCode commitOutput(const std::string& , bool doCommit);

    /// Convert the transient object to the requested representation.
    virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);

    /// Resolve the references of the converted object. 
    virtual StatusCode fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject);

    /// Create a Generic address using explicit arguments to identify a single object.
    virtual StatusCode createAddress(long typ, const CLID& clid, const std::string* par, 
                            const unsigned long* ip, IOpaqueAddress*& refpAddress);
  };
}      // End namespace LHCb
#endif // MDF_RAWDATACNVSVC_H
