// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlParserSvc.cpp,v 1.1 2001-05-18 16:48:47 sponce Exp $

// Include Files
#include "GaudiKernel/SvcFactory.h"
#include <parsers/DOMParser.hpp>
#include <util/PlatformUtils.hpp>
#include <dom/DOM_DOMException.hpp>

#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/MsgStream.h"

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
    }
}


// -----------------------------------------------------------------------
// Standard Destructor
// ------------------------------------------------------------------------
XmlParserSvc::~XmlParserSvc() {
  delete m_parser;
}


// -----------------------------------------------------------------------
//  Parse()
// -----------------------------------------------------------------------
DOM_Document XmlParserSvc::parse (const char* fileName) {
  // if a parser exists
  if (0 != m_parser) {
    // resets it
    m_parser->reset();
    // parses the file
    m_parser->parse(fileName);
    // returns the parsed document
    return m_parser->getDocument();
  }
  DOM_Document null_result;
  return null_result;
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
