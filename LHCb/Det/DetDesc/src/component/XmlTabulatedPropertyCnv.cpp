// STD & STL Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
// GaudiKernel
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
/// Detdesc 
#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvSvc.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/XmlCnvException.h"
/// local 
#include "XmlTabulatedPropertyCnv.h"


/// Factory /////////////////////////////////////////////////////////////////////////////////////
static CnvFactory<XmlTabulatedPropertyCnv>          s_FactoryXmlTabulatedPropertyCnv;
const ICnvFactory& XmlTabulatedPropertyCnvFactory = s_FactoryXmlTabulatedPropertyCnv;
/// Constructor /////////////////////////////////////////////////////////////////////////////////
XmlTabulatedPropertyCnv::XmlTabulatedPropertyCnv( ISvcLocator* svc )
  : XmlGenericCnv( svc, classID() )
  , m_tagRead    (  false         )
  , m_pcdata     (                )
  , m_xunit      (  1             )
  , m_yunit      (  1             )
{
  /// Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
};
/// Destructor /////////////////////////////////////////////////////////////////////////////////
XmlTabulatedPropertyCnv::~XmlTabulatedPropertyCnv(){};
/// Create the transient representation of an object ///////////////////////////////////////////
StatusCode XmlTabulatedPropertyCnv::createObj( IOpaqueAddress* Address, DataObject*& Object)
{
  ///
  MsgStream  log( msgSvc() , "XmlTabulatedPropertyCnv" );
  ///
  Object    = 0 ; 
  m_dataObj = Object;
  if( 0 == Address )
    { log << MSG::ERROR << "IOpaqueAddress* points to NULL" << endreq ; return StatusCode::FAILURE ; }
  ///
  GenericAddress* address = 0 ;
  address = dynamic_cast<XmlAddress*>( Address ); 
  if( 0 == address ) 
    { log << MSG::ERROR << "IOpaqueAddress* could not be cast to XmlAddress*" << endreq ; return StatusCode::FAILURE ; }
  ///  
  m_tagRead = false;
  m_pcdata  = ""   ;
  m_xunit   = 1    ;
  m_yunit   = 1    ;
  ///
  m_objRcpt = address;
  ///
  {
    StatusCode status = initParser();
    if( status.isFailure() )
      {
	log << MSG::FATAL << "XML Parser init failed, can't convert "
	    << address->objectName() << " " << endreq;
	finiParser();
	return status;
      }
  }
  ///
  {
    StatusCode status = parse( address->dbName().c_str() );
    if( status.isFailure() )
      {
	log << MSG::FATAL << "XML Parser parse failed, can't convert "
	    << address->objectName() << "!" << endreq;
	finiParser();
	return status ;
      }
  }
  ///
  {
    StatusCode status = finiParser();
    if( status.isFailure() )
      {
	log << MSG::FATAL << "XML Parser fini failed, can't convert "
	    << address->objectName() << "!" << endreq;
	return status;
      }
  }
  ///
  Object = m_dataObj;
  ///
  return 0 != Object ? StatusCode::SUCCESS : StatusCode::FAILURE ;
};
//////////////////////////////////////////////////////////////////////////////////////////
void XmlTabulatedPropertyCnv::startElement( const char* const    name ,
					    XmlCnvAttributeList& atts ) 
{
  ///
  MsgStream log( msgSvc(), "XmlTabulatedPropertyCnv" );
  ///
  const std::string tagName( name );
  ///
  log << MSG::VERBOSE << " startElement() for " << tagName << "<";
  for( unsigned int i = 0; i < atts.getLength(); i++ ) 
    {
      log << MSG::VERBOSE 
	  << " \tNAME='"  << atts.getName  ( i ) <<"'"
	  << " \tVALUE='" << atts.getValue ( i ) <<"'"
          << " \tTYPE='"  << atts.getType  ( i ) <<"'" << endreq ;
    }
  ///
  if( tag() == tagName && !m_tagRead)
    {
      // Basename of the transient object ID
      std::string::size_type pos = m_objRcpt->objectName().find_last_of('/');
      const std::string baseName = m_objRcpt->objectName().substr( pos + 1 );
      
      if( baseName != atts.getValue( "name" ) ) 
	{
	  log << MSG::WARNING 
	      << "Base name='"                        << baseName                << "'" 
	      << " \textracted from '"                << m_objRcpt->objectName() << "'"
              << " \tis not equal to atribute name='" << atts.getValue("name")   << "'" << endreq;
	}
      else /// OK
	{
	  ///
          TabulatedProperty* tab = new TabulatedProperty( baseName );
	  m_dataObj              = tab; 
	  /// get all attributes 	  
	  const std::string Type  ( atts.getValue( "type"  ) ) ;
	  const std::string xAxis ( atts.getValue( "xaxis" ) ) ;
	  const std::string yAxis ( atts.getValue( "yaxis" ) ) ;
	  const std::string xUnit ( atts.getValue( "xunit" ) ) ;
	  const std::string yUnit ( atts.getValue( "yunit" ) ) ;
	  /// 
          tab->table().clear()   ;
	  ///
          tab->setType (  Type ) ;
          tab->setXAxis( xAxis ) ;
          tab->setYAxis( yAxis ) ;
	  /// 
          m_xunit = xUnit.empty() ? 1.0 : xmlSvc()->eval( xUnit , false );
          m_yunit = yUnit.empty() ? 1.0 : xmlSvc()->eval( yUnit , false );
          ///
          m_tagRead = true;   
	}
    }      
  else if( "entry" == tagName && m_tagRead ) 
    {
      const std::string x = atts.getValue("x");
      const std::string y = atts.getValue("y");
      if     ( x.empty() ) 
	{ throw XmlCnvException("XmlTabulatedPropertyCnv: 'x' attribute for tag='entry' could bot be omitted! "); }
      else if( y.empty() ) 
	{ throw XmlCnvException("XmlTabulatedPropertyCnv: 'y' attribute for tag='entry' could bot be omitted! "); }
      const double vx = xmlSvc()->eval( x , false ) * m_xunit ;
      const double vy = xmlSvc()->eval( y , false ) * m_yunit ;
      if( 0 == m_dataObj ) { throw XmlCnvException("XmlTabulatedPropertyCnv: DataObject* points to NULL!"); }
      TabulatedProperty* tab = dynamic_cast<TabulatedProperty*> (m_dataObj); 
      if( 0 == tab       ) { throw XmlCnvException("XmlTabulatedPropertyCnv: TabulatedProperty* points to NULL!"); }
      /// add the entry to the table 
      tab->table().push_back( TabulatedProperty::Entry( vx , vy ) );
    }
  else 
    {
      log << MSG::ERROR << "Got unexpected tag/m_tagRead combinations in startElement:"
	  << atts.getValue( "name" )          << " " 
	  << ( m_tagRead ? "true" : "false" ) << " " << endreq;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////
void XmlTabulatedPropertyCnv::endElement( const char* const name )
{
  MsgStream log( msgSvc(), "XmlTabulatedPropertyCnv" );
  ///
  const std::string tagName( name );
  ///
  if      ( tag()   == tagName ) 
    {
      if( 0 == m_dataObj ) { throw XmlCnvException("XmlTabulatedPropertyCnv: DataObject* points to NULL!"); }
      TabulatedProperty* tab = dynamic_cast<TabulatedProperty*> (m_dataObj); 
      if( 0 == tab       ) { throw XmlCnvException("XmlTabulatedPropertyCnv: TabulatedProperty* points to NULL!"); }
      /// convert PCDATA 
      if( m_pcdata.empty() ) 
	{ log << MSG::VERBOSE << "no PCDATA is collected!" << endreq ; }
      else
	{
	  std::string x ;
          std::string y ;
	  std::istrstream cstr( m_pcdata.c_str() , m_pcdata.size() );
	  while( cstr >> x >> y )
	    {
	      const double vx = xmlSvc()->eval( x , false ) * m_xunit ;
	      const double vy = xmlSvc()->eval( y , false ) * m_yunit ;
	      /// add the entry to the table 
	      tab->table().push_back( TabulatedProperty::Entry( vx , vy ) );
	      ///
              x.erase() ; 
              y.erase() ;
	    }
	  if( !x.empty() || !y.empty() )
	    { throw XmlCnvException("XmlTabulatedPropertyCnv: Currupted Data #x does not match #y! "); }
	} 
      /// sort entries 
      std::sort( tab->table().begin() , tab->table().end() );
      ///
      m_xunit   = 1     ;
      m_yunit   = 1     ; 
      m_pcdata  = ""    ;
      ///
    } 
  else if ( "entry" == tagName ) 
    {
    }
  else 
    { log << MSG::ERROR << "Unknown tag in endElement='"<< tagName << "'" << endreq ; }

};
//////////////////////////////////////////////////////////////////////////////////////////
void XmlTabulatedPropertyCnv::ignorableWhitespace( const char* const chars         ,
						   const unsigned int /* length */ )
{ m_pcdata += chars ; };
//////////////////////////////////////////////////////////////////////////////////////////
void XmlTabulatedPropertyCnv::characters         ( const char* const     chars     ,
						   const unsigned int /* length */ )
{ m_pcdata += chars ; };
//////////////////////////////////////////////////////////////////////////////////////////











