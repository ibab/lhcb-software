#include "DetDesc/XmlMixtureCnv.h"
#include "DetDesc/XmlCnvException.h"

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
#include "GaudiKernel/DataObject.h"

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

#include "DetDesc/XmlExprParser.h"

#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"

extern unsigned char    XML_StorageType;

extern const    CLID&   CLID_Element;
extern const    CLID&   CLID_Mixture;

// Instantiation of a static factory class used by clients to create
// instances of this service
static CnvFactory<XmlMixtureCnv> s_factoryMixture;
const ICnvFactory& XmlMixtureCnvFactory = s_factoryMixture;

// Material state string to state enumeration map
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;

static Str2StateMap         s_sMap;

// Create the transient representation of an object.
StatusCode XmlMixtureCnv::createObj( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject)
{
  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
  StatusCode sc = StatusCode::SUCCESS;

  GenericAddress* addr;
  
  if( 0 == s_sMap.size() )                                                  {
    s_sMap.insert( Str2StateMap::value_type( std::string("undefined"), stateUndefined ) );
    s_sMap.insert( Str2StateMap::value_type( std::string("solid"), stateSolid ) );
    s_sMap.insert( Str2StateMap::value_type( std::string("liquid"), stateLiquid ) );
    s_sMap.insert( Str2StateMap::value_type( std::string("gas"), stateGas ) );
  }

  // Test and store hint
  if( 0 != pAddress )    {
    try{
      addr = dynamic_cast< XmlAddress* >( pAddress );
    }
    catch( ... )    {
      return StatusCode::FAILURE;
    }
  }

  GenericAddress* xmlAdd  = m_objRcpt;
  DataObject*     dataObj = m_dataObj;
  SAXParser*      parser  = m_xmlParser;
  unsigned long   level   = m_level;
  bool            found   = m_doFound;
  bool            frac    = m_byFraction;
  bool            send    = m_send;
  std::string     tag     = m_tag;

  m_itemObj = 0;
  m_itemFraction = 0.0;
  m_byFraction = true;
  m_objRcpt = addr;
  m_dataObj = 0;
  m_level = 0;
  m_xmlParser = 0;
  m_doFound = false;

  refpObject = 0;
  std::string gfdhjsf = addr->objectName();
  try   {
    sc = initParser();
    if( sc.isSuccess() )  {
      m_dataObj=new Mixture( "", 0.0, 0 );
      // We assume that most of the mixtures have composites by fraction of mass
      m_byFraction = true;
      sc = parse( addr->dbName().c_str() );
      if ( sc.isSuccess() )   {
        refpObject = m_dataObj;  
      }
      finiParser();
    }
  }
  catch (...)   {
    m_itemObj = 0;
    m_itemFraction = 0.0;
  }
  if ( !sc.isSuccess() )    {
    m_itemObj = 0;
    m_itemFraction = 0.0;
  }
  m_tag = tag;
  m_itemObj = 0;
  m_itemFraction = 0.0;
  m_byFraction = frac;
  m_objRcpt = xmlAdd;
  m_dataObj = dataObj;
  m_level = level;
  m_xmlParser = parser;
  m_doFound = found;
  m_send = send;
  return sc;
}

// Resolve the references of the created transient object.
StatusCode XmlMixtureCnv::fillObjRefs(
                                      IOpaqueAddress* pAddress
                                     ,DataObject* pObject
                                     )
{
  return StatusCode::SUCCESS;
}

// Update the transient object from the other representation.
StatusCode XmlMixtureCnv::updateObj(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject* //pObject
                                   )
{
  return StatusCode::SUCCESS;
}

// Convert the transient object to the requested representation
StatusCode XmlMixtureCnv::createRep(
                                     DataObject*      //pObject
                                    ,IOpaqueAddress*& //refpAddress
                                   )
{
  return StatusCode::SUCCESS;
}

// Update the converted representation of a transient object.
StatusCode XmlMixtureCnv::updateRep(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                   )
{
  return StatusCode::SUCCESS;
}

// Constructor
XmlMixtureCnv::XmlMixtureCnv( ISvcLocator* svc )
: XmlGenericCnv( svc, CLID_Mixture ),
  m_itemObj( 0 ), m_itemFraction( 0.0 ),
  m_byFraction( true )                                      {
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
}

static std::string s_collector;

