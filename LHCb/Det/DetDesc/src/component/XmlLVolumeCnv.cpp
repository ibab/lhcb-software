// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlLVolumeCnv.cpp,v 1.9 2001-06-05 16:17:59 sponce Exp $

// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"

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
#include "DetDesc/LVolume.h"
#include "DetDesc/Solids.h"
#include "DetDesc/Surface.h"
#include "DetDesc/XmlCnvException.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include "XmlLVolumeCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlLVolumeCnv> s_factoryLVolume;
const ICnvFactory& XmlLVolumeCnvFactory = s_factoryLVolume;


XmlLVolumeCnv::SolidItem sFound;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlLVolumeCnv::XmlLVolumeCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_LVolume),
  m_insideBoolean (false),
  m_insideParameterized (false),
  m_surfaces() {
  m_transContext = "";
  m_solid = 0;
  m_bsName = "";
  m_pvstore.clear();
  m_ppvstore.clear();
  m_bstore.clear();
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::i_createObj (DOM_Element element,
                                      DataObject*& refpObject) {
  // some initializations
  m_transContext        = "";
  m_insideBoolean       = false;
  m_insideParameterized = false;
  std::string materialName = dom2Std (element.getAttribute ("material"));
  if (materialName.empty() || materialName[0] != '/') {
    materialName.insert(0,"/dd/Materials/");
  }
  std::string magFieldName = dom2Std (element.getAttribute ("magfield"));
  std::string sensDetName = dom2Std (element.getAttribute ("sensdet"));
  std::string volName = dom2Std (element.getAttribute ("name"));
  m_solid = 0;
  m_bsName = "";

  m_pvstore.clear();
  m_ppvstore.clear();
  m_bstore.clear();
  m_surfaces.clear() ; 

  // creates the LVolume
  refpObject = new LVolume(volName, 
                           m_solid, 
                           materialName,
                           sensDetName,
                           magFieldName);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );

  // result
  StatusCode sc;
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  log << MSG::VERBOSE << "i_fillObj : tagName is " << tagName << endreq;
  // dispatches, based on the name
  if ("physvol" == tagName) {
    sc = dealWithPhysvol(childElement);
  } else if ("surf" == tagName) {
    m_surfaces.push_back(dom2Std (childElement.getAttribute ("address")));
    sc = StatusCode::SUCCESS;
  } else if ("paramphysvol" == tagName) {
    sc = dealWithParamphysvol(childElement);
  } else if ("box" == tagName) {
    sc = dealWithBox(childElement);
  } else if ("trd" == tagName) {
    sc = dealWithTrd(childElement);
  } else if ("trap" == tagName) {
    sc = dealWithTrap(childElement);
  } else if ("cons" == tagName) {
    sc = dealWithCons(childElement);
  } else if ("tubs" == tagName) {
    sc = dealWithTubs(childElement);
  } else if ("sphere" == tagName) {
    sc = dealWithSphere(childElement);
  } else if ("union" == tagName ||
             "subtraction" == tagName ||
             "intersection" == tagName) {
    sc = dealWithBoolean(childElement);
  } else if (tagName == "posXYZ") {
    sc = dealWithPosXYZ(childElement);
  } else if (tagName == "posRPhiZ") {
    sc = dealWithPosRPhiZ(childElement);
  } else if (tagName == "posRThPhi") {
    sc = dealWithPosRThPhi(childElement);
  } else if (tagName == "rotXYZ") {
    sc = dealWithRotXYZ(childElement);
  } else if (tagName == "rotAxis") {
    sc = dealWithRotAxis(childElement);
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING
        << "This tag makes no sense to LVolume: " << tagName << endreq;
    sc = StatusCode::FAILURE;
  }
  return sc;
} // end i_fillObj


// -----------------------------------------------------------------------
// Process an object
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::i_processObj (DataObject* refpObject) {
  // gets the object
  LVolume* dataObj = dynamic_cast<LVolume*> (refpObject);
  /// sets the solid
  dataObj->setSolid(m_solid);
  
  ///  Add all of the physical volumes
  std::vector<PVitem>::iterator pvit;
  for (pvit = m_pvstore.begin(); pvit != m_pvstore.end(); pvit++) {
    dataObj->createPVolume((*pvit).m_pvName,
                             (*pvit).m_lvName,
                             (*pvit).m_translation,
                             (*pvit).m_rotation );
  }
  /// Add all of the parametric physical volumes
  std::vector<ParamPV>::iterator ppvit;
  for (ppvit = m_ppvstore.begin(); ppvit != m_ppvstore.end(); ppvit++) {
    dataObj->createMultiPVolume((*ppvit).m_initialPos.m_pvName,
                                  (*ppvit).m_initialPos.m_lvName,
                                  (*ppvit).m_number,
                                  (*ppvit).m_initialPos.m_translation,
                                  (*ppvit).m_initialPos.m_rotation,
                                  (*ppvit).m_stepTranslation,
                                  (*ppvit).m_stepRotation);
  }
  /// add all surfaces 
  for (std::vector<std::string>::const_iterator it = m_surfaces.begin();
       m_surfaces.end() != it;
       ++it) {
    const std::string address = *it;
    long linkID = dataObj->addLink(address , 0);
    SmartRef<Surface> ref(dataObj, linkID);
    dataObj->surfaces().push_back(ref); 
  }
  m_surfaces.clear(); 
  m_pvstore.clear();
  m_ppvstore.clear();
  // returns
  return StatusCode::SUCCESS;
} // end i_processObj


