// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/CaloDet/src/component/XmlDeCalorimeterCnv.cpp,v 1.3 2001-06-14 16:09:45 sponce Exp $

/// Include files
#include <dom/DOM_NamedNodeMap.hpp>
#include "XmlDeCalorimeterCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlDeCalorimeterCnv> DeCalorimeterFactory ;
const ICnvFactory& XmlDeCalorimeterCnvFactory = DeCalorimeterFactory ;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlDeCalorimeterCnv::XmlDeCalorimeterCnv (ISvcLocator* svc)
  : XmlUserDetElemCnv<DeCalorimeter> (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlDeCalorimeterCnv::i_fillSpecificObj (DOM_Element childElement,
                                                   DeCalorimeter* dataObj) {
  MsgStream log (msgSvc(), "XmlDeCaloCnv");
  
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
  
  if ("CodingBit" == tagName) {
    std::string value = dom2Std (childElement.getAttribute ("value"));
    if (!value.empty()) { dataObj->setCoding (atoi (value.c_str())); }
  } else if( "EtInCenter" == tagName ) {
    std::string value = dom2Std (childElement.getAttribute ("value"));
    if (!value.empty()) { 
      dataObj->setEtInCenter( xmlSvc()->eval(value.c_str()) ); 
    }
  } else if( "EtSlope"  ==   tagName ) {
    std::string value = dom2Std (childElement.getAttribute ("value"));
    if (!value.empty()) { 
      dataObj->setEtSlope( xmlSvc()->eval(value.c_str()) ); 
    }
  } else if( "AdcMax"   ==   tagName ) {
    std::string value = dom2Std (childElement.getAttribute ("value"));
    if (!value.empty()) { dataObj->setAdcMax( atoi(value.c_str()) ); }
  } else if( "ActiveToTotal"   ==   tagName ) {
    std::string value = dom2Std (childElement.getAttribute ("value"));
    if (!value.empty()) { dataObj->setActiveToTotal( atof(value.c_str()) );}
  } else if( "ZShowerMax"   ==   tagName ) {
    std::string value = dom2Std (childElement.getAttribute ("value"));
    if (!value.empty()) { dataObj->setZShowerMax( atof(value.c_str()) );}
  } else { 
    log << MSG::ERROR << "Unknown XML user tag " << tagName  << endreq; 
  }
};






