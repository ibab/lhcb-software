// $Id: RootTreeCnvSvc.cpp,v 1.1 2009-12-15 15:37:25 frankb Exp $
//====================================================================
//	RootTreeCnvSvc implementation
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
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IUpdateable.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiUtils/IIODataManager.h"
#include "RootCnv/RootRefs.h"
#include "RootTreeCnvSvc.h"
#include "RootConverter.h"
#include "RootDataConnection.h"

// ROOT include files
#include "TROOT.h"
#include "TBranch.h"
#include "TClass.h"
#include "TInterpreter.h"

using namespace std;
using namespace Gaudi;
typedef const string& CSTR;

DECLARE_SERVICE_FACTORY(RootTreeCnvSvc);

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE
namespace Gaudi {
  void popCurrentDataObject();
  void pushCurrentDataObject(DataObject** pobjAddr);
}

//namespace GaudiPoolDb  {  bool patchStreamers(MsgStream& s); }

namespace {
    /// Release a pointer
    template<class T> inline void releasePtr(T*& p)  {
      if ( 0 != p )    {
	p->release();
	p = 0;
      }
    }
  struct DataObjectPush {
    DataObject* m_ptr;
    DataObjectPush(DataObject* p) : m_ptr(p) {
      Gaudi::pushCurrentDataObject(&m_ptr);
    }
    ~DataObjectPush() {
      Gaudi::popCurrentDataObject();
    }
  };
  
  union ObjectTypes {
    ObjectTypes(DataObject* p) { Object = p; }
    DataObject*                                             Object;
    KeyedContainer<KeyedObject<long>, Containers::Map>*     KeyMap;
    KeyedContainer<KeyedObject<long>, Containers::HashMap>* KeyHashMap;
    KeyedContainer<KeyedObject<long>, Containers::Array>*   KeyArray;
    StatusCode update(int flag)  {
      IUpdateable* obj = dynamic_cast<IUpdateable*>(Object);
      if ( obj ) {
        return obj->update(flag);
      }
      return S_FAIL;
    }
  };
}

/// Standard constructor
RootTreeCnvSvc::RootTreeCnvSvc(CSTR nam, ISvcLocator* svc)
  : ConversionSvc( nam, svc, ROOT_StorageType), 
    m_dataMgr(0), m_ioMgr(0), m_incidentSvc(0), 
    m_current(0)
{
  declareProperty("ShareFiles",       m_shareFiles       = "NO");
  declareProperty("EnableIncident",   m_incidentEnabled  = false);
}

// Small routine to issue exceptions
StatusCode RootTreeCnvSvc::error(CSTR msg)  {
  MsgStream log(messageService(),name());
  log << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}

/// Initialize the Db data persistency service
StatusCode RootTreeCnvSvc::initialize()  {
  StatusCode status = ConversionSvc::initialize();
  MsgStream log(messageService(), name());
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Failed to initialize ConversionSvc base class." << endmsg;
    return status;
  }
  if( !(status=service("IODataManager", m_ioMgr)).isSuccess() ) {
    log << MSG::ERROR
        << "Unable to localize interface from service:IODataManager" << endmsg;
    return status;
  }
  if( !(status=service("IncidentSvc", m_incidentSvc)).isSuccess() ) {
    log << MSG::ERROR
        << "Unable to localize interface from service:IncidentSvc" << endmsg;
    return status;
  }
  //GaudiPoolDb::patchStreamers(log);
  return S_OK;
}

