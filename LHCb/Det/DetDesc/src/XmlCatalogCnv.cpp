///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlCatalogCnv.cpp,v 1.6 2001-01-29 13:59:50 ibelyaev Exp $

/// Include files

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>


// gaudi kernel 
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

// detdesc 
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/CLIDIsotope.h"
#include "DetDesc/CLIDElement.h"
#include "DetDesc/CLIDMixture.h"
#include "DetDesc/XmlCnvException.h"


// local 
#include "XmlCatalogCnv.h"


/// Instantiation of a static factory class used by clients to create
/// instances of this service
static CnvFactory<XmlCatalogCnv> s_factory;
const ICnvFactory& XmlCatalogCnvFactory = s_factory;

// Create the transient representation of an object.
StatusCode XmlCatalogCnv::createObj( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject)   {
  MsgStream log( msgSvc(), "XmlCatalogCnv" );

  StatusCode sc = StatusCode::SUCCESS;
  GenericAddress* addr;
  m_nestedDir  = 0;

  // Test and store hint
  if( 0 != pAddress )    {
    try {
      addr = dynamic_cast< XmlAddress* >( pAddress );
    }
    catch( ... )    {
      return StatusCode::FAILURE;
    }
  }
  
  m_objRcpt = addr;

  sc = initParser();
  if( sc.isFailure() ) {
    log << MSG::FATAL
        << "XML Parser init failed, can't convert "
        << addr->objectName() << "!" << endreq;
    finiParser();
    return sc;
  }
  m_ignore = false;
  sc = parse( addr->dbName().c_str() );
    
  if( sc.isFailure() ) {
    finiParser();
    return StatusCode::FAILURE;
  }

  sc = finiParser();
  
  if( sc.isFailure() ) {
    log << MSG::FATAL
        << "XML Parser init failed, can't convert "
        << addr->objectName() << "!" << endreq;
    return sc;
  }
  refpObject = m_dataObj;
  return sc;
}

/// Update the transient object from the other representation.
StatusCode XmlCatalogCnv::updateObj(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                   )
{
  return StatusCode::SUCCESS;
}

/// Convert the transient object to the requested representation
StatusCode XmlCatalogCnv::createRep(
                                     DataObject*      //pObject
                                    ,IOpaqueAddress*& //refpAddress
                                   )
{
  return StatusCode::SUCCESS;
}

/// Update the converted representation of a transient object.
StatusCode XmlCatalogCnv::updateRep(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                   )
{
  return StatusCode::SUCCESS;
}

void XmlCatalogCnv::checkConverterExistence(const CLID& clsID)   {
    // Checking the other incoming guys according to our DTD!
    bool           cnvExists;
    ICnvManager*   cnvMgr;

    StatusCode stcod = serviceLocator()->queryInterface(IID_ICnvManager,
                                                        (void **)&cnvMgr );
    if( stcod.isFailure() ) {
      stcod.setCode( CANT_QUERY_INTERFACE );  
      throw XmlCnvException( "Query to ICnvManager interface failed", stcod );
    }

    // Check whether the converter for this storage type and class ID is available
    cnvExists = cnvMgr->existsConverter( XML_StorageType, clsID );
    cnvMgr->release();
    if( !cnvExists ) {
        MsgStream log(msgSvc(), "XmlCatalogCnv" );
        log << MSG::ERROR
            << "File " << m_objRcpt->dbName()
            << " class ID "
            << clsID << ", proper converter not found" << endreq;
      stcod.setCode( INVALID_CLASS_ID );  
      throw XmlCnvException( "Unknown class ID", stcod );
    } 
}
      
