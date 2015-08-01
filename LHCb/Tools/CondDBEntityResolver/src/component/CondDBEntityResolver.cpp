// $Id: CondDBEntityResolver.cpp,v 1.2 2008-07-17 17:36:21 marcocle Exp $
// Include files

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/Time.h"
#include "GaudiKernel/GaudiException.h"

#include "DetCond/ICondDBReader.h"

#include <xercesc/util/XMLString.hpp>

#include "XmlTools/ValidInputSource.h"

#include "CoolKernel/IObject.h"
#include "CoolKernel/IRecord.h"
#include "CoolKernel/RecordException.h"

#include "CondDBCompression.h"

// local
#include "CondDBEntityResolver.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBEntityResolverSvc
//
// 2005-10-18 : Marco Clemencic
//-----------------------------------------------------------------------------

// Factory implementation
DECLARE_TOOL_FACTORY(CondDBEntityResolver)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBEntityResolver::CondDBEntityResolver( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent ):
  AlgTool(type,name,parent),
  m_condDBReader(0),
  m_detDataSvc(0)
{

  declareInterface<IXmlEntityResolver>(this);
  declareInterface<IFileAccess>(this);

  declareProperty("DetDataSvc", m_detDataSvcName = "DetDataSvc/DetectorDataSvc");
  declareProperty("CondDBReader", m_condDBReaderName = "CondDBCnvSvc");

  m_protocols.push_back("conddb");
}
//=============================================================================
// Destructor
//=============================================================================
CondDBEntityResolver::~CondDBEntityResolver() {}


