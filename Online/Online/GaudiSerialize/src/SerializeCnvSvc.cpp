// $Id: SerializeCnvSvc.cpp,v 1.14 2010-09-23 12:13:36 frankb Exp $
//====================================================================
//	SerializeCnvSvc implementation
//--------------------------------------------------------------------
//
//  Description: Implementation of the Serialization data storage
//
//  Author     : M.Frank
//  Author     : A. Puig
//
//====================================================================

// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiSerialize/SerializeBaseCnv.h"
#include "GaudiSerialize/SerializeCnvSvc.h"
#include "GaudiSerialize/SerializeAddress.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"

// ROOT include files
#include "TROOT.h"
#include "TClass.h"
#include "TBufferFile.h"
#include "TInterpreter.h"

using namespace std;
using namespace LHCb;
using namespace Gaudi;
typedef const std::string& CSTR;

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE

namespace GaudiRoot  {
  bool patchStreamers(MsgStream& s);
  void popCurrentDataObject();
  void pushCurrentDataObject(DataObject** pobjAddr);
}

namespace {
  struct DataObjectPush {
    DataObject* m_ptr;
    DataObjectPush(DataObject* p) : m_ptr(p) {
      ::pushCurrentDataObject(&m_ptr);
    }
    ~DataObjectPush() {
      ::popCurrentDataObject();
    }
  };
}

/// Standard constructor
SerializeCnvSvc::SerializeCnvSvc(CSTR nam, ISvcLocator* svc)
  : ConversionSvc( nam, svc, SERIALIZE_StorageType), m_dataMgr(0)
{
  declareProperty("Banks",m_location="/Event/GaudiSerialize");
}

/// Standard destructor
SerializeCnvSvc::~SerializeCnvSvc()  {
}

/// Query interface
StatusCode
SerializeCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)    {
  if ( IID_ISerializeMgr == riid )  {
    *ppvInterface = this;
    addRef();
    return S_OK;
  }
  // Interface is not directly availible: try out a base class
  return ConversionSvc::queryInterface(riid, ppvInterface);
}

/// Initialize the Db data persistency service
StatusCode SerializeCnvSvc::initialize()  {
  StatusCode status = ConversionSvc::initialize();
  MsgStream log(messageService(), name());
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Failed to initialize ConversionSvc base class." << endmsg;
    return status;
  }
  IDataProviderSvc* pSvc = 0;
  status = service("EventDataSvc",pSvc);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Failed to get data provider." << endmsg;
    return status;
  }
  setDataProvider(pSvc);
  GaudiRoot::patchStreamers(log);
  return S_OK;
}

/// Finalize the Db data persistency service
StatusCode SerializeCnvSvc::finalize()    {
  MsgStream log(messageService(), name());
  if ( m_dataMgr ) {
    m_dataMgr->clearStore().ignore();
    m_dataMgr->release();
  }
  StatusCode status = ConversionSvc::finalize();
  log << MSG::INFO << "Serialization conversion service " << name() << " ";
  log << (char*)(status.isSuccess() ? "successfully" : "with errors") << " finalized." << endmsg;
  return status;
}

/// Update state of the service
StatusCode SerializeCnvSvc::updateServiceState(IOpaqueAddress* /* pAddr */)  {
  return S_OK;
}

/// Create new Converter using factory
IConverter*
SerializeCnvSvc::createConverter(long typ,const CLID& wanted,const ICnvFactory*)
{
  IConverter* pConverter;
  ConverterID cnvid0(SERIALIZE_StorageType, wanted);
  pConverter = SerializeBaseCnv::Factory::create(cnvid0, typ, wanted, serviceLocator().get());
  MsgStream log(msgSvc(), name());

  if ( 0 == pConverter )  {
    const CLID gen_clids[] = {
      /* ObjectList               */ CLID_Any + CLID_ObjectList,
      /* ObjectVector             */ CLID_Any + CLID_ObjectVector,
      /* Keyed Map                */ CLID_Any + CLID_ObjectVector+0x00030000,
      /* Keyed Hashmap            */ CLID_Any + CLID_ObjectVector+0x00040000,
      /* Keyed redirection array  */ CLID_Any + CLID_ObjectVector+0x00050000,
      /* Standard, non-container  */ CLID_Any
    };
    for ( unsigned int i = 0; i < sizeof(gen_clids)/sizeof(gen_clids[0]); i++ ) {
      if ( (wanted>>16) == (gen_clids[i]>>16) )  {
        ConverterID cnvid(SERIALIZE_StorageType, gen_clids[i]);
        pConverter = SerializeBaseCnv::Factory::create(cnvid,typ,wanted,serviceLocator().get());
        if ( 0 != pConverter ) {
          return pConverter;
        }
      }
    }
    // Check if a converter using object update is needed
    if ( (wanted>>24) != 0 )  {
      ConverterID cnvid_obj(SERIALIZE_StorageType,CLID_Any + CLID(1<<31));
      pConverter = SerializeBaseCnv::Factory::create(cnvid_obj,typ,wanted,serviceLocator().get());
      if ( 0 != pConverter ) {
        return pConverter;
      }
    }
    // If we do not have found any suitable container after searching
    // for standard containers, we will use the "ANY" converter
    // ... and pray for everything will go well.
    ConverterID cnvid_any(SERIALIZE_StorageType, CLID_Any);
    pConverter = SerializeBaseCnv::Factory::create(cnvid_any,typ,wanted,serviceLocator().get());
    if ( 0 != pConverter ) {
      log << MSG::INFO << "Using \"Any\" converter for objects of type "
	  << showbase << hex << wanted << endmsg;
    }
  }
  return pConverter;
}

