// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawDataCnvSvc.cpp,v 1.9 2006-06-26 08:37:18 frankb Exp $
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
: ConversionSvc(nam, loc, typ), MDFIO(MDFIO::MDF_RECORDS, nam)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",         m_compress=2);        // File compression
  declareProperty("CreateChecksum",   m_genChecksum=1);     // Generate checksum
}

// Initializing constructor
LHCb::RawDataCnvSvc::RawDataCnvSvc(const std::string& nam, ISvcLocator* loc) 
: ConversionSvc(nam, loc, RAWDATA_StorageType), 
  MDFIO(MDFIO::MDF_RECORDS, nam)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",         m_compress=2);        // File compression
  declareProperty("ChecksumType",     m_genChecksum=1);     // Generate checksum
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
        const std::string& id = pReg->identifier();
        if ( id == "/Event" )  {
          SmartIF<IDataManagerSvc> mgr(dataProvider());
          if ( !mgr.isValid() )  {
            return error("Failed to locate Manager interface in dataprovider!");
          }
          RawDataAddress* paddr = new RawDataAddress(*pA);
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
      setupMDFIO(msgSvc(),dataProvider());
      if ( typ == RAWDATA_StorageType )  {
        StatusCode sc = commitRawBanks(m_compress,m_genChecksum,(*m_current).second);
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

/// Read raw byte buffer from input stream
StatusCode LHCb::RawDataCnvSvc::readBuffer(void* const ioDesc, void* const data, size_t len)  {
  MDFMapEntry* ent = (MDFMapEntry*)ioDesc;
  if ( ent && ent->con.ioDesc > 0 ) {
    if ( StreamDescriptor::read(ent->con,data,len) )  {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

/// Read raw banks
StatusCode 
LHCb::RawDataCnvSvc::readRawBanks(RawDataAddress* pAddr, RawEvent* evt)
{
  void* iodesc = 0;
  const std::string* par = pAddr->par();
  StatusCode sc = connectInput(par[0], iodesc);
  if ( sc.isSuccess() )  {
    long long offset = pAddr->fileOffset();
    MDFMapEntry* ent = (MDFMapEntry*)iodesc;
    if ( ent->con.ioDesc > 0 )  {
      if ( StreamDescriptor::seek(ent->con, offset, SEEK_SET) != -1 )  {
        setupMDFIO(msgSvc(),dataProvider());
        std::pair<char*,int> result = readBanks(&ent);
        if ( result.first > 0 )  {
          sc = decodeRawBanks(result.first, result.first+result.second, evt);
          if ( sc.isSuccess() )  {
            return sc;
          }
          return error("Failed to decode raw data input from:"+par[0]);
        }
        return error("Failed read raw data input from:"+par[0]);
      }
      return error("Cannot seek data record: [Invalid I/O operation]");
    }
    return error("Cannot read data record: [Invalid I/O descriptor]");
  }
  return error("Cannot read data record: [Failed to open file: "+par[0]+"]");
}

/// Write data block to stream
StatusCode LHCb::RawDataCnvSvc::writeBuffer(void* iodesc, const void* data, size_t len)   {
  MDFMapEntry* ent = (MDFMapEntry*)iodesc;
  if ( ent && ent->con.ioDesc > 0 )  {
    if ( StreamDescriptor::write(ent->con, data, len) )  {
      return StatusCode::SUCCESS;
    }
    return error("Cannot write data record: [Invalid I/O operation]");
  }
  return error("Cannot write data record: [Invalid I/O descriptor]");
}