// -----------------------------------------------------------------------
// Deal with Physical volume
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithPhysvol (DOM_Element childElement) {
  std::string nameAttribute = dom2Std (childElement.getAttribute ("name"));
  std::string logvolAttribute =
    dom2Std (childElement.getAttribute ("logvol"));
  if (m_insideParameterized) {
    ParamPV& ppv = m_ppvstore.back();
    // We need to record the starting physical volume
    ppv.m_initialPos.m_pvName = nameAttribute;
    ppv.m_initialPos.m_lvName = logvolAttribute;
  } else {
    // Ordinary physical volume
    //
    // The following code does not work if we allow to place logical volumes
    // somewhere else than into the /dd/Geometry catalog
    // pv.m_lvName  = "/dd/Geometry/";
    // 
    // Remember name
    PVitem    pv;
    pv.m_pvName = nameAttribute;
    pv.m_lvName = logvolAttribute;
    m_pvstore.push_back(pv);
  }
  setTransContext("physvol");
  dealWithPosRotChildren(childElement);
  if (!m_insideParameterized) {
    setTransContext("");
  } else {
    setTransContext("paramphysvol");
  } 
  // returns
  return StatusCode::SUCCESS;
} // end dealWithPhysVol


// -----------------------------------------------------------------------
// Deal with parametrized physical volume
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithParamphysvol (DOM_Element childElement) {
  // Remember the number of copies
  int nn = (int) xmlSvc()->eval
    (dom2Std (childElement.getAttribute ("number")), false);
  if (nn <= 0) {
    /// This is an error
    StatusCode stcod;
    stcod.setCode( CORRUPTED_DATA );
    throw XmlCnvException
      ("paramphysvolume : number of copies must be integer positive ! ",
       stcod );
  }
  // stores it
  ParamPV   parampv;
  parampv.m_number = nn; 
  m_ppvstore.push_back( parampv );
  // Indicate to the others that paramphysvol tag is the current context
  m_insideParameterized = true;
  setTransContext("paramphysvol");
  dealWithPosRotChildren(childElement);
  m_insideParameterized = false;
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithParamphysvol


// -----------------------------------------------------------------------
// Deal with box
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithBox (DOM_Element childElement) {
  std::string sizeXAttribute = dom2Std (childElement.getAttribute ("sizeX"));
  std::string sizeYAttribute = dom2Std (childElement.getAttribute ("sizeY"));
  std::string sizeZAttribute = dom2Std (childElement.getAttribute ("sizeZ"));
  if (sizeXAttribute.empty()) { sizeXAttribute = "0.0"; }
  if (sizeYAttribute.empty()) { sizeYAttribute = "0.0"; }
  if (sizeZAttribute.empty()) { sizeZAttribute = "0.0"; }
  std::string solidName = dom2Std (childElement.getAttribute ("name"));
  m_solid = new SolidBox
    (solidName,
     xmlSvc()->eval(sizeXAttribute) / 2.0,
     xmlSvc()->eval(sizeYAttribute) / 2.0,
     xmlSvc()->eval(sizeZAttribute) / 2.0);
  setTransContext(solidName);
  dealWithPosRotChildren(childElement);
  if (m_insideBoolean == true) {
    sFound.m_solid = m_solid;
    m_bstore.push_back (sFound);
  }
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithBox


// -----------------------------------------------------------------------
// Deal with trd
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithTrd (DOM_Element childElement) {
  std::string sizeX1Attribute = 
    dom2Std (childElement.getAttribute ("sizeX1"));
  std::string sizeX2Attribute =
    dom2Std (childElement.getAttribute ("sizeX2"));
  std::string sizeY1Attribute =
    dom2Std (childElement.getAttribute ("sizeY1"));
  std::string sizeY2Attribute =
    dom2Std (childElement.getAttribute ("sizeY2"));
  std::string sizeZAttribute =
    dom2Std (childElement.getAttribute ("sizeZ"));
  if (sizeX1Attribute.empty()) { sizeX1Attribute = "0.0"; }
  if (sizeX2Attribute.empty()) { sizeX2Attribute = "0.0"; }
  if (sizeY1Attribute.empty()) { sizeY1Attribute = "0.0"; }
  if (sizeY2Attribute.empty()) { sizeY2Attribute = "0.0"; }
  if (sizeZAttribute.empty()) { sizeZAttribute = "0.0"; }
  std::string trdName = dom2Std (childElement.getAttribute ("name"));
  m_solid = new SolidTrd
    (trdName,
     xmlSvc()->eval(sizeX1Attribute) / 2.0,
     xmlSvc()->eval(sizeX2Attribute) / 2.0,
     xmlSvc()->eval(sizeY1Attribute) / 2.0,
     xmlSvc()->eval(sizeY2Attribute) / 2.0,
     xmlSvc()->eval(sizeZAttribute) / 2.0);
  setTransContext(trdName);
  dealWithPosRotChildren(childElement);
  if (m_insideBoolean == true) {
    sFound.m_solid = m_solid;
    m_bstore.push_back (sFound);
  }
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithTrd


// -----------------------------------------------------------------------
// Deal with trap
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithTrap (DOM_Element childElement) {
  std::string sizeZAttribute =
    dom2Std (childElement.getAttribute ("sizeZ"));
  std::string thetaAttribute =
    dom2Std (childElement.getAttribute ("theta"));
  std::string phiAttribute =
    dom2Std (childElement.getAttribute ("phi"));
  std::string sizeY1Attribute =
    dom2Std (childElement.getAttribute ("sizeY1"));
  std::string sizeX1Attribute = 
    dom2Std (childElement.getAttribute ("sizeX1"));
  std::string sizeX2Attribute =
    dom2Std (childElement.getAttribute ("sizeX2"));
  std::string alp1Attribute =
    dom2Std (childElement.getAttribute ("alp1"));
  std::string sizeY2Attribute =
    dom2Std (childElement.getAttribute ("sizeY2"));
  std::string sizeX3Attribute = 
    dom2Std (childElement.getAttribute ("sizeX3"));
  std::string sizeX4Attribute =
    dom2Std (childElement.getAttribute ("sizeX4"));
  std::string alp2Attribute =
    dom2Std (childElement.getAttribute ("alp2"));
  if (sizeZAttribute.empty()) { sizeZAttribute = "0.0"; }
  if (thetaAttribute.empty()) { thetaAttribute = "0.0"; }
  if (phiAttribute.empty()) { phiAttribute = "0.0"; }
  if (sizeY1Attribute.empty()) { sizeY1Attribute = "0.0"; }
  if (sizeX1Attribute.empty()) { sizeX1Attribute = "0.0"; }
  if (sizeX2Attribute.empty()) { sizeX2Attribute = "0.0"; }
  if (alp1Attribute.empty()) { alp1Attribute = "0.0"; }
  if (sizeY2Attribute.empty()) { sizeY2Attribute = "0.0"; }
  if (sizeX3Attribute.empty()) { sizeX3Attribute = "0.0"; }
  if (sizeX4Attribute.empty()) { sizeX4Attribute = "0.0"; }
  if (alp2Attribute.empty()) { alp2Attribute = "0.0"; }
  std::string trapName = dom2Std (childElement.getAttribute ("name"));
  m_solid = new SolidTrap
    (trapName,
     xmlSvc()->eval(sizeZAttribute) / 2.0,
     xmlSvc()->eval(thetaAttribute),
     xmlSvc()->eval(phiAttribute),
     xmlSvc()->eval(sizeY1Attribute) / 2.0,
     xmlSvc()->eval(sizeX1Attribute) / 2.0,
     xmlSvc()->eval(sizeX2Attribute) / 2.0,
     xmlSvc()->eval(alp1Attribute),
     xmlSvc()->eval(sizeY2Attribute) / 2.0,
     xmlSvc()->eval(sizeX3Attribute) / 2.0,
     xmlSvc()->eval(sizeX4Attribute) / 2.0,
     xmlSvc()->eval(alp2Attribute));
  setTransContext(trapName);
  dealWithPosRotChildren(childElement);
  if (m_insideBoolean == true) {
    sFound.m_solid = m_solid;
    m_bstore.push_back (sFound);
  }
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithTrap


// -----------------------------------------------------------------------
// Deal with cons
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithCons (DOM_Element childElement) {
  std::string sizeZAttribute =
    dom2Std (childElement.getAttribute ("sizeZ"));
  std::string outerRadiusPZAttribute =
    dom2Std (childElement.getAttribute ("outerRadiusPZ"));
  std::string outerRadiusMZAttribute =
    dom2Std (childElement.getAttribute ("outerRadiusMZ"));
  std::string innerRadiusPZAttribute =
    dom2Std (childElement.getAttribute ("innerRadiusPZ"));
  std::string innerRadiusMZAttribute =
    dom2Std (childElement.getAttribute ("innerRadiusMZ"));
  std::string startPhiAngleAttribute =
    dom2Std (childElement.getAttribute ("startPhiAngle"));
  std::string deltaPhiAngleAttribute =
    dom2Std (childElement.getAttribute ("deltaPhiAngle"));
  if (sizeZAttribute.empty()) { sizeZAttribute = "0.0"; }
  if (outerRadiusPZAttribute.empty()) { outerRadiusPZAttribute = "0.0"; }
  if (outerRadiusMZAttribute.empty()) { outerRadiusMZAttribute = "0.0"; }
  if (innerRadiusPZAttribute.empty()) { innerRadiusPZAttribute = "0.0"; }
  if (innerRadiusMZAttribute.empty()) { innerRadiusMZAttribute = "0.0"; }
  /// both angles could be omitted simultaneously ! 
  if ((startPhiAngleAttribute.empty()) != (deltaPhiAngleAttribute.empty())) {
    StatusCode stcod;
    stcod.setCode( CORRUPTED_DATA );
    if (startPhiAngleAttribute.empty()) {
      throw XmlCnvException (" cons :: only deltaPhiAngle is provided! ",
                             stcod );
    } 
    if (deltaPhiAngleAttribute.empty()) {
      throw XmlCnvException( " cons :: only startPhiAngle is provided! ",
                             stcod );
    } 
  }
  if (startPhiAngleAttribute.empty()) { startPhiAngleAttribute = "0.0"; }
  double deltaPhiAngleValue;
  if (deltaPhiAngleAttribute.empty()) {
    deltaPhiAngleValue = 360.0 * degree;
  } else {
    deltaPhiAngleValue = xmlSvc()->eval(deltaPhiAngleAttribute);
  }
  std::string consName = dom2Std (childElement.getAttribute ("name"));
  m_solid = new SolidCons
    (consName,
     xmlSvc()->eval(sizeZAttribute) / 2.0,
     xmlSvc()->eval(outerRadiusMZAttribute),
     xmlSvc()->eval(outerRadiusPZAttribute),
     xmlSvc()->eval(innerRadiusMZAttribute),
     xmlSvc()->eval(innerRadiusPZAttribute),
     xmlSvc()->eval(startPhiAngleAttribute),
     deltaPhiAngleValue);
  setTransContext(consName);
  dealWithPosRotChildren(childElement);
  if (m_insideBoolean == true) {
    sFound.m_solid = m_solid;
    m_bstore.push_back (sFound);
  }
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithCons


// -----------------------------------------------------------------------
// Deal with tub
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithTubs (DOM_Element childElement) {
  std::string sizeZAttribute =
    dom2Std (childElement.getAttribute ("sizeZ"));
  std::string outerRadiusAttribute =
    dom2Std (childElement.getAttribute ("outerRadius"));
  std::string innerRadiusAttribute =
    dom2Std (childElement.getAttribute ("innerRadius"));
  std::string startPhiAngleAttribute =
    dom2Std (childElement.getAttribute ("startPhiAngle"));
  std::string deltaPhiAngleAttribute =
    dom2Std (childElement.getAttribute ("deltaPhiAngle"));
  if (sizeZAttribute.empty()) { sizeZAttribute = "0.0"; }
  if (outerRadiusAttribute.empty()) { outerRadiusAttribute = "0.0"; }
  if (innerRadiusAttribute.empty()) { innerRadiusAttribute = "0.0"; }
  /// both angles could be omitted simultaneously ! 
  if ((startPhiAngleAttribute.empty()) != (deltaPhiAngleAttribute.empty())) {
    StatusCode stcod;
    stcod.setCode( CORRUPTED_DATA );
    if (startPhiAngleAttribute.empty()) {
      throw XmlCnvException (" cons :: only deltaPhiAngle is provided! ",
                             stcod );
    } 
    if (deltaPhiAngleAttribute.empty()) {
      throw XmlCnvException( " cons :: only startPhiAngle is provided! ",
                             stcod );
    } 
  }
  if (startPhiAngleAttribute.empty()) { startPhiAngleAttribute = "0.0"; }
  double deltaPhiAngleValue;
  if (deltaPhiAngleAttribute.empty()) {
    deltaPhiAngleValue = 360.0 * degree;
  } else {
    deltaPhiAngleValue = xmlSvc()->eval(deltaPhiAngleAttribute);
  }
  std::string tubsName = dom2Std (childElement.getAttribute ("name"));
  m_solid = new SolidTubs
    (tubsName,
     xmlSvc()->eval(sizeZAttribute) / 2.0,
     xmlSvc()->eval(outerRadiusAttribute),
     xmlSvc()->eval(innerRadiusAttribute),
     xmlSvc()->eval(startPhiAngleAttribute),
     deltaPhiAngleValue);
  setTransContext("tubs");
  dealWithPosRotChildren(childElement);
  if (m_insideBoolean == true) {
    sFound.m_solid = m_solid;
    m_bstore.push_back (sFound);
  }
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithTubs


// -----------------------------------------------------------------------
// Deal with sphere
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithSphere (DOM_Element childElement) {
  std::string outerRadiusAttribute =
    dom2Std (childElement.getAttribute ("outerRadius"));
  std::string innerRadiusAttribute =
    dom2Std (childElement.getAttribute ("innerRadius"));
  std::string startPhiAngleAttribute =
    dom2Std (childElement.getAttribute ("startPhiAngle"));
  std::string deltaPhiAngleAttribute =
    dom2Std (childElement.getAttribute ("deltaPhiAngle"));
  std::string startThetaAngleAttribute =
    dom2Std (childElement.getAttribute ("startThetaAngle"));
  std::string deltaThetaAngleAttribute =
    dom2Std (childElement.getAttribute ("deltaThetaAngle"));
  if (outerRadiusAttribute.empty()) { outerRadiusAttribute = "0.0"; }
  if (innerRadiusAttribute.empty()) { innerRadiusAttribute = "0.0"; }
  /// both angles could be omitted simultaneously ! 
  if ((startPhiAngleAttribute.empty()) != (deltaPhiAngleAttribute.empty())) {
    StatusCode stcod;
    stcod.setCode( CORRUPTED_DATA );
    if (startPhiAngleAttribute.empty()) {
      throw XmlCnvException (" cons :: only deltaPhiAngle is provided! ",
                             stcod );
    } 
    if (deltaPhiAngleAttribute.empty()) {
      throw XmlCnvException( " cons :: only startPhiAngle is provided! ",
                             stcod );
    } 
  }
  if (startPhiAngleAttribute.empty()) { startPhiAngleAttribute = "0.0"; }
  double deltaPhiAngleValue;
  if (deltaPhiAngleAttribute.empty()) {
    deltaPhiAngleValue = 360.0 * degree;
  } else {
    deltaPhiAngleValue = xmlSvc()->eval(deltaPhiAngleAttribute);
  }
  /// both angles could be omitted simultaneously ! 
  if (startThetaAngleAttribute.empty() != deltaThetaAngleAttribute.empty()) {
    StatusCode stcod;
    stcod.setCode( CORRUPTED_DATA );
    if (startThetaAngleAttribute.empty()) {
      throw XmlCnvException (" cons :: only deltaThetaAngle is provided! ",
                             stcod );
    } 
    if (deltaThetaAngleAttribute.empty()) {
      throw XmlCnvException( " cons :: only startThetaAngle is provided! ",
                             stcod );
    } 
  }
  if (startThetaAngleAttribute.empty()) { startThetaAngleAttribute = "0.0"; }
  double deltaThetaAngleValue;
  if (deltaThetaAngleAttribute.empty()) {
    deltaThetaAngleValue = 180.0 * degree;
  } else {
    deltaThetaAngleValue = xmlSvc()->eval(deltaThetaAngleAttribute);
  }
  std::string consName = dom2Std (childElement.getAttribute ("name"));
  m_solid = new SolidSphere
    (consName,
     xmlSvc()->eval(outerRadiusAttribute),
     xmlSvc()->eval(innerRadiusAttribute),
     xmlSvc()->eval(startPhiAngleAttribute),
     deltaPhiAngleValue,
     xmlSvc()->eval(startThetaAngleAttribute),
     deltaThetaAngleValue);
  setTransContext("sphere");
  dealWithPosRotChildren(childElement);
  if (m_insideBoolean == true) {
    sFound.m_solid = m_solid;
    m_bstore.push_back (sFound);
  }
  setTransContext("");
  // returns
  return StatusCode::SUCCESS;
} // end dealWithSphere


// -----------------------------------------------------------------------
// Deal with boolean
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithBoolean (DOM_Element childElement) {
  m_insideBoolean = true;
  std::string nameAttribute = dom2Std (childElement.getAttribute ("name"));
  m_bsName = nameAttribute;
  dealWithSimpleSolidChildren(childElement);
  // creates the object
  std::string tagName = dom2Std (childElement.getNodeName());
  if ("union" == tagName) {
    SolidUnion* sol = new SolidUnion(m_bsName, m_bstore[0].m_solid);  
    for (unsigned int i = 1; i < m_bstore.size(); i++) {
      sol->unite(m_bstore[i].m_solid, 
                 m_bstore[i].m_translation, 
                 m_bstore[i].m_rotation);
    }
    m_solid = sol;
  } else if ("subtraction" == tagName) {
    SolidSubtraction* sol = 
      new SolidSubtraction(m_bsName, m_bstore[0].m_solid);
    for (unsigned int i = 1; i < m_bstore.size(); i++) {
      sol->subtract( m_bstore[i].m_solid, 
                     m_bstore[i].m_translation,  
                     m_bstore[i].m_rotation);
    }
    m_solid = sol;
  } else if ("intersection" == tagName) {
    SolidIntersection* sol =
      new SolidIntersection(m_bsName, m_bstore[0].m_solid);
    for (unsigned int i = 1; i < m_bstore.size(); i++) {
      sol->intersect(m_bstore[i].m_solid,
                     m_bstore[i].m_translation,
                     m_bstore[i].m_rotation);
    }
    m_solid = sol;
  }
  m_insideBoolean = false;
  m_bstore.clear();
  return StatusCode::SUCCESS;
} // end dealWithBoolean


// -----------------------------------------------------------------------
// Deal with posXYZ
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithPosXYZ (DOM_Element childElement) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  log << MSG::DEBUG << "Entering dealWithPosXYZ" << endreq;
  if (transContext().empty()) {
    StatusCode stcod;
    stcod.setCode(CORRUPTED_DATA);
    throw XmlCnvException
      (" posXYZ in an empty transformation context", stcod);
  } else {
    log << MSG::VERBOSE << "posXYZ tag in the context of "
        << transContext() << endreq;
    std::string xAttribute = dom2Std (childElement.getAttribute ("x"));
    std::string yAttribute = dom2Std (childElement.getAttribute ("y"));
    std::string zAttribute = dom2Std (childElement.getAttribute ("z"));
    HepPoint3D point = doTranslation(xAttribute,
                                     yAttribute,
                                     zAttribute);
    if (m_insideParameterized) {
      // Parameterized physical volume initial position
      // or step for transformation
      ParamPV& ppv = m_ppvstore.back();
      if (transContext() == "physvol") {
        // Initial position  
        ppv.m_initialPos.m_translation = point;
      } else {
        // Step for transformation
        ppv.m_stepTranslation = point;
      }
    } else if (transContext() == "physvol") {
      // Position of a ordinary physical volume
      PVitem &pv = m_pvstore.back();
      pv.m_translation = point;
    } else {
      // Position of a solid (Hope...)
      if (m_insideBoolean) {
        sFound.m_translation = point;
      } else {
        log << MSG::WARNING
            << "posXYZ for a solid makes sense only inside boolean solid"
            << endreq;
      }
    }
  }
  // returns
  return StatusCode::SUCCESS;
} // end dealWithPosXYZ


// -----------------------------------------------------------------------
// Deal with posRPhiZ
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithPosRPhiZ (DOM_Element childElement) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  if (transContext().empty()) {
    StatusCode stcod;
    stcod.setCode(CORRUPTED_DATA);  
    throw XmlCnvException
      (" posRPhiZ in an empty transformation context", stcod);
  } else {
    log << MSG::VERBOSE << "posRPhiZ tag in the context of "
        << transContext() << endreq;
    std::string rAttribute = dom2Std (childElement.getAttribute ("r"));
    std::string phiAttribute = dom2Std (childElement.getAttribute ("phi"));
    std::string zAttribute = dom2Std (childElement.getAttribute ("z"));
    HepPoint3D point = doRPhiZTranslation(rAttribute,
                                          phiAttribute,
                                          zAttribute);
    if (m_insideParameterized) {
      // Parameterized physical volume initial position
      // or step for transformation
      ParamPV& ppv = m_ppvstore.back();
      if (transContext() == "physvol") {
        // Initial position
        ppv.m_initialPos.m_translation = point;
      } else {
        // Step for transformation
        ppv.m_stepTranslation = point;
      }
    } else if (transContext() == "physvol") {
      // Posistion of a physical volume
      PVitem &pv = m_pvstore.back();
      pv.m_translation = point;
    } else {
      // Position of a solid (Hope...)
      if (m_insideBoolean) {
        sFound.m_translation = point;
      } else {
        log << MSG::WARNING
            << "posRPhiZ for a solid makes sense only inside boolean solid"
            << endreq;
      }
    }
  }
  // returns
  return StatusCode::SUCCESS;
} // end dealWithPosRPhiZ


// -----------------------------------------------------------------------
// Deal with posRThPhi
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithPosRThPhi (DOM_Element childElement) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  if (transContext().empty()) {
    StatusCode stcod;
    stcod.setCode(CORRUPTED_DATA);  
    throw XmlCnvException
      (" posRThPhi in an empty transformation context", stcod);
  } else {
    log << MSG::VERBOSE << "posRThPhi tag in the context of "
        << transContext() << endreq;
    std::string rAttribute =
      dom2Std (childElement.getAttribute ("r"));
    std::string thetaAttribute =
      dom2Std (childElement.getAttribute ("theta"));
    std::string phiAttribute =
      dom2Std (childElement.getAttribute ("phi"));
    HepPoint3D point = doRThPhiTranslation(rAttribute,
                                           thetaAttribute,
                                           phiAttribute);
    if (m_insideParameterized) {
      // Parameterized physical volume initial position
      // or step for transformation
      ParamPV& ppv = m_ppvstore.back();        
      if(transContext() == "physvol") {
        // Initial position
        ppv.m_initialPos.m_translation = point;
      } else {
        // Step for transformation
        ppv.m_stepTranslation = point;
      }
    } else if (transContext() == "physvol") {
      // Posistion of a physical volume
      PVitem &pv = m_pvstore.back();
      pv.m_translation = point;
    } else {
      // Position of a solid ( Hope... )
      if (m_insideBoolean == true) {
        sFound.m_translation = point;
      } else {
        log << MSG::WARNING
            << "posRThPhi for a solid makes sense only inside boolean solid"
            << endreq;
      }
    }
  }
  // returns
  return StatusCode::SUCCESS;
} // end dealWithPosRThPhi


// -----------------------------------------------------------------------
// Deal with rotXYZ
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithRotXYZ (DOM_Element childElement) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  if (transContext().empty()) {
    StatusCode stcod;
    stcod.setCode(CORRUPTED_DATA);  
    throw XmlCnvException
      (" rotXYZ in an empty transformation context", stcod);
  } else {
    log << MSG::VERBOSE << "rotXYZ tag in the context of "
        << transContext() << endreq;
    std::string rotXAttribute = dom2Std (childElement.getAttribute ("rotX"));
    std::string rotYAttribute = dom2Std (childElement.getAttribute ("rotY"));
    std::string rotZAttribute = dom2Std (childElement.getAttribute ("rotZ"));
    HepRotation rotation = doRotation(rotXAttribute,
                                      rotYAttribute,
                                      rotZAttribute);
    if (m_insideParameterized) {
      // Parameterized physical volume initial position
      // or step for transformation
        
      ParamPV& ppv = m_ppvstore.back();
      if (transContext() == "physvol") {
        // Initial position
        ppv.m_initialPos.m_rotation = rotation;
      } else {
        // Step for transformation
        ppv.m_stepRotation = rotation;
      }
    } else if (transContext() == "physvol") {
      // Rotation of a physical volume
      PVitem &pv = m_pvstore.back();
      pv.m_rotation = rotation;
    } else {
      // Rotation of a solid (Hope...)
      if (m_insideBoolean) {
        sFound.m_rotation = rotation;
      }
      else {
        log << MSG::WARNING
            << "rotXYZ for a solid makes sense only inside boolean solid"
            << endreq;
      }
    }
  }
  // returns
  return StatusCode::SUCCESS;
} // end dealWithRotXYZ


// -----------------------------------------------------------------------
// Deal with rotAxis
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::dealWithRotAxis (DOM_Element childElement) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  if (transContext().empty()) {
    StatusCode stcod;
    stcod.setCode(CORRUPTED_DATA);  
    throw XmlCnvException
      (" rotAxis in an empty transformation context", stcod);
  } else {
    log << MSG::VERBOSE << "rotAxis tag in the context of "
        << transContext() << endreq;
    std::string axThetaAttribute =
      dom2Std (childElement.getAttribute ("axTheta"));
    std::string axPhiAttribute =
      dom2Std (childElement.getAttribute ("axPhi"));
    std::string angleAttribute =
      dom2Std (childElement.getAttribute ("angle"));
    HepRotation rotation = doAxisRotation(axThetaAttribute,
                                          axPhiAttribute,
                                          angleAttribute);
    if (m_insideParameterized) {
      // Parameterized physical volume initial position
      // or step for transformation
      ParamPV& ppv = m_ppvstore.back();
      if (transContext() == "physvol") {
        // Initial position
        ppv.m_initialPos.m_rotation = rotation;
      } else {
        // Step for transformation
        ppv.m_stepRotation = rotation;
      }
    } else if (transContext() == "physvol") {
      // Rotation of a physical volume
      PVitem &pv = m_pvstore.back();
      pv.m_rotation = rotation;
    } else {
      // Rotation of a solid (Hope...)
      if (m_insideBoolean == true) {
        sFound.m_rotation = rotation;
      } else {
        log << MSG::WARNING
            << "rotAxis for a solid makes sense only inside boolean solid"
            << endreq;
      }
    }
  }
  // returns
  return StatusCode::SUCCESS;
} // end dealWithRotAxis


// -----------------------------------------------------------------------
// Deal with translations and rotations
// -----------------------------------------------------------------------
void XmlLVolumeCnv::dealWithPosRotChildren (DOM_Element element) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  // gets the children
  DOM_NodeList childList = element.getChildNodes();
  // scans them
  unsigned int i;
  for (i = 0; i < childList.getLength(); i++) {
    if (childList.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
      // gets the current child
      DOM_Node childNode = childList.item(i);
      DOM_Element childElement = (DOM_Element &) childNode;
      std::string tagName = dom2Std (childElement.getNodeName());
      if ("posXYZ" == tagName) {
        dealWithPosXYZ(childElement);
      } else if ("posRPhiZ" == tagName) {
        dealWithPosRPhiZ(childElement);
      } else if ("posRThPhi" == tagName) {
        dealWithPosRThPhi(childElement);
      } else if ("rotXYZ" == tagName) {
        dealWithRotXYZ(childElement);
      } else if ("rotAxis" == tagName) {
        dealWithRotAxis(childElement);
      } else {
        log << MSG::WARNING
            << "This tag makes no sense : " << tagName << endreq;
      }
    }
  }
}