void SerializeCnvSvc::loadConverter(DataObject* pObject) {
  if (pObject) {
    MsgStream log(msgSvc(), name());
    string cname = System::typeinfoName(typeid(*pObject));
    log << MSG::DEBUG << "Trying to 'Autoload' dictionary for class " << cname << endmsg;
    gInterpreter->EnableAutoLoading();
    gInterpreter->AutoLoad( cname.c_str());
  }
}

StatusCode SerializeCnvSvc::setDataProvider(IDataProviderSvc* pSvc)  {
  MsgStream log(msgSvc(), name());
  IDataManagerSvc* tmp = m_dataMgr;
  if (pSvc)  {
    StatusCode sc = pSvc->queryInterface(IDataManagerSvc::interfaceID(),(void**)&m_dataMgr);
    if ( !sc.isSuccess() )    {
      return error("Cannot connect to \"IDataManagerSvc\" interface.");
    }
  }
  if ( tmp ) tmp->release();
  log<<MSG::DEBUG<< "Data provider:" << (void*)pSvc << endmsg;
  return ConversionSvc::setDataProvider(pSvc);
}

/// Connect the output file to the service with open mode.
StatusCode SerializeCnvSvc::connectOutput(CSTR /* dsn */, CSTR /* openMode */)   {
  m_objects.clear();
  return S_OK;
}

// Conect output stream (valid until overwritten)
StatusCode SerializeCnvSvc::connectOutput(CSTR db_name)  {
  return connectOutput(db_name, "NEW");
}

