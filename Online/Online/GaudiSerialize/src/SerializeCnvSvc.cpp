// $Id: SerializeCnvSvc.cpp,v 1.2 2008-06-17 14:34:16 frankb Exp $
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
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiSerialize/SerializeCnvSvc.h"
#include "GaudiSerialize/SerializeAddress.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include <iostream>
#include <fstream>  
#include <iomanip>
#include <map>
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"

// ROOT include files
#include "TROOT.h"
#include "TInterpreter.h"
#include "TClass.h"
#include "TBufferFile.h"
#include "TStreamerInfo.h"

using ROOT::Reflex::PluginService;
using namespace std;
using namespace LHCb;

DECLARE_SERVICE_FACTORY(SerializeCnvSvc);
typedef const std::string& CSTR;

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE

void popCurrentDataObject();
void pushCurrentDataObject(DataObject** pobjAddr);
namespace GaudiPoolDb  {  bool patchStreamers(MsgStream& s); }

namespace {
  struct DataObjectPush {
    DataObjectPush(DataObject*& p) {
      pushCurrentDataObject(&p);
    }
    ~DataObjectPush() {
      popCurrentDataObject();
    }
  };
}

/// Standard constructor
SerializeCnvSvc::SerializeCnvSvc(CSTR nam, ISvcLocator* svc)
  : ConversionSvc( nam, svc, SERIALIZE_StorageType), m_dataMgr(0)
{
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
  GaudiPoolDb::patchStreamers(log);
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
  log << MSG::INFO << "POOL conversion service " << name() << " ";
  log << (char*)(status.isSuccess() ? "successfully" : "with errors");
  log << " finalized." << endmsg;
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
  ConverterID cnvid(SERIALIZE_StorageType, wanted);  
  pConverter = PluginService::CreateWithId<IConverter*>(cnvid, typ, wanted, serviceLocator());
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << (void*)this << " Get conf " << typ << " " << wanted << endmsg;

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
        pConverter = PluginService::CreateWithId<IConverter*>(cnvid, typ, wanted, serviceLocator());
        if ( 0 != pConverter ) {
          return pConverter;
        }
      }
    }
    // Check if a converter using object update is needed
    if ( (wanted>>24) != 0 )  {
      ConverterID cnvid(SERIALIZE_StorageType, CLID_Any | 1<<31);  
      pConverter = PluginService::CreateWithId<IConverter*>(cnvid, typ, wanted, serviceLocator());
      if ( 0 != pConverter ) {
        return pConverter;
      }
    }
    // If we do not have found any suitable container after searching
    // for standard containers, we will use the "ANY" converter 
    // ... and pray for everything will go well.
    ConverterID cnvid(SERIALIZE_StorageType, CLID_Any);  
    pConverter = PluginService::CreateWithId<IConverter*>(cnvid, typ, wanted, serviceLocator());
    if ( 0 != pConverter ) {
      MsgStream log(msgSvc(), name());
      log << MSG::INFO << "Using \"Any\" converter for objects of type " 
	  << std::showbase << std::hex << wanted << endmsg;
    }
  }
  return pConverter;
}

void SerializeCnvSvc::loadConverter(DataObject* pObject) {
  if (pObject) {
    MsgStream log(msgSvc(), name());
    std::string cname = System::typeinfoName(typeid(*pObject));
    log << MSG::DEBUG << "Trying to 'Autoload' dictionary for class " << cname << endmsg;
    gInterpreter->EnableAutoLoading();
    gInterpreter->AutoLoad( cname.c_str());
  }
}  

