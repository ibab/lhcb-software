// $Id: RootCnvSvc.cpp,v 1.5 2010-08-17 17:23:04 frankb Exp $
//====================================================================
//  RootCnvSvc implementation
//--------------------------------------------------------------------
//
//  Description: Implementation of the ROOT data storage
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IUpdateable.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiUtils/IIODataManager.h"
#include "RootRefs.h"
#include "RootCnvSvc.h"
#include "RootAddress.h"
#include "RootConverter.h"
#include "RootDatabaseCnv.h"
#include "RootDirectoryCnv.h"
#include "RootNTupleCnv.h"
#include "RootDataConnection.h"
#include "RootUtils.h"

// ROOT include files
#include "TROOT.h"
#include "TClass.h"
#include "TTree.h"
#include "TBranch.h"

using namespace std;
using namespace Gaudi;
typedef const string& CSTR;

DECLARE_SERVICE_FACTORY(RootCnvSvc);

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE
namespace GaudiRoot {  bool patchStreamers(MsgStream& log);    }

namespace { 
  static map<string, TClass*> s_classesNames;
  static map<CLID, TClass*>   s_classesClids;

  union ObjectTypes {
    ObjectTypes(DataObject* p) { Object = p; }
    DataObject*                                             Object;
    KeyedContainer<KeyedObject<long>, Containers::Map>*     KeyMap;
    KeyedContainer<KeyedObject<long>, Containers::HashMap>* KeyHashMap;
    KeyedContainer<KeyedObject<long>, Containers::Array>*   KeyArray;
    StatusCode update(int flag)  {
      IUpdateable* obj = dynamic_cast<IUpdateable*>(Object);
      if ( obj ) return obj->update(flag);
      return S_FAIL;
    }
  };
}

// Standard constructor
RootCnvSvc::RootCnvSvc(CSTR nam, ISvcLocator* svc)
  : ConversionSvc( nam, svc, ROOT_StorageType), 
    m_dataMgr(0), m_ioMgr(0), m_incidentSvc(0), m_current(0), m_setup(0)
{
  m_setup = new RootConnectionSetup();
  declareProperty("ShareFiles",       m_shareFiles       = "NO");
  declareProperty("EnableIncident",   m_incidentEnabled  = false);
  declareProperty("Section",          m_section = "E");
  declareProperty("IOPerfStats",      m_ioPerfStats);

  declareProperty("CacheSize",        m_setup->cacheSize = 10000000);
  declareProperty("AutoFlush",        m_setup->autoFlush = 100);
  declareProperty("LearnEntries",     m_setup->learnEntries = 10);
}

// Standard destructor
RootCnvSvc::~RootCnvSvc() {
  if (m_setup) m_setup->release();
}

// Small routine to issue exceptions
StatusCode RootCnvSvc::error(CSTR msg)  {
  MsgStream log(msgSvc(),name());
  log << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}

// Initialize the Db data persistency service
StatusCode RootCnvSvc::initialize()  {
  IDataProviderSvc* pSvc = 0;
  StatusCode status = ConversionSvc::initialize();
  MsgStream log(msgSvc(), name());
  if ( !status.isSuccess() )
    return error("Failed to initialize ConversionSvc base class.");
  if( !(status=service("IODataManager", m_ioMgr)).isSuccess() )
    return error("Unable to localize interface from service:IODataManager");
  if( !(status=service("IncidentSvc", m_incidentSvc)).isSuccess() )
    return error("Unable to localize interface from service:IncidentSvc");
  if ( !(status=service("EventDataSvc",pSvc)).isSuccess() )
    return error("Failed to get data provider.");
  setDataProvider(pSvc);
  m_setup->setMessageSvc(msgSvc());
  GaudiRoot::patchStreamers(log);
  DataObject data_obj;
  m_classDO = getClass(&data_obj);
  string cname = System::typeinfoName(typeid(RootObjectRefs));
  m_classRefs = gROOT->GetClass(cname.c_str());

  if ( 0 == m_classDO )
    return error("Unable to load class description for DataObject");    
  if ( 0 == m_classRefs )
    return error("Unable to load class description for ObjectRefs");
  return S_OK;
}

