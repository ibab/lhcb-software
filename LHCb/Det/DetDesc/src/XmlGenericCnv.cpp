/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlGenericCnv.cpp,v 1.2 2001-01-22 09:55:40 ibelyaev Exp $

/// Include files
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/XmlCnvException.h"

#include <util/XMLUni.hpp>
#include <util/XMLString.hpp>
#include <parsers/SAXParser.hpp>

#include <string>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"
#include "GaudiKernel/DataObject.h"

#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/MsgStream.h"

#include "DetDesc/ISax8BitDocHandler.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvAttrList.h"

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: XmlGenericCnv.cpp,v 1.2 2001-01-22 09:55:40 ibelyaev Exp $";

/// Forward and external declarations

// extern       CLID const  CLID_Catalog;

extern const CLID&   CLID_DetectorElement;
extern const CLID&   CLID_LogicalVolume;
extern const CLID&   CLID_Isotope;
extern const CLID&   CLID_Element;
extern const CLID&   CLID_Mixture;


/// Standard constructor
XmlGenericCnv::XmlGenericCnv( ISvcLocator* svc, const CLID& clid)
: Converter(XML_StorageType, clid, svc),
  m_objRcpt( 0 ), m_dataObj( 0 ), m_level( 0 ), m_doFound( false ), m_xmlParser( 0 )
{
}

const unsigned char& XmlGenericCnv::storageType()
{
  return XML_StorageType;
}

/// Initialize the converter
StatusCode XmlGenericCnv::initialize()      {
  
  // Initialize the grand father
  StatusCode status = Converter::initialize();

  m_xmlParser = 0;
  m_doFound   = false;

  IDataProviderSvc* dp;
  serviceLocator()->getService( "DetectorDataSvc",
                                 IID_IDataProviderSvc,
                                 (IInterface*&)dp );
  setDataProvider( dp );
  return status;
}


/// Finalize the converter
StatusCode XmlGenericCnv::finalize()      {
  
  MsgStream log(msgSvc(), "XmlGenericCnv" );
  
  log << MSG::DEBUG << "Finalize..." << endreq;
  
  if( this->m_xmlParser != 0 ) {
    log << MSG::DEBUG << "Finalize... >> m_xmlParser: " << (int)m_xmlParser << endreq;
    delete m_xmlParser;
  }

  m_doFound   = false;
  
  // RIP dear grand father!
  return Converter::finalize();
}

/// Create the transient representation of an object.
StatusCode XmlGenericCnv::createObj(
                                    IOpaqueAddress* //pAddress
                                   ,DataObject*&    //refpObject
                                   )  {
  return StatusCode::SUCCESS;
}

/// Update the transient object from the other representation.
StatusCode XmlGenericCnv::updateObj(
                                    IOpaqueAddress* //pAddress
                                   ,DataObject*     //refpObject
                                   )  {
  return StatusCode::SUCCESS;
}

/// Convert the transient object to the requested representation.
StatusCode XmlGenericCnv::createRep(
                                    DataObject*      //pObject
                                   ,IOpaqueAddress*& //refpAddress
                                   )  {
  return StatusCode::SUCCESS;
}

/// Update the converted representation of a transient object.
StatusCode XmlGenericCnv::updateRep(
                                    IOpaqueAddress* //pAddress
                                   ,DataObject*     //pObject
                                   )  {
  return StatusCode::SUCCESS;
}

static std::string s_notFound = "";

void XmlGenericCnv::startDocument()   {

  MsgStream log(messageService(), "XmlGenericCnv" );

  log << MSG::DEBUG << "Document " << m_objRcpt->dbName() << " start..." << endreq;

  m_level     = 0;
  m_send      = false;
  m_doFound   = false;
  m_tag       = "empty_xml_tag";
  s_notFound.erase();
  
  if( 0 != get8BitDocHandler() ){
    get8BitDocHandler()->startDocument( "" );
  }
}

void XmlGenericCnv::endDocument(){

  MsgStream log(messageService(), "XmlGenericCnv" );

  log << MSG::DEBUG << "Document " << m_objRcpt->dbName() << " end..." << endreq;

  m_send      = false;
  m_tag       = "empty_xml_tag";
  
  if( !m_doFound ) {
    std::string msg = "XML tag ";
    msg += get8BitDocHandler()->tag();
    msg += " with attribute name equal to ";
    msg += s_notFound;
    msg += " not found!!!";
    StatusCode sc; sc.setCode( CORRUPTED_DATA );
    throw XmlCnvException( msg.c_str(), sc );
  }

  m_doFound   = false;
  s_notFound.erase();

  if( 0 != get8BitDocHandler() ){
    get8BitDocHandler()->endDocument( "" );
  }
}

