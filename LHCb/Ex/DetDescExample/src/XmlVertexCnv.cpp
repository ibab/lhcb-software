// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlVertexCnv.cpp,v 1.3 2001-05-23 15:26:39 sponce Exp $

// Include files
#include "DetDesc/XmlUserDetElemCnv.h"
#include "DeVertexDetector.h"


/** @class XmlVertexCnv
 *
 * XML converter for Vertex Detector
 *
 * @author Sebastien Ponce
 */
class XmlVertexCnv : virtual public XmlUserDetElemCnv<DeVertexDetector> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlVertexCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  ~XmlVertexCnv(){}

  
protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeVertexDetector* dataObj);

};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory< XmlVertexCnv > vertexdecnv_factory;
const ICnvFactory& XmlVertexCnvFactory = vertexdecnv_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlVertexCnv::XmlVertexCnv (ISvcLocator* svc) :
  XmlUserDetElemCnv<DeVertexDetector> (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlVertexCnv::i_fillSpecificObj (DOM_Element childElement,
                                            DeVertexDetector* dataObj) {
  MsgStream log (msgSvc(), "XmlVertexCnv");
  
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  
  // In the local DTD for vertex.xml file we have defined the elements:
  // <!ELEMENT stations>
  // <!ATTLIST stations n CDATA #REQUIRED>

  if ("stations" == tagName) {
    // get a value of the 'n' attribute
    const std::string n = dom2Std (childElement.getAttribute ("n"));
    
    if (!n.empty()) {
      log << MSG::DEBUG << "n has value        : " << n << endreq;
      log << MSG::DEBUG << "n has atol value   : " << atol(n.c_str()) << endreq;
      log << MSG::DEBUG << "n has parser value : "
          << xmlSvc()->eval(n) << endreq;
      dataObj->setStations ((unsigned int)atol (n.c_str()));
    }
  }
  return StatusCode::SUCCESS;
}