// -----------------------------------------------------------------------
// Deal with simple solid
// -----------------------------------------------------------------------
void XmlLVolumeCnv::dealWithSimpleSolidChildren (DOM_Element element) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  DOM_NodeList childList = element.getChildNodes();
  // scans them
  unsigned int i;
  for (i = 0; i < childList.getLength(); i++) {
    if (childList.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
      // gets the current child
      DOM_Node childNode = childList.item(i);
      DOM_Element childElement = (DOM_Element &) childNode;
      std::string tagName = dom2Std (childElement.getNodeName());
      if ("box" == tagName) {
        dealWithBox(childElement);
      } else if ("cons" == tagName) {
        dealWithCons(childElement);
      } else if ("sphere" == tagName) {
        dealWithSphere(childElement);
      } else if ("tubs" == tagName) {
        dealWithTubs(childElement);
      } else if ("trd" == tagName) {
        dealWithTrd(childElement);
      } else if ("trap" == tagName) {
        dealWithTrap(childElement);
      } else {
        log << MSG::WARNING
            << "This tag makes no sense : " << tagName << endreq;
      }
    }
  }
}


// -----------------------------------------------------------------------
// Create 3D translation
// -----------------------------------------------------------------------
HepPoint3D XmlLVolumeCnv::doRPhiZTranslation (std::string r,
                                              std::string phi,
                                              std::string z) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv::doRPhiZTranslation" );

  HepPoint3D ret;
  
  if (!r.empty() && !phi.empty() && !z.empty()) {

    // We must construct a unit vector here!!!!!!
    HepPoint3D tmp(1,1,1);
    ret = tmp;

    if (0 <= xmlSvc()->eval(r)) {
      ret.setPerp (xmlSvc()->eval (r));
    } else {
      StatusCode stcod;
      stcod.setCode (CORRUPTED_DATA);
      throw XmlCnvException
        ("doRPhiZTranslation: r must be non-negative value!", stcod);
    }
    
    ret.setPhi (xmlSvc()->eval(phi));
    ret.setZ (xmlSvc()->eval(z));
  }

  return ret;
}


