// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlVertexCnv.cpp,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#include "DetDesc/XmlUserDeCnv.h"

#include "DeVertexDetector.h"

class XmlVertexCnv : virtual public XmlUserDeCnv<DeVertexDetector>
{

public:

  XmlVertexCnv( ISvcLocator* svc );
  ~XmlVertexCnv(){}
  virtual void uStartElement( const char* const name, XmlCnvAttributeList& attributes);
};

/// Instantiation of a static factory class used by clients to create
/// instances of this service
static CnvFactory< XmlVertexCnv > vertexdecnv_factory;
const ICnvFactory& XmlVertexCnvFactory = vertexdecnv_factory;

XmlVertexCnv::XmlVertexCnv( ISvcLocator* svc )
: XmlUserDeCnv<DeVertexDetector>( svc, "XmlVertexCnv" )
{
}

/// Start of the XML element callback
void XmlVertexCnv::uStartElement( const char* const name, XmlCnvAttributeList& attributes)
{
  MsgStream log( msgSvc(), m_msId );
  
  std::string tagName( name );
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ ) {
    log << MSG::DEBUG << attributes.getName(i)  << "=" 
      << attributes.getValue(i) << " "
      << attributes.getType(i) << " ";
    ;
  }
  log << ">" << endreq;

  // In the local DTD for vertex.xml file we have defined the elements:
  // <!ELEMENT stations>
  // <!ATTLIST stations n CDATA #REQUIRED>

  if( "stations" == tagName ) {

    // get a value of the 'n' attribute
    std::string n = attributes.getValue( "n" );
    
    if( !n.empty() ) {
      log << MSG::DEBUG << "n has value        : " << n << endreq;
      log << MSG::DEBUG << "n has atol value   : " << atol(n.c_str()) << endreq;
      log << MSG::DEBUG << "n has parser value : " << xmlSvc()->eval(n.c_str()) << endreq;
      m_dataObj->setStations( (unsigned int)atol( n.c_str() ) );
    }
  }
}