void XmlGenericCnv::processingInstruction( const XMLCh* const target, const XMLCh* const data ){
  //Convert Unicode to ASCII
  char* asciiTarget = XMLString::transcode( target );
  char* asciiData   = XMLString::transcode( data );
  
  if( 0 != get8BitDocHandler() ){
    get8BitDocHandler()->processingInstruction( asciiTarget, asciiData );
  }
  
  delete [] asciiTarget;
  delete [] asciiData;
}

void XmlGenericCnv::startElement( const XMLCh* const name, AttributeList& attributes ){
  
  MsgStream log(msgSvc(), "XmlGenericCnv" );
  
  if( 0 != get8BitDocHandler() ) {
    // Convert XML tag name from Unicode to ASCII
    char* chaname = XMLString::transcode( name );
    std::string aname( chaname );
    delete [] chaname;
    
    std::string nameId;
    
    setContext( aname );

    // Ask for an XML tag we have to find and pass to the ASCII client
    std::string atag( get8BitDocHandler()->tag() );
    std::string notifyAll = NOTIFY_ALL;

    if( notifyAll != atag )              {
      if( "DDDB" == aname ) {
        // This guy we ignore except the check of its version
        // On NT work both versions of Unicode string, to be sure we use the second one
        //XMLCh vstr[] = {'v','e','r','s','i','o','n',0};
        XMLCh vstr[] = { chLatin_v,chLatin_e,chLatin_r,chLatin_s,chLatin_i,chLatin_o,chLatin_n,chNull};
        char* chversion = XMLString::transcode( attributes.getValue( vstr ) );
        std::string version( chversion);
        delete [] chversion;

        log << MSG::DEBUG
            << "Detector Description Markup Language Version " << version
            << endreq;
        if( version != "3.0" ) {
          StatusCode sc; sc.setCode(WRONG_DTD_VERSION);
          std::string msg = "DDDB DTD Version 3.0 required, ";
          msg += "please update your DTD and XML data files\a";
          throw XmlCnvException( msg.c_str(), sc );
        }
        m_send = false;
        m_tag  = "empty_xml_tag";
        return;
      }
    }

    // Convert attributes to ASCII
    XmlCnvAttributeList attrs;
    unsigned int i;

    for( i = 0; i < attributes.getLength(); i++ ) {

      char* name  = XMLString::transcode( attributes.getName(i) );
      char* value = XMLString::transcode( attributes.getValue(i) );
      char* type  = XMLString::transcode( attributes.getType(i) );

      attrs.add( name, value, type );

      if( strcmp( "name", name ) == 0 ) {
        nameId = value;
      }

      delete [] name;
      delete [] value;
      delete [] type;
    }
    
    // Basename of the transient object ID
    unsigned int sPos = m_objRcpt->objectName().find_last_of('/');
    std::string bname = m_objRcpt->objectName().substr( sPos + 1 );
    s_notFound = bname;

    try {

      if( notifyAll != atag ) {
        // If the current starting tag is found and its name ID matches then go
        if( aname == atag && nameId == bname ){
          // Since this moment we send all inner tags
          // unless we get the end of this tag event
          m_send    = true;
          m_doFound = true;
          m_tag  = aname;
          m_level++;

          log << MSG::DEBUG << "\n\n-------- FEEDING START ---------\n" << endreq;
          get8BitDocHandler()->startElement( aname.c_str(), attrs );
        } else if( true == m_send )                                          {
          // We send all inner tags unless we get end of the given tag event
          m_level++;
          get8BitDocHandler()->startElement( aname.c_str(), attrs );
        }
      }
      else                                {
        if( 0 != get8BitDocHandler() ) {
          m_level++;
          get8BitDocHandler()->startElement( aname.c_str(), attrs );
        }
      }
    }
    catch ( const XmlCnvException& cnvE ) {
      
      switch( cnvE.status().getCode() ) {
      case STOP_REQUEST:
      case CANT_QUERY_INTERFACE:
      case CANT_RETRIEVE_OBJECT:
      case CORRUPTED_DATA:
      case EXPRESSION_ERROR:
      case ERROR_ADDING_TO_TS:
        // This is fatal error if some of
        // fundamental interfaces is missing,
        // so we throw it for further processing
        //log << MSG::ERROR << "startElement " << cnvE.getMessage() << endreq;
        throw;
        break;
      default:
        log << MSG::ERROR << "startElement " << cnvE.getMessage() << endreq;
        break;
      }
    }
  }
}

