//  ====================================================================
//  RawDataCnvSvc.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "MDF/RawDataConnection.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawDataCnvSvc.h"
#include "MDF/MDFHeader.h"

#include "GaudiUtils/IIODataManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "Event/RawEvent.h"

#include <memory>
#include <vector>
#include <map>

using namespace std;
using namespace LHCb;
using namespace Gaudi;

namespace {
  struct MDFMapEntry  {
    string           name;
    IDataConnection* connection;
    StreamDescriptor desc;
  };
  static bool s_recursiveFlag = true;
  struct RecursiveDetection {
    RecursiveDetection()   { s_recursiveFlag = !s_recursiveFlag; }
    ~RecursiveDetection()  { s_recursiveFlag = !s_recursiveFlag; }
    bool isRecursive() const    { return s_recursiveFlag; }
  };
}

static string RAWDATA_INPUT = "RAW";

// Initializing constructor
RawDataCnvSvc::RawDataCnvSvc(CSTR nam, ISvcLocator* loc, long typ) 
  : ConversionSvc(nam, loc, typ), MDFIO(MDFIO::MDF_RECORDS, nam), 
    m_wrFlag(false), m_dataMgr(0), m_ioMgr(0)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",       m_compress=2);     // File compression
  declareProperty("CreateChecksum", m_genChecksum=1);  // Generate checksum
  declareProperty("EventsBefore",   m_evtsBefore=0);   // Events before T0
  declareProperty("EventsAfter",    m_evtsAfter=0);    // Events after T0
  declareProperty("DataType",       m_dataType=MDFIO::MDF_RECORDS);  // Input data type
  declareProperty("BankLocation",   m_bankLocation=RawEventLocation::Default);  // Location of the banks in the TES
  declareProperty("DstLocation",    m_dstLocation="DstEvent");                  // Location of DST banks in the TES
  declareProperty("DataManager",    m_ioMgrName="Gaudi::IODataManager/IODataManager");
  declareProperty("SourceType",     m_sourceType=RAWDATA_INPUT);
}

// Initializing constructor
RawDataCnvSvc::RawDataCnvSvc(CSTR nam, ISvcLocator* loc) 
  : ConversionSvc(nam, loc, RAWDATA_StorageType), 
    MDFIO(MDFIO::MDF_RECORDS, nam), 
    m_wrFlag(false), m_dataMgr(0), m_ioMgr(0)
{
  m_data.reserve(48*1024);
  declareProperty("Compress",       m_compress=2);     // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);  // Generate checksum
  declareProperty("EventsBefore",   m_evtsBefore=0);   // Events before T0
  declareProperty("EventsAfter",    m_evtsAfter=0);    // Events after T0
  declareProperty("DataType",       m_dataType=MDFIO::MDF_RECORDS);             // Input data type
  declareProperty("BankLocation",   m_bankLocation=RawEventLocation::Default);  // Location of RAW banks in the TES
  declareProperty("DstLocation",    m_dstLocation="DAQ/DstEvent");              // Location of DST banks in the TES
  declareProperty("DataManager",    m_ioMgrName="Gaudi::IODataManager/IODataManager");
  declareProperty("SourceType",     m_sourceType=RAWDATA_INPUT);
}

/// Service initialization
StatusCode RawDataCnvSvc::initialize()     {
  StatusCode sc = ConversionSvc::initialize();
  MsgStream log(msgSvc(),name());
  m_dataMgr = 0;
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Unable to initialize base class ConversionSvc." << endmsg;
    return sc;
  }

  // Add this conversion service to the EventPersistencySvc 
  IPersistencySvc *pSvc = 0;
  sc = service("EventPersistencySvc",pSvc,true);
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Unable to localize EventPersistencySvc." << endmsg;
    return sc;
  }

  sc = pSvc->addCnvService( this );
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Unable to add conversion service" << endmsg;
    return sc;
  }
  
  // get the IDataManagerSvc interface from the EventPersistencySvc
  sc = dataProvider()->queryInterface(IDataManagerSvc::interfaceID(),
                                      (void**)&m_dataMgr);
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Conversion service " << name() 
        << "not registered to EventPersistencySvc." << endmsg;
    return sc;
  }

  // Retrieve conversion service handling event iteration
  sc = Service::service(m_ioMgrName, m_ioMgr);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgrName << endmsg;
    return sc;
  }
  /// All OK
  return sc;
}

