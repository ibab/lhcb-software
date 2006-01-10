// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawDataCnvSvc.cpp,v 1.2 2006-01-10 09:43:16 frankb Exp $
//	====================================================================
//  RawDataCnvSvc.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/StorageTypes.h"
#include "MDF/RawDataCnvSvc.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFHeader.h"
#include "MDF/RawEventHelpers.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "Event/RawEvent.h"

#include <memory>

using LHCb::MDFHeader;
using LHCb::RawEvent;
using LHCb::RawBank;
using LHCb::RawEventLocation::Default;
using LHCb::StreamDescriptor;

namespace {
  struct MDFMapEntry  {
    StreamDescriptor::Access   bind;
    StreamDescriptor::Access   con;
  };
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,RawDataCnvSvc)

// Initializing constructor
LHCb::RawDataCnvSvc::RawDataCnvSvc(const std::string& nam, ISvcLocator* loc, long typ) 
: ConversionSvc(nam, loc, typ)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",         m_compress=0);        // File compression
  declareProperty("CreateChecksum",   m_genChecksum=0);     // Generate checksum
}

// Initializing constructor
LHCb::RawDataCnvSvc::RawDataCnvSvc(const std::string& nam, ISvcLocator* loc) 
: ConversionSvc(nam, loc, RAWDATA_StorageType)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",         m_compress=0);        // File compression
  declareProperty("CreateChecksum",   m_genChecksum=0);     // Generate checksum
}

/// Service initialization
StatusCode LHCb::RawDataCnvSvc::initialize()    
{
  StatusCode sc = ConversionSvc::initialize();
  if ( sc.isSuccess() )  {
    IPersistencySvc *pSvc = 0;
    sc = service("EventPersistencySvc",pSvc,true);
    if ( sc.isSuccess() )  {
      IConversionSvc *pCnv = 0;
      sc = pSvc->getService(repSvcType(), pCnv);
      if ( pCnv == this )  {
        /// All OK
      }
    }
  }
  return sc;
}

/// Service finalization
StatusCode LHCb::RawDataCnvSvc::finalize()    
{
  long typ = repSvcType();
  for(FileMap::iterator i=m_fileMap.begin(); i != m_fileMap.end(); ++i)  {
    if ( typ == RAWDATA_StorageType && (*i).second )  {
      closeIO((*i).second);
    }
  }
  m_fileMap.clear();
  return ConversionSvc::finalize();
}