// Finalize the Db data persistency service
StatusCode RootCnvSvc::finalize()    {
  MsgStream log(msgSvc(), name());
  if ( m_ioMgr )  {
    if ( ::toupper(m_shareFiles[0]) != 'Y' )  {
      IIODataManager::Connections cons = m_ioMgr->connections(this);
      for(IIODataManager::Connections::iterator i=cons.begin(); i != cons.end(); ++i)  {
	if ( !m_ioPerfStats.empty() )   {
	  RootDataConnection* pc = dynamic_cast<RootDataConnection*>(*i);
	  if ( pc ) pc->saveStatistics(m_ioPerfStats);
	}
        if ( m_ioMgr->disconnect(*i).isSuccess() )  {
          log << MSG::INFO << "Disconnected data IO:" << (*i)->fid();
	  log << "[" << (*i)->pfn() << "]";
	  log << endmsg;
          delete (*i);
        }
      }
    }
    else  {
      log << MSG::INFO << "File sharing enabled. Do not retire files." << endmsg;
    }
    releasePtr(m_ioMgr);
  }
  releasePtr(m_dataMgr);
  releasePtr(m_incidentSvc);
  return ConversionSvc::finalize();
}

// ConversionSvc overload: Create new Converter using factory
IConverter* RootCnvSvc::createConverter(long typ,const CLID& wanted,const ICnvFactory*)   {
  if ( wanted == CLID_StatisticsFile )
    return new RootDatabaseCnv(typ,wanted,serviceLocator().get(),this);
  else if ( wanted == CLID_StatisticsDirectory )
    return new RootDirectoryCnv(typ,wanted,serviceLocator().get(),this);
  else if ( wanted == CLID_RowWiseTuple )
    return new RootNTupleCnv(typ,wanted,serviceLocator().get(),this);
  else if ( wanted == CLID_ColumnWiseTuple )
    return new RootNTupleCnv(typ,wanted,serviceLocator().get(),this);
  else
    return new RootConverter(typ,wanted,serviceLocator().get(),this);
}

// ConversionSvc overload: Load the class (dictionary) for the converter 
void RootCnvSvc::loadConverter(DataObject* pObject) {
  if (pObject) {
    MsgStream log(msgSvc(), name());
    string cname = System::typeinfoName(typeid(*pObject));
    log << MSG::DEBUG << "Trying to 'Autoload' dictionary for class " << cname << endmsg;
    TClass* cl = s_classesNames[cname];
    if ( 0 == cl ) {
      cl = gROOT->GetClass(cname.c_str());
      if ( cl ) {
	s_classesNames[cname] = cl;
	s_classesClids[pObject->clID()] = cl;
      }
    }
  }
}  

// Helper: Get TClass for a given DataObject pointer
TClass* RootCnvSvc::getClass(DataObject* pObject) {
  map<CLID, TClass*>::iterator i=s_classesClids.find(pObject->clID());
  if ( i != s_classesClids.end() ) return (*i).second;
  loadConverter(pObject);
  i=s_classesClids.find(pObject->clID());
  if ( i != s_classesClids.end() ) return (*i).second;

  string cname = System::typeinfoName(typeid(*pObject));
  throw runtime_error("Unknown ROOT class for object:"+cname);
  return 0;
}

// Connect to data provider service. Re-connects to data manager service.
StatusCode RootCnvSvc::setDataProvider(IDataProviderSvc* pSvc)  {
  MsgStream log(msgSvc(), name());
  IDataManagerSvc* tmp = m_dataMgr;
  if (pSvc)  {
    StatusCode sc = pSvc->queryInterface(IDataManagerSvc::interfaceID(),(void**)&m_dataMgr);
    if ( !sc.isSuccess() )    {
      return error("Cannot connect to \"IDataManagerSvc\" interface.");
    }
  }
  if ( tmp ) tmp->release();
  return ConversionSvc::setDataProvider(pSvc);
}

// Connect the output file to the service with open mode.
StatusCode RootCnvSvc::connectOutput(CSTR dsn, CSTR openMode)   {
  StatusCode sc = S_FAIL;
  int mode = 0;
  m_current = 0;
  if ( ::strncasecmp(openMode.c_str(),"RECREATE",3)==0 )
    sc = connectDatabase(dsn, mode=IDataConnection::RECREATE, &m_current);
  else if ( ::strncasecmp(openMode.c_str(),"NEW",1)==0 )
    sc = connectDatabase(dsn, mode=IDataConnection::CREATE, &m_current);
  else if ( ::strncasecmp(openMode.c_str(),"CREATE",1)==0 )
    sc = connectDatabase(dsn, mode=IDataConnection::CREATE, &m_current);
  else if ( ::strncasecmp(openMode.c_str(),"UPDATE",1)==0 )
    sc = connectDatabase(dsn, mode=IDataConnection::UPDATE, &m_current);
  if ( sc.isSuccess() && m_current && m_current->isConnected() )  {
    return S_OK;
  }
  m_incidentSvc->fireIncident(Incident(dsn,IncidentType::FailOutputFile));
  error("The dataset "+dsn+" cannot be opened in mode "+openMode+". [Invalid mode]");
  return sc;
}