// Commit pending output on open container
StatusCode  SerializeCnvSvc::commitOutput(CSTR dsn, bool doCommit) {
  try  {
    if ( doCommit )    {
      MsgStream log(msgSvc(), name());
      log << MSG::DEBUG << "Serializing " << dsn << " with " << m_objects.size() << " objects." << endmsg;
      map<string, TClass*> classes;
      int object_counter=1;
      std::auto_ptr<RawEvent> raw(new RawEvent());
      for(Objects::iterator i=m_objects.begin(); i != m_objects.end(); ++i)      {
        TBufferFile rawBuffer(TBuffer::kWrite,256*1024);
	DataObject* pObj = (*i);
	DataObjectPush push_buffer(pObj);

	// We build a map so gROOT has to access the whole class database as little as possible
	string objClassName = System::typeinfoName(typeid(*pObj));
	TClass* cl = classes[objClassName];
	if ( 0 == cl )   {
	  classes[objClassName] = cl = gROOT->GetClass(objClassName.c_str());
	}
	if (cl==0){
	  log << MSG::ERROR << "No valid class found for " << objClassName << endmsg;
	  return S_FAIL;
	}

	string loc = pObj->registry()->identifier();
	rawBuffer.WriteString(loc.c_str());
        rawBuffer << (long)pObj->clID();
	rawBuffer.WriteString(cl->GetName());
	cl->Streamer(pObj,rawBuffer);

	LinkManager* linkMgr = pObj->linkMgr();
	int numLinks = linkMgr->size();
        rawBuffer << numLinks;
	for (int it = 0; it != numLinks; it++)        {
          rawBuffer.WriteString(linkMgr->link(it)->path().c_str());
	}

	// Write the bank taking into account size limitations
        const char* start = rawBuffer.Buffer();
        const char* end = start+rawBuffer.Length();
        int bnk_count = 0;
	RawBank* bank = raw->createBank(object_counter, RawBank::GaudiSerialize, ++bnk_count, loc.length()+1, loc.c_str());
	raw->adoptBank(bank,true);
        while(start<end) {
          int bnk_len = end-start > 32*1024 ? 32*1024 : end-start;
          bank = raw->createBank(object_counter, RawBank::GaudiSerialize, ++bnk_count, bnk_len, start);
          raw->adoptBank(bank,true);
          start += bnk_len;
        }
	++object_counter;
	// rawBuffer.Delete();
      }
      //
      //  Add MDF Header bank using the original ODIN information if present
      //
      SmartDataPtr<RawEvent> raw_org(dataProvider(),RawEventLocation::Default);
      if ( raw_org ) {
	bool hdr_filled = false;
	MDFHeader* hdr, *hdr0;
	typedef vector<RawBank*> _V;
	unsigned int trMask[4] = {~0u,~0u,~0u,0x100u};  /// !!! MAGIC Number 0x100 for reconstructed events in routing
	const _V& hdr_banks=raw_org->banks(RawBank::DAQ);
	const _V& odin=raw_org->banks(RawBank::ODIN);
	// Get proper trigger mask for new MDF header
	size_t len = rawEventLength(raw.get());

	RawBank* hdrBank = raw->createBank(0, RawBank::DAQ, DAQ_STATUS_BANK, sizeof(MDFHeader)+sizeof(MDFHeader::Header1), 0);
	for(_V::const_iterator j=hdr_banks.begin(); j != hdr_banks.end(); ++j)  {
	  RawBank* b = *j;
	  if ( b->version() == DAQ_STATUS_BANK )  {
	    hdr0 = (MDFHeader*)b->data();
	    hdr = (MDFHeader*)hdrBank->data();
	    hdr->setChecksum(0);
	    hdr->setCompression(0);
	    hdr->setHeaderVersion(3);
	    hdr->setSpare(0);
	    hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
	    hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
	    hdr->setSize(len);
	    MDFHeader::SubHeader h  = hdr->subHeader();
	    MDFHeader::SubHeader h0 = hdr0->subHeader();
	    ::memcpy(trMask,h0.H1->triggerMask(),3*sizeof(trMask[0]));
	    h.H1->setTriggerMask(trMask);
	    h.H1->setRunNumber(h0.H1->runNumber());
	    h.H1->setOrbitNumber(h0.H1->orbitNumber());
	    h.H1->setBunchID(h0.H1->bunchID());
	    raw->adoptBank(hdrBank, true);
	    hdr_filled = true;
	    break;
	  }
	}
	// Fill information from ODIN bank if no MDF header bank was found!
	if ( !hdr_filled && !odin.empty() ) {
	  const OnlineRunInfo* odin_bank=odin[0]->begin<OnlineRunInfo>();
	  hdr = (MDFHeader*)hdrBank->data();
	  hdr->setChecksum(0);
	  hdr->setCompression(0);
	  hdr->setHeaderVersion(3);
	  hdr->setSpare(0);
	  hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
	  hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
	  hdr->setSize(len);
	  MDFHeader::SubHeader h = hdr->subHeader();
	  h.H1->setTriggerMask(trMask);
	  h.H1->setRunNumber(odin_bank->Run);
	  h.H1->setOrbitNumber(odin_bank->Orbit);
	  h.H1->setBunchID(odin_bank->bunchID);
	  raw->adoptBank(hdrBank, true);
	}
      }

      StatusCode sc = dataProvider()->registerObject(m_location,raw.get());
      if (!sc.isSuccess())   {
	log << MSG::ERROR << "\"" << m_location << "\" not successfully registered"<<endmsg;
      }
      raw.release();
    }
    // Rollback: just clear the object buffer!
    m_objects.clear();
    return S_OK;
  }
  catch (exception& e)  {
    error(string("commitOutput> Caught exception:")+e.what());
  }
  catch (...)   {
    error("commitOutput> Unknown Fatal Exception on commit to "+dsn);
  }
  m_objects.clear();
  return error("commitOutput> Fatal Error on commit to:"+dsn);
}

/// Disconnect from an existing data stream.
StatusCode SerializeCnvSvc::disconnect(CSTR /* dsn */)  {
  return S_OK;
}

/// IAddressCreator implementation: Address creation
StatusCode SerializeCnvSvc::createAddress(long  typ,
					  const CLID& clid,
					  const string* par,
					  const unsigned long* ip,
					  IOpaqueAddress*& refpAddress)
{
  refpAddress = new SerializeAddress(typ,clid,par[0],par[1],ip[0],ip[1]);
  return S_OK;
}

