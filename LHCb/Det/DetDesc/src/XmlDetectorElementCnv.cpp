///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlDetectorElementCnv.cpp,v 1.1.1.1 2000-11-23 13:44:47 ranjard Exp $

/// Include files
#include "DetDesc/XmlDetectorElementCnv.h"
#include "DetDesc/XmlCnvException.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"

#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/DetectorElement.h"

#include "DetDesc/IUserSax8BitDocHandler.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvSvc.h"
#include "DetDesc/XmlCnvAttrList.h"


#include "DetDesc/GeometryInfo.h"

extern unsigned char    XML_StorageType;
extern const    CLID&   CLID_DetectorElement;

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: XmlDetectorElementCnv.cpp,v 1.1.1.1 2000-11-23 13:44:47 ranjard Exp $";

/// Instantiation of a static factory class used by clients to create
/// instances of this service
static CnvFactory<XmlDetectorElementCnv> s_factory;
const ICnvFactory& XmlDetectorElementCnvFactory = s_factory;

// Iinitialize the converter
StatusCode XmlDetectorElementCnv::initialize()
{
  StatusCode sc = XmlGenericCnv::initialize();

  if( sc.isSuccess() )
  {
    MsgStream log( msgSvc(), "XmlDetElemCnv" );

    log << MSG::VERBOSE << "Initializing" << endreq;

    /// Get the XML conversion service interface
    IService* svc        = 0;
    XmlCnvSvc* xmlcnvsvc = 0;

    sc = serviceLocator()->getService( "XmlCnvSvc", (IService*&)svc );
    
    try
    {
      xmlcnvsvc = dynamic_cast<XmlCnvSvc*>(svc);
      m_doGenericCnv = xmlcnvsvc->allowGenericCnv();
      log << MSG::INFO << "Generic conversion status: " << (unsigned int)m_doGenericCnv << endreq;
    }
    catch( ... )
    {
      sc = StatusCode::FAILURE;
      log << MSG::ERROR << "Getting XmlCnvSvc failed" << endreq;
    }
  }

  return sc;
}

// Create the transient representation of an object.
StatusCode XmlDetectorElementCnv::createObj( IOpaqueAddress* pAddress,
                                            DataObject*& refpObject)
{
  MsgStream log(msgSvc(), "XmlDetElemCnv" );
  
  StatusCode sc = StatusCode::SUCCESS;

  GenericAddress* addr;

  // Test and store hint
  if( 0 != pAddress )    {
    try{
      addr = dynamic_cast< XmlAddress* >( pAddress );
    }
    catch( ... )    {
      return StatusCode::FAILURE;
    }
  }
  
  m_objRcpt = addr;

  if( 0 != getUserSaxDocHandler() && CLID_DetectorElement != m_objRcpt->clID() ) {
    log << MSG::DEBUG << "Called by user, remembering data object" << endreq;
    m_dataObj = refpObject;
  }

  
  // Reinitialize the values before the conversion
  m_lvname  = "";
  m_support = "";
  m_rpath   = "";
  m_npath   = "";

  sc = initParser();
  if( sc.isFailure() )
  {
    log << MSG::FATAL << "XML Parser init failed, can't convert "
                      << addr->objectName() << "!" << endreq;
    finiParser();
    return sc;
  }

  sc = parse( addr->dbName().c_str() );
    
  if( sc.isFailure() )
  {
    finiParser();
    return StatusCode::FAILURE;
  }

  sc = finiParser();
  if( sc.isFailure() )
  {
    log << MSG::FATAL << "XML Parser init failed, can't convert "
                      << addr->objectName() << "!" << endreq;
    return sc;
  }

  if( 0 == getUserSaxDocHandler() && CLID_DetectorElement == addr->clID() ) {
    /// We are not called by user defined XML detector element converter
    /// so we send now the object we have allocated ourselves
    refpObject = m_dataObj;
  }

  return sc;
}

/// Update the transient object from the other representation.
StatusCode XmlDetectorElementCnv::updateObj(
                                             IOpaqueAddress* //pAddress
                                            ,DataObject*     //pObject
                                           )
{
  return StatusCode::SUCCESS;
}