// Connect the output file to the service with open mode.
StatusCode
RootCnvSvc::connectDatabase(CSTR dataset, int mode, RootDataConnection** con)  {
  try {
    IDataConnection* c = m_ioMgr->connection(dataset);
    bool fire_incident = false;
    bool enable_stats = false;
    if ( !c )  {
      auto_ptr<IDataConnection> connection(new RootDataConnection(this,dataset,m_setup));
      StatusCode sc = (mode != IDataConnection::READ)
	? m_ioMgr->connectWrite(connection.get(),IDataConnection::IoType(mode),"ROOT_TREE")
	: m_ioMgr->connectRead(false,connection.get());
      c = sc.isSuccess() ? m_ioMgr->connection(dataset) : 0;
      if ( c )   {
	fire_incident = m_incidentEnabled && (0 != (mode&(IDataConnection::UPDATE|IDataConnection::READ)));
	enable_stats  = !m_ioPerfStats.empty() && 0 != (mode&IDataConnection::READ);
	connection.release();
      }
      else  {
	return sc;
      }
    }
    RootDataConnection* pc = dynamic_cast<RootDataConnection*>(c);
    if ( pc )  {
      if ( !pc->isConnected() ) pc->connectRead();
      if ( enable_stats ) pc->enableStatistics(m_section);
      *con = pc;
      pc->resetAge();
    }
    if ( *con )  {
      if ( fire_incident ) {
	//m_incidentSvc->fireIncident(ContextIncident<IOpaqueAddress*>(c->fid(),"FILE_OPEN_READ",pAddr));
      }
      return S_OK;
    }
    m_incidentSvc->fireIncident(Incident(dataset,IncidentType::FailOutputFile));
    return S_FAIL;
  }
  catch (exception& e)  {
    m_incidentSvc->fireIncident(Incident(dataset,IncidentType::FailOutputFile));
    return error(string("connectDatabase> Caught exception:")+e.what());
  }
  catch (...)   {
    m_incidentSvc->fireIncident(Incident(dataset,IncidentType::FailOutputFile));
    return error("connectDatabase> Unknown Fatal Exception for "+dataset);
  }
}

// Conect output stream (valid until overwritten)
StatusCode RootCnvSvc::connectOutput(CSTR db_name)  {
  return connectOutput(db_name, "NEW");
}

// Commit pending output on open container
StatusCode  RootCnvSvc::commitOutput(CSTR /* dsn */, bool /* doCommit */) {
  return S_OK;
}

// Disconnect from an existing data stream.
StatusCode RootCnvSvc::disconnect(CSTR dataset)  {
  IDataConnection* c = m_ioMgr->connection(dataset);
  return c ? m_ioMgr->disconnect(c) : S_FAIL;
}

// IAddressCreator implementation: Address creation
StatusCode RootCnvSvc::createAddress(long  typ,
				     const CLID& clid,
				     const string* par, 
				     const unsigned long* ip,
				     IOpaqueAddress*& refpAddress) 
{
  refpAddress = new RootAddress(typ,clid,par[0],par[1],ip[0],ip[1]);
  return S_OK;
}

// Mark an object for write given an object reference
StatusCode RootCnvSvc::i__createRep(DataObject* pObj, IOpaqueAddress*& refpAddr)  {
  refpAddr = 0;
  if ( pObj ) {
    CLID       clid = pObj->clID();
    IRegistry* pR   = pObj->registry();
    string     p[2] = {m_current->fid(), pR->identifier()};
    TClass*    cl   = (pObj->clID() == CLID_DataObject) ? m_classDO : getClass(pObj);
    pair<int,unsigned long> ret = m_current->saveObj(m_section,p[1],cl,pObj);
    if ( ret.first > 1 || (pObj->clID() == CLID_DataObject && ret.first==1) ) {
      unsigned long ip[2] = {0,ret.second};
      return createAddress(repSvcType(),clid,p,ip,refpAddr);
    }
    return error("Failed to write object data for:"+p[1]);
  }
  return error("markWrite> Current Database is invalid!");
}

