// $Id: CondDBDispatcherSvc.cpp,v 1.4 2007-02-14 16:13:31 marcocle Exp $
// Include files

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"

// local
#include "CondDBDispatcherSvc.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBDispatcherSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBDispatcherSvc
//
// 2006-07-10 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBDispatcherSvc::CondDBDispatcherSvc( const std::string& name, ISvcLocator* svcloc ):
  Service(name,svcloc),
  m_mainDB(0),
  m_alternatives()
{
  declareProperty("MainAccessSvc", m_mainAccessSvcName = "CondDBAccessSvc" );
  declareProperty("Alternatives",  m_alternativesDeclaration               );
}

//=============================================================================
// Destructor
//=============================================================================
CondDBDispatcherSvc::~CondDBDispatcherSvc() {} 

//=============================================================================
// queryInterface
//=============================================================================
StatusCode CondDBDispatcherSvc::queryInterface(const InterfaceID& riid,
                                               void** ppvUnknown){
  if ( IID_ICondDBReader.versionMatch(riid) ) {
    *ppvUnknown = (ICondDBReader*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBDispatcherSvc::initialize(){
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );

  log << MSG::DEBUG << "Initialize" << endmsg;
  
  // locate the main access service
  sc = service(m_mainAccessSvcName,m_mainDB,true);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate " << m_mainAccessSvcName << endreq;
    return sc;
  }

  // locate all the alternative AccessSvcs
  std::vector<std::string>::iterator decl;
  for ( decl = m_alternativesDeclaration.begin(); decl != m_alternativesDeclaration.end(); ++decl ) {
    std::string::size_type pos = decl->find('=');
    if ( pos == std::string::npos ) {
      log << MSG::ERROR << "Malformed declaration of alternative: it sould be '/path/in/cool=ServiceType/ServiceName'" << endmsg;
      return StatusCode::FAILURE;
    }
    
    std::string svcName(*decl,pos+1);
    std::string altPath(*decl,0,pos);
    
    if ( m_alternatives.find(altPath) != m_alternatives.end() ) {
      log << MSG::ERROR << "More than one alternative for path " << altPath << endreq;
      return StatusCode::FAILURE;
    }

    ICondDBReader *svcPtr;
    sc = service(svcName,svcPtr,true);
    if (  !sc.isSuccess() ) {
      log << MSG::ERROR << "Could not locate " << svcName << endreq;
      return sc;
    }

    m_alternatives[altPath] = svcPtr;
    log << MSG::DEBUG << "Retrieved '" << svcName << "' (for path '" << altPath << "')" << endreq;

  }
  
  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBDispatcherSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;

  if (m_mainDB) {
    m_mainDB->release();
    m_mainDB = 0;
  }

  std::map<std::string,ICondDBReader*>::iterator alt;
  for ( alt = m_alternatives.begin(); alt != m_alternatives.end(); ++alt ) {
    if (alt->second) alt->second->release();
  }
  m_alternatives.clear();

  return Service::finalize();
}

//=========================================================================
//  find the appropriate alternative
//=========================================================================
ICondDBReader *CondDBDispatcherSvc::alternativeFor(const std::string &path) {
  MsgStream log(msgSvc(), name() );

  log << MSG::VERBOSE << "Get alternative DB for '" << path << "'" << endmsg;
  if ( path.empty() || (path == "/") ) {
    log << MSG::VERBOSE << "Root node: using '" << m_mainAccessSvcName << "'" << endmsg;
    return m_mainDB;
  }
  
  // loop over alternatives
  std::map<std::string,ICondDBReader*>::reverse_iterator alt;
  for ( alt = m_alternatives.rbegin(); alt != m_alternatives.rend(); ++alt ) {
    if ( m_outputLevel <= MSG::VERBOSE ) {
      log << MSG::VERBOSE << "Comparing with " << alt->first << endmsg;
    }
    if ( ( path.size() >= alt->first.size() ) &&
         ( path.substr(0,alt->first.size()) == alt->first ) ){
      if ( m_outputLevel <= MSG::VERBOSE ) {
        IService *svc = dynamic_cast<IService*>(alt->second);
        log << MSG::VERBOSE << "Using '" ;
        if (svc) log << svc->name();
        else log << "unknown";
        log << "'" << endmsg;
      }
      
      return alt->second;
    }
  }

  log << MSG::VERBOSE << "Not found: using '" << m_mainAccessSvcName << "'" << endmsg;
  return m_mainDB;
}

//=========================================================================
//  retrieve an object
//=========================================================================
StatusCode CondDBDispatcherSvc::getObject (const std::string &path, const Gaudi::Time &when,
                                           DataPtr &data,
                                           std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel) {
  return alternativeFor(path)->getObject(path,when,data,descr,since,until,channel);
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBDispatcherSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names) {
  return alternativeFor(path)->getChildNodes(path,node_names);
}


//=============================================================================