/// Convert the transient object to the requested representation
StatusCode XmlDetectorElementCnv::createRep(
                                             DataObject*      //pObject
                                            ,IOpaqueAddress*& //refpAddress
                                           )
{
  return StatusCode::SUCCESS;
}

/// Update the converted representation of a transient object.
StatusCode XmlDetectorElementCnv::updateRep(
                                             IOpaqueAddress* //pAddress
                                            ,DataObject*     //pObject
                                           )
{
  return StatusCode::SUCCESS;
}

/// Report to outside the class ID this converter is used for
const CLID& XmlDetectorElementCnv::classID()
{
  return CLID_DetectorElement;
}

/// Constructor
XmlDetectorElementCnv::XmlDetectorElementCnv(ISvcLocator* svc)
: XmlGenericCnv( svc, XmlDetectorElementCnv::classID() ), m_customData( false ) {
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
  setUserSaxDocHandler( *((IUserSax8BitDocHandler *)0) );
  m_lvname  = "";
  m_support = "";
  m_rpath   = "";
  m_npath   = "";
  m_doGenericCnv = false;
}

XmlDetectorElementCnv::XmlDetectorElementCnv(ISvcLocator* svc, const CLID& clsID )
: XmlGenericCnv( svc, clsID ), m_customData( false ) {
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
  setUserSaxDocHandler( *((IUserSax8BitDocHandler *)0) );
  m_lvname  = "";
  m_support = "";
  m_rpath   = "";
  m_npath   = "";
  m_doGenericCnv = false;
}

