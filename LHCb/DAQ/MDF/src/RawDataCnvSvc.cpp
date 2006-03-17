// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawDataCnvSvc.cpp,v 1.6 2006-03-17 19:37:53 frankb Exp $
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
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"
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
    StreamDescriptor           desc;
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
  MsgStream log(msgSvc(),name());
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
StatusCode LHCb::RawDataCnvSvc::createObj(IOpaqueAddress* pAddr, DataObject*& refpObj)  
{
  if ( pAddr )  {
    IRegistry* pReg = pAddr->registry();
    if ( pReg )  {
      const std::string& id = pReg->identifier();
      if ( id == "/Event" )  {
        refpObj = new DataObject();
        return StatusCode::SUCCESS;
      }
      else if ( id == "/Event/DAQ" )  {
        refpObj = new DataObject();
        return StatusCode::SUCCESS;
      }
      else if ( id.find(Default) != std::string::npos )  {
        refpObj = new RawEvent();
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Callback for reference processing (misused to attach leaves)
StatusCode LHCb::RawDataCnvSvc::fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObj )
{
  RawDataAddress* pA = dynamic_cast<RawDataAddress*>(pAddress);
  if ( pA && pObj )  {
    try {
      IRegistry* pReg = pA->registry();
      if ( pReg )  {
        RawDataAddress* paddr = 0;
        const std::string& id = pReg->identifier();
        SmartIF<IDataManagerSvc> mgr(dataProvider());
        if ( !mgr.isValid() )  {
          return error("Failed to locate Manager interface in dataprovider!");
        }
        if ( id == "/Event" )  {
          paddr = new RawDataAddress(*pA);
          paddr->setClID(DataObject::classID());
          StatusCode sc = mgr->registerAddress(pReg, "/DAQ", paddr);
          if ( !sc.isSuccess() )  {
            paddr->release();
            return error("Failed to register address for object /Event/DAQ");
          }
          paddr = new RawDataAddress(*pA);
          paddr->setClID(RawEvent::classID());
          if ( Default.find(id) == 0 )
            sc = mgr->registerAddress(Default, paddr);
          else
            sc = mgr->registerAddress(pReg, Default, paddr);
          if ( !sc.isSuccess() )  {
            paddr->release();
            return error("Failed to register address for object "+Default);
          }
          return sc;
        }
        else if ( id == "/Event/DAQ" )  {
          return StatusCode::SUCCESS;
        }
        else if ( id.find(Default) != std::string::npos )  {
          typedef std::vector<RawBank*> _B;
          RawEvent* raw = dynamic_cast<RawEvent*>(pObj);
          const _B* banks = pA->banks();
          if ( banks )  {
            // MDF file/MBM input was opened by the event selector: banks already filled...
            for(_B::const_iterator i=banks->begin(); i!=banks->end(); ++i)  {
              raw->adoptBank(*i, false);
            }
            return StatusCode::SUCCESS;
          }
          /// Need to open MDF file to get access!
          if ( readRawBanks(pA, raw).isSuccess() )  {
            return StatusCode::SUCCESS;
          }
          return error("Failed to open raw data input:"+pA->par()[0]);
        }
      }
    }
    catch (std::exception& e) {
      return error(std::string("Exception:") + e.what());
    }
    catch(...) {
      return error("Exception: (Unknown)");
    }
  }
  return error("No valid event object/address present.");
}

/// Connect the output file to the service with open mode.
StatusCode 
LHCb::RawDataCnvSvc::connectOutput(const std::string&    outputFile,
                                   const std::string&    openMode) 
{
  m_wrFlag = false;
  m_current = m_fileMap.find(outputFile);
  if ( m_current == m_fileMap.end() )   {
    void* entry = openIO(outputFile, openMode);
    if ( entry ) {
      m_fileMap.insert(std::make_pair(outputFile,entry));
      m_current = m_fileMap.find(outputFile);
      return StatusCode::SUCCESS;
    }
    return error("connectOutput> FAILED to bind I/O channel:"+outputFile);
  }
  return StatusCode::SUCCESS;
}

/// Connect the input file to the service with READ mode
StatusCode LHCb::RawDataCnvSvc::connectInput(const std::string& fname, void*& iodesc)   {
  FileMap::const_iterator it = m_fileMap.find(fname);
  if ( it == m_fileMap.end() )   {
    iodesc = openIO(fname, "READ");
    if ( iodesc ) {
      m_fileMap.insert(std::make_pair(fname,iodesc));
      return StatusCode::SUCCESS;
    }
    return error("connectInput> FAILED to bind I/O channel:"+fname);
  }
  iodesc = (*it).second;
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
StatusCode 
LHCb::RawDataCnvSvc::fillRepRefs(IOpaqueAddress* /* pAddr */, DataObject* /* pObj */) 
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
  refpAddress = new RawDataAddress(typ, clid, par[0], par[1], ip[0], ip[1]);
  return StatusCode::SUCCESS;
}

void* LHCb::RawDataCnvSvc::openIO(const std::string& fname, const std::string& mode) const    {
  MDFMapEntry* ent = new MDFMapEntry;
  if ( strncasecmp(mode.c_str(),"N",1)==0 || strncasecmp(mode.c_str(),"REC",3)==0 )  {
    ent->con = StreamDescriptor::connect(fname);
    if ( ent->con.ioDesc > 0 )  {
      return ent;
    }
  }
  else if ( strncasecmp(mode.c_str(),"O",1)==0 || strncasecmp(mode.c_str(),"REA",3)==0 )  {
    ent->bind = StreamDescriptor::bind(fname);
    if ( ent->bind.ioDesc > 0 )  {
      ent->con = StreamDescriptor::accept(ent->bind);
      if ( ent->con.ioDesc > 0 )  {
        return ent;
      }
    }
  }
  error("Unknown openmode "+mode+" for MDF file :"+fname);
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

/// Read raw banks
StatusCode 
LHCb::RawDataCnvSvc::readRawBanks(RawDataAddress* pAddr, RawEvent* evt)
{
  void* iodesc = 0;
  const std::string* par = pAddr->par();
  const unsigned long* ipar = pAddr->ipar();
  StatusCode sc = connectInput(par[0], iodesc);
  if ( sc.isSuccess() )  {
    long long offset = pAddr->fileOffset();
    MDFMapEntry* ent = (MDFMapEntry*)iodesc;
    if ( ent->con.ioDesc > 0 )  {
      if ( StreamDescriptor::seek(ent->con, offset, SEEK_SET) != -1 )  {
        if ( readMDFrecord(ent->desc, ent->con).isSuccess() )  {
          MDFHeader* h = (MDFHeader*)ent->desc.data();
          char* ptr = ent->desc.data()+sizeof(MDFHeader);
          sc = decodeRawBanks(ptr,ptr+h->size(),evt);
          if ( sc.isSuccess() )  {
            pAddr->setSize(h->size());
            pAddr->setEventType(h->eventType());
            pAddr->setTriggerMask(h->triggerMask());
            return sc;
          }
          return error("Failed to decode raw data input from:"+par[0]);
        }
        return error("Cannot read data record: [I/O read error]");
      }
      return error("Cannot seek data record: [Invalid I/O operation]");
    }
    return error("Cannot read data record: [Invalid I/O descriptor]");
  }
  return error("Cannot read data record: [Failed to open file: "+par[0]+"]");
}

/// Write data block to stream
StatusCode LHCb::RawDataCnvSvc::streamWrite(void* iodesc, void* ptr, size_t len)   {
  MDFMapEntry* ent = (MDFMapEntry*)iodesc;
  if ( ent && ent->con.ioDesc > 0 )  {
    if ( StreamDescriptor::write(ent->con, ptr, len) )  {
      return StatusCode::SUCCESS;
    }
    return error("Cannot write data record: [Invalid I/O operation]");
  }
  return error("Cannot write data record: [Invalid I/O descriptor]");
}

StatusCode LHCb::RawDataCnvSvc::writeDataSpace(void* ioDesc,
                                               size_t len, 
                                               long long trNumber, 
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
      int chk = m_genChecksum ? genChecksum(1,m_tmp.data()+sizeof(MDFHeader),newlen) : 0;
      makeMDFHeader(m_tmp.data(),newlen,evType,hdrType,trNumber,trMask,m_compress,chk);
      return streamWrite(ioDesc,m_tmp.data(),newlen+sizeof(MDFHeader));
    }
    // Bad compression; file uncompressed buffer
  }
  int chk = m_genChecksum ? genChecksum(1,m_data.data()+sizeof(MDFHeader),len) : 0;
  makeMDFHeader(m_data.data(),len,evType,hdrType,trNumber,trMask,0,chk);
  return streamWrite(ioDesc,m_data.data(),len+sizeof(MDFHeader));
}

/// Commit output to MDF stream
StatusCode LHCb::RawDataCnvSvc::commitRawBanks(void* ioDesc)  {
  typedef std::vector<RawBank*> _BankV;
  SmartDataPtr<RawEvent> raw(dataProvider(),Default);
  if ( raw )  {
    unsigned int trMask[4] = { 0x103, 0, 0, 0 };
    int evType  = 1;
    int hdrType = 0;
    long long trNumber = 0;
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