// Helper to print errors and return bad status
StatusCode LHCb::RawDataCnvSvc::error(const std::string& msg)   const 
{
  MsgStream err(msgSvc(), name());
  err << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

/// Concrete class type
const CLID& LHCb::RawDataCnvSvc::objType() const  {
  return DataObject::classID();     
}

/// Object creation callback
StatusCode LHCb::RawDataCnvSvc::createObj(IOpaqueAddress* /* pAddr */, DataObject*& refpObj)  
{
  refpObj = new DataObject();
  return StatusCode::SUCCESS;
}

/// Callback for reference processing (misused to attach leaves)
StatusCode LHCb::RawDataCnvSvc::fillObjRefs(IOpaqueAddress* pAddr, DataObject* /* pObj */ )
{
  if ( pAddr )  {
    try {
      std::auto_ptr<DataObject> obj(new DataObject());
      std::auto_ptr<RawEvent> raw(new RawEvent());
      if ( dataProvider()->registerObject("/Event/DAQ", obj.get()).isSuccess() )  {
        typedef std::vector<RawBank*> _B;
        obj.release();
        long typ = repSvcType();
        const _B* banks = (const _B*)pAddr->ipar()[0];
        for(_B::const_iterator i=banks->begin(); i!=banks->end(); ++i)  {
          raw->adoptBank(*i, false);
        }
        StatusCode sc = dataProvider()->registerObject(Default,raw.get());
        if ( sc.isSuccess() )  {
          raw.release();
          return StatusCode::SUCCESS;
        }
        return error("Failed to register object "+Default);
      }
      return error("Failed to register object DAQ");
    }
    catch (std::exception& e) {
      return error(std::string("Exception:") + e.what());
    }
    catch(...) {
      return error("Exception: (Unknown)");
    }
  }
  return error("No valid top level event object present.");
}

/// Connect the output file to the service with open mode.
StatusCode 
LHCb::RawDataCnvSvc::connectOutput(const std::string&    outputFile,
                                   const std::string&    openMode) 
{
  m_wrFlag = false;
  m_current = m_fileMap.find(outputFile);
  if ( m_current == m_fileMap.end() )   {
    void* entry = 0;
    entry = openIO(outputFile, openMode);
    if ( entry ) {
      m_fileMap.insert(std::make_pair(outputFile,entry));
      m_current = m_fileMap.find(outputFile);
      return StatusCode::SUCCESS;
    }
    return error("connectOutput> FAILED to bind I/O channel:"+outputFile);
  }
  return StatusCode::SUCCESS;
}

/// Commit pending output.
StatusCode LHCb::RawDataCnvSvc::commitOutput(const std::string& , bool doCommit ) 
{
  if ( doCommit && m_wrFlag )  {
    if ( m_current != m_fileMap.end() )   {
      long typ = repSvcType();
      if ( typ == RAWDATA_StorageType )  {
        StatusCode sc = commitRawBanks((*m_current).second);
        m_current = m_fileMap.end();
        return sc;
      }
      else if ( typ == RAWDESC_StorageType )    {
        StatusCode sc = commitDescriptors((*m_current).second);
        m_current = m_fileMap.end();
        return sc;
      }
      return error("commitOutput> Unknown channel type.");
    }
    return error("commitOutput> No valid output channel known.");
  }
  return StatusCode::SUCCESS;
}

/// Commit output to buffer manager
StatusCode LHCb::RawDataCnvSvc::commitDescriptors(void* /* ioDesc */)  {
  return error("Event descriptor output is not implemented for this class!");
}

/// Convert the transient object to the requested representation.
StatusCode LHCb::RawDataCnvSvc::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress) 
{
  if ( pObject )  {
    if ( m_current != m_fileMap.end() )   {
      IRegistry* reg = pObject->registry();
      std::string spar[]   = {(*m_current).first,reg->identifier()};
      unsigned long ipar[] = {0,0};
      return createAddress(repSvcType(),pObject->clID(),spar,ipar,refpAddress);
    }
    return error("createRep> Cannot write object: No output file is connected!");
  }
  return error("createRep> Cannot write object: Object pointer is NULL!");
}

/// Resolve the references of the converted object. 
StatusCode LHCb::RawDataCnvSvc::fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject) 
{
  if ( m_current != m_fileMap.end() )   {
    m_wrFlag = true;
    return StatusCode::SUCCESS;
  }
  return error("fillRepRefs> Cannot write object: No output file is connected!");
}

/// Create a Generic address using explicit arguments to identify a single object.
StatusCode LHCb::RawDataCnvSvc::createAddress(long typ, 
                                              const CLID& clid, 
                                              const std::string* par, 
                                              const unsigned long* ip,
                                              IOpaqueAddress*& refpAddress)    
{
  refpAddress = new GenericAddress(typ, clid, par[0], par[1], ip[0], ip[1]);
  return StatusCode::SUCCESS;
}

void* LHCb::RawDataCnvSvc::openIO(const std::string& outputFile, const std::string& mode) const    {
  MDFMapEntry* ent = new MDFMapEntry;
  if ( strncasecmp(mode.c_str(),"N",1)==0 || strncasecmp(mode.c_str(),"REC",3)==0 )  {
    ent->con = StreamDescriptor::connect(outputFile);
    if ( ent->con.ioDesc > 0 )  {
      return ent;
    }
  }
  else if ( strncasecmp(mode.c_str(),"O",1)==0 || strncasecmp(mode.c_str(),"REA",3)==0 )  {
    ent->bind = StreamDescriptor::bind(outputFile);
    if ( ent->bind.ioDesc > 0 )  {
      ent->con = StreamDescriptor::accept(ent->bind);
      if ( ent->con.ioDesc > 0 )  {
        return ent;
      }
    }
  }
  error("Unknown openmode "+mode+" for MDF file :"+outputFile);
  delete ent;
  return 0;
}