void XmlGenericCnv::endElement( const XMLCh* const name ){

  MsgStream log(msgSvc(), "XmlGenericCnv" );

  // Convert XML tag name from Unicode to ASCII
  char* chaname = XMLString::transcode( name );
  std::string aname( chaname );
  delete [] chaname;

  // Ask for an XML tag we have to find and pass to the ASCII client
  std::string atag( get8BitDocHandler()->tag() );
  std::string notifyAll = NOTIFY_ALL;

  if( notifyAll != atag ) {
    if( "DDDB" == aname || false == m_send ) {
      // This guy we ignore
      return;
    }
    
    if( m_level > 0 ) {
      m_level--;
    }
    
    //log << MSG::INFO    << "Level: "     << m_level
    //                    << " Tag name: " << aname << endreq;

    // Check whether we need stop sending the SAX events to ASCII client or not
    if( m_tag == aname && m_level == 0 ) {
      // Yes, this is gonna be the last event we send
      m_send = false;
      m_tag  = "empty_xml_tag";
    }
  }

  try
  {
    if( 0 != get8BitDocHandler() ) {
      get8BitDocHandler()->endElement( aname.c_str() );
      if( !m_send ) {
        log << MSG::DEBUG << "\n\n-------- FEEDING STOP ---------\n" << endreq;
      }
    }
  }
  catch ( const XmlCnvException& cnvE )
  {
    switch( cnvE.status().getCode() )
    {
    case STOP_REQUEST:
    case CANT_QUERY_INTERFACE:
    case CANT_RETRIEVE_OBJECT:
    case EXPRESSION_ERROR:
      // This is fatal error if some of
      // fundamental interfaces is missing,
      // so we throw it for further processing
      //log << MSG::ERROR << "endElement()>>" << cnvE.getMessage() << endreq;
      throw;
      break;
    default:
      log << MSG::ERROR << "endElement()>>" << cnvE.getMessage() << endreq;
      break;
    }
  }

}

void XmlGenericCnv::characters( const XMLCh* const chars, const unsigned int length ){
  // Convert Unicode to ASCII
  char* asciiChars = XMLString::transcode( chars );

  // Ask for an XML tag we have to find and pass to the ASCII client
  std::string atag( get8BitDocHandler()->tag() );
  std::string notifyAll = NOTIFY_ALL;

  if( notifyAll != atag ) {
    if( 0 !=  get8BitDocHandler() && true == m_send ) {
      get8BitDocHandler()->characters( asciiChars, length );
    }
  } else {
    if( 0 != get8BitDocHandler() ) {
      get8BitDocHandler()->characters( asciiChars, length );
    }
  }

  delete [] asciiChars;
}

void XmlGenericCnv::ignorableWhitespace( const XMLCh* const chars, const unsigned int length ){
  // Convert Unicode to ASCII
  char* asciiChars = XMLString::transcode( chars );
  
  // Ask for an XML tag we have to find and pass to the ASCII client
  std::string atag( get8BitDocHandler()->tag() );
  std::string notifyAll = NOTIFY_ALL;

  if( notifyAll != atag ) {
    if( 0 != get8BitDocHandler() && true == m_send ) {
      get8BitDocHandler()->ignorableWhitespace( asciiChars, length );
    }
  } else {
    if( 0 != get8BitDocHandler() ) {
      get8BitDocHandler()->ignorableWhitespace( asciiChars, length );
    }
  }

  delete [] asciiChars;
}

// -----------------------------------------------------------------------
//  Implementations of the SAX ErrorHandler interface
// -----------------------------------------------------------------------
void XmlGenericCnv::warning( const SAXParseException& exception ){
  
  MsgStream log(msgSvc(), "XmlGenericCnv" );
  
  char*        aSysId  = XMLString::transcode( exception.getSystemId() );
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = XMLString::transcode( exception.getMessage() );

  log << MSG::WARNING << "SAX>> File "    << aSysId
                      << ", line "        << aLine
                      << ", column "      << aColumn
                      << ": "             << aMsg << endreq;
  delete [] aSysId;
  delete [] aMsg;

  throw;
}

