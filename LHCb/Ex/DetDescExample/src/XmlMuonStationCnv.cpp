// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlMuonStationCnv.cpp,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#include "DetDesc/XmlUserDeCnv.h"

#include "DeMuonStation.h"

class XmlMuonStationCnv : public XmlUserDeCnv<DeMuonStation>
{
public:

  XmlMuonStationCnv(ISvcLocator* svc);
  
  ~XmlMuonStationCnv()  {
  }
  
  /// Start of the XML element callback
  virtual void uStartElement( const char* const name, XmlCnvAttributeList& attributes);

};


/// Instantiation of a static factory class used by clients to create
/// instances of this service
static CnvFactory<XmlMuonStationCnv> muonst_factory;
const ICnvFactory& XmlMuonStationCnvFactory = muonst_factory;

/// Constructor
XmlMuonStationCnv::XmlMuonStationCnv(ISvcLocator* svc)
: XmlUserDeCnv<DeMuonStation>( svc, "XmlMuonStationCnv" )
{
}

/// Start of the XML element callback
void XmlMuonStationCnv::uStartElement( const char* const name, XmlCnvAttributeList& attributes)
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

  /*
  <!-- Aluminium plate thickness -->
  <!ELEMENT Al_plate_thickness EMPTY>
  <!ATTLIST Al_plate_thickness value CDATA #REQUIRED>

  <!-- Dimensions of pads -->
  <!ELEMENT pad_dimensions EMPTY>
  <!ATTLIST pad_dimensions padX CDATA #REQUIRED>
  <!ATTLIST pad_dimensions padY CDATA #REQUIRED>
  */

  if( tagName == "Al_plate_thickness" ) {

    // get a value of the 'value' attribute
    std::string value = attributes.getValue( "value" );
    
    if( !value.empty() ) {
      log << MSG::DEBUG << "value has value           : " << value               << endreq;
      log << MSG::DEBUG << "value has converted value : " << xmlSvc()->eval(value.c_str()) << endreq;
      m_dataObj->setThickness( xmlSvc()->eval(value.c_str()) );
    }
  }
  else if( tagName == "pad_dimensions" ) {
    // get a values of the 'padX' and 'padY' attributes
    std::string padX = attributes.getValue( "padX" );
    std::string padY = attributes.getValue( "padY" );
    
    if( !padX.empty() && !padY.empty() ) {
      log << MSG::DEBUG << "padX has value           : " << padX                   << endreq;
      log << MSG::DEBUG << "padX has converted value : " << xmlSvc()->eval(padX.c_str(), false) << endreq;
      log << MSG::DEBUG << "padY has value           : " << padY                   << endreq;
      log << MSG::DEBUG << "padY has converted value : " << xmlSvc()->eval(padY.c_str(), false) << endreq;
      
      // We don't want to evaluate units so we say it explicitly for eval(...) method
      m_dataObj->setPadx( xmlSvc()->eval(padX.c_str(), false) );
      m_dataObj->setPady( xmlSvc()->eval(padY.c_str(), false) );
    }
  }
  else {
    // Unknown tag, a warning message can be issued here
    ;
  }
}