/// Service finalization
StatusCode RawDataCnvSvc::finalize()    {
  long typ = repSvcType();
  for(FileMap::iterator i=m_fileMap.begin(); i != m_fileMap.end(); ++i)  {
    if ( typ == RAWDATA_StorageType && (*i).second )  {
      closeIO((*i).second).ignore();
    }
  }
  m_fileMap.clear();
  if ( m_dataMgr ) m_dataMgr->release();
  m_dataMgr = 0;
  if ( m_ioMgr ) m_ioMgr->release();
  m_ioMgr = 0;
  return ConversionSvc::finalize();
}

/// Helper to print errors and return bad status
StatusCode RawDataCnvSvc::error(CSTR msg) const {
  MsgStream err(msgSvc(), name());
  err << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

/// Concrete class type
const CLID& RawDataCnvSvc::objType() const  {
  return DataObject::classID();     
}

/// Connect the output file to the service with open mode.
StatusCode RawDataCnvSvc::connectOutput(CSTR outputFile,CSTR openMode)  {
  m_wrFlag = false;
  m_current = m_fileMap.find(outputFile);
  if ( m_current == m_fileMap.end() )   {
    void* entry = openIO(outputFile, openMode);
    closeDisconnected();
    if ( entry ) {
      m_fileMap.emplace(outputFile,entry);
      m_current = m_fileMap.find(outputFile);
      return StatusCode::SUCCESS;
    }
    return error("connectOutput> FAILED to bind I/O channel:"+outputFile);
  }
  return StatusCode::SUCCESS;
}

/// Connect the input file to the service with READ mode
StatusCode RawDataCnvSvc::connectInput(CSTR fname, void*& iodesc)   {
  FileMap::const_iterator it = m_fileMap.find(fname);
  if ( it == m_fileMap.end() )   {
    iodesc = openIO(fname, "READ");
    closeDisconnected();
    if ( iodesc ) {
      m_fileMap.emplace(fname,iodesc);
      return StatusCode::SUCCESS;
    }
    return error("connectInput> FAILED to bind I/O channel:"+fname);
  }
  iodesc = (*it).second;
  return StatusCode::SUCCESS;
}

/// Object creation callback
StatusCode 
RawDataCnvSvc::createObj(IOpaqueAddress* pA, DataObject*& refpObj) {
  if ( pA )  {
    bool default_reader = m_bankLocation == RawEventLocation::Default;
    // default_reader = 
    if ( default_reader )   {
      if ( pA->clID() == CLID_DataObject )  {
        refpObj = new DataObject();
        return StatusCode::SUCCESS;
      }
      else if ( pA->clID() == RawEvent::classID() )  {
        try {
          IRegistry* pReg = pA->registry();
          RawDataAddress* pAddRaw = dynamic_cast<RawDataAddress*>(pA);
          refpObj = nullptr;
          //if ( rand() < int(RAND_MAX*0.05) )
          //  return error("TEST: Triggered access failure for "+pReg->identifier());
          if ( pReg && pAddRaw ) {
            std::unique_ptr<RawEvent> raw(new RawEvent());
            StatusCode sc  = StatusCode::FAILURE;
            int        typ = pAddRaw->type();
            // MBM input from event selector: banks already filled...
            if ( pAddRaw->banks() && typ == RawDataAddress::BANK_TYPE ) {
              sc = adoptBanks(raw.get(),*pAddRaw->banks());
              if ( sc.isSuccess() )   {
                refpObj = raw.release();
                return sc;
              }
            }
            else {
              MDFDescriptor dat = accessRawData(pAddRaw);
              if ( dat.second > 0 )  {
                if ( typ == RawDataAddress::DATA_TYPE ) {
                  sc = unpackTAE(dat,pReg->identifier(),raw.get());
                }
                else if ( typ == RawDataAddress::MEP_TYPE ) {
                  sc = unpackMEP(dat,pReg->identifier(),raw.get());
                }
                else {
                  return error("UNKNOWN decoding requested - not yet implemented:"+pA->par()[0]);
		}
              }
              if ( sc.isSuccess() )   {
                refpObj = raw.release();
                return sc;
              }
              return error("Failed to decode raw data from input from:"+pA->par()[0]);
            }
            return error("Failed to access raw data from input:"+pA->par()[0]);
          }
          return error("No valid object address present:"+
		       (pReg ? pReg->identifier() : pA->par()[0]));
	}
        catch (exception& e) {
          return error(string("Exception:") + e.what());
        }
        catch(...) {
          return error("Exception: (Unknown)");
        }
      }
      return error("No valid event object address present [INVALID_CLID].");
    }
    else  {
      RawDataAddress* rawAdd = dynamic_cast<RawDataAddress*>(pA);
      if ( rawAdd ) {
	rawAdd->setSvcType(ROOT_StorageType);
	return dataProvider()->retrieveObject("/Event",refpObj);
      }
      return error("No valid object address present for object \"/Event\"");
    }
  }
  return StatusCode::FAILURE;
}

StatusCode RawDataCnvSvc::regAddr(IRegistry* pReg, 
                                  RawDataAddress* pA,
                                  CSTR path,
                                  const CLID& clid)  
{
  RawDataAddress* paddr = new RawDataAddress(*pA);
  paddr->setClID(clid);
  StatusCode sc = StatusCode::FAILURE;
  if ( pReg ) {
    sc = m_dataMgr->registerAddress(pReg, path, paddr);
  }
  else {
    sc = m_dataMgr->registerAddress(path, paddr);
  }
  if ( sc.isSuccess() )  {
    return sc;
  }
  paddr->release();
  return error("Failed to register address for object "+path);
}

/// Access the raw data from MDF file
MDFDescriptor RawDataCnvSvc::accessRawData(RawDataAddress* pAddRaw)  {
  MDFDescriptor dat = pAddRaw->data();
  if ( dat.second == 0 )  {
    // Need to open MDF file to get access!
    if ( readRawBanks(pAddRaw,dat).isSuccess() )  {
      pAddRaw->setData(dat);
    }
  }
  return dat;
}

/// Decode a TAE event record from MDF banks
StatusCode 
RawDataCnvSvc::registerRawAddresses(IRegistry* pReg,RawDataAddress* pAddRaw, const vector<string>& names)  {
  typedef vector<string> _N;
  if ( !names.empty() )  {
    StatusCode sc, iret = StatusCode::SUCCESS;
    for(_N::const_iterator i=names.begin(); i !=names.end(); ++i)  {
      const string& s = *i;
      string path = s.length()>7 ? s.substr(7) : string("/DAQ");
      sc = regAddr(pReg,pAddRaw,path,CLID_DataObject);
      if ( !sc.isSuccess() ) iret = sc;
    }
    return iret.isSuccess() ? iret : error("Error registering RawEvent leaves in TES.");
  }
  return regAddr(pReg,pAddRaw,"/DAQ",DataObject::classID());
}

/// Decode a MEP (Multi event packets) record
StatusCode 
RawDataCnvSvc::unpackMEP(const MDFDescriptor& dat, const string& loc, RawEvent* raw)  {
  RecursiveDetection rec;            // Triggered by a retrieveObject call during
  if ( !rec.isRecursive() )  {       // a call to fillObjRefs
    map<unsigned int,vector<RawBank*> > evts;
    unsigned int pID = 0;
    StatusCode sc = decodeMEP((MEPEvent*)dat.first,pID,evts);
    if ( sc.isSuccess() )  {
      RawEvent* r = 0;
      MsgStream log(msgSvc(), name());
      map<unsigned int,vector<RawBank*> >::iterator it = evts.begin();
      vector<string> names = buffersMEP(dat.first);
      vector<string>::const_iterator i=names.begin();
      setupMDFIO(msgSvc(),dataProvider());
      for(; i != names.end() && it != evts.end(); ++i, ++it)   {
        string obj_loc = (*i)+"/DAQ/RawEvent";
        if ( obj_loc == loc )    {
          r = raw;
        }
        else  {
          SmartDataPtr<RawEvent> rawp( dataProvider(), obj_loc );
          if( rawp ){
            r = rawp;
          }
          else {
            return error("Failed to access raw event at:"+obj_loc);
          }
        }
        if( UNLIKELY(log.level() <= MSG::DEBUG) ) log << MSG::DEBUG
           << "unpackMEP: Filling banks for " << obj_loc << " " << r->clID()
           << " " << " [" << (*it).second.size() << " banks] (" << loc << ") "
           << (void*)r << "<>" << (void*)raw << endmsg;
        sc = adoptBanks(r,(*it).second);
        if ( sc.isSuccess() )  {
          continue;
        }
        return error("Failed to add MEP banks to raw event structure:"+obj_loc);
      }
      return StatusCode::SUCCESS;
    }
    return error("Failed to decode raw MEP data.");
  }
  return StatusCode::SUCCESS;
}

/// Callback for reference processing (misused to attach leaves)
StatusCode RawDataCnvSvc::fillObjRefs(IOpaqueAddress* pA, DataObject* pObj)  {
  if ( pA && pObj )  {
    try {
      IRegistry* pReg = pA->registry();
      RawDataAddress* pAddRaw = dynamic_cast<RawDataAddress*>(pA);
      if ( pReg && pAddRaw )  {
	typedef RawDataAddress::Banks _B;
	const _B* banks = pAddRaw->banks();
        string id = pReg->identifier().substr(6);
        if ( id.empty() )  {
          if ( banks && pAddRaw->type() == RawDataAddress::BANK_TYPE ) {
            return regAddr(pReg, pAddRaw, "/DAQ", DataObject::classID());
          }
          else {
            MDFDescriptor dat = pAddRaw->data();
            if ( dat.second>0 )  {
              if ( pAddRaw->type() == RawDataAddress::DATA_TYPE )
                return registerRawAddresses(pReg,pAddRaw,buffersTAE(dat.first));
              else if ( pAddRaw->type() == RawDataAddress::MEP_TYPE )
                return registerRawAddresses(pReg,pAddRaw,buffersMEP(dat.first));
              return regAddr(pReg, pAddRaw, "/DAQ", DataObject::classID());
            }
          }
          return error("Failed to access raw data input:"+pA->par()[0]);
        }
        else if (id.substr(id.length()-4) == "/DAQ" )   {
	  MDFDescriptor dat = pAddRaw->data();
          if ( dat.second>0 && pAddRaw->type() == RawDataAddress::DATA_TYPE )   {
	    char* start = dat.first, *end = start+dat.second;
	    while(start<end) {
	      RawBank* b = (RawBank*)start;
	      if ( b->type() == RawBank::DstAddress )   {
		// cout << "Reg:" << pReg->identifier() << "  " << m_dstLocation << endl;
		StatusCode sc = StatusCode::FAILURE;
		const char* objLoc = m_dstLocation.c_str();
		const char* regLoc = pReg->identifier().c_str();
		size_t regLen = pReg->identifier().length();
		if ( objLoc[0] == '/' && strncmp(objLoc,regLoc,regLen)==0 )
		  sc = regAddr(pReg, pAddRaw, objLoc+regLen+1, RawEvent::classID());
		else if ( objLoc[0] != '/' && strncmp(objLoc,"DAQ/",4)==0 )
		  sc = regAddr(pReg, pAddRaw, objLoc+4, RawEvent::classID());
		else
		  sc = regAddr(pReg, pAddRaw, m_dstLocation, RawEvent::classID());
		if ( sc.isSuccess() ) {
		  unsigned int *ptr = b->begin<unsigned int>();
		  long unsigned int clid = *ptr++, ip[2] = {*ptr++, *ptr++}, svc_typ = *ptr++;
		  size_t len = strlen((char*)ptr)+1;
		  string p[2] = { std::string((char*)ptr), std::string(((char*)ptr)+len)};
		  string raw_loc = ((char*)ptr)+len+1+p[1].length();
		  IOpaqueAddress* addr = 0;
		  // cout << "P0:" << p[0] << " P1:" << p[1] << " IP0:" << ip[0] << " IP1:" << ip[1] << " " << svc_typ << endl;
		  sc = m_addressCreator->createAddress(svc_typ,clid,p,ip,addr);
		  if ( sc.isSuccess() ) {
		    sc = m_dataMgr->registerAddress(raw_loc,addr);
		    if ( sc.isSuccess() ) {
		      return sc;
		    }
		    return error("Failed to register address to raw data "+p[1]+" in "+raw_loc);
		  }
		  return error("Failed to create address to raw data "+p[1]+" in "+raw_loc);
		}
		return error("Failed to register address to DstEvent");
	      }
	      start += b->totalSize();
	    }
	  }
	  return regAddr(pReg, pAddRaw, "/RawEvent", RawEvent::classID());
	}
        else if (id.length() == 6 && (id[1] == 'N' || id[1] == 'P') ) {
          return regAddr(pReg, pAddRaw, "/DAQ", DataObject::classID());
	}
        // Nothing to do otherwise. Return success.
        return StatusCode::SUCCESS;
      }
    }
    catch (exception& e) {
      return error(string("Exception:") + e.what());
    }
    catch(...) {
      return error("Exception: (Unknown)");
    }
  }
  return error("No valid event object/address present.");
}

/// Commit pending output.
StatusCode RawDataCnvSvc::commitOutput(CSTR , bool doCommit )   {
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
StatusCode RawDataCnvSvc::commitDescriptors(void* /* ioDesc */)  {
  return error("Event descriptor output is not implemented for this class!");
}

/// Convert the transient object to the requested representation.
StatusCode RawDataCnvSvc::createRep(DataObject* pObj,IOpaqueAddress*& refpAddr)  {
  if ( pObj )  {
    if ( m_current != m_fileMap.end() )   {
      IRegistry* reg = pObj->registry();
      string spar[]   = {(*m_current).first,reg->identifier()};
      unsigned long ipar[] = {0,0};
      return createAddress(repSvcType(),pObj->clID(),spar,ipar,refpAddr);
    }
    return error("createRep> Cannot write object: No output file is connected!");
  }
  return error("createRep> Cannot write object: Object pointer is NULL!");
}

/// Resolve the references of the converted object. 
StatusCode 
RawDataCnvSvc::fillRepRefs(IOpaqueAddress* /* pAddr */, DataObject* /* pObj */) {
  if ( m_current != m_fileMap.end() )   {
    m_wrFlag = true;
    return StatusCode::SUCCESS;
  }
  return error("fillRepRefs> Cannot write object: No output file is connected!");
}

/// Create a Generic address using explicit arguments to identify a single object.
StatusCode RawDataCnvSvc::createAddress(long typ, 
                                        const CLID& clid, 
                                        const string* par, 
                                        const unsigned long* ip,
                                        IOpaqueAddress*& refpAddress)    
{
  RawDataAddress* pA = new RawDataAddress(typ, clid, par[0], par[1], ip[0], ip[1]);
  pA->setType(RawDataAddress::DATA_TYPE);
  refpAddress = pA;
  return StatusCode::SUCCESS;
}

/// Close all files disconnected from the IO manager
void RawDataCnvSvc::closeDisconnected()   {
  for(FileMap::iterator i=m_fileMap.begin(); i != m_fileMap.end(); )  {
    MDFMapEntry* e = (MDFMapEntry*)(*i).second;
    if ( e && e->connection && !e->connection->isConnected() ) {
      closeIO(e).ignore();
      m_fileMap.erase(i);
      i = m_fileMap.begin();
      continue;
    }
    ++i;
  }
}      

/// Open MDF file
void* RawDataCnvSvc::openIO(CSTR fname, CSTR mode) const    {
  MsgStream log(msgSvc(), name());
  MDFMapEntry* ent = new MDFMapEntry;
  ent->name = fname;
  if ( strncasecmp(mode.c_str(),"N",1)==0 || strncasecmp(mode.c_str(),"REC",3)==0 )  {
    ent->connection = new RawDataConnection(this,fname);
    if ( m_ioMgr->connectWrite(ent->connection,IDataConnection::RECREATE,"MDF").isSuccess() )  {
      log << MSG::INFO << "Opened(NEW)  MDF stream:" << ent->name
          << " ID:" << (void*)ent << endmsg;
      return ent;
    }
  }
  else if ( strncasecmp(mode.c_str(),"O",1)==0 || strncasecmp(mode.c_str(),"REA",3)==0 )  {
    ent->connection = new RawDataConnection(this,fname);
    if ( m_ioMgr->connectRead(false,ent->connection).isSuccess() )  {
      log << MSG::INFO << "Opened(READ) MDF stream:" << ent->name 
          << " ID:" << (void*)ent << endmsg;
      return ent;
    }
  }
  error("Unknown openmode "+mode+" for MDF file :"+fname);
  delete ent;
  return 0;
}

/// Close MDF file
StatusCode RawDataCnvSvc::closeIO(void* ioDesc)  const {
  MDFMapEntry* ent = (MDFMapEntry*)ioDesc;
  if ( ent )  {
    MsgStream log(msgSvc(), name());
    bool connected = ent->connection->isConnected();
    m_ioMgr->disconnect(ent->connection).ignore();
    log << MSG::INFO << (connected ? "Closed " : "Removed dis") 
	<< "connected MDF stream:" << ent->name
	<< " ID:" << (void*)ent << endmsg;
    delete ent;
  }
  return StatusCode::SUCCESS;
}

/// Read raw byte buffer from input stream
StatusCode RawDataCnvSvc::readBuffer(void* const ioDesc, void* const data, size_t len)  {
  MDFMapEntry* ent = (MDFMapEntry*)ioDesc;
  if ( ent && ent->connection ) {
    return m_ioMgr->read(ent->connection,data,len);
  }
  return StatusCode::FAILURE;
}

/// Read raw banks
StatusCode 
RawDataCnvSvc::readRawBanks(RawDataAddress* pAddr, MDFDescriptor& result)   {
  void* iodesc = 0;
  const string* par = pAddr->par();
  StatusCode sc = connectInput(par[0], iodesc);
  if ( sc.isSuccess() )  {
    long long offset = pAddr->fileOffset();
    MDFMapEntry* ent = (MDFMapEntry*)iodesc;
    if ( ent->connection )  {
      if ( m_ioMgr->seek(ent->connection, offset, SEEK_SET) != -1 )  {
        setupMDFIO(msgSvc(),dataProvider());
        result = readBanks(ent);
        if ( result.first )  {
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
StatusCode RawDataCnvSvc::writeBuffer(void* iodesc, const void* data, size_t len)   {
  MDFMapEntry* ent = (MDFMapEntry*)iodesc;
  if ( ent && ent->connection )  {
    if ( m_ioMgr->write(ent->connection, data, len).isSuccess() )  {
      return StatusCode::SUCCESS;
    }
    return error("Cannot write data record: [Invalid I/O operation]");
  }
  return error("Cannot write data record: [Invalid I/O descriptor]");
}
