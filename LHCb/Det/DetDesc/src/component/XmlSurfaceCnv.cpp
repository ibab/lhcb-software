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
#include "XmlSurfaceCnv.h"


/// Factory /////////////////////////////////////////////////////////////////////////////////////
static CnvFactory<XmlSurfaceCnv>          s_FactoryXmlSurfaceCnv;
const ICnvFactory& XmlSurfaceCnvFactory = s_FactoryXmlSurfaceCnv;
/// Constructor /////////////////////////////////////////////////////////////////////////////////
XmlSurfaceCnv::XmlSurfaceCnv( ISvcLocator* svc )
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
XmlSurfaceCnv::~XmlSurfaceCnv(){};
/// Create the transient representation of an object ///////////////////////////////////////////
StatusCode XmlSurfaceCnv::createObj( IOpaqueAddress* Address, DataObject*& Object)
{
  ///
  MsgStream  log( msgSvc() , "XmlSurfaceCnv" );
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
void XmlSurfaceCnv::startElement( const char* const    name ,
					    XmlCnvAttributeList& atts ) 
{
  ///
  MsgStream log( msgSvc(), "XmlSurfaceCnv" );
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
	  Surface* surface = new Surface( baseName );
	  m_dataObj        = surface ; 
	  /// model 
 	  {
	    const std::string    value ( atts.getValue( "model"      )  );
	    const double       v_value = xmlSvc()->eval( value , false ) ;
	    const unsigned int i_value = (unsigned int) v_value          ; 
	    if( i_value != v_value ) 
	      { 
		log << MSG::WARNING 
		    <<  " Surface("         << baseName <<")"  
		    <<  " parameter '"      << "model"  << "'" 
                    << "("                  << value    << ")"
                    <<  "="                 << v_value 
		    <<  " is truncated to=" << i_value  << endreq; 
	      }           
	    surface->setModel ( i_value ); 
	  }
	  /// finish
 	  {
	    const std::string    value ( atts.getValue( "finish"      )  );
	    const double       v_value = xmlSvc()->eval( value , false ) ;
	    const unsigned int i_value = (unsigned int) v_value          ; 
	    if( i_value != v_value ) 
	      { 
		log << MSG::WARNING 
		    <<  " Surface("         << baseName <<")"  
		    <<  " parameter '"      << "finish" << "'" 
                    << "("                  << value    << ")"
                    <<  "="                 << v_value 
		    <<  " is truncated to=" << i_value  << endreq; 
	      }           
	    surface->setFinish ( i_value ); 
	  }
	  /// type 
 	  {
	    const std::string    value ( atts.getValue( "type"   )  );
	    const double       v_value = xmlSvc()->eval( value , false ) ;
	    const unsigned int i_value = (unsigned int) v_value          ; 
	    if( i_value != v_value ) 
	      { 
		log << MSG::WARNING 
		    <<  " Surface("         << baseName <<")"  
		    <<  " parameter '"      << "type"   << "'"  
                    << "("                  << value    << ")"
                    <<  "="                 << v_value 
		    <<  " is truncated to=" << i_value  << endreq; 
	      }           
	    surface->setType ( i_value ); 
	  }
	  /// value 
 	  {
	    const std::string    value ( atts.getValue( "value"    )  );
	    const double       v_value = xmlSvc()->eval( value , false ) ;
	    surface->setValue ( v_value ); 
	  }
	  /// first volume  
 	  {
	    const std::string    value ( atts.getValue( "volfirst" )  );
	    surface->setFirstVol( value ); 
	  }
	  /// second volume  
 	  { 
	    const std::string    value ( atts.getValue( "volsecond" )  );
	    surface->setSecondVol( value ); 
	  }
          ///
          m_tagRead = true;   
	}
    }      
  else if( "tabprops" == tagName && m_tagRead ) 
    {
      if( 0 == m_dataObj ) { throw XmlCnvException("XmlSurfaceCnv: DataObject* points to NULL!"); }
      Surface* surf = dynamic_cast<Surface*> (m_dataObj); 
      if( 0 == surf      ) { throw XmlCnvException("XmlSurfaceCnv: Surface* points to NULL!"); }
      /// add the entry to the table
      long linkID = m_dataObj->addLink( atts.getValue( "address" ) , 0 ) ;
      SmartRef<TabulatedProperty> ref( m_dataObj, linkID );
      surf->tabulatedProperties().push_back(ref);
    }
  else 
    {
      log << MSG::ERROR << "Got unexpected tag/m_tagRead combinations in startElement:"
	  << atts.getValue( "name" )          << " " 
	  << ( m_tagRead ? "true" : "false" ) << " " << endreq;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////
void XmlSurfaceCnv::endElement( const char* const name )
{
  MsgStream log( msgSvc(), "XmlSurfaceCnv" );
  ///
  const std::string tagName( name );
  ///
  if      ( tag()      == tagName ) {  /* do nothing */ }  
  else if ( "tabprops" == tagName ) {  /* do nothing */ }
  else 
    { 
      log << MSG::FATAL << "Unknown tag in endElement='"<< tagName << "'" << endreq ; 
      throw XmlCnvException("XmlSurfaceCnv: unknown tag in endElement="+tagName);
    }
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////










