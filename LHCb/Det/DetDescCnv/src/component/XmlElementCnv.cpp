#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/TabulatedProperty.h"

#include "XmlTools/IXmlSvc.h"
#include "DetDescCnv/XmlCnvException.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include "XmlElementCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlElementCnv)

// -----------------------------------------------------------------------
// Material state string to state enumeration map
// -----------------------------------------------------------------------
static const std::map< std::string, eState > s_sMap = { 
                                     { "undefined", stateUndefined },
                                     { "solid",     stateSolid },
                                     { "liquid",    stateLiquid },
                                     { "gas",       stateGas } };
// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
XmlElementCnv::XmlElementCnv (ISvcLocator* svc) : 
  XmlGenericCnv (svc, CLID_Element) {
  nameString = xercesc::XMLString::transcode("name");
  temperatureString = xercesc::XMLString::transcode("temperature");
  pressureString = xercesc::XMLString::transcode("pressure");
  stateString = xercesc::XMLString::transcode("state");
  densityString = xercesc::XMLString::transcode("density");
  radlenString = xercesc::XMLString::transcode("radlen");
  lambdaString = xercesc::XMLString::transcode("lambda");
  symbolString = xercesc::XMLString::transcode("symbol");
  AString = xercesc::XMLString::transcode("A");
  ZeffString = xercesc::XMLString::transcode("Zeff"); 
  IString = xercesc::XMLString::transcode("I");
  hrefString = xercesc::XMLString::transcode("href");
  fractionmassString = xercesc::XMLString::transcode("fractionmass");
  tabpropsString = xercesc::XMLString::transcode("tabprops");
  addressString = xercesc::XMLString::transcode("address");
  isotoperefString = xercesc::XMLString::transcode("isotoperef");
  atomString = xercesc::XMLString::transcode("atom");
  
}


// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
XmlElementCnv::~XmlElementCnv () {
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&temperatureString);
  xercesc::XMLString::release((XMLCh**)&pressureString);
  xercesc::XMLString::release((XMLCh**)&stateString);
  xercesc::XMLString::release((XMLCh**)&densityString);
  xercesc::XMLString::release((XMLCh**)&radlenString);
  xercesc::XMLString::release((XMLCh**)&lambdaString);
  xercesc::XMLString::release((XMLCh**)&symbolString);
  xercesc::XMLString::release((XMLCh**)&AString);
  xercesc::XMLString::release((XMLCh**)&ZeffString);  
  xercesc::XMLString::release((XMLCh**)&IString);
  xercesc::XMLString::release((XMLCh**)&hrefString);
  xercesc::XMLString::release((XMLCh**)&fractionmassString);
  xercesc::XMLString::release((XMLCh**)&tabpropsString);
  xercesc::XMLString::release((XMLCh**)&addressString);
  xercesc::XMLString::release((XMLCh**)&isotoperefString);
  xercesc::XMLString::release((XMLCh**)&atomString);
  
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::i_createObj (xercesc::DOMElement* element,
                                       DataObject*& refpObject) {
  // creates an object for the node found
  std::string elementName = dom2Std (element->getAttribute (nameString));
 
  Element* dataObj = new Element(elementName);
  refpObject = dataObj;
  // Now we have to process more material attributes if any      
  std::string temperatureAttribute =
    dom2Std (element->getAttribute (temperatureString));
  if (!temperatureAttribute.empty()) {
    dataObj->setTemperature
      (xmlSvc()->eval(temperatureAttribute));
  }
  std::string pressureAttribute =
    dom2Std (element->getAttribute (pressureString));
  if (!pressureAttribute.empty()) {
    dataObj->setPressure
      (xmlSvc()->eval(pressureAttribute));
  }
  std::string stateAttribute = dom2Std (element->getAttribute (stateString));
  if (!stateAttribute.empty()) {
    dataObj->setState (s_sMap.at(stateAttribute));
  }
  std::string densityAttribute =
    dom2Std (element->getAttribute (densityString));
  if (!densityAttribute.empty()) 
    dataObj->setDensity
      (xmlSvc()->eval(densityAttribute));
  
  
  std::string iAttribute =
      dom2Std (element->getAttribute (IString));
  if (!iAttribute.empty()) 
      dataObj->setI (xmlSvc()->eval(iAttribute));
  


  std::string radlenAttribute = dom2Std (element->getAttribute (radlenString));
  if (!radlenAttribute.empty()) {
    dataObj->setRadiationLength
      (xmlSvc()->eval(radlenAttribute));
  }
  std::string lambdaAttribute = dom2Std (element->getAttribute (lambdaString));
  if (!lambdaAttribute.empty()) {
    dataObj->setAbsorptionLength
      (xmlSvc()->eval(lambdaAttribute));
  }
  std::string symbolAttribute = dom2Std (element->getAttribute (symbolString));
  if (!symbolAttribute.empty()) {
    dataObj->setSymbol (symbolAttribute);
  }
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::i_fillObj (xercesc::DOMElement*        childElement ,
                                     DataObject*        refpObject   , 
                                     IOpaqueAddress* /* address  */  ) 
{
  MsgStream log(msgSvc(), "XmlElementCnv" );
  
  // gets the object
  Element* dataObj = dynamic_cast<Element*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  
  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(tabpropsString, tagName)) {
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "looking at tabprops" << endmsg;
    // if we have a tabprops element, adds it to the current object
    const std::string address =
      dom2Std (childElement->getAttribute (addressString));
    long linkID = dataObj->linkMgr()->addLink(address, nullptr);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else if (0 == xercesc::XMLString::compareString(atomString, tagName)) {
    
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "looking at an atom" << endmsg;
    // Now we have to process atom attributes
    std::string aAttribute = dom2Std (childElement->getAttribute (AString));
    if (!aAttribute.empty()) 
      dataObj->setA (xmlSvc()->eval(aAttribute));
     
    std::string zeffAttribute =
      dom2Std (childElement->getAttribute (ZeffString));
    if (!zeffAttribute.empty()) 
      dataObj->setZ (xmlSvc()->eval(zeffAttribute, false));
      
  } else if (0 == xercesc::XMLString::compareString
             (isotoperefString, tagName)) {

    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "looking at an isotoperef" << endmsg;
    // Unlike XmlCatalogCnv we don't create XmlAddress hooks for children
    // we try to load the referred elements and mixtures instead
    // gets and parses the href attribute
    std::string hrefAttribute =
      dom2Std (childElement->getAttribute (hrefString));
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "href attribute is : " << hrefAttribute << endmsg;
    unsigned int poundPosition = hrefAttribute.find_last_of('#');
    // builds an entry name for the child
    std::string entryName = "/dd/Materials/" +
      hrefAttribute.substr(poundPosition + 1);
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "entry name to retrieve is " << entryName << endmsg;
    // retrieves the object corresponding to this child
    DataObject* itemObj = 0;
    StatusCode stcod = dataProvider()->retrieveObject (entryName, itemObj);
    if (stcod.isFailure()) {
      log << MSG::ERROR << "Failed to retrieve isotoperef "
          << entryName << endmsg;
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
      dom2Std (childElement->getAttribute (fractionmassString));
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
      StatusCode sc;
      sc.setCode (INVALID_CLASS_ID);
      std::string msg =
        "Wrong element or mixture composite, invalid combination";
      throw XmlCnvException(msg.c_str(),sc);
    }
  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "This tag makes no sense to element : "
        << tagNameString << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Process an object
// -----------------------------------------------------------------------
StatusCode XmlElementCnv::i_processObj (DataObject*        refpObject ,
                                        IOpaqueAddress* /* address */ ) 
{
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
    dataObj->ComputeInteractionLength(); 
    dataObj->ComputeRadiationLength();
    dataObj->ComputeMeanExcitationEnergy();
    dataObj->ComputeDensityEffect();

  
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_processObj

// ============================================================================
// End 
// ============================================================================

