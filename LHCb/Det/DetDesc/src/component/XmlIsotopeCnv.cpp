// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlIsotopeCnv.cpp,v 1.8 2001-11-20 15:22:24 sponce Exp $

// include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/IXmlSvc.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/XmlCnvException.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "XmlIsotopeCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlIsotopeCnv> s_factoryIsotope;
const ICnvFactory& XmlIsotopeCnvFactory = s_factoryIsotope;


// -----------------------------------------------------------------------
// Material state string to state enumeration map
// -----------------------------------------------------------------------
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;
static Str2StateMap ssMap;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlIsotopeCnv::XmlIsotopeCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_Isotope) {
  // Initializes State map
  if (0 == ssMap.size()) {
    ssMap.insert (Str2StateMap::value_type
                  (std::string ("undefined"), stateUndefined));
    ssMap.insert (Str2StateMap::value_type
                  (std::string("solid"), stateSolid));
    ssMap.insert (Str2StateMap::value_type
                  (std::string("liquid"), stateLiquid));
    ssMap.insert (Str2StateMap::value_type
                  (std::string("gas"), stateGas));
  }
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlIsotopeCnv::i_createObj (DOM_Element element,
                                       DataObject*& refpObject) {
  // creates an object for the node found
  std::string elementName = dom2Std (element.getAttribute ("name"));
  Isotope* dataObj = new Isotope (elementName);
  refpObject = dataObj;
  
  // Now we have to process more material attributes if any
  std::string temperatureAttribute =
    dom2Std (element.getAttribute ("temperature"));
  if (!temperatureAttribute.empty()) {
    dataObj->setTemperature (xmlSvc()->eval(temperatureAttribute));
  }
  std::string pressureAttribute = dom2Std (element.getAttribute ("pressure"));
  if (!pressureAttribute.empty()) {
    dataObj->setPressure (xmlSvc()->eval(pressureAttribute));
  }
  std::string stateAttribute = dom2Std (element.getAttribute ("state"));
  if (!stateAttribute.empty()) {
    dataObj->setState (ssMap[stateAttribute]);
  }
  std::string aAttribute = dom2Std (element.getAttribute ( "A" ));
  if (!aAttribute.empty()) {
    dataObj->setA (xmlSvc()->eval(aAttribute));
  }
  std::string zAttribute = dom2Std (element.getAttribute ( "Z" ));
  if (!zAttribute.empty()) {
    dataObj->setZ (xmlSvc()->eval(zAttribute, false));
  }
  std::string nAttribute = dom2Std (element.getAttribute ( "N" ));
  if (!nAttribute.empty()) {
    dataObj->setN (xmlSvc()->eval(nAttribute, false));
  }
  std::string densityAttribute = dom2Std (element.getAttribute ("density"));
  if (!densityAttribute.empty()) {
    dataObj->setDensity (xmlSvc()->eval(densityAttribute));
  }
  std::string radlenAttribute = dom2Std (element.getAttribute ("radlen"));
  if (!radlenAttribute.empty()) {
    dataObj->setRadiationLength (xmlSvc()->eval(radlenAttribute));
  }
  std::string lambdaAttribute = dom2Std (element.getAttribute ("lambda"));
  if (!lambdaAttribute.empty()) {
    dataObj->setAbsorptionLength (xmlSvc()->eval(lambdaAttribute));
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlIsotopeCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject) {
  MsgStream log(msgSvc(), "XmlElementCnv" );

  // gets the object
  Isotope* dataObj = dynamic_cast<Isotope*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if ("tabprops" == tagName) {
    // if we have a tabprops element, adds it to the current object
    const std::string address =
      dom2Std (childElement.getAttribute("address"));
    long linkID = dataObj->linkMgr()->addLink(address, 0);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to isotope: "
        << tagName << endreq;
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj

