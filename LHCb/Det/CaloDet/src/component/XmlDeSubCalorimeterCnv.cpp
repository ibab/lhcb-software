
// local 
#include  "XmlDeSubCalorimeterCnv.h"


///
/// Instantiation of a static factory class used by clients to create
/// instances of this service
static    CnvFactory<XmlDeSubCalorimeterCnv>           DeSubCalorimeterFactory ;
const    ICnvFactory&  XmlDeSubCalorimeterCnvFactory = DeSubCalorimeterFactory ;


///
/// Constructor
///
XmlDeSubCalorimeterCnv::XmlDeSubCalorimeterCnv ( ISvcLocator* svc )
  : XmlUserDeCnv<DeSubCalorimeter>( svc , "XmlDeSubCalorimeterCnv" )
{};


///
/// Start of the XML element callback
///
void XmlDeSubCalorimeterCnv::uStartElement( const char* const name,
					    XmlCnvAttributeList& attributes) {
  MsgStream log( messageService(), "XmlDeSubCalorimeterCnv" );
  
  std::string tagName( name );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) 
    {
      log << MSG::DEBUG << attributes.getName(i)  << "=" 
	  << attributes.getValue(i) << " "
	  << attributes.getType(i) << " ";
      ;
    }
  log << ">" << endreq;
  
  if      ( "specific" == tagName ) 
    {
    }
  else if ( "CellSize" == tagName ) 
    {
      
      // get a value of the 'value' attribute
      std::string value = attributes.getValue( "value" );
      if( !value.empty() ) 
	{
	  log << MSG::VERBOSE << "value has value           : " << value               << endreq;
	  log << MSG::VERBOSE << "value has converted value : " 
                              << xmlSvc()->eval(value.c_str()) << endreq;
	  m_dataObj->setSize(  xmlSvc()->eval(value.c_str()) );
	}
    } 
  else 
    { log << MSG::ERROR << "Unknown XML user tag " << tagName  << endreq; }
};