// -----------------------------------------------------------------------
// Create 3D translation
// -----------------------------------------------------------------------
HepPoint3D XmlLVolumeCnv::doRThPhiTranslation (std::string r,
                                               std::string theta,
                                               std::string phi) {
  MsgStream log(msgSvc(), "XmlLVolumeCnv::doRThPhiTranslation" );
  HepPoint3D ret;

  if (!r.empty() && !theta.empty() && !phi.empty()) {

    // We must construct a unit vector here!!!!!!
    HepPoint3D tmp(1,1,1);
    ret = tmp;

    if (0 <= xmlSvc()->eval(r)) {
      ret.setMag (xmlSvc()->eval(r));
    } else {
      StatusCode stcod;
      stcod.setCode (CORRUPTED_DATA);
      throw XmlCnvException
        ("doRThPhiTranslation: r must be non-negative value!", stcod);
    }

    ret.setTheta (xmlSvc()->eval(theta));
    ret.setPhi (xmlSvc()->eval(phi));
  }
  
  return ret;
}


// -----------------------------------------------------------------------
// Create 3D translation
// -----------------------------------------------------------------------
HepPoint3D XmlLVolumeCnv::doTranslation (std::string x,
                                         std::string y,
                                         std::string z) {
  MsgStream log (msgSvc(), "XmlLVolumeCnv::doTranslation");

  HepPoint3D ret;
  
  if (!x.empty() && !y.empty() && !z.empty()) {
    double l_x = xmlSvc()->eval( x ); 
    double l_y = xmlSvc()->eval( y ); 
    double l_z = xmlSvc()->eval( z ); 
    
    ret.setX( l_x );
    ret.setY( l_y );
    ret.setZ( l_z );
  }

  return ret;
}


