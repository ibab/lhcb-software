
// STD & STL Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

// Headers
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvSvc.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/XmlCnvException.h"



/// local 
#include "XmlIsotopeCnv.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
static CnvFactory<XmlIsotopeCnv> s_factoryIsotope;
const ICnvFactory& XmlIsotopeCnvFactory = s_factoryIsotope;

// Material state string to state enumeration map
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;
static Str2StateMap ssMap;

// Create the transient representation of an object.
StatusCode XmlIsotopeCnv::createObj( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject)
{
  MsgStream log(msgSvc(), "XmlIsotopeCnv" );
  
  StatusCode sc = StatusCode::SUCCESS;

  if( 0 == ssMap.size() ) {
    ssMap.insert( Str2StateMap::value_type( std::string("undefined"), stateUndefined ) );
    ssMap.insert( Str2StateMap::value_type( std::string("solid"), stateSolid ) );
    ssMap.insert( Str2StateMap::value_type( std::string("liquid"), stateLiquid ) );
    ssMap.insert( Str2StateMap::value_type( std::string("gas"), stateGas ) );
  }

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
  
  m_tagRead = false;

  m_objRcpt = addr;
  
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
  
  refpObject = m_dataObj;
  
  return sc;
}

// Update the transient object from the other representation.
StatusCode XmlIsotopeCnv::updateObj( 
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                   )
{
  return StatusCode::SUCCESS;
}

// Convert the transient object to the requested representation
StatusCode XmlIsotopeCnv::createRep(
                                     DataObject*      //pObject
                                    ,IOpaqueAddress*& //refpAddress
                                   )
{
  return StatusCode::SUCCESS;
}

// Update the converted representation of a transient object.
StatusCode XmlIsotopeCnv::updateRep(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                   )
{
  return StatusCode::SUCCESS;
}

// Constructor
XmlIsotopeCnv::XmlIsotopeCnv( ISvcLocator* svc )
: XmlGenericCnv( svc, CLID_Isotope ), m_tagRead( false )
{
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
}

static std::string s_collector;

void XmlIsotopeCnv::startElement( const char* const name,
                                 XmlCnvAttributeList& attributes) {
  MsgStream log(msgSvc(), "XmlIsotopeCnv" );
  
  std::string tagName( name );

  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) {
    log << MSG::DEBUG << attributes.getName(i)  << "=" 
      << attributes.getValue(i) << " "
      << attributes.getType(i) << " ";
  }

  log << MSG::DEBUG << ">" << endreq;
  
  if( "isotope" == tagName ) {
    
    // Basename of the transient object ID
    unsigned int sPos = m_objRcpt->objectName().find_last_of('/');
    std::string baseName = m_objRcpt->objectName().substr( sPos + 1 );
    
    if( baseName == attributes.getValue( "name" ) ) {
      
      // We're converter for this concrete XML tag
      // We need to create our transient representation
      // according the required class ID
      
      m_dataObj = new Isotope( baseName );

      // Now we have to process more material attributes if any

      std::string tAtt = attributes.getValue( "temperature" );
      
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setTemperature( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "pressure" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setPressure( xmlSvc()->eval(tAtt) );
      }      
      tAtt = attributes.getValue( "state" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setState( ssMap[ tAtt ] );
      }
      tAtt = attributes.getValue( "A" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setA( xmlSvc()->eval(tAtt) );
      }      
      tAtt = attributes.getValue( "Z" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setZ( xmlSvc()->eval(tAtt,false) );
      }      
      tAtt = attributes.getValue( "N" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setN( xmlSvc()->eval(tAtt,false) );
      }      
      tAtt = attributes.getValue( "density" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setDensity( xmlSvc()->eval(tAtt) );
      }      
      tAtt = attributes.getValue( "radlen" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setRadiationLength( xmlSvc()->eval(tAtt) );
      }      
      tAtt = attributes.getValue( "lambda" );
      if( !tAtt.empty() ) {
        ((Material *)m_dataObj)->setAbsorptionLength( xmlSvc()->eval(tAtt) );
      }      
    } else {
      // This should never happen!
      log << MSG::WARNING << "Got unexpected material tag: "
          << attributes.getValue( "name" ) << endreq;
    }
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to isotope: " << tagName << endreq;
  }
}

void XmlIsotopeCnv::characters(
			       const char* const  /* chars */  , 
                               const unsigned int /* length */
                              )
{
}

void XmlIsotopeCnv::ignorableWhitespace(
					const char* const  /* chars */ , 
                                        const unsigned int /* length */
					)
{
}


void XmlIsotopeCnv::endElement( const char* const /* name */ )
{
}

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notified about
const char* XmlIsotopeCnv::tag() const
{
  return "isotope";
}