/// Finalize the Db data persistency service
StatusCode RootTreeCnvSvc::finalize()    {
  MsgStream log(messageService(), name());
  if ( m_ioMgr )  {
    if ( ::toupper(m_shareFiles[0]) != 'Y' )  {
      IIODataManager::Connections cons = m_ioMgr->connections(this);
      for(IIODataManager::Connections::iterator i=cons.begin(); i != cons.end(); ++i)  {
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

/// Create new Converter using factory
IConverter* RootTreeCnvSvc::createConverter(long typ,const CLID& wanted,const ICnvFactory*)   {
  return new RootConverter(typ,wanted,serviceLocator().get(),this);
}

static map<string, TClass*> s_classesNames;
static map<CLID, TClass*> s_classesClids;
void RootTreeCnvSvc::loadConverter(DataObject* pObject) {
  if (pObject) {
    MsgStream log(msgSvc(), name());
    string cname = System::typeinfoName(typeid(*pObject));
    log << MSG::DEBUG << "Trying to 'Autoload' dictionary for class " << cname << endmsg;
    TClass* cl = s_classesNames[cname];
    if ( 0 == cl ) {
      gInterpreter->EnableAutoLoading();
      gInterpreter->AutoLoad( cname.c_str());
      cl = gROOT->GetClass(cname.c_str());
      if ( cl ) {
	s_classesNames[cname] = cl;
	s_classesClids[pObject->clID()] = cl;
      }
    }
  }
}  

static TClass* getClass(DataObject* pObject) {
  map<CLID, TClass*>::iterator i=s_classesClids.find(pObject->clID());
  if ( i != s_classesClids.end() ) return (*i).second;
  string cname = System::typeinfoName(typeid(*pObject));
  throw runtime_error("Unknown ROOT class for object:"+cname);
  return 0;
}

StatusCode RootTreeCnvSvc::setDataProvider(IDataProviderSvc* pSvc)  {
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
StatusCode RootTreeCnvSvc::connectOutput(CSTR dsn, CSTR openMode)   {
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

/// Connect the output file to the service with open mode.
StatusCode
RootTreeCnvSvc::connectDatabase(CSTR dataset, int mode, RootDataConnection** con)  {
  try {
    IDataConnection* c = m_ioMgr->connection(dataset);
    bool fire_incident = false;
    if ( !c )  {
      auto_ptr<IDataConnection> connection(new RootDataConnection(this,dataset));
      StatusCode sc = (mode != IDataConnection::READ)
	? m_ioMgr->connectWrite(connection.get(),IDataConnection::IoType(mode),"ROOT_TREE")
	: m_ioMgr->connectRead(false,connection.get());
      c = sc.isSuccess() ? m_ioMgr->connection(dataset) : 0;
      if ( c )   {
	fire_incident = m_incidentEnabled && (0 != (mode&(IDataConnection::UPDATE|IDataConnection::READ)));
	connection.release();
      }
      else  {
	return sc;
      }
    }
    RootDataConnection* pc = dynamic_cast<RootDataConnection*>(c);
    if ( pc )  {
      if ( !pc->isConnected() ) pc->connectRead();
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
StatusCode RootTreeCnvSvc::connectOutput(CSTR db_name)  {
  return connectOutput(db_name, "NEW");
}

// Commit pending output on open container
StatusCode  RootTreeCnvSvc::commitOutput(CSTR /* dsn */, bool /* doCommit */) {
  return S_OK;
}

/// Disconnect from an existing data stream.
StatusCode RootTreeCnvSvc::disconnect(CSTR dataset)  {
  IDataConnection* c = m_ioMgr->connection(dataset);
  return c ? m_ioMgr->disconnect(c) : S_FAIL;
}

/// IAddressCreator implementation: Address creation
StatusCode RootTreeCnvSvc::createAddress(long  typ,
					 const CLID& clid,
					 const string* par, 
					 const unsigned long* ip,
					 IOpaqueAddress*& refpAddress) 
{
  refpAddress = new GenericAddress(typ,clid,par[0],par[1],ip[0],ip[1]);
  return S_OK;
}

/// Mark an object for write given an object reference
StatusCode RootTreeCnvSvc::createRep(DataObject* pObj, IOpaqueAddress*& refpAddr)  {
  refpAddr = 0;
  if ( pObj ) {
    DataObjectPush push(pObj);
    CLID clid = pObj->clID();
    IRegistry* pR = pObj->registry();
    string loc = pR->identifier();
    string br_name = loc+"___Data";
    TClass* cl = (pObj->clID() == CLID_DataObject) ? m_classDO : getClass(pObj);
    TBranch* b = m_current->getBranch(cl, br_name);
    if ( b ) {
      int p1 = b->GetEntries()-1;
      b->SetAddress(pObj);
      int nb = b->Fill();
      if ( nb > 1 ) {
	refpAddr = new GenericAddress(ROOT_StorageType,clid,m_current->fid(),loc,0,p1);
	return S_OK;
      }
    }
  }
  return error("markWrite> Current Database is invalid!");
}

/// Save object references to data file
StatusCode RootTreeCnvSvc::fillRepRefs(IOpaqueAddress* pA, DataObject* pObj) {
  if ( pA && pObj ) {
    ObjectRefs refs;
    IRegistry* pR = pObj->registry();
    string br_name = pR->identifier()+"___Refs";
    TBranch* b = m_current->getBranch(m_classRefs, br_name);
    if ( b ) {
      typedef std::vector<IRegistry*> Leaves;
      Leaves leaves;
      StatusCode status = m_dataMgr->objectLeaves(pObj, leaves);
      if ( status.isSuccess() )  {
	RootRef ref;
	LinkManager* pLinks = pObj->linkMgr();
	for(Leaves::iterator i=leaves.begin(), iend=leaves.end(); i != iend; ++i)  {
	  m_current->makeRef(*i,ref);
          ref.entry = (*i)->address()->ipar()[1];
	  refs.refs.push_back(ref);
	}
	for(int i = 0, n=pLinks->size(); i < n; ++i)  {
	  LinkManager::Link* lnk = pLinks->link(i);
	  int link_id = m_current->makePath(lnk->path());
	  refs.links.push_back(link_id);
	}
	b->SetAddress(&refs);
	int nb = b->Fill();
	if ( nb > 1 ) {
	  return S_OK;
	}
      }
    }
  }
  return S_FAIL;
}

/// Read existing object. Open transaction in read mode if not active
StatusCode RootTreeCnvSvc::createObj(IOpaqueAddress* pA, DataObject*& refpObj)  {
  refpObj = 0;
  if ( pA ) {
    RootDataConnection* con = 0;
    IRegistry* pR = pA->registry();
    const string* par = pA->par();
    const unsigned long* ipar = pA->ipar();
    StatusCode sc = connectDatabase(par[0],IDataConnection::READ,&con);
    if ( sc.isSuccess() ) {
      TBranch* b1 = con->getBranch(par[1]+"___Data");
      TBranch* b2 = con->getBranch(par[1]+"___Refs");
      if ( b1 && b2 ) {
	TClass* cl = gROOT->GetClass(b1->GetClassName(),kTRUE);
	if ( cl ) {
	  ObjectRefs refs;
	  DataObject* pObj = (DataObject*)cl->New();
	  b1->SetAddress(pObj);
	  b2->SetAddress(&refs);
	  int nb1 = b1->GetEntry(ipar[1]);
	  int nb2 = b2->GetEntry(ipar[1]);
	  if ( nb1 > 0 && nb2 > 0 ) {
	    string npar[3];
	    unsigned long nipar[2];
	    IOpaqueAddress* nPA;
	    LinkManager* mgr = pObj->linkMgr();
	    for(vector<int>::const_iterator i=refs.links.begin(); i!=refs.links.end();++i) {
	      mgr->addLink(con->getPath(*i),0);
	    }
	    for(size_t j=0, n=refs.refs.size(); j<n; ++j)  {
	      const RootRef& r = refs.refs[j];
	      npar[0] = con->getDb(r.dbase);
	      npar[1] = con->getCont(r.container);
	      npar[2] = con->getLink(r.link);
	      nipar[0] = j;
	      nipar[1] = r.entry;
	      sc = addressCreator()->createAddress(r.svc,r.clid,npar,nipar,nPA);
	      if ( sc.isSuccess() ) {
		sc = m_dataMgr->registerAddress(pR,npar[2],nPA);
		if ( sc.isSuccess() ) {
		  continue;
		}
	      }
	      return S_FAIL;
	    }
	    refpObj = pObj;
	    return S_OK;
	  }
	}
      }
    }
  }
  return S_FAIL;
}

/// Resolve the references of the created transient object.
StatusCode RootTreeCnvSvc::fillObjRefs(IOpaqueAddress* pA, DataObject* pObj) {
  if ( pA && pObj )  {
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
    return S_FAIL;
  }
  return error("read> Cannot read object -- no valid object address present ");
}