void XmlDetectorElementCnv::startElement( const char* const name,
                                         XmlCnvAttributeList& attributes) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );
  
  std::string tagName( name );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) {
    log << MSG::DEBUG << attributes.getName(i)  << "=" 
      << attributes.getValue(i) << " "
      << attributes.getType(i) << " ";
  }
  log << ">" << endreq;

  bool allowGeneric = false;

  if( "detelem" == tagName ) {

    // Basename of the transient object ID
    unsigned int sPos = m_objRcpt->objectName().find_last_of('/');
    std::string baseName = m_objRcpt->objectName().substr( sPos + 1 );
    
    if( baseName == attributes.getValue( "name" ) ) {

      if( 0 == getUserSaxDocHandler() && CLID_DetectorElement == m_objRcpt->clID() ) {
        // We're converter for this concrete XML tag, generic DetectorElement
        // We need to create our transient representation
        m_dataObj = new DetectorElement( m_objRcpt->objectName(),
                                         dataProvider(),
                                         msgSvc()
                                       );
        log << MSG::DEBUG << "Normal generic detector element conversion" << endreq;
      } else {
        /// We are called by user defined XML detector element converter
        /// and "new" must be done there
        log << MSG::DEBUG << "Called by user defined converter" << endreq;
      }

    } else {
      // This should never happen!
      log << MSG::WARNING << "Got unexpected detelem tag: "
          << attributes.getValue( "name" ) << endreq;
    }
  } else if( "detelemref" == tagName ) {
    // Checking the other incoming guys according to our DTD!
    
    XmlAddress*    xmlAddr;
    bool           cnvExists;
    ICnvManager*   cnvMgr;
    
    StatusCode stcod = serviceLocator()->queryInterface(IID_ICnvManager,
      (void **)&cnvMgr );
    if( stcod.isFailure() ) {
      stcod.setCode( CANT_QUERY_INTERFACE );  
      throw XmlCnvException( "Query to ICnvManager interface failed", stcod );
    }
    
    // We have to decode class ID of the referred object
    const CLID& clsID = (unsigned long)atol( attributes.getValue( "classID" ).c_str() );
    
    // Check whether the converter for this storage type and class ID is available
    cnvExists = cnvMgr->existsConverter( XML_StorageType, clsID );
    cnvMgr->release();
    
    if( !cnvExists ) {
      if( !m_doGenericCnv)
      {
      log << MSG::ERROR
        << "File " << m_objRcpt->dbName()
        << ": " << tagName << " class ID "
        << clsID << ", proper converter not found" << endreq;
      stcod.setCode( INVALID_CLASS_ID );  
      throw XmlCnvException( "Unknown class ID", stcod );
      }

      log << MSG::INFO << "Performing generic detector element conversion for";
      log << MSG::INFO << " class ID: " << clsID
                       << " URI: "    << attributes.getValue( "href" )
                       << " referenced from " << m_objRcpt->dbName()
                       << endreq;
      allowGeneric = true;
    } 
    
    // Create new XML address for the daughter
    if( allowGeneric )
    {
      xmlAddr = new XmlAddress( CLID_DetectorElement );
    }
    else
    {
      xmlAddr = new XmlAddress( clsID );
    }
    
    // Let's decode URI of the object location
    // We must split the URI into XML file name, container name and object name
    // General URL is in format:
    //
    //         protocol://host.domain.name/path/to/file.xml#objectID
    //
    unsigned int sPos    = attributes.getValue( "href" ).find_last_of('#');
    std::string location = attributes.getValue( "href" ).substr( 0, sPos );
    
    /// We need to get directory where the XML files are located
    unsigned int dPos  = m_objRcpt->dbName().find_last_of('/');
    std::string locDir = m_objRcpt->dbName().substr( 0, dPos + 1 );
    
    // OK, we got location part which can be in general of a form like:
    // 
    //  protocol://host.domain.name/path/to/file.xml
    //
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing
      location = m_objRcpt->dbName();
    }
    else {
/*      dPos = location.find_last_of('/');
      if( dPos != std::string::npos ) {
        location = locDir + location.substr( dPos + 1 );
      }
      else {
        location = locDir + location;
      }
*/    
      location = locDir + location;
    }

    log << MSG::DEBUG << "Location: " << location << endreq;

    // Let's set object's dbName XML address attribute
    xmlAddr->setDbName( location );
    
    xmlAddr->setContainerName( m_objRcpt->objectName() ); // UNUSED for now
    
    std::string entryName = attributes.getValue( "href" ).substr( sPos + 1);
    entryName             = m_objRcpt->objectName() + "/" + entryName;
    // OK, we got object ID
    // Let's set object's objectName XML address attribute
    xmlAddr->setObjectName( entryName );
    
    // Now create its registry entry hook
    RegistryEntry* entry   = new RegistryEntry( entryName );
    
    // Attach the address to it
    entry->setAddress( xmlAddr );
    
    // And register it to current data object we're converting now
    m_objRcpt->directory()->add( entry );
  } else if( "version" == tagName || "author" == tagName ) {
    // For these guys we do nothing at this point since they have no attributes
    // defined in their DTD and their content model is processed inside
    // characters(...) 8 bit SAX handler callback
    ;
  } else if( "geometryinfo" == tagName ) {

    // Do nothing here, all the processing is done in endElement() callback
    ;

  } else if( tagName == "lvname" )                                         {
    m_lvname  = attributes.getValue( "name" );
  } else if( tagName == "support" )                                        {
    m_support = attributes.getValue( "name" );
  } else if( tagName == "rpath" )                                          {
    m_rpath   = attributes.getValue( "value" );
  } else if( tagName == "npath" )                                          {
    m_npath   = attributes.getValue( "value" );
  } else if( tagName == "specific" )                                       {
    
    try                                                                    {
      if( 0 != getUserSaxDocHandler() )                                    {
        // At this point we have to call UserSax8BitDocHandler uStartElement
        // callback
        log << MSG::DEBUG
            << "A call to UserSax8BitDocHandler uStartElement will be done"
            << endreq;

        // Set flag that we are parsing user defined tags inside the
        // <specific> XML tag not recongnized by
        // the generic detector element converter.
        m_customData = true;
        getUserSaxDocHandler()->uStartElement( name, attributes );
      }
    } catch( const XmlCnvException& cnvE )                                 {
      log << MSG::ERROR << "User exception caught, status"
          << cnvE.status() << endreq;
      throw;
    }
  } else                                                                   {
    try                                                                    {
      if( 0 != getUserSaxDocHandler() && true == m_customData )            {
        getUserSaxDocHandler()->uStartElement( name, attributes );
      }
    } catch( XmlCnvException& cnvE )                                       {
      log << MSG::ERROR << "User exception caught, status"
          << cnvE.status() << endreq;
      throw;
    }
  }
}

