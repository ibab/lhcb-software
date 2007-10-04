// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawDataCnvSvc.cpp,v 1.15 2007-10-04 13:57:07 frankb Exp $
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
#include <vector>
#include <map>

using LHCb::MDFHeader;
using LHCb::RawEvent;
using LHCb::RawBank;
using LHCb::StreamDescriptor;

namespace {
  struct MDFMapEntry  {
    std::string                name;
    StreamDescriptor::Access   bind;
    StreamDescriptor::Access   con;
    StreamDescriptor           desc;
  };
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,RawDataCnvSvc)

// Initializing constructor
LHCb::RawDataCnvSvc::RawDataCnvSvc(CSTR nam, ISvcLocator* loc, long typ) 
: ConversionSvc(nam, loc, typ), MDFIO(MDFIO::MDF_RECORDS, nam), m_dataMgr(0)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",       m_compress=2);     // File compression
  declareProperty("CreateChecksum", m_genChecksum=1);  // Generate checksum
  declareProperty("EventsBefore",   m_evtsBefore=0);   // Events before T0
  declareProperty("EventsAfter",    m_evtsAfter=0);    // Events after T0
  declareProperty("DataType",       m_dataType=MDFIO::MDF_RECORDS);  // Input data type
	declareProperty("BankLocation",		m_bankLocation=LHCb::RawEventLocation::Default);  // Location of the banks in the TES
}

// Initializing constructor
LHCb::RawDataCnvSvc::RawDataCnvSvc(CSTR nam, ISvcLocator* loc) 
: ConversionSvc(nam, loc, RAWDATA_StorageType), 
  MDFIO(MDFIO::MDF_RECORDS, nam), m_dataMgr(0)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",       m_compress=2);     // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);  // Generate checksum
  declareProperty("EventsBefore",   m_evtsBefore=0);   // Events before T0
  declareProperty("EventsAfter",    m_evtsAfter=0);    // Events after T0
  declareProperty("DataType",       m_dataType=MDFIO::MDF_RECORDS);     // Input data type
	declareProperty("BankLocation",		m_bankLocation=LHCb::RawEventLocation::Default);  // Location of the banks in the TES
}

/// Service initialization
StatusCode LHCb::RawDataCnvSvc::initialize()     {
  StatusCode sc = ConversionSvc::initialize();
  MsgStream log(msgSvc(),name());
  m_dataMgr = 0;
  if ( sc.isSuccess() )  {
    IPersistencySvc *pSvc = 0;
    sc = service("EventPersistencySvc",pSvc,true);
    if ( sc.isSuccess() )  {
      IConversionSvc *pCnv = 0;
      sc = pSvc->getService(repSvcType(), pCnv);
      if ( pCnv == this )  {
        sc = dataProvider()->queryInterface(IID_IDataManagerSvc,(void**)&m_dataMgr);
        if ( sc.isSuccess() )  {
          /// All OK
          return sc;
        }
      }
    }
  }
  return sc;
}

/// Service finalization
StatusCode LHCb::RawDataCnvSvc::finalize()    {
  long typ = repSvcType();
  for(FileMap::iterator i=m_fileMap.begin(); i != m_fileMap.end(); ++i)  {
    if ( typ == RAWDATA_StorageType && (*i).second )  {
      closeIO((*i).second);
    }
  }
  m_fileMap.clear();
  if ( m_dataMgr ) m_dataMgr->release();
  m_dataMgr = 0;
  return ConversionSvc::finalize();
}

