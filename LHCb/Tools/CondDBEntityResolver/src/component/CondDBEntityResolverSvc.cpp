// $Id: CondDBEntityResolverSvc.cpp,v 1.2 2006-02-01 19:49:06 marcocle Exp $
// Include files 

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

#include "GaudiKernel/Time.h"

#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/ICondDBCnvSvc.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "CoolKernel/IObject.h"

#include "AttributeList/AttributeList.h"
#include "AttributeList/AttributeExceptions.h"

// local
#include "CondDBEntityResolverSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBEntityResolverSvc
//
// 2005-10-18 : Marco Clemencic
//-----------------------------------------------------------------------------

/// Instantiation of a static factory to create instances of this service
static SvcFactory<CondDBEntityResolverSvc>          CondDBEntityResolverSvc_factory;
const ISvcFactory& CondDBEntityResolverSvcFactory = CondDBEntityResolverSvc_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBEntityResolverSvc::CondDBEntityResolverSvc( const std::string& name, ISvcLocator* svc ):
                                                  Service(name,svc) {

  declareProperty("DetDataSvc", m_detDataSvcName = "DetDataSvc/DetectorDataSvc");
  declareProperty("CondDBCnvSvc", m_condDBCnvSvcName = "CondDBCnvSvc");

}
//=============================================================================
// Destructor
//=============================================================================
CondDBEntityResolverSvc::~CondDBEntityResolverSvc() {}; 


//=========================================================================
//  Initialize the service
//=========================================================================
StatusCode CondDBEntityResolverSvc::initialize ( ) {
  StatusCode sc = Service::initialize();
  if ( ! sc.isSuccess() ) return sc;

  MsgStream log(msgSvc(), name() );
  log << MSG::INFO << "Initializing..." << endmsg;
  
  sc = service(m_detDataSvcName,m_detDataSvc,true);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't locate service " << m_detDataSvcName << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Succesfully located service " << m_detDataSvcName << endmsg;
  }

  sc = service(m_condDBCnvSvcName,m_condDBCnvSvc,true);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't locate service " << m_condDBCnvSvcName << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Succesfully located service " << m_condDBCnvSvcName << endmsg;
  }

  // Initialize the Xerces-C++ XML subsystem
  try {
    xercesc::XMLPlatformUtils::Initialize();
  } catch(const xercesc::XMLException& toCatch) {
    char *message = xercesc::XMLString::transcode(toCatch.getMessage());
    log << MSG::FATAL << "Error during Xerces-c Initialization." << endmsg;
    log << MSG::FATAL << "  Exception message:" << message << endmsg;
    xercesc::XMLString::release(&message);
    return StatusCode::FAILURE;
  }

  log << MSG::INFO << "Successfully initialized." << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Finalize the service
//=========================================================================
StatusCode CondDBEntityResolverSvc::finalize ( ) {
  
  if ( m_condDBCnvSvc ) m_condDBCnvSvc->release();
  if ( m_detDataSvc ) m_detDataSvc->release();  

  // Finalize the Xerces-C++ XML subsystem
  xercesc::XMLPlatformUtils::Terminate();

  return Service::finalize();
}

//=========================================================================
//  Query the interfaces of the object
//=========================================================================
StatusCode CondDBEntityResolverSvc::queryInterface(const InterfaceID& riid,
                                                   void** ppvUnknown){
  if ( IID_IXmlEntityResolverSvc.versionMatch(riid) ) {
    *ppvUnknown = (IXmlEntityResolverSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=========================================================================
//  Return a pointer to the actual implementation of a xercesc::EntityResolver
//=========================================================================
xercesc::EntityResolver *CondDBEntityResolverSvc::resolver() {
  return this;
}

//=========================================================================
//  Create a Xerces-C input source based on the given systemId (publicId is ignored).
//=========================================================================
xercesc::InputSource *CondDBEntityResolverSvc::resolveEntity(const XMLCh *const, const XMLCh *const systemId) {

  MsgStream log(msgSvc(), name()+"::resolveEntity" );
  
  std::string systemIdString;
  
  char *cString = xercesc::XMLString::transcode(systemId);
  if (cString) {
    systemIdString = cString;
    xercesc::XMLString::release(&cString);
  }
  
  log << MSG::DEBUG << "systemId = \"" << systemIdString << "\"" << endmsg;
  
  if ( systemIdString.find("conddb:") != 0 ) {
    // the string does not start with "conddb:", so I cannot handle it
    log << MSG::VERBOSE << "Not a conddb URL" << endmsg;
    return NULL;
  }
  
  std::string path(systemIdString,7,systemIdString.size());
  // extract the channel id
  cool::ChannelId channel = 0;
  size_t column_pos = path.find(":");
  if ( column_pos != path.npos ) { // It means that I have another ':' in the path
    log << MSG::DEBUG << "Found channel id in the URI" << endmsg;
    std::istringstream oss(path.substr(column_pos+1));
    oss >> channel;
    path = path.substr(0,column_pos);
    log << MSG::DEBUG << "path   = " << path << endmsg;
    log << MSG::DEBUG << "ch. id = " << channel << endmsg;
  }
  // work-around a path like "conddb:path/to/folder" should be interpreted as "conddb:/path/to/folder"
  if (path[0] != '/') path = "/" + path;

  Gaudi::Time now;
  if ( m_detDataSvc->validEventTime() ) {
    now =  m_detDataSvc->eventTime();
  } else {
    log << MSG::ERROR << "resolveEntity event time undefined" << endmsg;
    return NULL;
  }

  // outputs
  std::string descr;
  boost::shared_ptr<pool::AttributeList> data;
  Gaudi::Time since, until;
  
  bool found_object = false;

  for ( std::vector<ICondDBAccessSvc*>::iterator accSvc = m_condDBCnvSvc->accessServices().begin();
        accSvc !=  m_condDBCnvSvc->accessServices().end() && ! found_object ; ++accSvc ) {
    found_object = (*accSvc)->getObject(path,now,data,descr,since,until,channel).isSuccess();
  }

  if (found_object) {
    std::string xml_data;
    try {
      (*data)["data"].getValue(xml_data);
    } catch (pool::attribute_not_found &e) {
      log << MSG::ERROR << "I cannot find the data inside COOL object: " << e.what() << endmsg;
      return NULL;
    }
    // Create a copy of the string for the InputSource
    unsigned int buff_size = xml_data.size();
    XMLByte* buff = new XMLByte[buff_size];
    std::string::iterator chIt;
    XMLByte* buff_ptr = buff;
    for ( chIt = xml_data.begin(); chIt != xml_data.end(); ++chIt ){
      *buff_ptr = *chIt;
      ++buff_ptr;
    }
    
    // Create the input source using the string
    xercesc::MemBufInputSource *inputSource = new xercesc::MemBufInputSource((XMLByte*) buff,
                                                                             buff_size,
                                                                             systemId,
                                                                             true);
    // Done!
    return inputSource;
  }
  
  // tell Xerces to use the default action
  return NULL;
}

//=============================================================================
