
/// local 
#include "XmlDeCalorimeterCnv.h"

///
/// Instantiation of a static factory class used by clients to create
/// instances of this service
static    CnvFactory<XmlDeCalorimeterCnv>           DeCalorimeterFactory ;
const    ICnvFactory&  XmlDeCalorimeterCnvFactory = DeCalorimeterFactory ;


///
/// Constructor
///
XmlDeCalorimeterCnv::XmlDeCalorimeterCnv ( ISvcLocator* svc )
  : XmlUserDeCnv<DeCalorimeter>( svc , "XmlDeCaloCnv" )
{};

///
/// Start of the XML element callback
///
void XmlDeCalorimeterCnv::uStartElement( const char* const name,
					 XmlCnvAttributeList& attributes) 
{
  MsgStream log( messageService(), "XmlDeCaloCnv" );
  
  const std::string tagName( name );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) 
    {
      log << MSG::DEBUG << attributes.getName(i)  << "=" 
	  << attributes.getValue(i) << " "
	  << attributes.getType(i) << " ";
    }
  
  log << MSG::DEBUG << ">" << endreq;
  
  if(      "specific"   == tagName ) 
    {
    } 
  else if( "CodingBit"  == tagName ) 
    {
      std::string value = attributes.getValue( "value" );
      if( !value.empty() ) { m_dataObj->setCoding( atoi(value.c_str()) ); }
    } 
  else if( "EtInCenter" == tagName ) 
    {
      std::string value = attributes.getValue( "value" );
      if( !value.empty() ) { m_dataObj->setEtInCenter( xmlSvc()->eval(value.c_str()) ); }
    } 
  else if( "EtSlope"  ==   tagName ) 
    {
      std::string value = attributes.getValue( "value" );
      if( !value.empty() ) { m_dataObj->setEtSlope( xmlSvc()->eval(value.c_str()) ); }
    } 
  else if( "AdcMax"   ==   tagName ) 
    {
      std::string value = attributes.getValue( "value" );
      if( !value.empty() ) { m_dataObj->setAdcMax( atoi(value.c_str()) ); }
    } 
  else if( "ActiveToTotal"   ==   tagName ) 
    {
      std::string value = attributes.getValue( "value" );
      if( !value.empty() ) { m_dataObj->setActiveToTotal( atof(value.c_str()) ); }
    } 
  else 
    { log << MSG::ERROR << "Unknown XML user tag " << tagName  << endreq; }
};

///
/// End of the XML element callback
/// void XmlDeCalorimeterCnv::uEndElement( const char* const name ) 
/// {
///   MsgStream log( messageService(), "XmlDeCalorimeterCnv" );
///   log << MSG::DEBUG << "</" << name << ">" << endreq;
///};





