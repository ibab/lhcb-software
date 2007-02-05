// $Id: XmlParserSvc.cpp,v 1.13 2007-02-05 18:51:19 marcocle Exp $

// Include Files
#include <limits.h>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDetDataSvc.h"

#include "XmlTools/IXmlEntityResolverSvc.h"
#include "XmlTools/ValidInputSource.h"

#include "XmlParserSvc.h"
#include "XmlTools/IOVDOMDocument.h"

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
  
  // Name of the xerces::EntityResolver provider
  declareProperty ("DetectorDataSvc", m_detDataSvcName = "DetectorDataSvc" );
  
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

  // find the detector data svc
  sc = service(m_detDataSvcName,m_detDataSvc,true);
  if (!sc.isSuccess()) {
    log << MSG::WARNING << "Unable to get a handle to the detector data service, I will not consider event time" << endmsg;
  } else {
    log << MSG::DEBUG << "Got pointer to IDetDataSvc \"" << m_detDataSvcName << '"' << endmsg;
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
  
  if (m_detDataSvc) m_detDataSvc->release();
  
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
IOVDOMDocument* XmlParserSvc::parse (const char* fileName) {
  MsgStream log (msgSvc(), name());
  // first look in the cache
  cacheType::iterator it = m_cache.find(fileName);
  
  if (it != m_cache.end()) {
    // we found an object in the cache
    if ( m_detDataSvc && m_detDataSvc->validEventTime() ) {
      // since we have a service that knows the event time,
      // we can check if the cached item is still valid
      if ( it->second.document->isValid(m_detDataSvc->eventTime()) ) {
        // the cached DOM is valid for the current event, so it is what we wanted
        increaseCacheAge();
        ++it->second.utility;
        ++it->second.lock;
        return it->second.document;
      }
      else {
        // the document is not valid: I try to remove it from the cache
        if ( ! it->second.lock ) {
          // we can remove the object from the cache
          delete it->second.document;
          m_cache.erase(it);
        }
        else {
          // This should never happen: a document that is not valid cannot be locked!
          throw GaudiException("BAD status of XmlParserSvc cache:"
                               " a cache document is invalid and locked at the same time",
                               "XmlParserSvc::parse",StatusCode::FAILURE);
        }
      }
    }
  }
  // There was nothing in the cache, try to parse the file if a parser exists
  if (0 != m_parser) {
    // resets it
    m_parser->reset();
    // parses the file
    try {
      log << MSG::DEBUG << "parsing file " << fileName << endreq;
      xercesc::DOMDocument *doc = NULL;
      std::auto_ptr<xercesc::InputSource> is;
      // If we have an entity resolver, we try to use it
      if (m_resolverSvc){
        XMLCh *sysId = xercesc::XMLString::transcode(fileName);
        is = std::auto_ptr<xercesc::InputSource>(m_resolverSvc->resolver()->resolveEntity(NULL,sysId));
        xercesc::XMLString::release(&sysId);
      }
      
      if ( is.get() != NULL ) { // If the entity resolver succeded, we parse the InputSource
        m_parser->parse(*is);
      }
      else { // otherwise try to pass the filename to XercesC
        m_parser->parse(fileName);
      }
      // get a pointer to the DOM Document and also take the responsibility of
      // freeing the memory
      doc = m_parser->adoptDocument();
      // if the document is not null, cache it
      if (doc != 0) {
        IOVDOMDocument *cache_doc = new IOVDOMDocument(doc);
        // Try to see if the InputSource knows about validity/
        ValidInputSource *iov_is = dynamic_cast<ValidInputSource *>(is.get());
        if (iov_is){ // it knows
          cache_doc->setValidity(iov_is->validSince(),iov_is->validTill());
        }
        cacheItem (fileName, cache_doc);
        // returns the parsed document
        return cache_doc;
      }
      // return empty document
      return NULL;
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
IOVDOMDocument* XmlParserSvc::parseString (std::string source) {
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
    xercesc::DOMDocument *doc = m_parser->adoptDocument();
    // returns the parsed document if successful
    if (doc != 0) {
      return new IOVDOMDocument(doc);
    }
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
    // i->second.document->release();
    delete i->second.document;
  }
  //    then clear the chached pointers
  m_cache.clear();
}


//=========================================================================
//  Release the lock for documents in cache
//=========================================================================
void XmlParserSvc::releaseDoc(IOVDOMDocument* doc) {
  // find the DOMDocument in the cache
  cacheType::iterator it = m_cache.begin();
  while (it != m_cache.end() && it->second.document != doc) {
    ++it;
  }
  if ( it == m_cache.end() ) {
    // the item was never cached (parsed from a string) ==> delete it
    delete doc;
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
                              IOVDOMDocument* document) {
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
      //winner->second.document->release();
      delete winner->second.document;
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

