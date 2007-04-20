// $Id: RawDataCnvSvc.h,v 1.7 2007-04-20 12:40:25 cattanem Exp $
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
#include "MDF/MDFIO.h"
#include <map>

// Forward declarations
class IDataManagerSvc;
class IRegistry;

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
  class RawDataCnvSvc : public ConversionSvc, public MDFIO  {
  protected:
    typedef const std::string&            CSTR;
    typedef std::vector<RawBank*>         Banks;
    typedef std::map<std::string, void*>  FileMap;
    FileMap::iterator m_current;
    FileMap           m_fileMap;
    bool              m_wrFlag;
    /// Compression algorithm identifier
    int               m_compress;
    /// Flag to create checksum
    int               m_genChecksum;
    /// Streambuffer to hold uncompressed data
    StreamBuffer      m_data;
    /// Reference to data manager interface
    IDataManagerSvc*  m_dataMgr;
    int               m_evtsBefore, m_evtsAfter;

    /// Helper to print errors and return bad status
    StatusCode error(CSTR msg)  const;

    /// Open MDF file
    virtual void* openIO(CSTR fname, CSTR  mode) const;

    /// Close MDF file
    virtual StatusCode closeIO(void* ioDesc) const;

    /// Commit output to buffer manager
    virtual StatusCode commitDescriptors(void* ioDesc);

    /// Read raw banks
    virtual StatusCode readRawBanks(RawDataAddress* pAddr,std::pair<char*,int>& data);

    /// Allocate data space for output
    virtual std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_data.reserve(len);
      return std::pair<char*,int>(m_data.data(), m_data.size());
    }

    /// Write data block to stream
    virtual StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len);

    /// Read raw byte buffer from input stream
    virtual StatusCode readBuffer(void* const ioDesc, void* const data, size_t len);

    /// Helper to install opaque address leaf
    StatusCode regAddr(IRegistry* pReg,IOpaqueAddress* pA,CSTR path,const CLID& clid);

    /// Pass raw banks to RawEvent identified by its path
    StatusCode adoptRawBanks(CSTR path, const Banks& banks);

  public:
    /** Initializing constructor
      *  @param[in]   nam   Name of the service
      *  @param[in]   typ   Storage type if different from MDF_StorageType
      *  @param[in]   loc   Pointer to the service locator object
      *  @return Initialized reference to service object
      */
    RawDataCnvSvc(CSTR nam, ISvcLocator* loc, long typ);

    /** Initializing constructor
      *  @param[in]   nam   Name of the service
      *  @param[in]   loc   Pointer to the service locator object
      *  @return Initialized reference to service object
      */
    RawDataCnvSvc(CSTR nam, ISvcLocator* loc);

    /// Standard destructor      
    virtual ~RawDataCnvSvc()  {}

    /// Service initialization
    virtual StatusCode initialize();

    /// Service finalization
    virtual StatusCode finalize();

    /// Concrete class type
    virtual const CLID& objType() const;

    /** Object creation callback
      *  @param[in]   pAddr     Reference to opaque object address
      *  @param[out]  refpObj   Reference to store pointer to created object
      *
      *  @return Status code indicating success or failure
      */
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& refpObj);

    /** Callback for reference processing (misused to attach leaves)
      *  @param[in]   pAddr     Reference to opaque object address
      *  @param[out]  refpObj   Reference to store pointer to created object
      *
      *  @return Status code indicating success or failure
      */
    virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj);

    /// Connect the output file to the service with open mode.
    virtual StatusCode connectOutput(CSTR name, CSTR mode);

    /// Connect the input file to the service with READ mode
    virtual StatusCode connectInput(CSTR fname, void*& iodesc);

    /// Commit pending output.
    virtual StatusCode commitOutput(CSTR , bool doCommit);

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