// -----------------------------------------------------------------------
// Create 3D rotation
// -----------------------------------------------------------------------
HepRotation XmlLVolumeCnv::doRotation (std::string rx,
                                       std::string ry,
                                       std::string rz) {
  MsgStream log (msgSvc(), "XmlLVolumeCnv::doRotation");

  HepRotation rot;

  if (!rx.empty() && !ry.empty() && !rz.empty()) {
    rot =       HepRotateX3D (xmlSvc()->eval(rx)).getRotation();
    rot = rot * HepRotateY3D (xmlSvc()->eval(ry)).getRotation();
    rot = rot * HepRotateZ3D (xmlSvc()->eval(rz)).getRotation();
    rot = rot.inverse();
  }

  return rot;
}


// -----------------------------------------------------------------------
// Create 3D rotation
// -----------------------------------------------------------------------
HepRotation XmlLVolumeCnv::doAxisRotation (std::string axtheta,
                                           std::string axphi,
                                           std::string angle) {
  MsgStream log (msgSvc(), "XmlLVolumeCnv::doAxisRotation");
  
  HepRotation rot;
  
  if (!axtheta.empty() && !axphi.empty() && !angle.empty()) {
    double axt  = xmlSvc()->eval (axtheta);
    double axp  = xmlSvc()->eval (axphi);
    double angl = xmlSvc()->eval (angle);
    
    if (axt < 0 || axt > 180 * degree) { 
      StatusCode stcod;
      stcod.setCode (CORRUPTED_DATA);
      throw XmlCnvException
        (" doAxisRotation: axTheta must be inside 0*degree"
         " and 180*degree ! ", stcod);
    }
    
    if (axp < 0 || axp > 360 * degree) { 
      StatusCode stcod;
      stcod.setCode (CORRUPTED_DATA);
      throw XmlCnvException 
        (" doAxisRotation: axPhi   must be inside 0*degree"
         " and 360*degree ! ", stcod);
    }

    // Construction of vector with input of theta and phi
    rot = HepRotate3D (angl,
                       Hep3Vector (sin(axt)*cos(axp),
                                   sin(axt)*sin(axp),
                                   cos(axt))).getRotation();
    rot = rot.inverse();
  }
  
  return rot;
}
