// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlMixtureCnv.cpp,v 1.4 2001-03-13 11:58:09 mato Exp $

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
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/XmlCnvException.h"

// STD & STL Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

/// local 
#include "XmlMixtureCnv.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
static CnvFactory<XmlMixtureCnv> s_factoryMixture;
const ICnvFactory& XmlMixtureCnvFactory = s_factoryMixture;

// Material state string to state enumeration map
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;

static Str2StateMap         s_sMap;

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
XmlMixtureCnv::XmlMixtureCnv( ISvcLocator* svc )
: XmlGenericCnv( svc, CLID_Mixture ),
  m_itemObj( 0 ), m_itemFraction( 0.0 ) {
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );

  // Initizalie the state map
  if( 0 == s_sMap.size() )                                                  {
    s_sMap.insert( Str2StateMap::value_type( std::string("undefined"), stateUndefined ) );
    s_sMap.insert( Str2StateMap::value_type( std::string("solid"), stateSolid ) );
    s_sMap.insert( Str2StateMap::value_type( std::string("liquid"), stateLiquid ) );
    s_sMap.insert( Str2StateMap::value_type( std::string("gas"), stateGas ) );
  }
}

// -----------------------------------------------------------------------
// Create the transient representation of an object.
// -----------------------------------------------------------------------
StatusCode XmlMixtureCnv::createObj( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject)
{
  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
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

  GenericAddress* xmlAdd  = m_objRcpt;
  DataObject*     dataObj = m_dataObj;
  SAXParser*      parser  = m_xmlParser;
  unsigned long   level   = m_level;
  bool            found   = m_doFound;
  bool            send    = m_send;
  std::string     tag     = m_tag;

  m_itemObj = 0;
  m_itemFraction = 0.0;
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
      m_dataObj = new Mixture( "", 0.0, 0 );
      // We do not assume that most of the mixtures have composites by fraction of mass
      m_mixMode = MM_undefined;
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
  m_objRcpt = xmlAdd;
  m_dataObj = dataObj;
  m_level = level;
  m_xmlParser = parser;
  m_doFound = found;
  m_send = send;
  return sc;
}

// -----------------------------------------------------------------------
// Resolve the references of the created transient object.
// -----------------------------------------------------------------------
StatusCode XmlMixtureCnv::fillObjRefs(
                                      IOpaqueAddress* /* pAddress */ , 
                                      DataObject*     /* pObject  */ 
                                      )
{
  return StatusCode::SUCCESS;
}