StatusCode LHCb::RawDataCnvSvc::closeIO(void* ioDesc)  const {
  MDFMapEntry* ent = (MDFMapEntry*)ioDesc;
  if ( ent )  {
    if ( ent->bind.ioDesc > 0 )  {
      StreamDescriptor::close(ent->bind);
    }
    if ( ent->con.ioDesc > 0 )  {
      StreamDescriptor::close(ent->con);
    }
    delete ent;
  }
  return StatusCode::SUCCESS;
}

char* const LHCb::RawDataCnvSvc::getDataSpace(void* /* ioDesc */, size_t len)  {
  m_data.reserve(len + sizeof(MDFHeader));
  return m_data.data() + sizeof(MDFHeader);
}

/// Write data block to stream
StatusCode LHCb::RawDataCnvSvc::streamWrite(void* iodesc, void* ptr, size_t len)   {
  MDFMapEntry* ent = (MDFMapEntry*)iodesc;
  if ( ent && ent->con.ioDesc > 0 )  {
    if ( StreamDescriptor::write(ent->con, ptr, len+sizeof(MDFHeader)) )  {
      return StatusCode::SUCCESS;
    }
    return error("Cannot write MDF record: [Invalid I/O operation]");
  }
  return error("Cannot write MDF record: [Invalid I/O descriptor]");
}

StatusCode LHCb::RawDataCnvSvc::writeDataSpace(void* ioDesc,
                                               size_t len, 
                                               longlong trNumber, 
                                               unsigned int trMask[4],
                                               int evType, 
                                               int hdrType)
{
  if ( m_compress )   {
    size_t newlen = len;
    m_tmp.reserve(len+sizeof(MDFHeader));
    if ( compressBuffer(m_compress,
                        m_tmp.data()+sizeof(MDFHeader),len,
                        m_data.data()+sizeof(MDFHeader),len, newlen).isSuccess() )  {
      int chk = m_genChecksum ? xorChecksum(m_tmp.data()+sizeof(MDFHeader),newlen) : 0;
      makeMDFHeader(m_tmp.data(),newlen,evType,hdrType,trNumber,trMask,m_compress,chk);
      return streamWrite(ioDesc,m_tmp.data(),newlen+sizeof(MDFHeader));
    }
    // Bad compression; file uncompressed buffer
  }
  int chk = m_genChecksum ? xorChecksum(m_data.data()+sizeof(MDFHeader),len) : 0;
  makeMDFHeader(m_data.data(),len,evType,hdrType,trNumber,trMask,m_compress,chk);
  return streamWrite(ioDesc,m_data.data(),len+sizeof(MDFHeader));
}

/// Commit output to MDF stream
StatusCode LHCb::RawDataCnvSvc::commitRawBanks(void* ioDesc)  {
  typedef std::vector<RawBank*> _BankV;
  SmartDataPtr<RawEvent> raw(dataProvider(),Default);
  if ( raw )  {
    unsigned int trMask[4] = { 0x103, 0, 0, 0 };
    int evType = 1;
    int hdrType = 0;
    longlong trNumber = 0;
    size_t len = rawEventLength(raw);
    char* data = getDataSpace(ioDesc, len);
    if ( data )  {
      if ( encodeRawBanks(raw, data, len).isSuccess() )   {
        return writeDataSpace(ioDesc, len, trNumber, trMask, evType, hdrType);
      }
      return error("Failed to copy raw event object:"+Default);
    }
    return error("Failed to allocate dump buffer for raw event object:"+Default);
  }
  return error("Failed to retrieve raw event object:"+Default);
}