void XmlDetectorElementCnv::endElement( const char* const name ) {

  MsgStream log(msgSvc(), "XmlDetElemCnv" );
  log << MSG::DEBUG << "</" << name << ">" << endreq;
  
  std::string aname = name;

  if( aname == "geometryinfo" )                                            {
  
    DetectorElement *de = 0;
    
    try {
      if( 0 != getUserSaxDocHandler() &&
          CLID_DetectorElement != m_objRcpt->clID() )                       {
        // Called by user defined converter
        de = reinterpret_cast<DetectorElement*>(m_dataObj);
      }
      else                                                                  {
        de = dynamic_cast<DetectorElement*>(m_dataObj);
      }
    } catch ( ... ) {
      log << MSG::ERROR << "Bad cast exception caught" << endreq;
    }

    IGeometryInfo*   gInfo = 0;
    std::string logVolName  = m_lvname;
    std::string support     = m_support;
    std::string replicaPath = m_rpath;
    std::string namePath    = m_npath;

    //unsigned int which = 0;
    
    log << MSG::DEBUG << "GI volume : " << logVolName  << endreq;
    log << MSG::DEBUG << "GI support: " << support     << endreq;
    log << MSG::DEBUG << "GI rpath  : " << replicaPath << endreq;
    log << MSG::DEBUG << "GI npath  : " << namePath    << endreq;

    try                                                                    {
      if( !logVolName.empty() )                                            {
        if( !support.empty() )                                             {
          
          ILVolume::ReplicaPath repPath;
          
          if( !replicaPath.empty() )                                       {
            // Replica path has the format "1/3/7/2"
            const char*  rp = replicaPath.c_str();
            char         buf[512]; char* replica = buf;
            bool         wasColon = false; unsigned int i = 0;
            
            do                                                             {
              wasColon = false;
              if( *rp == '/')                                              {
                wasColon = true;
              }
              else if( isdigit(*rp) )                                      {
                *replica = *rp;
                replica++; i++;
              }
              
              if( true == wasColon || *(rp + 1) == '\0' )                  {
                if( i > 0 )                                                {
                  *replica = '\0';
                  i = (unsigned int)atol( buf );
                  repPath.push_back( i );
                  
                  log << MSG::DEBUG << "Found replica number "
                    << repPath.back() << endreq;
                  replica = buf; i = 0;
                }
              }
              rp++;
            }while( *rp != 0 );
            
            gInfo = new GeometryInfo(de,logVolName,support,repPath,
                                     dataProvider(),msgSvc());
          } else if( !namePath.empty() )                                   {
            gInfo = new GeometryInfo(de,logVolName,support,namePath,
                                     dataProvider(),msgSvc());
          } else                                                           {
            log << MSG::ERROR
              << "File " << m_objRcpt->dbName()
              << ": " << aname
              << " Missing \"rpath\" or \"npath\" element, please correct XML data\n"
              << " Either remove support element or provide proper rpath or npath"
              << endreq;

            StatusCode stcod;
            stcod.setCode( CORRUPTED_DATA );  
            throw XmlCnvException( " Corrupted XML data", stcod );
          }
        } else                                                             {
          // Orphan
          gInfo = new GeometryInfo( de, logVolName,
                                    dataProvider(), msgSvc() );
        }
      } else                                                               {
        // Ghost
        gInfo = new GeometryInfo( de, dataProvider(), msgSvc() );
      }
    }
    catch( XmlCnvException& )                                              {
      throw;
    }
    catch( ... )                                                           {
      log << MSG::ERROR << "What is going on here?\a" << endreq;
    }

    if( de != 0 )                                                          {
      de->setGeometry( gInfo );
    }

    
  }

  // At this point we have to call UserSax8BitDocHandler uEndElement callback
  // if we have registered one
  try {
    if( 0 != getUserSaxDocHandler() && true == m_customData ) {
      
      if( "specific" == aname ) {
        m_customData = false;
      }

      getUserSaxDocHandler()->uEndElement( name );
    }
  } catch( const XmlCnvException& cnvE ) {
    log << MSG::ERROR << "User exception caught, status" << cnvE.status() << endreq;
    throw;
  }
}

