// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlCalorimeterCnv.cpp,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#include "DetDesc/XmlUserDeCnv.h"

#include "DeCalorimeter.h"


class XmlCalorimeterCnv : public XmlUserDeCnv<DeCalorimeter>
{
public:

  XmlCalorimeterCnv(ISvcLocator* svc);
  
  ~XmlCalorimeterCnv()  {
  }
  
  /// Start of the XML element callback
  virtual void uStartElement( const char* const name, XmlCnvAttributeList& attributes);

};

/// Instantiation of a static factory class used by clients to create
/// instances of this service
static CnvFactory<XmlCalorimeterCnv> calost_factory;
const ICnvFactory& XmlCalorimeterCnvFactory = calost_factory;

/// Constructor
XmlCalorimeterCnv::XmlCalorimeterCnv(ISvcLocator* svc)
: XmlUserDeCnv<DeCalorimeter>( svc, "XmlCalorimeterCnv" )
{
}

/// Start of the XML element callback
void XmlCalorimeterCnv::uStartElement( const char* const name, XmlCnvAttributeList& attributes)
{
  MsgStream log( msgSvc(), m_msId );
  
  //XmlExprParser xep( log );

  std::string tagName( name );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) {
    log << MSG::DEBUG << attributes.getName(i)  << "=" 
      << attributes.getValue(i) << " "
      << attributes.getType(i) << " ";
    ;
  }
  log << ">" << endreq;

  if( tagName == "CellSize" ) {

    // get a value of the 'value' attribute
    std::string value = attributes.getValue( "value" );
    
    if( !value.empty() ) {
      log << MSG::DEBUG << "value has value        : " << value               << endreq;
      log << MSG::DEBUG << "value has atof value   : " << atof(value.c_str()) << endreq;
      log << MSG::DEBUG << "value has parser value : " << xmlSvc()->eval(value.c_str()) << endreq;
      m_dataObj->setSize( xmlSvc()->eval(value.c_str()) );
    }
  }
  else {
    // Unknown tag, a warning message can be issued here
    ;
  }
}