StatusCode SerializeCnvSvc::setDataProvider(IDataProviderSvc* pSvc)  {
  MsgStream log(msgSvc(), name());
  IDataManagerSvc* tmp = m_dataMgr;
  if (pSvc)  {
    StatusCode sc = pSvc->queryInterface(IID_IDataManagerSvc,(void**)&m_dataMgr);
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
      int ObjObjCounter=1;
      RawEvent* raw = new RawEvent();
      for(Objects::iterator i=m_objects.begin(); i != m_objects.end(); ++i)      {    
        TBufferFile rawBuffer(TBuffer::kWrite,256*1024);
	DataObject* pObj = (*i);
	DataObjectPush p(pObj);

	// We build a map so gROOT has to access the whole class database as little as possible
	TClass* cl;
	const type_info& objClass = typeid(*pObj);
	string objClassName = System::typeinfoName(objClass);
	if (classes[objClassName])   {
	  log << MSG::VERBOSE << "Class "<<objClassName<<" already mapped"<<endmsg;
	  cl=classes[objClassName];
	}
	else	  {
	  log << MSG::VERBOSE << "Looking for class "<<objClassName<<" in gROOT"<<endmsg;
	  const char* clName = objClassName.c_str();
	  cl = gROOT->GetClass(clName);
	  classes[objClassName]=cl;
	}
	if (cl==0){
	  log<<MSG::ERROR<<"No valid class found!"<<endmsg;
	  log<<MSG::ERROR<<"     Class Name: "<<objClassName<<endmsg;    
	  return StatusCode::FAILURE;      
	}
        
	std::string loc=pObj->registry()->identifier();
	rawBuffer.WriteString(loc.c_str());
        rawBuffer << (long)pObj->clID();
	rawBuffer.WriteString(cl->GetName());
	cl->Streamer(pObj,rawBuffer);
        
	LinkManager* linkMgr=pObj->linkMgr();
	int numLinks = linkMgr->size();
        rawBuffer << numLinks;
	for (int it = 0; it != numLinks; it++)        {
	  const string& link = linkMgr->link(it)->path();
          rawBuffer.WriteString(link.c_str());
	}
        
	// Write the bank taking into account size limitations
        const char* start = rawBuffer.Buffer();
        const char* end = start+rawBuffer.Length();
        int bnk_count = 0;
        while(start<end) {
          int bnk_len = end-start > 32*1024 ? 32*1024 : end-start;
          RawBank* bank = raw->createBank(ObjObjCounter, RawBank::GaudiSerialize, bnk_count, bnk_len, start);
          raw->adoptBank(bank,true);
          bnk_count++;
          start += bnk_len;
        }
	ObjObjCounter++;
	rawBuffer.Delete();
      }
      
      StatusCode sc = dataProvider()->registerObject("/Event/GaudiSerialize",new DataObject());
      if (!sc.isSuccess())   {
	log<<MSG::ERROR<<"\"/Event/GaudiSerialize\" not successfully registered"<<endmsg;
      }
      sc = dataProvider()->registerObject("/Event/GaudiSerialize/Banks",raw);
      if (!sc.isSuccess())   {
	log<<MSG::ERROR<<"\"/Event/GaudiSerialize/Banks\" not successfully registered"<<endmsg;
      }      
      
    }
    // Rollback: just clear the object buffer!
    m_objects.clear();
    return S_OK;
  }
  catch (std::exception& e)  {
    error(std::string("commitOutput> Caught exception:")+e.what());
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
					  const std::string* par, 
					  const unsigned long* ip,
					  IOpaqueAddress*& refpAddress) 
{
  refpAddress = new SerializeAddress(typ,clid,par[0],par[1],ip[0],ip[1]);
  return StatusCode::SUCCESS;
}

/// Mark an object for write given an object reference
StatusCode 
SerializeCnvSvc::writeObject(DataObject* pObj, IOpaqueAddress*& refpAddr)  {
  if ( pObj ) {
    CLID clid = pObj->clID();
    std::string loc = pObj->registry()->identifier();
    m_objects.push_back(pObj);
    refpAddr = new SerializeAddress(SERIALIZE_StorageType,clid,loc,"",0,0);
    return S_OK;
  }
  return error("markWrite> Current Database is invalid!");
}

