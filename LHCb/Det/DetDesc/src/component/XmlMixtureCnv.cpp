// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlMixtureCnv.cpp,v 1.12 2001-12-11 10:02:29 sponce Exp $

// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/XmlCnvException.h"
#include "DetDesc/IXmlSvc.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "XmlMixtureCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlMixtureCnv> s_factoryMixture;
const ICnvFactory& XmlMixtureCnvFactory = s_factoryMixture;


// -----------------------------------------------------------------------
// Material state string to state enumeration map
// -----------------------------------------------------------------------
typedef std::map< std::string, eState, std::less<std::string> > Str2StateMap;
static Str2StateMap         s_sMap;


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
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlMixtureCnv::i_createObj (DOM_Element element,
                                       DataObject*& refpObject) {
  // Since we do not have all the needed information yet, we create
  // an empty mixture and hope we get all we need to build it properly
  Mixture* dataObj = new Mixture("", 0.0, 0);
  refpObject = dataObj;

  // We do not assume that most of the mixtures have
  // composites by fraction of mass
  m_mixMode = MM_undefined;
  
  // Now we have to process more material attributes if any
  std::string temperatureAttribute =
    dom2Std (element.getAttribute ("temperature"));
  if (!temperatureAttribute.empty()) {
    dataObj->setTemperature (xmlSvc()->eval(temperatureAttribute));
  }
  std::string pressureAttribute =
    dom2Std (element.getAttribute ("pressure"));
  if (!pressureAttribute.empty()) {
    dataObj->setPressure (xmlSvc()->eval(pressureAttribute));
  }
  std::string stateAttribute = dom2Std (element.getAttribute ("state"));
  if (!stateAttribute.empty()) {
    dataObj->setState (s_sMap[stateAttribute]);
  }
  std::string aeffAttribute = dom2Std (element.getAttribute ("Aeff"));
  if (!aeffAttribute.empty()) {
    dataObj->setA (xmlSvc()->eval(aeffAttribute));
  }
  std::string zeffAttribute = dom2Std (element.getAttribute ("Zeff"));
  if (!zeffAttribute.empty()) {
    dataObj->setZ (xmlSvc()->eval(zeffAttribute, false));
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
StatusCode XmlMixtureCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject,
                                     IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "XmlMixtureCnv" );
  
  // gets the object
  Mixture* dataObj = dynamic_cast<Mixture*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());

  // dispatches, based on the name
  if ("tabprops" == tagName) {
    log << MSG::VERBOSE << "looking at tabprops" << endreq;
    // if we have a tabprops element, adds it to the current object
    const std::string address =
      dom2Std (childElement.getAttribute ("address"));
    long linkID = dataObj->linkMgr()->addLink(address, 0);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else if ("component" == tagName) {
    StatusCode stcod;
        
    // We need to get directory where the XML files are located
    unsigned int slashPosition = address->par()[0].find_last_of('/');
    std::string locDir = address->par()[0].substr( 0, slashPosition + 1 );
    
    // builds the entry name
    std::string nameAttribute = dom2Std (childElement.getAttribute ("name"));
    std::string entryName = "/dd/Materials/" + nameAttribute;
    
    // Check if path is a relative path, if it has ../ 
    int dotsPos = entryName.find_first_of("..");
    if (-1 != dotsPos) {
      // get rid of the ".." to get the absolute path
      entryName = compactPath(entryName);
    }
    
    log << MSG::VERBOSE << "Converter for " << address->par()[1]
        << " is gonna retrieve " << entryName << endreq;

    DataObject *itemObj = 0;
    stcod = dataProvider()->retrieveObject (entryName, itemObj);
    if (!stcod.isSuccess()) {
      if (0 != itemObj) {
        itemObj->release();
      }
      log << MSG::ERROR << "Error retrieving material: " << entryName << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::VERBOSE << "Converter for " << address->par()[1]
        << " retrieved successfully " << ((Material *)itemObj)->name()
        << endreq;
    
    // Get now the natoms or fraction mass from the attributes
    // The default in the DTD is "-1" for both, so it can be used
    // to detect which is the one that is provided.
    double m_itemFraction = 0.0;
    std::string natom = dom2Std (childElement.getAttribute ("natoms"));
    std::string fract = dom2Std (childElement.getAttribute ("fractionmass"));
    
    log << MSG::VERBOSE << "MixMode has value " << m_mixMode << endreq;
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
      log << MSG::VERBOSE
          << "XmlCnvException due to natoms/fractionmass inconsistency"
          << endreq;
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
        log << MSG::VERBOSE
            << "XmlCnvException due clID inconsistency"
            << endreq;
        // This should not happen
        itemObj->release();
        StatusCode stcod;
        stcod.setCode( INVALID_CLASS_ID );
        std::string msg = 
          "Wrong element or mixture composite, invalid combination";
        throw XmlCnvException(msg.c_str(),stcod);      
      }
    } catch (const GaudiException& ge) {
      throw XmlCnvException("XmlMixtureCnv::endElement: Gaudi exception ",ge);
    } catch (...) {
      throw XmlCnvException("XmlMixtureCnv::endElement: unknown exception ");
    }
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to mixture : "
        << tagName << endreq;
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
  Mixture* dataObj = dynamic_cast<Mixture*> (refpObject);
  // Material is (hopefully) converted so
  // in case of mixture we have to compute some stuff    
  if (CLID_Mixture == address->clID()) {
    dataObj->compute() ; 
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