void XmlMixtureCnv::startElement( const char* const name,
                                  XmlCnvAttributeList& attributes)         {

  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
  std::string tagName( name );

  s_collector = "";
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ )               {
    log << MSG::DEBUG << attributes.getName(i)  << "=" 
      << attributes.getValue(i) << " "
      << attributes.getType(i) << " ";
  }
  log << MSG::DEBUG << ">" << endreq;

  if( "material" == tagName )                                              {
    
    // Basename of the transient object ID
    unsigned int sPos = m_objRcpt->objectName().find_last_of('/');
    std::string baseName = m_objRcpt->objectName().substr( sPos + 1 );
    
    if( baseName == attributes.getValue( "name" ) )                        {
      
      // We're converter for this concrete XML tag
      // We need to create our transient representation
      // Since we do not have all the needed information yet, we create
      // an empty mixture and hope we get all we need to build it properly
      ((Material *)m_dataObj)->setName(baseName);

      XmlExprParser xep( msgSvc() );

      // Now we have to process more material attributes if any      
      std::string tAtt = attributes.getValue( "temperature" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setTemperature( xep.eval(tAtt) );
      }
      tAtt = attributes.getValue( "pressure" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setPressure( xep.eval(tAtt) );
      }
      tAtt = attributes.getValue( "state" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setState( s_sMap[ tAtt ] );
      }
      tAtt = attributes.getValue( "Aeff" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setA( xep.eval(tAtt) );
      }
      tAtt = attributes.getValue( "Zeff" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setZ( xep.eval(tAtt,false) );
      }
      tAtt = attributes.getValue( "density" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setDensity( xep.eval(tAtt) );
      }
      tAtt = attributes.getValue( "radlen" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setRadiationLength( xep.eval(tAtt) );
      }
      tAtt = attributes.getValue( "lambda" );
      if( !tAtt.empty() )                                                  {
        ((Material *)m_dataObj)->setAbsorptionLength( xep.eval(tAtt) );
      }
    }
    else                                                                   {
      // This should not happen!
      log << MSG::WARNING << "Ignoring material tag: "
          << attributes.getValue( "name" ) << endreq;
    }
  }
  else if( "materialref" == tagName )                                      {
    StatusCode        stcod;
    
    // Let's decode URI of the object location
    // We must split the URI into XML file name, container name and object name
    // General URL is in format:
    //         protocol://host.domain.name/path/to/file.xml#objectID
    unsigned int sPos    = attributes.getValue( "href" ).find_last_of('#');
    std::string location = attributes.getValue( "href" ).substr( 0, sPos );
    
    /// We need to get directory where the XML files are located
    unsigned int dPos  = m_objRcpt->dbName().find_last_of('/');
    std::string locDir = m_objRcpt->dbName().substr( 0, dPos + 1 );
    
    // OK, we got location part which can be in general of a form like:
    //  protocol://host.domain.name/path/to/file.xml
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing
      location = m_objRcpt->dbName();
    }
    else {
      dPos = location.find_last_of('/');
      if( dPos != std::string::npos ) {
        location = locDir + location.substr( dPos + 1 );
      }
      else {
        location = locDir + location;
      }
    }
    std::string entryName = "/dd/Materials/"+attributes.getValue( "href" ).substr( sPos + 1);
    log << MSG::VERBOSE << "Converter for " << m_objRcpt->objectName()
                    << " is gonna retrieve " << entryName << endreq;
    stcod = dataProvider()->retrieveObject( entryName, m_itemObj );
    if ( !stcod.isSuccess() )   {
      log << MSG::FATAL << "Error retrieving material: " << entryName << endreq;
    }
    log << MSG::VERBOSE << "Converter for " << m_objRcpt->objectName()
                     << " retrieved successfully " << ((Material *)m_itemObj)->name() << endreq;
  }
  else if( "natoms" == tagName )                                            {
    // We got now the number of atoms for the last "materialref" tag
    // Since this moment we assume that mixture is being composed by atoms
    m_byFraction = false;
    m_itemFraction = atof( attributes.getValue("value").c_str() );
  }
  else if( "fractionmass" == tagName )                                             {
    // We got now the fraction of the mass for the last "materialref" tag

    // Check if the XML data materialrefs are consistent and do not mix
    // mixture composites by atoms and by fraction
    if( false == m_byFraction )                                            {
      // XML materialrefs are not consistent, ERROR
      std::string msg = "Material references for mixture ";
      msg += m_objRcpt->objectName();
      msg += " are not consistent, please correct XML data in file ";
      msg += m_objRcpt->dbName();
      StatusCode st = CORRUPTED_DATA;
      throw XmlCnvException(msg.c_str(),st);
    }
    m_itemFraction = atof( attributes.getValue("value").c_str() );
  }
  else                                                                     {
    // Something goes wrong, does it?
    ;
  }
}

void XmlMixtureCnv::characters( const char* const chars, const unsigned int length )
{
}

void XmlMixtureCnv::ignorableWhitespace( const char* const chars,
                                         const unsigned int length )
{
}


void XmlMixtureCnv::endElement( const char* const name ) {
  
  MsgStream log(msgSvc(), "XmlMixtureCnv" );

  XmlExprParser xep( msgSvc() );
  
  log << MSG::DEBUG << "</" << name << ">" << endreq;
  
  std::string tagName = name;
  
  if( "material" == tagName ) {
    // Since we ask only for material tag XmlGenericCnv SAX handler should
    // send us only end tag of our material the others should be ignored
    // Material is (hopefully) converted so
    // in case of mixture we have to compute some stuff    
    if( CLID_Mixture == m_objRcpt->clID() ) {
      Mixture* m = dynamic_cast<Mixture*>(m_dataObj);
      if( 0 != m->nOfItems() ) {
        if( true == m_byFraction ) {
          m->computeByFraction();
        } else {
          m->computeByAtoms();
        }
      }
    } else { // What to do ?
    }
    // Just to be sure...
    m_byFraction = true;
  } else if( "materialref" == tagName ) {
    // At this point we should have loaded referred material so we need
    // to find out its form either element or mixture and add it

    if( CLID_Element == m_itemObj->clID() ) {
      Mixture* m = dynamic_cast<Mixture*>(m_dataObj);
      Element* e = dynamic_cast<Element*>(m_itemObj);
      if( false == m_byFraction ) {
        int atoms = (int)m_itemFraction;
        m->addElement( e, atoms, false );
      } else {
        m->addElement( e, m_itemFraction, false );
      }
    } else if( CLID_Mixture == m_itemObj->clID() )     {
      Mixture* m  = dynamic_cast<Mixture*>(m_dataObj);
      Mixture* mc = dynamic_cast<Mixture*>(m_itemObj);
      m->addMixture( mc, m_itemFraction, false );
    } else {
      // This should not happen
      m_itemObj->release();
      StatusCode stcod;
      stcod.setCode( INVALID_CLASS_ID );
      std::string msg = "Wrong element or mixture composite, invalid combination";
      throw XmlCnvException(msg.c_str(),stcod);      
    }
    m_itemObj = 0;
  }
}

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notified about
const char* XmlMixtureCnv::tag() const
{
  return "material";
}