/// Read existing object. Open transaction in read mode if not active
StatusCode SerializeCnvSvc::readObject(IOpaqueAddress* pA, DataObject*& refpObj)  {
  char* memory = 0;
  refpObj = 0;
  if ( pA ) {
    MsgStream log(msgSvc(), name());
    IRegistry* pReg = pA->registry();
    std::string id = pReg->identifier();
    RawDataAddress* pAddRaw = dynamic_cast<RawDataAddress*>(pA);
    std::vector<RawBank*> banks;
    std::pair<char*,int> d = pAddRaw->data();
    
    decodeRawBanks(d.first,d.first+d.second,banks);
    for(std::vector<RawBank*>::const_iterator i=banks.begin(); i!=banks.end();++i)  {
      // log << RawEventPrintout::bankHeader(*i) << endmsg;      
    }
    
    for(std::vector<RawBank*>::const_iterator i=banks.begin(); i!=banks.end();++i)  {
      RawBank* readBank = *i;
      if ( readBank->version() == 0 && id == readBank->begin<char>() ) { //We only want banks with version()=0
        int srcID = readBank->sourceID();
        size_t len = 0;
        for(std::vector<RawBank*>::const_iterator k=banks.begin(); k!=banks.end();++k)  {
          RawBank* checkBank = *k;
          if ( checkBank->sourceID() == srcID ) { //Banks with the same sourceID() correspond to the same DataObject
            len += checkBank->size();
          } 
        }
        if ( len > 32*1024 ) { // We need to concatenate banks if the total size of the data is greater than 32*1024
          log << MSG::INFO << "Need to concatenate banks (length is "<<len<<")!" << endmsg;
        }

	//  The TBuffer is filled with as many banks as necessary        
        memory = new char[len];
        char* mem = memory;
        for(std::vector<RawBank*>::const_iterator k=banks.begin(); k!=banks.end();++k)  {
          RawBank* copyBank = *k;
          if ( copyBank->sourceID() == srcID ) {
            memcpy(mem,copyBank->begin<char>(),copyBank->size());
            mem += copyBank->size();
          }
        }
      
	//  The TBuffer is read
        char text[4096];
        long class_id = 0;
        TBufferFile buffer(TBuffer::kRead,len,memory,kFALSE);
        buffer.ReadString(text,sizeof(text));
        buffer >> class_id;        
        buffer.ReadString(text,sizeof(text));
	TClass* cl = gROOT->GetClass(text);        
	char* obj = (char *)cl->New();
        DataObject* pObj = (DataObject*)obj;
	DataObjectPush push(pObj); // This is magic!
        cl->Streamer(obj, buffer);

        if ( pObj ) { // Restore links
          int nlink = 0;
          LinkManager* lnkMgr = pObj->linkMgr();
          buffer >> nlink;
          log << MSG::DEBUG << "Adding links: " << nlink << endmsg;
          
          for (int i=0; i<nlink; ++i) {
            buffer.ReadString(text,sizeof(text));
            log << MSG::INFO << "    ->Adding links: " << text << endmsg;
            lnkMgr->addLink(text,0);
          }
          // Now register addresses! We need to register the leaves of the object (some string magic)
          for(std::vector<RawBank*>::const_iterator j=banks.begin(); j!=banks.end();++j)  {
            RawBank* loopBank = *j;
            if ( loopBank->version() == 0 && loopBank != readBank ) {
              const char* name = loopBank->begin<char>();
              if ( 0 == ::strncmp(name,id.c_str(),id.length()) ) {
                name += id.length();
                if ( strchr(name+1,'/') == 0 ) {
                  RawDataAddress* leafAddr = new RawDataAddress(*pAddRaw);
                  StatusCode sc = m_dataMgr->registerAddress(pReg, name, leafAddr);
                  if ( !sc.isSuccess() )  {
                    leafAddr->release();
                    error("Failed to register address for object "+std::string(name));
                  }
                }
              }
            }
          }
          refpObj = pObj;  // Return the desired object
          delete [] memory;          
          return StatusCode::SUCCESS;
        }        
        delete [] memory;          
        return error("read> Cannot read object "+id);
      }
    }
  }
  delete [] memory;          
  return error("read> Cannot read object "+pA->registry()->identifier()+" ");
}

// Small routine to issue exceptions
StatusCode SerializeCnvSvc::error(CSTR msg)  {
  MsgStream log(messageService(),name());
  log << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}
