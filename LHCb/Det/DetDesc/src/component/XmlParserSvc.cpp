// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlParserSvc.cpp,v 1.5 2001-10-05 12:17:24 sponce Exp $

// Include Files
#include <limits.h>

#include <parsers/DOMParser.hpp>
#include <util/PlatformUtils.hpp>
#include <dom/DOM_DOMException.hpp>

#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

#include "XmlParserSvc.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static SvcFactory<XmlParserSvc> xmlparsersvc_factory;
const ISvcFactory& XmlParserSvcFactory = xmlparsersvc_factory;


// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
XmlParserSvc::XmlParserSvc (const std::string& name, ISvcLocator* svc) :
  Service (name, svc) {
  MsgStream log (msgSvc(), "XmlParserSvc");

  // initializes the xerces XML subsystem
  try {
    XMLPlatformUtils::Initialize();
  } catch(const XMLException& toCatch) {
    char *message = DOMString(toCatch.getMessage()).transcode();
    log << "Error during Xerces-c Initialization.\n"
        << "  Exception message:"
        << message << endreq;
    delete [] message;
  }

  // creates a new DOMParser
  m_parser = new DOMParser;
  // if the creation was successfull, sets some properties
    if( 0 != m_parser ) {
      // sets the error handler to this object
      m_parser->setErrorHandler(this);
      // asks the parser to validate the parsed xml
      m_parser->setValidationScheme(DOMParser::Val_Auto);
      // asks the parser to continue parsing after a fatal error
      m_parser->setExitOnFirstFatalError(false);
      // asks the parser to ignore whitespaces when possible
      m_parser->setIncludeIgnorableWhitespace (false);
    }

    // gets the maximum number of caches documents from the joboption file
    // by default, this is 5.
    declareProperty ("MaxDocNbInCache", m_maxDocNbInCache = 5);

    // gets the cacheBehavior from the joboption file. A 0 value means
    // FIFO cache. The bigger this value is, the more you tend to keep
    // only reused items. Default value is 2
    declareProperty ("CacheBehavior", m_cacheBehavior = 2);
    
    // initializes the cacheAge to 0
    m_cacheAge = 0;
}


// -----------------------------------------------------------------------
// Standard Destructor
// ------------------------------------------------------------------------
XmlParserSvc::~XmlParserSvc() {
  delete m_parser;
}


// -----------------------------------------------------------------------
//  Parse
// -----------------------------------------------------------------------
DOM_Document XmlParserSvc::parse (const char* fileName) {
  // first look in the cache
  cacheType::iterator it = m_cache.find(fileName);
  // if something was there, update it and return the document
  if (it != m_cache.end()) {
    increaseCacheAge();
    it->second.utility += 1;
    return it->second.document;
  }
  // There was nothing in the cache, try to parse the file if a parser exists
  if (0 != m_parser) {
    // resets it
    m_parser->reset();
    // parses the file
    m_parser->parse(fileName);
    MsgStream log (msgSvc(), "XmlParserSvc");
    log << MSG::INFO << "parsing file " << fileName << endreq;
    // if the document is not null, cache it
    if (m_parser->getDocument() != 0) {
      cacheItem (fileName, m_parser->getDocument());
    }
    // returns the parsed document
    return m_parser->getDocument();
  }
  // no way to parse the file, returns an empty document
  DOM_Document null_result;
  return null_result;
}


// -----------------------------------------------------------------------
//  Parse
// -----------------------------------------------------------------------
void XmlParserSvc::clearCache() {
  // just remove everything from the cache
  m_cache.erase(m_cache.begin(), m_cache.end());
}


// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::warning (const SAXParseException& exception){
  
  MsgStream log (msgSvc(), "XmlParserSvc");
  
  char*        aSysId  = XMLString::transcode( exception.getSystemId() );
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = XMLString::transcode( exception.getMessage() );

  log << MSG::WARNING << "DOM>> File "    << aSysId
                      << ", line "        << aLine
                      << ", column "      << aColumn
                      << ": "             << aMsg << endreq;
  delete [] aSysId;
  delete [] aMsg;

}

// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::error (const SAXParseException& exception){
  
  MsgStream log (msgSvc(), "XmlParserSvc");
  
  char*        aSysId  = XMLString::transcode (exception.getSystemId());
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = XMLString::transcode (exception.getMessage());

  log << MSG::ERROR   << "DOM>> File "    << aSysId
                      << ", line "        << aLine
                      << ", column "      << aColumn
                      << ": "             << aMsg << endreq;
  delete [] aSysId;
  delete [] aMsg;

}


// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::fatalError (const SAXParseException& exception){
  MsgStream log (msgSvc(), "XmlParserSvc");
  
  char*        aSysId  = XMLString::transcode (exception.getSystemId());
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = XMLString::transcode (exception.getMessage());

  log << MSG::FATAL   << "DOM>> File "    << aSysId
                      << ", line "        << aLine
                      << ", column "      << aColumn
                      << ": "             << aMsg << endreq;

  delete [] aSysId;
  delete [] aMsg;

}


// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlParserSvc::resetErrors () {}


// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode XmlParserSvc::queryInterface(const IID& riid, void** ppvInterface) {
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
void XmlParserSvc::cacheItem (std::string fileName, DOM_Document document) {
  // first increase the cache age
  increaseCacheAge();
  
  // first check whether the cache is full
  if (m_maxDocNbInCache <= m_cache.size()) {
    // the cache is full, scan the elements and find the minimum for
    // birthDate+cacheBehavior*utility
    unsigned long smallestScore = ULONG_MAX; // highest possible value
    cacheType::iterator winner = m_cache.end();
    for (cacheType::iterator it = m_cache.begin(); it != m_cache.end(); it++) {
      unsigned long score =
        it->second.birthDate + m_cacheBehavior * it->second.utility;
      if (score < smallestScore) {
        smallestScore = score;
        winner = it;
      }
    }
    if (m_cache.end() == winner) {
      // this is an error : the cache is empty and full at the same time
      MsgStream log (msgSvc(), "XmlParserSvc");     
      log << MSG::ERROR << "The cache is full and empty at the same time !!!"
          << endreq;
    } else {
      // else remove the winner
      m_cache.erase (winner);
    }
  }

  // Now, there is at least one place in the cache, we can add a new item
  cachedItem newItem;
  newItem.birthDate = m_cacheAge;
  newItem.utility = 0;
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