// Save object references to data file
StatusCode RootCnvSvc::i__fillRepRefs(IOpaqueAddress* /* pA */, DataObject* pObj) {
  if ( pObj ) {
    typedef std::vector<IRegistry*> Leaves;
    Leaves leaves;
    RootObjectRefs refs;
    IRegistry* pR = pObj->registry();
    StatusCode status = m_dataMgr->objectLeaves(pObj, leaves);
    if ( status.isSuccess() )  {
      RootRef ref;
      LinkManager* pLinks = pObj->linkMgr();
      for(Leaves::iterator i=leaves.begin(), iend=leaves.end(); i != iend; ++i)  {
	if ( (*i)->address() ) {
	  m_current->makeRef(*i,ref);
	  ref.entry = (*i)->address()->ipar()[1];
	  refs.refs.push_back(ref);
	}
      }
      for(int i = 0, n=pLinks->size(); i < n; ++i)  {
	LinkManager::Link* lnk = pLinks->link(i);
	int link_id = m_current->makeLink(lnk->path());
	refs.links.push_back(link_id);
      }
      pair<int,unsigned long> ret = m_current->save(m_section,pR->identifier()+"#R",m_classRefs,&refs);
      if ( ret.first > 1 ) {
	return S_OK;
      }
    }
  }
  return S_FAIL;
}

// Read existing object. Open transaction in read mode if not active
StatusCode RootCnvSvc::i__createObj(IOpaqueAddress* pA, DataObject*& refpObj)  {
  refpObj = 0;
  if ( pA ) {
    RootDataConnection* con = 0;
    const string* par = pA->par();
    unsigned long* ipar = const_cast<unsigned long*>(pA->ipar());
    StatusCode sc = connectDatabase(par[0],IDataConnection::READ,&con);
    if ( sc.isSuccess() ) {
      ipar[0] = (unsigned long)con;
      DataObject* pObj = 0;
      int nb = con->loadObj(m_section,par[1],ipar[1],pObj);
      if ( nb > 1 || (pObj->clID() == CLID_DataObject && nb==1) ) {
	refpObj = pObj;
	return S_OK;
      }
      delete pObj;
    }
    string tag = par[0]+":"+par[1];
    if ( m_badFiles.find(tag) == m_badFiles.end() ) {
      m_badFiles.insert(tag);
      return error("createObj> Cannot access the object:"+tag);
    }
    return S_FAIL;
  }
  return S_FAIL;
}

// Resolve the references of the created transient object.
StatusCode RootCnvSvc::i__fillObjRefs(IOpaqueAddress* pA, DataObject* pObj) {
  if ( pA && pObj )  {
    const unsigned long* ipar = pA->ipar();
    RootDataConnection* con = (RootDataConnection*)ipar[0];
    if ( con ) {
      RootObjectRefs refs;
      const string* par = pA->par();
      int nb = con->loadRefs(m_section,par[1],ipar[1],refs);
      if ( nb >= 1 ) {
	string npar[3];
	unsigned long nipar[2];
	IOpaqueAddress* nPA;
	LinkManager* mgr = pObj->linkMgr();
	for(vector<int>::const_iterator i=refs.links.begin(); i!=refs.links.end();++i) {
	  mgr->addLink(con->getLink(*i),0);
	}
	for(size_t j=0, n=refs.refs.size(); j<n; ++j)  {
	  const RootRef& r = refs.refs[j];
	  npar[0] = con->getDb(r.dbase);
	  npar[1] = con->getCont(r.container);
	  npar[2] = con->getLink(r.link);
	  nipar[0] = 0;
	  nipar[1] = r.entry;
	  StatusCode sc = addressCreator()->createAddress(r.svc,r.clid,npar,nipar,nPA);
	  if ( sc.isSuccess() ) {
	    //cout << "Register:" << pA->registry()->identifier() << " " << npar[2] << endl;
	    sc = m_dataMgr->registerAddress(pA->registry(),npar[2],nPA);
	    if ( sc.isSuccess() ) {
	      continue;
	    }
	  }
	  error("Failed to create address!!!!");
	  return S_FAIL;
	}
	ObjectTypes obj(pObj);
	const CLID id = (pObj->clID()&0xFFFF0000);
	switch(id) {
	case CLID_ObjectList:              /* ObjectList               */
	  return S_OK;
	case CLID_ObjectVector:            /* ObjectVector             */
	  return S_OK;
	case CLID_ObjectVector+0x0030000:  /* Keyed object map         */
	  obj.KeyMap->configureDirectAccess();
	  return S_OK;
	case CLID_ObjectVector+0x0040000:  /* Keyed object hashmap     */
	  obj.KeyHashMap->configureDirectAccess();
	  return S_OK;
	case CLID_ObjectVector+0x0050000:  /* Keyed indirection array  */
	  obj.KeyArray->configureDirectAccess();
	  return S_OK;
	case 0:                            /* Any other object         */
	  return S_OK;
	default:
	  return obj.update(0);
	}
      }
    }
    return S_FAIL;
  }
  return error("read> Cannot read object -- no valid object address present ");
}