// Helper to print errors and return bad status
StatusCode LHCb::RawDataCnvSvc::error(CSTR msg) const {
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
    if ( m_bankLocation == LHCb::RawEventLocation::Default )  {
      if ( pAddr->clID() == CLID_DataObject )  {
        refpObj = new DataObject();
        return StatusCode::SUCCESS;
      }
      else if ( pAddr->clID() == RawEvent::classID() )  {
        refpObj = new RawEvent();
        return StatusCode::SUCCESS;
      }
    }
    else  {
      RawDataAddress* rawAdd = dynamic_cast<RawDataAddress*>(pAddr);
      rawAdd->setSvcType(ROOT_StorageType);
      return dataProvider()->retrieveObject("/Event",refpObj);
    }
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::RawDataCnvSvc::regAddr(IRegistry* pReg, 
                                        IOpaqueAddress* pAddress,
                                        CSTR path,
                                        const CLID& clid)  
{
  RawDataAddress* pA = dynamic_cast<RawDataAddress*>(pAddress);
  if ( pA )  {
    RawDataAddress* paddr = new RawDataAddress(*pA);
    paddr->setClID(clid);
    StatusCode sc = m_dataMgr->registerAddress(pReg, path, paddr);
    if ( !sc.isSuccess() )  {
      paddr->release();
      return error("Failed to register address for object "+path);
    }
    return sc;
  }
  return error("No valid event object address present.");
}

// Pass raw banks to RawEvent identified by its path
StatusCode 
LHCb::RawDataCnvSvc::adoptRawBanks(CSTR path, const std::vector<RawBank*>& banks)  {
  RawEvent* e = 0;
  StatusCode sc = dataProvider()->retrieveObject(path,(DataObject*&)e);
  return sc.isSuccess() ? adoptBanks(e, banks) : error("Failed to retrieve object:"+path);
}

/// Callback for reference processing (misused to attach leaves)
StatusCode 
LHCb::RawDataCnvSvc::fillObjRefs(IOpaqueAddress* pA, DataObject* pObj)  {
  typedef std::vector<RawBank*> _B;
  if ( pA && pObj )  {
    try {
      IRegistry* pReg = pA->registry();
      if ( pReg )  {
        std::string id = pReg->identifier().substr(6);
        if ( id == "" )  {
          StatusCode sc;
          std::string prev("/Prev"), next("/Next");
          for(int i=m_evtsBefore; i>0; --i)  {
            sc = regAddr(pReg,pA,prev+char(i+'0'),DataObject::classID());
            if ( !sc.isSuccess() ) return sc;
          }
          sc = regAddr(pReg, pA, "/DAQ", DataObject::classID());
          if ( !sc.isSuccess() ) return sc;
          for(int j=0; j<m_evtsAfter; ++j)  {
            sc = regAddr(pReg,pA,next+char(j+'1'),DataObject::classID());
            if ( !sc.isSuccess() ) return sc;
          }
          return sc;
        }
        else if (id.substr(id.length()-4) == "/DAQ" )
          return regAddr(pReg, pA, "/RawEvent", RawEvent::classID());
        else if (id.length() == 6 && (id[1] == 'N' || id[1] == 'P') )
          return regAddr(pReg, pA, "/DAQ", DataObject::classID());
        else if (id == "/DAQ/RawEvent" )  {
          RawDataAddress* pAddRaw = dynamic_cast<RawDataAddress*>(pA);
          RawEvent*       raw     = dynamic_cast<RawEvent*>(pObj);
          if ( pAddRaw->type() == RawDataAddress::BANK_TYPE )  {
            return pAddRaw->banks()  // MBM input from event selector: banks already filled...
              ? adoptBanks(raw,*pAddRaw->banks())
              : error("No banks found for address:"+pReg->identifier());
          }
          else if ( pAddRaw->type() == RawDataAddress::DATA_TYPE )  {
            // Check for contiguous data buffer...
            std::pair<char*,int> dat = pAddRaw->data();
            if ( dat.second == 0 )  {
              // Need to open MDF file to get access!
              if ( !readRawBanks(pAddRaw,dat).isSuccess() )  {
                return error("Failed to open raw data input:"+pA->par()[0]);
              }
            }
            StatusCode sc = decodeRawBanks(dat.first,dat.first+dat.second,raw);
            return sc.isSuccess() 
              ? sc : error("Failed to decode raw data input from:"+pA->par()[0]);
          }
          else if ( pAddRaw->type() == RawDataAddress::MEP_TYPE )  {
            typedef std::map<unsigned int,_B> _E;
            std::pair<char*,int> dat = pAddRaw->data();
            if ( dat.second > 0 )  {
              _E evts;
              unsigned int pID = 0;
              StatusCode sc = decodeMEP((MEPEvent*)dat.first,pID,evts);
              if ( sc.isSuccess() )  {
                _E::iterator it_evt = evts.begin();
                if ( int(evts.size()) == m_evtsBefore+m_evtsAfter+1 )  {
                  std::string prev("/Event/Prev"), next("/Event/Next"), raw_name("/DAQ/RawEvent");
                  for(int i=m_evtsBefore; i>0; --i, ++it_evt)  {
                    sc = adoptRawBanks(prev+char(i+'0')+raw_name,(*it_evt).second);
                    if ( !sc.isSuccess() ) return sc;
                  }
                  sc = adoptBanks(raw,(*it_evt).second);
                  if ( !sc.isSuccess() ) return sc;
                  ++it_evt;
                  for(int j=0; j<m_evtsAfter; ++j, ++it_evt)  {
                    sc = adoptRawBanks(next+char(j+'1')+raw_name,(*it_evt).second);
                    if ( !sc.isSuccess() ) return sc;
                  }
                  return sc;
                }
                return error("Inconsistent number of events found in MEP!");
              }
              return error("Failed to decode raw MEP data input:"+pA->par()[0]);
            }
            return error("Failed to access raw MEP data input:"+pA->par()[0]);
          }
          return error("UNKNOWN decoding not yet implemented:"+pA->par()[0]);
        }
        else if (id.substr(id.length()-9) == "/RawEvent" )  {
          return StatusCode::SUCCESS;
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
LHCb::RawDataCnvSvc::connectOutput(CSTR    outputFile,
                                   CSTR    openMode) 
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
StatusCode LHCb::RawDataCnvSvc::connectInput(CSTR fname, void*& iodesc)   {
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
StatusCode LHCb::RawDataCnvSvc::commitOutput(CSTR , bool doCommit ) 
{
  if ( doCommit && m_wrFlag )  {
    if ( m_current != m_fileMap.end() )   {
      long typ = repSvcType();
      setupMDFIO(msgSvc(),dataProvider());
      if ( typ == RAWDATA_StorageType || typ == MBM_StorageType )  {
        StatusCode sc = commitRawBanks(m_compress,m_genChecksum,(*m_current).second, m_bankLocation);
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
  RawDataAddress* pA = new RawDataAddress(typ, clid, par[0], par[1], ip[0], ip[1]);
  pA->setType(RawDataAddress::DATA_TYPE);
  refpAddress = pA;
  return StatusCode::SUCCESS;
}

void* LHCb::RawDataCnvSvc::openIO(CSTR fname, CSTR mode) const    {
  MsgStream log(msgSvc(), name());
  MDFMapEntry* ent = new MDFMapEntry;
  ent->name = fname;
  if ( strncasecmp(mode.c_str(),"N",1)==0 || strncasecmp(mode.c_str(),"REC",3)==0 )  {
    ent->con = StreamDescriptor::connect(fname);
    if ( ent->con.ioDesc > 0 )  {
      log << MSG::INFO << "Opened(NEW)  MDF stream:" << ent->name
          << " ID:" << (void*)ent << endmsg;
      return ent;
    }
  }
  else if ( strncasecmp(mode.c_str(),"O",1)==0 || strncasecmp(mode.c_str(),"REA",3)==0 )  {
    ent->bind = StreamDescriptor::bind(fname);
    if ( ent->bind.ioDesc > 0 )  {
      ent->con = StreamDescriptor::accept(ent->bind);
      if ( ent->con.ioDesc > 0 )  {
        log << MSG::INFO << "Opened(READ) MDF stream:" << ent->name 
            << " ID:" << (void*)ent << endmsg;
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
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Closed MDF stream:" << ent->name
        << " ID:" << (void*)ent << endmsg;
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
LHCb::RawDataCnvSvc::readRawBanks(RawDataAddress* pAddr, std::pair<char*,int>& result)
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
        result = readBanks(ent);
        if ( result.first > 0 )  {
          return StatusCode::SUCCESS;
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