void XmlGenericCnv::error( const SAXParseException& exception ){
  
  MsgStream log(msgSvc(), "XmlGenericCnv" );
  
  char*        aSysId  = XMLString::transcode( exception.getSystemId() );
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = XMLString::transcode( exception.getMessage() );

  log << MSG::ERROR   << "SAX>> File "    << aSysId
                      << ", line "        << aLine
                      << ", column "      << aColumn
                      << ": "             << aMsg << endreq;
  delete [] aSysId;
  delete [] aMsg;

  throw;
}

void XmlGenericCnv::fatalError( const SAXParseException& exception ){
  MsgStream log(msgSvc(), "XmlGenericCnv" );
  
  // getType() is not a member of SAXParseException class in Xerces-C
  //char*       aType    = XMLString::transcode( exception.getType() );
  //log << MSG::DEBUG << "Exception type: " << aType  << endreq;

  char*        aSysId  = XMLString::transcode( exception.getSystemId() );
  unsigned int aLine   = exception.getLineNumber();
  unsigned int aColumn = exception.getColumnNumber();
  char*        aMsg    = XMLString::transcode( exception.getMessage() );

  log << MSG::FATAL   << "SAX>> File "    << aSysId
                      << ", line "        << aLine
                      << ", column "      << aColumn
                      << ": "             << aMsg << endreq;
  //delete [] aType;
  delete [] aSysId;
  delete [] aMsg;

  throw;
}

// -----------------------------------------------------------------------
//  Implementation of the SAX DTDHandler interface
// -----------------------------------------------------------------------
void XmlGenericCnv::notationDecl
(
 const   XMLCh* const    //name
 , const XMLCh* const    //publicId
 , const XMLCh* const    //systemId
 )
{
}

void XmlGenericCnv::unparsedEntityDecl
(
 const   XMLCh* const    //name
 , const XMLCh* const    //publicId
 , const XMLCh* const    //systemId
 , const XMLCh* const    //notationName
 )
{
}

StatusCode XmlGenericCnv::initParser() {
  m_xmlParser = new SAXParser();
  // I am the chief of XmlCnvSvc orchestra so I handle SAX Unicode events
  if( 0 != m_xmlParser ) {
    m_xmlParser->setDocumentHandler( this );
    m_xmlParser->setErrorHandler( this );
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode XmlGenericCnv::finiParser() {
  StatusCode sc = StatusCode::SUCCESS;

  try {
    // RIP Xml worker!
    if( 0 != m_xmlParser ) {
      delete m_xmlParser;
      m_xmlParser = 0;
    }
  }
  catch ( ... ) {
    sc = StatusCode::FAILURE;
  }

  return sc;
}

// Calling this method must done inside try{...}catch(...){...} block
StatusCode XmlGenericCnv::parse( const char* fileName ){

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "XmlGenericCnv::parse" );
  
  log << MSG::DEBUG << "Going to parse " << fileName << " file" << endreq;
  log << MSG::DEBUG << "Looking for tag \'" << get8BitDocHandler()->tag() ;
  log << MSG::DEBUG << "\' by name \'" << m_objRcpt->objectName() << "\'" << endreq;

  try {
    m_xmlParser->parse( fileName );
  }
  catch (const XmlCnvException& cnvE)  {
    if( STOP_REQUEST != cnvE.status() ) {
      log << MSG::ERROR          << cnvE.getType() << " for file "
          << m_objRcpt->dbName() << ": "
          << cnvE.getMessage()   << endreq;
      sc = cnvE.status();
    }
    else {
      log << MSG::ERROR << "XML PARSER STOPPED BY REQUEST" << endreq;
    }
  }
  catch ( XMLException& toCatch)  {
    log << MSG::FATAL          << "\nFile not found: '"
        << m_objRcpt->dbName() << "', Type "
        << XMLString::transcode( toCatch.getType() ) << ": "
        << XMLString::transcode( toCatch.getMessage() ) << endreq;

    sc = StatusCode::FAILURE;
  }

  return sc;
}


