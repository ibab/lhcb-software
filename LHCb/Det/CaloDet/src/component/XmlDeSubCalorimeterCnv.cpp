// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/CaloDet/src/component/XmlDeSubCalorimeterCnv.cpp,v 1.2 2001-06-14 16:09:45 sponce Exp $

// local 
#include <dom/DOM_NamedNodeMap.hpp>
#include  "XmlDeSubCalorimeterCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlDeSubCalorimeterCnv> DeSubCalorimeterFactory ;
const ICnvFactory&  XmlDeSubCalorimeterCnvFactory = DeSubCalorimeterFactory ;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlDeSubCalorimeterCnv::XmlDeSubCalorimeterCnv (ISvcLocator* svc)
  : XmlUserDetElemCnv<DeSubCalorimeter> (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode
XmlDeSubCalorimeterCnv::i_fillSpecificObj (DOM_Element childElement,
                                           DeSubCalorimeter* dataObj) {
  MsgStream log( messageService(), "XmlDeSubCalorimeterCnv" );
  
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  
  // displays the element
  log << MSG::DEBUG << "<" << tagName << " ";
  DOM_NamedNodeMap attributes = childElement.getAttributes();
  for (unsigned int i = 0; i < attributes.getLength(); i++) {
    DOM_Node node = attributes.item(i);
    log << MSG::DEBUG << dom2Std (node.getNodeName())  << "=" 
        << dom2Std (node.getNodeType()) << " "
        << dom2Std (node.getNodeValue()) << " ";
  }
  log << MSG::DEBUG << ">" << endreq;
  
  if ("CellSize" == tagName) {
    // get a value of the 'value' attribute
    std::string value = dom2Std (childElement.getAttribute ("value"));
      if (!value.empty()) {
        log << MSG::VERBOSE << "value has value           : "
            << value << endreq;
        log << MSG::VERBOSE << "value has converted value : " 
            << xmlSvc()->eval(value.c_str()) << endreq;
        dataObj->setSize (xmlSvc()->eval(value.c_str()));
      }
  } else {
    log << MSG::ERROR << "Unknown XML user tag " << tagName  << endreq;
  }
};














