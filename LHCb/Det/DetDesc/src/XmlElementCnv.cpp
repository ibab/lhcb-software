// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlElementCnv.cpp,v 1.5 2001-01-25 14:19:55 ibelyaev Exp $
#include "DetDesc/XmlElementCnv.h"
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

#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
static CnvFactory<XmlElementCnv> s_factoryMixture;
const ICnvFactory& XmlElementCnvFactory = s_factoryMixture;

// Material state string to state enumeration map
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;

static Str2StateMap         s_sMap;

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
XmlElementCnv::XmlElementCnv( ISvcLocator* svc )
: XmlGenericCnv( svc, CLID_Element ),
  m_itemObj( 0 ), m_itemFraction( 0.0 )
{
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );

  // Initialize State map
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
StatusCode XmlElementCnv::createObj( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject)
{
  MsgStream log(msgSvc(), "XmlElementCnv" );
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
  
  sc = initParser();
  if( sc.isFailure() )
  {
    log << MSG::FATAL << "XML Parser init failed, can't convert "
      << addr->objectName() << "!" << endreq;
    finiParser();
    return sc;
  }
  
  // This is the moment the whole file is parse. The callbacks startElement(), endElement()
  // will be called. It is not currently very efficient since the file is parse many
  // times.
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

// -----------------------------------------------------------------------
// Update the transient object from the other representation.
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::updateObj( IOpaqueAddress* /*pAddress*/,
                                     DataObject*     /*pObject*/ ) {
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------
// Convert the transient object to the requested representation
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::createRep( DataObject*     /*pObject*/,
                                     IOpaqueAddress*& /*refpAddress*/ ){ 
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------
// Update the converted representation of a transient object.
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::updateRep( IOpaqueAddress* /*pAddress*/,
                                     DataObject*     /*pObject*/ ) {
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------
// XML start element
// -----------------------------------------------------------------------
void XmlElementCnv::startElement( const char* const name,
                                  XmlCnvAttributeList& attributes)         {
  MsgStream log(msgSvc(), "XmlElementCnv" );
  std::string tagName( name );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ )               {
    log << MSG::DEBUG << attributes.getName(i)  << "=" 
      << attributes.getValue(i) << " "
      << attributes.getType(i) << " ";
  }
  log << MSG::DEBUG << ">" << endreq;

  if( "element" == tagName )                                              {
    // Basename of the transient object ID
    unsigned int sPos = m_objRcpt->objectName().find_last_of('/');
    std::string baseName = m_objRcpt->objectName().substr( sPos + 1 );
    if( baseName == attributes.getValue( "name" ) )                        {
      // We're converter for this concrete XML tag
      // We need to create our transient representation
      // according the required class ID
      Element* el = new Element( baseName );
      m_dataObj = el;

      // Now we have to process more material attributes if any      
      std::string tAtt = attributes.getValue( "temperature" );
      if( !tAtt.empty() )                                                  {
        el->setTemperature( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "pressure" );
      if( !tAtt.empty() )                                                  {
        el->setPressure( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "state" );
      if( !tAtt.empty() )                                                  {
        el->setState( s_sMap[ tAtt ] );
      }
      tAtt = attributes.getValue( "density" );
      if( !tAtt.empty() )                                                  {
        el->setDensity( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "radlen" );
      if( !tAtt.empty() )                                                  {
        el->setRadiationLength( xmlSvc()->eval(tAtt) );
      }
      tAtt = attributes.getValue( "lambda" );
      if( !tAtt.empty() )                                                  {
        el->setAbsorptionLength( xmlSvc()->eval(tAtt) );
      }
    }
    else                                                                   {
      // This should not happen!
      log << MSG::WARNING << "Ignoring material tag: "
          << attributes.getValue( "name" ) << endreq;
    }
  }
  else if( "atom" == tagName )                                            {
    // Now we have to process atom attributes
    Element* el = dynamic_cast<Element*>(m_dataObj);
    std::string tAtt = attributes.getValue( "A" );
    if( !tAtt.empty() )                                                  {
      el->setA( xmlSvc()->eval(tAtt) );
    }
    tAtt = attributes.getValue( "Zeff" );
    if( !tAtt.empty() )                                                  {
      el->setZ( xmlSvc()->eval(tAtt,false) );
    }
  }
  else if( "isotoperef" == tagName )                                      {
    // Unlike XmlCatalogCnv we don't create XmlAdress hooks for children
    // we try to load the referred elements and mixtures instead
    unsigned int sPos    = attributes.getValue( "href" ).find_last_of('#');
    std::string entryName = "/dd/Materials/"+attributes.getValue( "href" ).substr( sPos + 1);
    StatusCode stcod = dataProvider()->retrieveObject( entryName, m_itemObj );    
    if( stcod.isFailure() )                                                {
      stcod = CANT_RETRIEVE_OBJECT;
      std::string msg = "Can't retrieve material ";
      msg += entryName+" ";
      msg +=__FILE__;
      throw XmlCnvException( msg.c_str(), stcod );
    }

    // Get now the fraction mass from the attributes
    // The default in the DTD is "-1" for both, so it can be used to detect which is the one
    // that is provided.
    std::string fract = attributes.getValue( "fractionmass" );
    if( !fract.empty() )                                                  {
      m_itemFraction = xmlSvc()->eval(fract,false);
    }

    // At this point we should have loaded referred material so we need
    // to find out its form either element or mixture and add it    
    if( CLID_Isotope == m_itemObj->clID() )                               {
      Element* el = dynamic_cast<Element*>(m_dataObj);
      Isotope* is = dynamic_cast<Isotope*>(m_itemObj);
      el->addIsotope( is, m_itemFraction, false );
    }
    else                                                                   {
      StatusCode stcod;
      stcod.setCode( INVALID_CLASS_ID );
      std::string msg = "Wrong element or mixture composite, invalid combination";
      throw XmlCnvException(msg.c_str(),stcod);      
    }
  }
}

// -----------------------------------------------------------------------
// XML characters
// -----------------------------------------------------------------------
void XmlElementCnv::characters(
			       const char* const  chars , 
			       const unsigned int /* length */
			       )
{
  MsgStream log(msgSvc(), "XmlElementCnv" );
  log << MSG::VERBOSE << "\"" << chars << "\"" << endreq;
}

// -----------------------------------------------------------------------
// XML ignorableWhitespaces
// -----------------------------------------------------------------------
void XmlElementCnv::ignorableWhitespace(
					const char* const chars , 
                                        const unsigned int /* length */ 
					)
{
  MsgStream log(msgSvc(), "XmlElementCnv" );
  log << MSG::VERBOSE << "\"" << chars << "\"" << endreq;
}

// -----------------------------------------------------------------------
// XML end element
// -----------------------------------------------------------------------
void XmlElementCnv::endElement( const char* const name ) {
  
  MsgStream log(msgSvc(), "XmlElementCnv" );

  log << MSG::DEBUG << "</" << name << ">" << endreq;
  
  std::string tagName = name;

  if( "element" == tagName ) {
    // Since we ask only for material tag XmlGenericCnv SAX handler should
    // send us only end tag of our material the others should be ignored
    // Material is (hopefully) converted so
    // in case of an element we have to compute some stuff    
    if( CLID_Element == m_objRcpt->clID() ) {
      
      Element* el = dynamic_cast<Element*>(m_dataObj);
      
      if( 0 != el->nOfIsotopes() ) {
        el->compute();
      } else {
        // We created the element from scratch so now we need to compute
        // the derived quantities
        el->ComputeCoulombFactor();
        el->ComputeLradTsaiFactor();
      }
    } else {
      // What to do ?
      ;
    }
  }
}

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notified about
const char* XmlElementCnv::tag() const
{
  return "element";
}


