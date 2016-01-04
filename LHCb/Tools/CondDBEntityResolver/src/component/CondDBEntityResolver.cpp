// $Id: CondDBEntityResolver.cpp,v 1.2 2008-07-17 17:36:21 marcocle Exp $
// Include files

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/MsgStream.h"
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

namespace {

// custom istream, which, when given an rvalue std::string,
// will 'steal' its buffer. This avoids a copy compared to
// std::istringstream... (which only accepts a string by const&)
template <typename char_type>
class basic_isstream : public std::basic_istream<char_type> {
  class isstreambuf : public std::basic_streambuf<char_type, std::char_traits<char_type> >
  {
    std::basic_string<char_type> m_buffer;
  public:
    template <typename... Args>
    isstreambuf(Args&&... args) : m_buffer( std::forward<Args>(args)... ) {
      // there could be a write to this buffer through sputbackc, (i.e. istream::putback)
      // if one puts back a different character then originally there -- which would be
      // undefined behaviour, as writes to data() are undefined behaviour...
      // (think: cow string implementation where it doesn't realize there is a write,
      // but cow isn't C++11 conforming anyway)
      char_type* b = const_cast<char_type*>(m_buffer.data());
      this->setg( b, b, b+m_buffer.size() );
    }
  } m_sbuf;
public:
  template <typename ... Args>
  basic_isstream(Args&&... args) : std::istream(&m_sbuf), m_sbuf(std::forward<Args>(args)...) { }
};

using isstream = basic_isstream<char>;

}

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
  base_class(type,name,parent)
{
  declareInterface<IXmlEntityResolver>(this);
  declareInterface<IFileAccess>(this);

  declareProperty("DetDataSvc", m_detDataSvcName = "DetDataSvc/DetectorDataSvc");
  declareProperty("CondDBReader", m_condDBReaderName = "CondDBCnvSvc");

}
//=============================================================================
// Destructor
//=============================================================================
CondDBEntityResolver::~CondDBEntityResolver() = default;


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

  m_condDBReader.reset();
  m_detDataSvc.reset();

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
    m_condDBReader = service(m_condDBReaderName,true);
    if( !m_condDBReader) {
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
    m_detDataSvc = service(m_detDataSvcName,true);
    if( !m_detDataSvc ) {
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
boost::optional<std::string> CondDBEntityResolver::i_getData(const std::string &url,
                                           Gaudi::Time &since, Gaudi::Time &until ) {
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
    return boost::none;
  }

  // outputs
  std::string descr;
  ICondDBReader::DataPtr data;

  StatusCode sc = condDBReader()->getObject(path,now,data,descr,since,until,channel);
  if (!sc.isSuccess()) return boost::none;

  if ( !data ) {
    log << MSG::ERROR << "Cannot find any data at " << url << endmsg;
    return boost::none;
  }
  try {
    return CondDBCompression::decompress((*data)[data_field_name].data<std::string>());

  } catch (cool::RecordSpecificationUnknownField &e) {
    log << MSG::ERROR << "I cannot find the data inside COOL object: "
        << e.what() << endmsg;
  }
  return boost::none;
}
//=========================================================================
//  Returns an input stream to read from the opened file.
//=========================================================================
CondDBEntityResolver::open_result_t CondDBEntityResolver::open(const std::string &url) {

  Gaudi::Time since, until;
  auto str = i_getData(url,since,until);
  if (!str){
    throw GaudiException("Cannot open URL " + url, name(), StatusCode::FAILURE);
  }
  return open_result_t(new isstream(std::move(*str)));
}
//=========================================================================
//  Returns the list of supported protocols.
//=========================================================================
const std::vector<std::string> &CondDBEntityResolver::protocols() const {
  /// Vector of supported protocols. (for IFileAccess)
  static const std::vector<std::string> s_protocols = { { "conddb" } };
  return s_protocols;
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
    return nullptr;
  }

  Gaudi::Time since, until;
  auto str = i_getData(systemIdString,since,until);

  if (!str) {
    // tell XercesC to use the default action
    return nullptr;
  }

  // _move_ the string into the inputSource
  ValidInputSource *inputSource = new ValidInputSource(std::move(*str),
                                                       systemId);
  inputSource->setSystemId(systemId);
  inputSource->setValidity(since, until);

  // Done!
  return inputSource;

}
//=============================================================================