void XmlCatalogCnv::startElement( const char* const name,
                                  XmlCnvAttributeList& attributes)
{
  if ( m_ignore )   {
    return;
  }
  MsgStream log(msgSvc(), "XmlCatalogCnv" );
  XmlAddress* xmlAddr = 0;
  DataObject* pObj = 0;
  std::string tagName( name ), entryName( "_0_" );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) {
    log << MSG::DEBUG << attributes.getName(i)  << "=" << attributes.getValue(i) << " " << attributes.getType(i) << " ";
  }
  log << MSG::DEBUG << ">" << endreq;

  // We need to perform lookup to avoid duplicated entries on the transient store
  RegistryEntry* searchedDir = 0;
  // transient name manipulation & setting lookup directory
  if( m_nestedDir == 0 )  {
    searchedDir = dynamic_cast<RegistryEntry*>( m_objRcpt->directory() );
  }
  else  {
    searchedDir = m_nestedDir;
  }
  
  if( "catalog" == tagName ) {
    
    // Basename of the transient object ID
    unsigned int sPos    = m_objRcpt->objectName().find_last_of('/');
    std::string baseName = m_objRcpt->objectName().substr( sPos + 1 );
    std::string aname    = attributes.getValue( "name" );
    if( baseName ==  aname ) {
      
      // We're converter for this concrete XML tag
      // We need to create our transient representation
      m_dataObj = new DataObject();
    }
    else
    {
      // This our subcatalog (embedded XML definition) so we need to prepare XmlAddresses
      // for our children in our IDataDirectory
      // We have to decode class ID of the object
      const CLID& clsID = (unsigned long)atol( attributes.getValue( "classID" ).c_str() );
      if( clsID != CLID_Catalog )
      {
        log << MSG::ERROR << "File " << m_objRcpt->dbName()  << " object " << m_objRcpt->objectName()
                          << ": Catalog class ID should be " << CLID_Catalog << " found " << clsID << endreq;
        // We need an exception to be thrown at this point to
        // handle this fatal problem, because there is no way
        // to get out of here!
        StatusCode stcod = INVALID_CLASS_ID; throw XmlCnvException( "Unknown class ID", stcod );
      }
      else
      {
        xmlAddr = new XmlAddress( clsID, m_objRcpt->dbName(), m_objRcpt->objectName() );
        pObj = new DataObject();                        // New XML address + daughter itself
        entryName =  "/"+attributes.getValue( "name" ); // OK, we get object ID
      }
    }
  }
  else if( tagName == "catalogref" || tagName == "detelemref" ||
           tagName == "logvolref" )
  {
    // We have to decode class ID of the referred object
    const CLID& clsID = (unsigned long)atol( attributes.getValue( "classID" ).c_str() );
    checkConverterExistence(clsID);
    // Let's decode URI of the object location
    // We must split the URI into XML file name, container name and object name
    // General URL is in format: protocol://host.domain.name/path/to/file.xml#objectID
    unsigned int sPos     = attributes.getValue( "href" ).find_last_of('#');
    entryName = "/"+attributes.getValue( "href" ).substr( sPos + 1);
    std::string location  = attributes.getValue( "href" ).substr( 0, sPos );
    
    /// We need to get directory where the XML files are located
    unsigned int dPos  = m_objRcpt->dbName().find_last_of('/');
    std::string locDir = m_objRcpt->dbName().substr( 0, dPos + 1 );
    
    // OK, we got location part which can be in general of a form like:
    // protocol://host.domain.name/path/to/file.xml
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing
      location = m_objRcpt->dbName();
    }
    else {
      location = locDir + location;
    }
    // Create new XML address for the daughter
    xmlAddr = new XmlAddress( clsID, location, searchedDir->fullpath());
  }
  else if( tagName == "detelem" || tagName == "logvol" )
  {
    
    // Checking the other incoming guys according to our DTD!
    // We have to decode class ID of the referred object
    const CLID& clsID = (unsigned long)atol( attributes.getValue( "classID" ).c_str() );
    checkConverterExistence(clsID);
    // OK, we get object ID
    entryName = "/"+attributes.getValue( "name" );
    // Create new XML address for the daughter
    xmlAddr = new XmlAddress( clsID, m_objRcpt->dbName(), searchedDir->fullpath());
    m_ignore = true;
  }
  else if( tagName == "isotope" || tagName == "element" || tagName == "material" )
  {
    unsigned long classIDFound = 0;
    if( tagName == "isotope" ) {
      classIDFound = CLID_Isotope;
    }
    else if( tagName == "element" ) {
      classIDFound = CLID_Element;
    }
    else if( tagName == "material" ) {
      classIDFound = CLID_Mixture;
    }
    const CLID& clsID = classIDFound;
    checkConverterExistence(clsID);
    // OK, we get object ID
    entryName = "/"+attributes.getValue( "name" );
    // Create new XML address for the daughter
    xmlAddr = new XmlAddress( clsID, m_objRcpt->dbName(), searchedDir->fullpath());
    m_ignore = true;
  }
  else
  {
    // Ooops, you should not have done it again :-)
    ;
  }

  if( xmlAddr )
  {
    // Now we got it so let's iterate over the entries to find whether there is
    // a RegistryEntry of an object we want to create address hook for
    StatusCode status = searchedDir->add(entryName, xmlAddr);
    if ( !status.isSuccess() )   {
      status = searchedDir->add(entryName, xmlAddr);
      log << MSG::FATAL << " File " << __FILE__ << " line " << __LINE__ << endreq;
      log << MSG::FATAL << " XML address:" << endreq;
      log << MSG::FATAL << " entryName:" << entryName << " objectName: " << xmlAddr->objectName()
                        << " containerName:" << xmlAddr->containerName()
                        << " dbName:" << xmlAddr->dbName() << endreq;
      xmlAddr->release(); xmlAddr = 0; StatusCode stcod = ERROR_ADDING_TO_TS;
      throw XmlCnvException( "Error adding registry entry to detector transient store", stcod );
      return;
    }
    if ( 0 != pObj )
    {
      IDataDirectory* pDir = searchedDir->find(entryName);
      searchedDir = dynamic_cast<RegistryEntry*>(pDir);
      if ( 0 != searchedDir )
      {
        searchedDir->setObject(pObj);
        // We say to our children that they are nested and should be put at the next level
        m_nestedDir  = searchedDir;
      }
      else
      {
        log << MSG::FATAL << " File " << __FILE__ << " line " << __LINE__ << endreq;
        log << MSG::FATAL << " XML address:" << endreq;
        log << MSG::FATAL << " entryName:" << entryName << " objectName: " << xmlAddr->objectName()
                          << " containerName:" << xmlAddr->containerName()
                          << " dbName:" << xmlAddr->dbName() << endreq;
        xmlAddr->release(); xmlAddr = 0; StatusCode stcod = ERROR_ADDING_TO_TS;
        throw XmlCnvException( "Error setting a dataobject in the detector transient store", stcod );
      }
    }
    log << MSG::VERBOSE << " XML address:objectName: " << xmlAddr->objectName()
                        << "  containerName:" << xmlAddr->containerName()
                        << "  dbName:" << xmlAddr->dbName() << endreq;
  }
}

void XmlCatalogCnv::endElement( const char* const name ) {

  MsgStream log( msgSvc(), "XmlCatalogCnv" );
  log << MSG::DEBUG << "</" << name << ">" << endreq;
  std::string tagName = name;
  if( tagName == "catalog" )      {
    if ( m_nestedDir != 0 )   {
      // We need to find our parent RegistryEntry
      IDataDirectory* parentDir  = m_nestedDir->parent();
      m_nestedDir                = dynamic_cast<RegistryEntry*>(parentDir);
    }
  }
  else if( tagName == "detelem" || tagName == "logvol" || tagName == "material" ||
           tagName == "isotope" || tagName == "element" )  {
    m_ignore = false;
  }
}

const CLID& XmlCatalogCnv::classID()
{
  return CLID_Catalog;
}

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notfied about
const char* XmlCatalogCnv::tag() const
{
  return "catalog";
}

/// Constructor
XmlCatalogCnv::XmlCatalogCnv(ISvcLocator* svc)
  : XmlGenericCnv( svc, CLID_Catalog ), m_nestedDir( 0 )
{
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
}

