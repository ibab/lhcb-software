// Include files
#include "GaudiKernel/IOpaqueAddress.h"
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
#include "DetDescCnv/XmlCnvException.h"
#include "XmlTools/IXmlSvc.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include "XmlMixtureCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlMixtureCnv)

// -----------------------------------------------------------------------
// Material state string to state enumeration map
// -----------------------------------------------------------------------
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;
static Str2StateMap s_sMap;


// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
XmlMixtureCnv::XmlMixtureCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_Mixture) {
  // Initizalie the state map
  if (0 == s_sMap.size()) {
    s_sMap.insert (Str2StateMap::value_type
                   (std::string("undefined"), stateUndefined));
    s_sMap.insert (Str2StateMap::value_type
                   (std::string("solid"), stateSolid));
    s_sMap.insert (Str2StateMap::value_type
                   (std::string("liquid"), stateLiquid));
    s_sMap.insert (Str2StateMap::value_type
                   (std::string("gas"), stateGas));
  }
  temperatureString = xercesc::XMLString::transcode("temperature");
  pressureString = xercesc::XMLString::transcode("pressure");
  stateString = xercesc::XMLString::transcode("state");
  AeffString = xercesc::XMLString::transcode("Aeff");
  ZeffString = xercesc::XMLString::transcode("Zeff");
  IeffString = xercesc::XMLString::transcode("I");
  densityString = xercesc::XMLString::transcode("density");
  radlenString = xercesc::XMLString::transcode("radlen");
  lambdaString = xercesc::XMLString::transcode("lambda");
  tabpropsString = xercesc::XMLString::transcode("tabprops");
  addressString = xercesc::XMLString::transcode("address");
  componentString = xercesc::XMLString::transcode("component");
  nameString = xercesc::XMLString::transcode("name");
  natomsString = xercesc::XMLString::transcode("natoms");
  fractionmassString = xercesc::XMLString::transcode("fractionmass");
  m_mixMode = MM_undefined;
}


// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
XmlMixtureCnv::~XmlMixtureCnv () {
  xercesc::XMLString::release((XMLCh**)&temperatureString);
  xercesc::XMLString::release((XMLCh**)&pressureString);
  xercesc::XMLString::release((XMLCh**)&stateString);
  xercesc::XMLString::release((XMLCh**)&AeffString);
  xercesc::XMLString::release((XMLCh**)&ZeffString);
  xercesc::XMLString::release((XMLCh**)&IeffString);
  xercesc::XMLString::release((XMLCh**)&densityString);
  xercesc::XMLString::release((XMLCh**)&radlenString);
  xercesc::XMLString::release((XMLCh**)&lambdaString);
  xercesc::XMLString::release((XMLCh**)&tabpropsString);
  xercesc::XMLString::release((XMLCh**)&addressString);
  xercesc::XMLString::release((XMLCh**)&componentString);
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&natomsString);
  xercesc::XMLString::release((XMLCh**)&fractionmassString);
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlMixtureCnv::i_createObj (xercesc::DOMElement* element,
                                       DataObject*& refpObject) {
  // Since we do not have all the needed information yet, we create
  // an empty mixture and hope we get all we need to build it properly
  std::string mixtureName = dom2Std (element->getAttribute (nameString));
  Mixture* dataObj = new Mixture(mixtureName, 0.0, 0);
  refpObject = dataObj;

  // We do not assume that most of the mixtures have
  // composites by fraction of mass
  m_mixMode = MM_undefined;
  
  // Now we have to process more material attributes if any
  std::string temperatureAttribute =
    dom2Std (element->getAttribute (temperatureString));
  if (!temperatureAttribute.empty()) {
    dataObj->setTemperature (xmlSvc()->eval(temperatureAttribute));
  }
  std::string pressureAttribute =
    dom2Std (element->getAttribute (pressureString));
  if (!pressureAttribute.empty()) {
    dataObj->setPressure (xmlSvc()->eval(pressureAttribute));
  }
  std::string stateAttribute = dom2Std (element->getAttribute (stateString));
  if (!stateAttribute.empty()) {
    dataObj->setState (s_sMap[stateAttribute]);
  }
  std::string aeffAttribute = dom2Std (element->getAttribute (AeffString));
  if (!aeffAttribute.empty()) {
    dataObj->setA (xmlSvc()->eval(aeffAttribute));
  }
  std::string zeffAttribute = dom2Std (element->getAttribute (ZeffString));
  if (!zeffAttribute.empty()) {
    dataObj->setZ (xmlSvc()->eval(zeffAttribute, false));
  }
  std::string ieffAttribute = dom2Std (element->getAttribute (IeffString));
  if (!ieffAttribute.empty()) {
    dataObj->setI (xmlSvc()->eval(ieffAttribute));
  }

  std::string densityAttribute =
    dom2Std (element->getAttribute (densityString));
  if (!densityAttribute.empty()) {
    dataObj->setDensity (xmlSvc()->eval(densityAttribute));
  }
  std::string radlenAttribute = dom2Std (element->getAttribute (radlenString));
  if (!radlenAttribute.empty()) {
    dataObj->setRadiationLength (xmlSvc()->eval(radlenAttribute));
  }
  std::string lambdaAttribute = dom2Std (element->getAttribute (lambdaString));
  if (!lambdaAttribute.empty()) {
    dataObj->setAbsorptionLength (xmlSvc()->eval(lambdaAttribute));
  }
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlMixtureCnv::i_fillObj (xercesc::DOMElement* childElement,
                                     DataObject* refpObject,
                                     IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
  // gets the object
  Mixture* dataObj = static_cast<Mixture*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  
  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(tabpropsString, tagName)) {
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "looking at tabprops" << endmsg;
    // if we have a tabprops element, adds it to the current object
    const std::string addr = dom2Std (childElement->getAttribute (addressString));
    long linkID = dataObj->linkMgr()->addLink(addr, nullptr);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else if (0 == xercesc::XMLString::compareString(componentString, tagName)) {
    StatusCode stcod;
        
    // builds the entry name
    std::string nameAttribute =
      dom2Std (childElement->getAttribute (nameString));
    std::string entryName = "/dd/Materials/" + nameAttribute;
    
    // Check if path is a relative path, if it has ../ 
    int dotsPos = entryName.find_first_of("..");
    if (-1 != dotsPos) {
      // get rid of the ".." to get the absolute path
      entryName = compactPath(entryName);
    }

    DataObject *itemObj = 0;
    stcod = dataProvider()->retrieveObject (entryName, itemObj);
    if (!stcod.isSuccess()) {
      if (0 != itemObj) {
        itemObj->release();
      }
      log << MSG::ERROR << "Error retrieving material: " << entryName << endmsg;
      return StatusCode::FAILURE;
    }
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "Converter for " << address->par()[1]
          << " retrieved successfully " << ((Material *)itemObj)->name()
          << endmsg;
    
    // Get now the natoms or fraction mass from the attributes
    // The default in the DTD is "-1" for both, so it can be used
    // to detect which is the one that is provided.
    double m_itemFraction = 0.0;
    std::string natom = dom2Std (childElement->getAttribute (natomsString));
    std::string fract =
      dom2Std (childElement->getAttribute (fractionmassString));
    
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "MixMode has value " << m_mixMode << endmsg;
    if (m_mixMode == MM_undefined) {
      if (natom != "-1") {
        m_mixMode = MM_byNAtoms;
      } else {
        m_mixMode = MM_byFractionMass;
      }
    }

    if (m_mixMode == MM_byFractionMass && !fract.empty() && fract != "-1") {
      m_itemFraction = xmlSvc()->eval(fract, false);
    } else if (m_mixMode == MM_byNAtoms && !natom.empty() && natom != "-1") {
      m_itemFraction = xmlSvc()->eval(natom, false);
    } else {
      // XML materialrefs are not consistent, ERROR
      itemObj->release();
      if( log.level() <= MSG::VERBOSE ) log << MSG::VERBOSE
            << "XmlCnvException due to natoms/fractionmass inconsistency"
            << endmsg;
      std::string msg = "Material references for material ";
      msg += address->par()[1];
      msg += " are not consistent.";
      StatusCode st = CORRUPTED_DATA;
      throw XmlCnvException(msg.c_str(),st);
    }
    try {
      // At this point we should have loaded referred material so we need
      // to find out its form either element or mixture and add it
      if (CLID_Element == itemObj->clID()) {
        Element* e = dynamic_cast<Element*>(itemObj);
        if (m_mixMode == MM_byNAtoms) {
          dataObj->addElement(e, (int) m_itemFraction, false);
        } else {
          dataObj->addElement(e, m_itemFraction, false);
        }
      } else if (CLID_Mixture == itemObj->clID()) {
        Mixture* mc = dynamic_cast<Mixture*>(itemObj);
        dataObj->addMixture(mc, m_itemFraction, false); 
      } else {
        if( log.level() <= MSG::VERBOSE ) log << MSG::VERBOSE
              << "XmlCnvException due clID inconsistency" << endmsg;
        // This should not happen
        itemObj->release();
        StatusCode sc;
        sc.setCode( INVALID_CLASS_ID );
        std::string msg = 
          "Wrong element or mixture composite, invalid combination";
        throw XmlCnvException(msg.c_str(),sc);      
      }
    } catch (const GaudiException& ge) {
      throw XmlCnvException("XmlMixtureCnv::endElement: Gaudi exception ",ge);
    } catch (...) {
      throw XmlCnvException("XmlMixtureCnv::endElement: unknown exception ");
    }
  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "This tag makes no sense to mixture : "
        << tagNameString << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }
  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Process an object
// -----------------------------------------------------------------------
StatusCode XmlMixtureCnv::i_processObj (DataObject* refpObject,
                                        IOpaqueAddress* address) {
  // gets the object
  Mixture* dataObj = static_cast<Mixture*> (refpObject);
  // Material is (hopefully) converted so
  // in case of mixture we have to compute some stuff    

  if (CLID_Mixture == address->clID()) {
    StatusCode sc = dataObj->compute(); 
    if( !sc.isSuccess() ) {
      MsgStream log(msgSvc(), "XmlMixtureCnv" );
      log << MSG::WARNING
          << "computation of mixture '"
          << address->registry()->identifier()
          << "' not successful. Please check XML." << endmsg;
    }
    m_mixMode = MM_undefined;
  }
  
  // returns
  return StatusCode::SUCCESS;
} // end i_processObj


// -----------------------------------------------------------------------
// Compact a path
// -----------------------------------------------------------------------
std::string XmlMixtureCnv::compactPath (std::string path) {
  int dotPos = path.find_first_of("..");
  std::string beforeDots = path.substr(0, dotPos-2);  // get rid of slash
  std::string afterDots = path.substr(dotPos+2);
  std::string parent = beforeDots.substr(0, beforeDots.find_last_of("/"));
  std::string compactPath = parent + afterDots;
  int dotsPos = compactPath.find_first_of("..");
  if (-1 != dotsPos){
    path = compactPath;
    compactPath = this->compactPath(path);
  }
  return compactPath;
}
