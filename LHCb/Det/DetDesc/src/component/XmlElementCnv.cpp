// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlElementCnv.cpp,v 1.7 2001-06-25 09:02:50 sponce Exp $

// Include Files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/IXmlSvc.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/XmlCnvException.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "XmlElementCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlElementCnv> s_factoryMixture;
const ICnvFactory& XmlElementCnvFactory = s_factoryMixture;


// -----------------------------------------------------------------------
// Material state string to state enumeration map
// -----------------------------------------------------------------------
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;
static Str2StateMap         s_sMap;


// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
XmlElementCnv::XmlElementCnv (ISvcLocator* svc) : 
  XmlGenericCnv (svc, CLID_Element) {
  // Initialize State map
  if (0 == s_sMap.size()) {
    s_sMap.insert (Str2StateMap::value_type
                   (std::string ("undefined"), stateUndefined));
    s_sMap.insert (Str2StateMap::value_type
                   (std::string ("solid"), stateSolid));
    s_sMap.insert (Str2StateMap::value_type
                   (std::string ("liquid"), stateLiquid));
    s_sMap.insert (Str2StateMap::value_type
                   (std::string ("gas"), stateGas));
  }
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::i_createObj (DOM_Element element,
                                       DataObject*& refpObject) {
  // creates an object for the node found
  std::string elementName = dom2Std (element.getAttribute ("name"));
  Element* dataObj = new Element(elementName);
  refpObject = dataObj;
  
  // Now we have to process more material attributes if any      
  std::string temperatureAttribute =
    dom2Std (element.getAttribute ("temperature"));
  if (!temperatureAttribute.empty()) {
    dataObj->setTemperature
      (xmlSvc()->eval(temperatureAttribute));
  }
  std::string pressureAttribute = dom2Std (element.getAttribute ("pressure"));
  if (!pressureAttribute.empty()) {
    dataObj->setPressure
      (xmlSvc()->eval(pressureAttribute));
  }
  std::string stateAttribute = dom2Std (element.getAttribute ("state"));
  if (!stateAttribute.empty()) {
    dataObj->setState (s_sMap[stateAttribute]);
  }
  std::string densityAttribute = dom2Std (element.getAttribute ("density"));
  if (!densityAttribute.empty()) {
    dataObj->setDensity
      (xmlSvc()->eval(densityAttribute));
  }
  std::string radlenAttribute = dom2Std (element.getAttribute ("radlen"));
  if (!radlenAttribute.empty()) {
    dataObj->setRadiationLength
      (xmlSvc()->eval(radlenAttribute));
  }
  std::string lambdaAttribute = dom2Std (element.getAttribute ("lambda"));
  if (!lambdaAttribute.empty()) {
    dataObj->setAbsorptionLength
      (xmlSvc()->eval(lambdaAttribute));
  }
  std::string symbolAttribute = dom2Std (element.getAttribute ("symbol"));
  if (!symbolAttribute.empty()) {
    dataObj->setSymbol (symbolAttribute);
  }
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject) {
  MsgStream log(msgSvc(), "XmlElementCnv" );
  
  // gets the object
  Element* dataObj = dynamic_cast<Element*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if ("tabprops" == tagName) {
    log << MSG::VERBOSE << "looking at tabprops" << endreq;
    // if we have a tabprops element, adds it to the current object
    const std::string address =
      dom2Std (childElement.getAttribute ("address"));
    long linkID = dataObj->addLink(address, 0);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else if ("atom" == tagName) {
    log << MSG::VERBOSE << "looking at an atom" << endreq;
    // Now we have to process atom attributes
    std::string aAttribute = dom2Std (childElement.getAttribute ("A"));
    if (!aAttribute.empty()) {
      dataObj->setA (xmlSvc()->eval(aAttribute));
    }
    std::string zeffAttribute = dom2Std (childElement.getAttribute ("Zeff"));
    if (!zeffAttribute.empty()) {
      dataObj->setZ (xmlSvc()->eval(zeffAttribute, false));
    }
  } else if ("isotoperef" == tagName) {
    log << MSG::VERBOSE << "looking at an isotoperef" << endreq;
    // Unlike XmlCatalogCnv we don't create XmlAdress hooks for children
    // we try to load the referred elements and mixtures instead
    // gets and parses the href attribute
    std::string hrefAttribute = dom2Std (childElement.getAttribute ("href"));
    log << MSG::VERBOSE << "href attribute is : " << hrefAttribute << endreq;
    unsigned int poundPosition = hrefAttribute.find_last_of('#');
    // builds an entry name for the child
    std::string entryName = "/dd/Materials/" +
      hrefAttribute.substr(poundPosition + 1);
    log << MSG::VERBOSE << "entry name to retrieve is " << entryName
        << endreq;
    // retrieves the object corresponding to this child
    DataObject* itemObj = 0;
    StatusCode stcod = dataProvider()->retrieveObject (entryName, itemObj);
    if (stcod.isFailure()) {
      log << MSG::ERROR << "Failed to retrieve isotoperef "
          << entryName << endreq;
      stcod = CANT_RETRIEVE_OBJECT;
      itemObj->release();
      std::string msg = "Can't retrieve material ";
      msg += entryName + " ";
      msg += __FILE__;
      throw XmlCnvException (msg.c_str(), stcod);
    }
    // gets the fraction mass from the attributes
    // The default in the DTD is "-1" for both,
    // so it can be used to detect which is the one that is provided.
    double m_itemFraction = 0.0;
    std::string fractionMassAttribute =
      dom2Std (childElement.getAttribute ("fractionmass"));
    if (!fractionMassAttribute.empty()) {
      m_itemFraction = xmlSvc()->eval (fractionMassAttribute, false);
    }
    // At this point we should have loaded referred material so we need
    // to find out its form either element or mixture and add it    
    if (CLID_Isotope == itemObj->clID()) {
      Isotope* is = dynamic_cast<Isotope*>(itemObj);
      dataObj->addIsotope(is, m_itemFraction, false);
    } else {
      itemObj->release();
      StatusCode stcod;
      stcod.setCode (INVALID_CLASS_ID);
      std::string msg =
        "Wrong element or mixture composite, invalid combination";
      throw XmlCnvException(msg.c_str(),stcod);
    }
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to element : "
        << tagName << endreq;
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Process an object
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::i_processObj (DataObject* refpObject) {
  // gets the object
  Element* dataObj = dynamic_cast<Element*> (refpObject);
  // computes some values for this object
  if (0 != dataObj->nOfIsotopes()) {
    dataObj->compute();
  } else {
    // We created the element from scratch so now we need to compute
    // the derived quantities
    dataObj->ComputeCoulombFactor();
    dataObj->ComputeLradTsaiFactor();
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_processObj

