// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlCalorimeterCnv.cpp,v 1.3 2001-05-23 15:26:38 sponce Exp $

// Include files
#include "DetDesc/XmlUserDetElemCnv.h"
#include "DeCalorimeter.h"


/** @class XmlCalorimeterCnv
 *
 * XML converter for Calorimeter
 *
 * @author Sebastien Ponce
 */
class XmlCalorimeterCnv : public XmlUserDetElemCnv<DeCalorimeter> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlCalorimeterCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  ~XmlCalorimeterCnv() {}

  
protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeCalorimeter* dataObj);

};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlCalorimeterCnv> calost_factory;
const ICnvFactory& XmlCalorimeterCnvFactory = calost_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlCalorimeterCnv::XmlCalorimeterCnv (ISvcLocator* svc) :
  XmlUserDetElemCnv<DeCalorimeter> (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlCalorimeterCnv::i_fillSpecificObj (DOM_Element childElement,
                                                 DeCalorimeter* dataObj) {
  MsgStream log (msgSvc(), "XmlCalorimeterCnv");
  
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  
  if ("CellSize" == tagName) {
    // get a value of the 'value' attribute
    const std::string value = dom2Std (childElement.getAttribute ("value"));
    
    if (!value.empty()) {
      log << MSG::DEBUG << "value has value        : " << value << endreq;
      log << MSG::DEBUG << "value has atof value   : "
          << atof (value.c_str()) << endreq;
      log << MSG::DEBUG << "value has parser value : "
          << xmlSvc()->eval(value) << endreq;
      dataObj->setSize (xmlSvc()->eval(value));
    }
  } else {
    // Unknown tag, a warning message could be issued here
  }
  return StatusCode::SUCCESS;
}