//=========================================================================
//  Initialize the service
//=========================================================================
StatusCode CondDBEntityResolver::initialize ( ) {
  StatusCode sc = AlgTool::initialize();
  if ( ! sc.isSuccess() ) return sc;

  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Initializing..." << endmsg;

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

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Successfully initialized." << endmsg;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Finalize the service
//=========================================================================
StatusCode CondDBEntityResolver::finalize ( ) {

  if ( m_condDBReader ) {
    m_condDBReader->release();
    m_condDBReader = 0;
  }
  if ( m_detDataSvc ) {
    m_detDataSvc->release();
    m_detDataSvc = 0;
  }

  // Finalize the Xerces-C++ XML subsystem
  xercesc::XMLPlatformUtils::Terminate();

  return AlgTool::finalize();
}

//=========================================================================
//  Return a pointer to the actual implementation of a xercesc::EntityResolver
//=========================================================================
xercesc::EntityResolver *CondDBEntityResolver::resolver() {
  return this;
}

//=========================================================================
// Return the pointer to the CondDBReader (loading it if not yet done).
//=========================================================================
ICondDBReader *CondDBEntityResolver::condDBReader() {
  if (!m_condDBReader) {
    StatusCode sc = service(m_condDBReaderName,m_condDBReader,true);
    if( !sc.isSuccess() ) {
      throw GaudiException("Can't locate service " + m_condDBReaderName,
                           name(),StatusCode::FAILURE);
    } else {
      MsgStream log(msgSvc(), name());
      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
        log << MSG::DEBUG << "Successfully located service " << m_condDBReaderName << endmsg;
    }
  }
  return m_condDBReader;
}
//=========================================================================
// Return the pointer to the detector data service (loading it if not yet done).
//=========================================================================
IDetDataSvc *CondDBEntityResolver::detDataSvc() {
  if (!m_detDataSvc) {
    StatusCode sc = service(m_detDataSvcName,m_detDataSvc,true);
    if( !sc.isSuccess() ) {
      throw GaudiException("Can't locate service " + m_detDataSvcName,
                           name(),StatusCode::FAILURE);
    } else {
      MsgStream log(msgSvc(), name());
      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
        log << MSG::DEBUG << "Successfully located service " << m_detDataSvcName << endmsg;
    }
  }
  return m_detDataSvc;
}
//=========================================================================
// fill validity limits and data (str) with the content retrieved from the url
//=========================================================================
StatusCode CondDBEntityResolver::i_getData(const std::string &url,
                                           Gaudi::Time &since, Gaudi::Time &until,
                                           std::string &str){
  MsgStream log(msgSvc(), name());

  std::string path;
  if (url.substr(0,7) == "conddb:") {
    if (url.substr(7,2) == "//") {
      path = url.substr(9); // url starts with "conddb://"
    } else {
      path = url.substr(7); // url starts with "conddb:"
    }
  } else {
    path = url;
  }

  // extract the channel id
  cool::ChannelId channel = 0;
  size_t column_pos = path.find(":");
  if ( column_pos != path.npos ) { // It means that I have another ':' in the path
    std::istringstream oss(path.substr(column_pos+1));
    oss >> channel;
    path = path.substr(0,column_pos);
    if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
      log << MSG::DEBUG << "Found channel id in the URI" << endmsg;
      log << MSG::DEBUG << "path   = " << path << endmsg;
      log << MSG::DEBUG << "ch. id = " << channel << endmsg;
    }
  }
  // work-around a path like "conddb:path/to/folder" should be interpreted as "conddb:/path/to/folder"
  if (path[0] != '/') path = "/" + path;

  // Extract the COOL field name from the condition path
  // "conddb:/path/to/field@folder"
  std::string data_field_name = "data"; // default value
  std::string::size_type at_pos = path.find('@');
  if ( at_pos != path.npos ) {
    std::string::size_type slash_pos = path.rfind('/',at_pos);
    if ( slash_pos+1 < at_pos ) { // item name is not null
      data_field_name = path.substr(slash_pos+1,at_pos - (slash_pos +1));
    } // if I have "/@", I should use the default ("data")
    // always remove '@' from the path
    path = path.substr(0,slash_pos+1) +  path.substr(at_pos+1);
  }

  Gaudi::Time now;
  if ( detDataSvc()->validEventTime() ) {
    now =  detDataSvc()->eventTime();
  } else {
    log << MSG::ERROR << "event time undefined" << endmsg;
    return StatusCode::FAILURE;
  }

  // outputs
  std::string descr;
  ICondDBReader::DataPtr data;

  StatusCode sc = condDBReader()->getObject(path,now,data,descr,since,until,channel);
  if (sc.isSuccess()) {
    if ( data.get() == NULL ) {
      log << MSG::ERROR << "Cannot find any data at " << url << endmsg;
      return StatusCode::FAILURE;
    }
    try {
      // try to copy the data into the istringstream
      std::string str2 = (*data)[data_field_name].data<std::string>();
      str = CondDBCompression::decompress(str2);
    } catch (cool::RecordSpecificationUnknownField &e) {
      log << MSG::ERROR << "I cannot find the data inside COOL object: "
          << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
  }
  return sc;
}
//=========================================================================
//  Returns an input stream to read from the opened file.
//=========================================================================
CondDBEntityResolver::open_result_t CondDBEntityResolver::open(const std::string &url) {
  MsgStream log(msgSvc(), name());

  Gaudi::Time since, until;
  std::string str;
  StatusCode sc = i_getData(url,since,until,str);
  if (sc.isFailure()){
    throw GaudiException("Cannot open URL " + url, name(), StatusCode::FAILURE);
  }
  return open_result_t(new std::istringstream(str));
}
//=========================================================================
//  Returns the list of supported protocols.
//=========================================================================
const std::vector<std::string> &CondDBEntityResolver::protocols() const {
  return m_protocols;
}
//=========================================================================
//  Create a Xerces-C input source based on the given systemId (publicId is ignored).
//=========================================================================
xercesc::InputSource *CondDBEntityResolver::resolveEntity(const XMLCh *const, const XMLCh *const systemId) {

  MsgStream log(msgSvc(), name());

  std::string systemIdString;

  char *cString = xercesc::XMLString::transcode(systemId);
  if (cString) {
    systemIdString = cString;
    xercesc::XMLString::release(&cString);
  }

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "systemId = \"" << systemIdString << "\"" << endmsg;

  if ( systemIdString.find("conddb:") != 0 ) {
    // the string does not start with "conddb:", so I cannot handle it
    if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
      log << MSG::VERBOSE << "Not a conddb URL" << endmsg;
    // tell XercesC to use the default action
    return NULL;
  }

  Gaudi::Time since, until;
  std::string str;
  StatusCode sc = i_getData(systemIdString,since,until,str);

  if (sc.isSuccess()) {
    // Create a copy of the string for the InputSource

    // fill the buffer
    unsigned int buff_size = static_cast<unsigned int>(str.size());
    XMLByte* buff = new XMLByte[buff_size];
    std::copy(str.begin(),str.end(),buff);

    // Create the input source using the string
    ValidInputSource *inputSource = new ValidInputSource((XMLByte*) buff,
                                                         buff_size,
                                                         systemId,
                                                         true);
    inputSource->setSystemId(systemId);
    inputSource->setValidity(since, until);

    // Done!
    return inputSource;
  }

  // tell XercesC to use the default action
  return NULL;
}
//=============================================================================