void XmlDetectorElementCnv::characters( const char* const chars,
                                       const unsigned int length ) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );

  if( "version" == context() ) {
    log << MSG::DEBUG << "\"" << chars << "\"" << endreq;
  } else if( "author" == context() ) {
    log << MSG::DEBUG << "\"" << chars << "\"" << endreq;
  } else {
    // This is unknown context, ignore it!
    // OR
    // At this point we have to call UserSax8BitDocHandler uCharacters callback
    // if we have registered one
    try {
      if( 0 != getUserSaxDocHandler() && true == m_customData ) {
        getUserSaxDocHandler()->uCharacters( chars, length );
      }
    } catch( const XmlCnvException& cnvE ) {
      log << MSG::ERROR << "User exception caught, status" << cnvE.status() << endreq;
      throw;
    }
  }
}

void XmlDetectorElementCnv::ignorableWhitespace(
                                                 const char* const chars
                                                ,const unsigned int length
                                               )
{
  MsgStream log( msgSvc(), "XmlDetElemCnv" );
  
  if( length > 0 )
    log << MSG::DEBUG << "\"" << chars << "\"" << endreq;

  // At this point we have to call UserSax8BitDocHandler uCharacters callback
  // if we have registered one
  try {
    if( 0 != getUserSaxDocHandler() && true == m_customData ) {
      getUserSaxDocHandler()->uIgnorableWhitespace( chars, length );
    }
  } catch( const XmlCnvException& cnvE ) {
    log << MSG::ERROR << "User exception caught, status" << cnvE.status() << endreq;
    throw;
  }
}

void XmlDetectorElementCnv::startDocument(
                                           const char* //info
                                         )
{
  MsgStream log( msgSvc(), "XmlDetElemCnv" );

  // At this point we have to call UserSax8BitDocHandler uCharacters callback
  // if we have registered one
  try {
    if( 0 != getUserSaxDocHandler() ) {
      getUserSaxDocHandler()->uStartDocument();
    }
  } catch( const XmlCnvException& cnvE ) {
    log << MSG::ERROR << "User exception caught, status" << cnvE.status() << endreq;
    throw;
  }
}

void XmlDetectorElementCnv::endDocument(
                                         const char* //info
                                       )
{
  MsgStream log( msgSvc(), "XmlDetElemCnv" );

  // At this point we have to call UserSax8BitDocHandler uCharacters callback
  // if we have registered one
  try {
    if( 0 != getUserSaxDocHandler() ) {
      getUserSaxDocHandler()->uEndDocument();
    }
  } catch( const XmlCnvException& cnvE ) {
    log << MSG::ERROR << "User exception caught, status" << cnvE.status() << endreq;
    throw;
  }
}
  
void XmlDetectorElementCnv::processingInstruction(
                                                   const char* const target
                                                  ,const char* const data
                                                 )
{
  MsgStream log( msgSvc(), "XmlDetElemCnv" );

  // At this point we have to call UserSax8BitDocHandler uCharacters callback
  // if we have registered one
  try {
    if( 0 != getUserSaxDocHandler() ) {
      getUserSaxDocHandler()->uProcessingInstruction( target, data );
    }
  } catch( const XmlCnvException& cnvE ) {
    log << MSG::ERROR << "User exception caught, status" << cnvE.status() << endreq;
    throw;
  }
}

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notfied about
const char* XmlDetectorElementCnv::tag() const
{
  return "detelem";
}
