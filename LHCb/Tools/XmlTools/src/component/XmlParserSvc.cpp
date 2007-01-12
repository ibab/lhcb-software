// $Id: XmlParserSvc.cpp,v 1.11 2007-01-12 12:43:42 cattanem Exp $

// Include Files
#include <limits.h>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

#include "XmlParserSvc.h"

#include "XmlTools/IXmlEntityResolverSvc.h"

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_SERVICE_FACTORY(XmlParserSvc)

// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
XmlParserSvc::XmlParserSvc (const std::string& name, ISvcLocator* svc) :
  Service (name, svc),m_parser(NULL),m_resolverSvc(NULL) {

  // gets the maximum number of caches documents from the joboption file
  // by default, this is 10.
  declareProperty ("MaxDocNbInCache", m_maxDocNbInCache = 10);
  
  // gets the cacheBehavior from the joboption file. A 0 value means
  // FIFO cache. The bigger this value is, the more you tend to keep
  // only reused items. Default value is 2
  declareProperty ("CacheBehavior", m_cacheBehavior = 2);
  
  // Name of the xerces::EntityResolver provider
  declareProperty ("EntityResolverSvc", m_resolverSvcName = "" );    
  
  // initializes the cacheAge to 0
  m_cacheAge = 0;
}


//=========================================================================
//  Initialization
//=========================================================================
StatusCode XmlParserSvc::initialize( ) {
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() ) return sc;

  MsgStream log (msgSvc(), name());

  // initializes the xerces XML subsystem
  try {
    xercesc::XMLPlatformUtils::Initialize();
  } catch(const xercesc::XMLException& toCatch) {
    char *message = xercesc::XMLString::transcode(toCatch.getMessage());
    log << "Error during Xerces-c Initialization.\n"
        << "  Exception message:"
        << message << endmsg;
    xercesc::XMLString::release(&message);
  }

  // creates a new XercesDOMParser
  m_parser = new xercesc::XercesDOMParser;
  // if the creation was successfull, sets some properties
  if( 0 != m_parser ) {
    // sets the error handler to this object
    m_parser->setErrorHandler(this);
    // asks the parser to validate the parsed xml
    m_parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
    // asks the parser to continue parsing after a fatal error
    m_parser->setExitOnFirstFatalError(false);
    // asks the parser to ignore whitespaces when possible
    m_parser->setIncludeIgnorableWhitespace (false);
    // asks the parser to avoid the creation of EntityReference nodes
    m_parser->setCreateEntityReferenceNodes (false);
    
    if( ! m_resolverSvcName.empty() ) {    
      sc = service(m_resolverSvcName,m_resolverSvc,true);
      if (  !sc.isSuccess() ) {
        log << MSG::ERROR << "Could not locate the IXmlEntityResolverSvc " << m_resolverSvcName << endmsg;
        return sc;
      }
      m_parser->setEntityResolver(m_resolverSvc->resolver());
      log << MSG::DEBUG << "using the xercesc::EntityResolver provided by " << m_resolverSvcName << endmsg;
    }
  } else {
    log << MSG::ERROR << "Could not create xercesc::XercesDOMParser" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Finalization
//=========================================================================
StatusCode XmlParserSvc::finalize() {
  clearCache();
  
  if (m_parser) delete m_parser;

  if (m_resolverSvc) m_resolverSvc->release();

  xercesc::XMLPlatformUtils::Terminate();

  return Service::finalize();
}
// -----------------------------------------------------------------------
// Standard Destructor
// ------------------------------------------------------------------------
XmlParserSvc::~XmlParserSvc() { }


// -----------------------------------------------------------------------
//  Parse
// -----------------------------------------------------------------------
xercesc::DOMDocument* XmlParserSvc::parse (const char* fileName) {
  MsgStream log (msgSvc(), name());
  // first look in the cache
  cacheType::iterator it = m_cache.find(fileName);
  // if something was there, update it and return the document
  if (it != m_cache.end()) {
    increaseCacheAge();
    ++it->second.utility;
    ++it->second.lock;
    return it->second.document;
  }
  // There was nothing in the cache, try to parse the file if a parser exists
  if (0 != m_parser) {
    // resets it
    m_parser->reset();
    // parses the file
    try {
      m_parser->parse(fileName);
      log << MSG::DEBUG << "parsing file " << fileName << endreq;
      // get a pointer to the DOM Document and also take the responsibility of
      // freeing the memory
      xercesc::DOMDocument *doc = m_parser->adoptDocument();
      // if the document is not null, cache it
      if (doc != 0) {
        cacheItem (fileName, doc);
      }
      // returns the parsed document
      return doc;
    } catch (xercesc::XMLPlatformUtilsException e) {
      log << MSG::ERROR << "Unable to find file " << fileName
          << " !" << endreq;      
    }
  }
  // no way to parse the file, returns an empty document
  return 0;
}


// -----------------------------------------------------------------------
// Parses an Xml file and provides the DOM tree representing it
// -----------------------------------------------------------------------
xercesc::DOMDocument* XmlParserSvc::parseString (std::string source) {
  // there is of course no cache for parsing XML strings directly
  // try to parse the string if a parser exists
  if (0 != m_parser) {
    // resets it
    m_parser->reset();
    // builds a new InputSource
    xercesc::MemBufInputSource inputSource ((XMLByte*) source.data(),
                                            source.length(),
                                            "");
    // parses the file
    m_parser->parse(inputSource);
    MsgStream log (msgSvc(), name());
    log << MSG::DEBUG << "parsing xml string..." << endreq;
    // returns the parsed document
    return m_parser->adoptDocument();
  }
  // no way to parse the string, returns an empty document
  return 0;
}


// -----------------------------------------------------------------------
//  Parse
// -----------------------------------------------------------------------
void XmlParserSvc::clearCache() {
  // remove everything from the cache
  //    first delete the DOM documents
  m_parser->resetDocumentPool();
  //    release the memory used by the cached objects
  for (cacheType::iterator i = m_cache.begin(); i != m_cache.end(); ++i){
    if ( i->second.lock > 0 ) {
      MsgStream log (msgSvc(), name());
      log << MSG::WARNING << "Item in cache with lock count = " << i->second.lock
          << " " << i->first << endmsg;
    }
    i->second.document->release();
  }
  //    then clear the chached pointers
  m_cache.clear();
}


//=========================================================================
//  Release the lock for documents in cache
//=========================================================================
void XmlParserSvc::releaseDoc(xercesc::DOMDocument* doc) {
  // find the DOMDocument in the cache
  cacheType::iterator it = m_cache.begin();
  while (it != m_cache.end() && it->second.document != doc) {
    ++it;
  }
  if ( it == m_cache.end() ) {
    // the item was never cached (parsed from a string) ==> delete it
    doc->release();
  } else {
    if ( --it->second.lock < 0 ) {
      MsgStream log (msgSvc(), name());
      log << MSG::WARNING << "XmlParserSvc::release request for an object which wasn't locked!" << endmsg;
      it->second.lock = 0;
    }
  }
}

// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::warning (const xercesc::SAXParseException& exception){
  
  MsgStream log (msgSvc(), name());
  
  char*        aSysId  = xercesc::XMLString::transcode(exception.getSystemId());
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = xercesc::XMLString::transcode(exception.getMessage());

  log << MSG::WARNING << "DOM>> File "    << aSysId
      << ", line "        << aLine
      << ", column "      << aColumn
      << ": "             << aMsg << endreq;
  xercesc::XMLString::release(&aSysId);
  xercesc::XMLString::release(&aMsg);

}

// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::error (const xercesc::SAXParseException& exception){
  
  MsgStream log (msgSvc(), name());
  
  char* aSysId = xercesc::XMLString::transcode (exception.getSystemId());
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = xercesc::XMLString::transcode (exception.getMessage());

  log << MSG::ERROR   << "DOM>> File "    << aSysId
      << ", line "        << aLine
      << ", column "      << aColumn
      << ": "             << aMsg << endreq;
  xercesc::XMLString::release(&aSysId);
  xercesc::XMLString::release(&aMsg);

}


// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::fatalError (const xercesc::SAXParseException& exception){
  MsgStream log (msgSvc(), name());
  
  char* aSysId = xercesc::XMLString::transcode (exception.getSystemId());
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = xercesc::XMLString::transcode (exception.getMessage());

  log << MSG::FATAL   << "DOM>> File "    << aSysId
      << ", line "        << aLine
      << ", column "      << aColumn
      << ": "             << aMsg << endreq;

  xercesc::XMLString::release(&aSysId);
  xercesc::XMLString::release(&aMsg);

}


// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::resetErrors () {}


// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode XmlParserSvc::queryInterface( const InterfaceID& riid,
                                         void** ppvInterface     ) {
  if (IID_IXmlParserSvc.versionMatch(riid))  {
    *ppvInterface = (IXmlParserSvc*)this;
  } else {
    // Interface is not directly availible: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// CacheItem
// -----------------------------------------------------------------------
void XmlParserSvc::cacheItem (std::string fileName,
                              xercesc::DOMDocument* document) {
  // first increase the cache age
  increaseCacheAge();
  
  // first check whether the cache is full
  if (m_maxDocNbInCache <= m_cache.size()) {
    // the cache is full, scan the elements and find the minimum for
    // birthDate+cacheBehavior*utility
    unsigned long smallestScore = ULONG_MAX; // highest possible value
    cacheType::iterator winner = m_cache.end();
    for (cacheType::iterator it = m_cache.begin(); it != m_cache.end(); ++it) {
      unsigned long score =
        it->second.birthDate + m_cacheBehavior * it->second.utility;
      if (score < smallestScore && it->second.lock == 0) {
        smallestScore = score;
        winner = it;
      }
    }
    if (m_cache.end() == winner) {
      // This means that the cache is too small: I increase it
      MsgStream log (msgSvc(), name());
      log << MSG::WARNING << "The cache is full and I cannot delete anything: I increase the max size to " 
          << ++m_maxDocNbInCache << endmsg;
    } else {
      // else release the memory used by the winner
      winner->second.document->release();
      // and its entry in the map
      m_cache.erase(winner);
    }
  }

  // Now, there is at least one place in the cache, we can add a new item
  cachedItem newItem;
  newItem.birthDate = m_cacheAge;
  newItem.utility = 0;
  newItem.lock = 1;
  newItem.document = document;
  m_cache[fileName] = newItem;
}


// -----------------------------------------------------------------------
// Increases cache age
// -----------------------------------------------------------------------
void XmlParserSvc::increaseCacheAge () {
  m_cacheAge += 1;
  if (0 == m_cacheAge) {
    // the cacheAge is an unsigned int. We are here because the age
    // has just exceeded the maximum possible value for unsigned ints.
    // As a consequence, we put every birthDate to 0.
    for (cacheType::iterator it = m_cache.begin();
         it != m_cache.end();
         it++) {
      it->second.birthDate = 0;
    }
  }
}

