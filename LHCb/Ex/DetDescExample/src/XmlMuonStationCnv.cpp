// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlMuonStationCnv.cpp,v 1.2 2001-05-14 15:16:23 sponce Exp $

// Include files
#include "DetDesc/XmlUserDetElemCnv.h"
#include "DeMuonStation.h"


/** @class XmlMuonStationCnv
 *
 * XML converter for MuonStation
 *
 * @author Sebastien Ponce
 */
class XmlMuonStationCnv : public XmlUserDetElemCnv<DeMuonStation> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonStationCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  ~XmlMuonStationCnv() {}

  
protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeMuonStation* dataObj);

};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlMuonStationCnv> muonst_factory;
const ICnvFactory& XmlMuonStationCnvFactory = muonst_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlMuonStationCnv::XmlMuonStationCnv(ISvcLocator* svc) :
  XmlUserDetElemCnv<DeMuonStation> (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlMuonStationCnv::i_fillSpecificObj (DOM_Element childElement,
                                                 DeMuonStation* dataObj) {
  MsgStream log (msgSvc(), "XmlMuonStationCnv");
  
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());

  /*
  <!-- Aluminium plate thickness -->
  <!ELEMENT Al_plate_thickness EMPTY>
  <!ATTLIST Al_plate_thickness value CDATA #REQUIRED>

  <!-- Dimensions of pads -->
  <!ELEMENT pad_dimensions EMPTY>
  <!ATTLIST pad_dimensions padX CDATA #REQUIRED>
  <!ATTLIST pad_dimensions padY CDATA #REQUIRED>
  */

  if ("Al_plate_thickness" == tagName) {
    // get a value of the 'value' attribute
    const std::string value = dom2Std (childElement.getAttribute ("value"));
    
    if (!value.empty()) {
      log << MSG::DEBUG << "value has value           : " << value << endreq;
      log << MSG::DEBUG << "value has converted value : " <<
        xmlSvc()->eval(value.c_str()) << endreq;
      dataObj->setThickness (xmlSvc()->eval(value));
    }
  } else if ("pad_dimensions" == tagName) {
    // get a values of the 'padX' and 'padY' attributes
    const std::string padX = dom2Std (childElement.getAttribute ("padX"));
    const std::string padY = dom2Std (childElement.getAttribute ("padY"));
    
    if (!padX.empty() && !padY.empty()) {
      log << MSG::DEBUG << "padX has value           : " << padX << endreq;
      log << MSG::DEBUG << "padX has converted value : "
          << xmlSvc()->eval(padX.c_str(), false) << endreq;
      log << MSG::DEBUG << "padY has value           : " << padY << endreq;
      log << MSG::DEBUG << "padY has converted value : "
          << xmlSvc()->eval(padY.c_str(), false) << endreq;
      
      // We don't want to evaluate units so we say it explicitly for
      // eval(...) method
      dataObj->setPadx (xmlSvc()->eval(padX, false));
      dataObj->setPady (xmlSvc()->eval(padY, false));
    }
  } else {
    // Unknown tag, a warning message could be issued here
  }
}