/// Mark an object for write given an object reference
StatusCode
SerializeCnvSvc::writeObject(DataObject* pObj, IOpaqueAddress*& refpAddr)  {
  if ( pObj ) {
    CLID clid = pObj->clID();
    string loc = pObj->registry()->identifier();
    m_objects.push_back(pObj);
    refpAddr = new SerializeAddress(SERIALIZE_StorageType,clid,loc,"",0,0);
    return S_OK;
  }
  return error("markWrite> Current Database is invalid!");
}

/// Read existing object. Open transaction in read mode if not active
StatusCode SerializeCnvSvc::readObject(IOpaqueAddress* pA, DataObject*& refpObj)  {
  refpObj = 0;
  if ( pA ) {
    size_t len;
    char text[4096], *p, *q;
    long class_id = 0;
    vector<RawBank*> banks;
    IRegistry* pReg = pA->registry();
    string id       = pReg->identifier();
    RawDataAddress* pAraw = dynamic_cast<RawDataAddress*>(pA);
    pair<char*,int> d = pAraw->data();

    decodeRawBanks(d.first,d.first+d.second,banks);
    for(vector<RawBank*>::const_iterator k, e=banks.end(), b=banks.begin(), i=b; i != e; ++i)  {
      const char* b_nam = (*i)->begin<char>();
      if ( (*i)->type()     != RawBank::GaudiSerialize ) continue;
      if ( (*i)->version()  != 1 ) continue;
      if ( id == b_nam )     { //  We only want banks with version()=1
	RawBank *readBank = *i;
        for (len=0, k=b; k != e; ++k)  {
	  // Banks with the same sourceID() correspond to the same DataObject and need to be concatenated
	  if ( (*k)->type()     != RawBank::GaudiSerialize ) continue;
	  if ( (*k)->sourceID() != readBank->sourceID() ) continue;
          if ( (*k)->version() <= 1 ) continue;
	  len += (*k)->size();
        }
	if ( 0 == len ) {
	  return error("read> Cannot read object "+id+" -- No data banks were found.");
	}
	p = q = new char[len];
        for( k=b; k != e; ++k)  {
	  if ( (*k)->type()     != RawBank::GaudiSerialize ) continue;
	  if ( (*k)->sourceID() != readBank->sourceID() ) continue;
          if ( (*k)->version() <= 1 ) continue;
	  ::memcpy(p,(*k)->begin<char>(),(*k)->size());
	  p += (*k)->size();
        }

	//  The TBuffer is filled with as many banks as necessary. The memory is adopted by the TBuffer!
        TBufferFile buffer(TBuffer::kRead,len,q,kTRUE);
        buffer.ReadString(text,sizeof(text));
        buffer >> class_id;
        buffer.ReadString(text,sizeof(text));
	TClass* cl = gROOT->GetClass(text);
        std::auto_ptr<DataObject> pObj(cl ? (DataObject*)cl->New() : 0);
        if ( pObj.get() ) { // Restore links
          int nlink = 0;
          LinkManager* lnkMgr = pObj->linkMgr();
	  DataObjectPush push(pObj.get());
	  cl->Streamer(pObj.get(), buffer);
          buffer >> nlink;
          for (int cnt=0; cnt<nlink; ++cnt) {
            buffer.ReadString(text,sizeof(text));
            lnkMgr->addLink(text,0);
          }
          // Now register addresses and the leaves of the object
	  // We have stored the full registry identifier in the beginning of the TBuffer
	  // We also rely on the fact that ROOT does not add magic in front of the TBuffer
          for( k=b; k != e; ++k)  {
	    if ( (*k)->type()     != RawBank::GaudiSerialize ) continue;
            if ( (*k)->version()  != 1 ) continue;
	    if ( (*k) != readBank )   {
              p = (*k)->begin<char>();
              if ( 0 == ::strncmp(p,id.c_str(),id.length()) ) {
                p += id.length();
                if ( ::strchr(p+1,'/') == 0 ) { // May not be sub-leaf further down the tree!
                  std::auto_ptr<RawDataAddress> leaf(new RawDataAddress(*pAraw));
                  StatusCode sc = m_dataMgr->registerAddress(pReg,p,leaf.get());
                  if ( sc.isSuccess() )
		    leaf.release();
		  else
                    error("Failed to register address for object "+string(p));
                }
              }
            }
          }
          refpObj = pObj.release();
          return S_OK;
        }
        return error("read> Cannot read object "+id+" -- Data serialization failed!");
      }
    }
    return error("read> Cannot read object "+id);
  }
  return error("read> Cannot read object -- no valid object address present ");
}

// Small routine to issue exceptions
StatusCode SerializeCnvSvc::error(CSTR msg)  {
  MsgStream log(messageService(),name());
  log << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}

#include "GaudiKernel/SvcFactory.h"
DECLARE_SERVICE_FACTORY(SerializeCnvSvc)