// Update the transient object from the other representation.
StatusCode XmlMixtureCnv::updateObj(
                                    IOpaqueAddress* /* pAddress */ , 
                                    DataObject*     /* pObject  */
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


// -----------------------------------------------------------------------
// Start XML Element 
// -----------------------------------------------------------------------
void XmlMixtureCnv::startElement( const char* const name,
                                  XmlCnvAttributeList& attributes)         {

  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
  std::string tagName( name );

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
      Material* material = dynamic_cast< Material* >(m_dataObj); 
      // material->setName(baseName);

      // Now we have to process more material attributes if any      
      std::string tAtt = attributes.getValue( "temperature" );
      if( !tAtt.empty() )                                                  {
        material->setTemperature( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "pressure" );
      if( !tAtt.empty() )                                                  {
        material->setPressure( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "state" );
      if( !tAtt.empty() )                                                  {
        material->setState( s_sMap[ tAtt ] );
      }
      tAtt = attributes.getValue( "Aeff" );
      if( !tAtt.empty() )                                                  {
        material->setA( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "Zeff" );
      if( !tAtt.empty() )                                                  {
        material->setZ( xmlSvc()->eval(tAtt,false) );
      }
      tAtt = attributes.getValue( "density" );
      if( !tAtt.empty() )                                                  {
        material->setDensity( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "radlen" );
      if( !tAtt.empty() )                                                  {
        material->setRadiationLength( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "lambda" );
      if( !tAtt.empty() )                                                  {
        material->setAbsorptionLength( xmlSvc()->eval(tAtt) );
      }
    }
    else if ( "tabprops" == tagName ) 
      {
        const std::string address = attributes.getValue( "address" );
        long linkID = m_dataObj->addLink( address , 0 ) ;
        SmartRef<TabulatedProperty> ref( m_dataObj, linkID );
        ((Material*) m_dataObj)->tabulatedProperties().push_back(ref); 
      }
    else                                                                   {
      // This should not happen!
      log << MSG::WARNING << "Ignoring material tag: "
          << attributes.getValue( "name" ) << endreq;
    }
  }

  else if( "materialref" == tagName || "elementref" == tagName  ) {
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

    // Get now the natoms or fraction mass from the attributes
    // The default in the DTD is "-1" for both, so it can be used to detect which is the one
    // that is provided.
    std::string natom = attributes.getValue( "natoms" );
    std::string fract = attributes.getValue( "fractionmass" );
  
    if( m_mixMode == MM_undefined ) {
      if( tagName == "materialref") m_mixMode = MM_byFractionMass;
      else if ( natom != "-1" )     m_mixMode = MM_byNAtoms;
      else                          m_mixMode = MM_byFractionMass;
    }
    
    if( m_mixMode == MM_byFractionMass && !fract.empty() && fract != "-1" ) {
      m_itemFraction = xmlSvc()->eval(fract, false);
    }
    else if ( m_mixMode == MM_byNAtoms && !natom.empty() && natom != "-1" ) {
      m_itemFraction = xmlSvc()->eval(natom, false);
    }
    else {
      // XML materialrefs are not consistent, ERROR
      std::string msg = "Material references for material ";
      msg += m_objRcpt->objectName();
      msg += " are not consistent.";
      StatusCode st = CORRUPTED_DATA;
      throw XmlCnvException(msg.c_str(),st);
    }
  }
  else {
    // Something goes wrong, does it?
  }
}

// -----------------------------------------------------------------------
// XML characters
// -----------------------------------------------------------------------
void XmlMixtureCnv::characters( const char* const  /* chars */, 
                                const unsigned int /* length */  ) { }

// -----------------------------------------------------------------------
// XML ignorableWhitespace
// -----------------------------------------------------------------------
void XmlMixtureCnv::ignorableWhitespace( const char* const  /* chars  */ ,
                                         const unsigned int /* length */  ) { }

// -----------------------------------------------------------------------
// End XML Element 
// -----------------------------------------------------------------------
void XmlMixtureCnv::endElement( const char* const name ) 
{
  
  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
  log << MSG::VERBOSE << "</" << name << ">" << endreq;
  
  std::string tagName = name;
  try
    {      
      if( "material" == tagName ) 
        {
          // Since we ask only for material tag XmlGenericCnv SAX handler should
          // send us only end tag of our material the others should be ignored
          // Material is (hopefully) converted so
          // in case of mixture we have to compute some stuff    
          if( CLID_Mixture == m_objRcpt->clID() ) 
            {
              Mixture* m = dynamic_cast<Mixture*>(m_dataObj);
              if( 0 == m ) { throw XmlCnvException("XmlMixtureCnv::endElement: bad cast") ; }
              m->compute() ; 
              m_mixMode = MM_undefined;
            } 
          else { /* What to do here ? */ }
        } 
      else if( "materialref" == tagName || "elementref" == tagName ) 
        {
          // At this point we should have loaded referred material so we need
          // to find out its form either element or mixture and add it
          
          Mixture* m = dynamic_cast<Mixture*>(m_dataObj);
          if( 0 == m ) { throw XmlCnvException("XmlMixtureCnv::endElement: bad cast") ; }
          
          if(      CLID_Element == m_itemObj->clID() ) 
            {
              Element* e = dynamic_cast<Element*>(m_itemObj);
              if( m_mixMode == MM_byNAtoms ) 
                { m->addElement( e, (int) m_itemFraction , false ); } 
              else 
                { m->addElement( e, m_itemFraction, false ); }
            } 
          else if( CLID_Mixture == m_itemObj->clID() )     
            {
              Mixture* mc = dynamic_cast<Mixture*>(m_itemObj);
              m->addMixture( mc, m_itemFraction, false ); 
            }
          else 
            {
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
  catch( const GaudiException& ge ) 
    { throw XmlCnvException("XmlMixtureCnv::endElement: Gaudi exception ",ge); }
  catch(...) 
    { throw XmlCnvException("XmlMixtureCnv::endElement: unknown exception "); }
};

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notified about
const char* XmlMixtureCnv::tag() const
{
  return "material";
}












