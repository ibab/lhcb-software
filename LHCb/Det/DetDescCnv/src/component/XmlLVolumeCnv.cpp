// $Id: XmlLVolumeCnv.cpp,v 1.16 2009-05-04 14:57:09 ocallot Exp $ 
// Include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/LogVolBase.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/LAssembly.h"
#include "DetDesc/Surface.h"

#include "DetDescCnv/XmlCnvException.h"

#include <sstream>
#include <cstdlib>
#include <float.h>
#include <map>

#include <xercesc/dom/DOMNodeList.hpp>

#include "XmlLVolumeCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlLVolumeCnv)

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlLVolumeCnv::XmlLVolumeCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_LVolume) {
  unionString = xercesc::XMLString::transcode("union");
  subtractionString = xercesc::XMLString::transcode("subtraction");
  intersectionString = xercesc::XMLString::transcode("intersection");
  boxString = xercesc::XMLString::transcode("box");
  trdString = xercesc::XMLString::transcode("trd");
  trapString = xercesc::XMLString::transcode("trap");
  consString = xercesc::XMLString::transcode("cons");
  polyconeString = xercesc::XMLString::transcode("polycone");
  tubsString = xercesc::XMLString::transcode("tubs");
  sphereString = xercesc::XMLString::transcode("sphere");
  transformationString = xercesc::XMLString::transcode("transformation");
  posXYZString = xercesc::XMLString::transcode("posXYZ");
  posRPhiZString = xercesc::XMLString::transcode("posRPhiZ");
  posRThPhiString = xercesc::XMLString::transcode("posRThPhi");
  rotXYZString = xercesc::XMLString::transcode("rotXYZ");
  rotAxisString = xercesc::XMLString::transcode("rotAxis");
  paramphysvolString = xercesc::XMLString::transcode("paramphysvol");
  paramphysvol2DString = xercesc::XMLString::transcode("paramphysvol2D");
  paramphysvol3DString = xercesc::XMLString::transcode("paramphysvol3D");
  materialString = xercesc::XMLString::transcode("material");
  magfieldString = xercesc::XMLString::transcode("magfield");
  sensdetString = xercesc::XMLString::transcode("sensdet");
  nameString = xercesc::XMLString::transcode("name");
  physvolString = xercesc::XMLString::transcode("physvol");
  surfString = xercesc::XMLString::transcode("surf");
  logvolString = xercesc::XMLString::transcode("logvol");
  numberString = xercesc::XMLString::transcode("number");
  addressString = xercesc::XMLString::transcode("address");
  outerRadiusMZString = xercesc::XMLString::transcode("outerRadiusMZ");
  outerRadiusPZString = xercesc::XMLString::transcode("outerRadiusPZ");
  innerRadiusMZString = xercesc::XMLString::transcode("innerRadiusMZ");
  innerRadiusPZString = xercesc::XMLString::transcode("innerRadiusPZ");
  outerRadiusString = xercesc::XMLString::transcode("outerRadius");
  innerRadiusString = xercesc::XMLString::transcode("innerRadius");
  sizeXString = xercesc::XMLString::transcode("sizeX");
  sizeX1String = xercesc::XMLString::transcode("sizeX1");
  sizeX2String = xercesc::XMLString::transcode("sizeX2");
  sizeX3String = xercesc::XMLString::transcode("sizeX3");
  sizeX4String = xercesc::XMLString::transcode("sizeX4");
  sizeYString = xercesc::XMLString::transcode("sizeY");
  sizeY1String = xercesc::XMLString::transcode("sizeY1");
  sizeY2String = xercesc::XMLString::transcode("sizeY2");
  sizeZString = xercesc::XMLString::transcode("sizeZ");
  thetaString = xercesc::XMLString::transcode("theta");
  phiString = xercesc::XMLString::transcode("phi");
  alp1String = xercesc::XMLString::transcode("alp1");
  alp2String = xercesc::XMLString::transcode("alp2");
  xString = xercesc::XMLString::transcode("x");
  yString = xercesc::XMLString::transcode("y");
  zString = xercesc::XMLString::transcode("z");
  rString = xercesc::XMLString::transcode("r");
  axThetaString = xercesc::XMLString::transcode("axTheta");
  axPhiString = xercesc::XMLString::transcode("axPhi");
  angleString = xercesc::XMLString::transcode("angle");
  zplaneString = xercesc::XMLString::transcode("zplane");
  startThetaAngleString = xercesc::XMLString::transcode("startThetaAngle");
  deltaThetaAngleString = xercesc::XMLString::transcode("deltaThetaAngle");
  startPhiAngleString = xercesc::XMLString::transcode("startPhiAngle");
  deltaPhiAngleString = xercesc::XMLString::transcode("deltaPhiAngle");
  rotXString = xercesc::XMLString::transcode("rotX");
  rotYString = xercesc::XMLString::transcode("rotY");
  rotZString = xercesc::XMLString::transcode("rotZ");
  serialNumber = xercesc::XMLString::transcode("SerialNumber");
  m_numeral = "";
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlLVolumeCnv::~XmlLVolumeCnv () {
  xercesc::XMLString::release((XMLCh**)&unionString);
  xercesc::XMLString::release((XMLCh**)&subtractionString);
  xercesc::XMLString::release((XMLCh**)&intersectionString);
  xercesc::XMLString::release((XMLCh**)&boxString);
  xercesc::XMLString::release((XMLCh**)&trdString);
  xercesc::XMLString::release((XMLCh**)&trapString);
  xercesc::XMLString::release((XMLCh**)&consString);
  xercesc::XMLString::release((XMLCh**)&polyconeString);
  xercesc::XMLString::release((XMLCh**)&tubsString);
  xercesc::XMLString::release((XMLCh**)&sphereString);
  xercesc::XMLString::release((XMLCh**)&transformationString);
  xercesc::XMLString::release((XMLCh**)&posXYZString);
  xercesc::XMLString::release((XMLCh**)&posRPhiZString);
  xercesc::XMLString::release((XMLCh**)&posRThPhiString);
  xercesc::XMLString::release((XMLCh**)&rotXYZString);
  xercesc::XMLString::release((XMLCh**)&rotAxisString);
  xercesc::XMLString::release((XMLCh**)&paramphysvolString);
  xercesc::XMLString::release((XMLCh**)&paramphysvol2DString);
  xercesc::XMLString::release((XMLCh**)&paramphysvol3DString);
  xercesc::XMLString::release((XMLCh**)&materialString);
  xercesc::XMLString::release((XMLCh**)&magfieldString);
  xercesc::XMLString::release((XMLCh**)&sensdetString);
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&physvolString);
  xercesc::XMLString::release((XMLCh**)&surfString);
  xercesc::XMLString::release((XMLCh**)&logvolString);
  xercesc::XMLString::release((XMLCh**)&numberString);
  xercesc::XMLString::release((XMLCh**)&addressString);
  xercesc::XMLString::release((XMLCh**)&outerRadiusMZString);
  xercesc::XMLString::release((XMLCh**)&outerRadiusPZString);
  xercesc::XMLString::release((XMLCh**)&innerRadiusMZString);
  xercesc::XMLString::release((XMLCh**)&innerRadiusPZString);
  xercesc::XMLString::release((XMLCh**)&outerRadiusString);
  xercesc::XMLString::release((XMLCh**)&innerRadiusString);
  xercesc::XMLString::release((XMLCh**)&sizeXString);
  xercesc::XMLString::release((XMLCh**)&sizeX1String);
  xercesc::XMLString::release((XMLCh**)&sizeX2String);
  xercesc::XMLString::release((XMLCh**)&sizeX3String);
  xercesc::XMLString::release((XMLCh**)&sizeX4String);
  xercesc::XMLString::release((XMLCh**)&sizeYString);
  xercesc::XMLString::release((XMLCh**)&sizeY1String);
  xercesc::XMLString::release((XMLCh**)&sizeY2String);
  xercesc::XMLString::release((XMLCh**)&sizeZString);
  xercesc::XMLString::release((XMLCh**)&thetaString);
  xercesc::XMLString::release((XMLCh**)&phiString);
  xercesc::XMLString::release((XMLCh**)&alp1String);
  xercesc::XMLString::release((XMLCh**)&alp2String);
  xercesc::XMLString::release((XMLCh**)&xString);
  xercesc::XMLString::release((XMLCh**)&yString);
  xercesc::XMLString::release((XMLCh**)&zString);
  xercesc::XMLString::release((XMLCh**)&rString);
  xercesc::XMLString::release((XMLCh**)&axThetaString);
  xercesc::XMLString::release((XMLCh**)&axPhiString);
  xercesc::XMLString::release((XMLCh**)&angleString);
  xercesc::XMLString::release((XMLCh**)&zplaneString);
  xercesc::XMLString::release((XMLCh**)&startThetaAngleString);
  xercesc::XMLString::release((XMLCh**)&deltaThetaAngleString);
  xercesc::XMLString::release((XMLCh**)&startPhiAngleString);
  xercesc::XMLString::release((XMLCh**)&deltaPhiAngleString);
  xercesc::XMLString::release((XMLCh**)&rotXString);
  xercesc::XMLString::release((XMLCh**)&rotYString);
  xercesc::XMLString::release((XMLCh**)&rotZString);
  xercesc::XMLString::release((XMLCh**)&serialNumber);
}


// -----------------------------------------------------------------------
// Tests whether the tag denotes a boolean solid
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isBooleanSolid (const XMLCh* tag) {
  if (0 == xercesc::XMLString::compareString(unionString, tag) ||
      0 == xercesc::XMLString::compareString(subtractionString, tag) ||
      0 == xercesc::XMLString::compareString(intersectionString, tag)) {
    return true;
  }
  return false;
} // end isBooleanSolid


// -----------------------------------------------------------------------
// Tests whether the tag denotes a simple solid
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isSimpleSolid (const XMLCh* tag) {
  if (0 == xercesc::XMLString::compareString(boxString, tag) ||
      0 == xercesc::XMLString::compareString(trdString, tag) ||
      0 == xercesc::XMLString::compareString(trapString, tag) ||
      0 == xercesc::XMLString::compareString(consString, tag) ||
      0 == xercesc::XMLString::compareString(polyconeString, tag) ||
      0 == xercesc::XMLString::compareString(tubsString, tag) ||
      0 == xercesc::XMLString::compareString(sphereString, tag)) {
    return true;
  }
  return false;
} // end isSimpleSolid


// -----------------------------------------------------------------------
// Tests whether the tag denotes a solid
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isSolid (const XMLCh* tag) {
  if (isSimpleSolid(tag) ||
      isBooleanSolid(tag)) {
    return true;
  }
  return false;
} // end isSolid


// -----------------------------------------------------------------------
// Tests whether the tag denotes a transformation
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isTransformation (const XMLCh* tag) {
  if (0 == xercesc::XMLString::compareString(transformationString, tag) ||
      0 == xercesc::XMLString::compareString(posXYZString, tag) ||
      0 == xercesc::XMLString::compareString(posRPhiZString, tag) ||
      0 == xercesc::XMLString::compareString(posRThPhiString, tag) ||
      0 == xercesc::XMLString::compareString(rotXYZString, tag) ||
      0 == xercesc::XMLString::compareString(rotAxisString, tag)) {
    return true;
  }
  return false;
} // end isTransformation


// -----------------------------------------------------------------------
// Tests whether the tag denotes a parametrized physical volume
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isParamphysvol (const XMLCh* tag) {
  if (0 == xercesc::XMLString::compareString(paramphysvolString, tag) ||
      0 == xercesc::XMLString::compareString(paramphysvol2DString, tag) ||
      0 == xercesc::XMLString::compareString(paramphysvol3DString, tag)) {
    return true;
  }
  return false;
} // end isTransformation

      
// -----------------------------------------------------------------------
// returns a string locating the element
// -----------------------------------------------------------------------
std::string XmlLVolumeCnv::locateElement (xercesc::DOMElement* element) {
  // tries to find a parent element with a name
  xercesc::DOMNode* parentNode = element;
  xercesc::DOMElement* parentElement = 0;
  std::string parentName;
  bool hasName = false;
  while (!hasName) {
    parentNode = parentNode->getParentNode();
    if (0 == parentNode) {
      break;
    }
    if (parentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      parentElement = (xercesc::DOMElement*) parentNode;
      parentName = dom2Std (parentElement->getAttribute(nameString));
      if (parentName != "") {
        hasName = true;
      }
    }
  }
  // if one was found, builds the result from it
  std::string result("");
  if (hasName) {
    parentNode = element;
    xercesc::DOMNode* grandParent = element->getParentNode();
    while (parentNode != parentElement) {
      xercesc::DOMNodeList* list = grandParent->getChildNodes();
      unsigned int i = 0;
      for (i = 0; i < list->getLength(); i++) {
        if (parentNode == list->item(i)) {
          break;
        }
      }

      result = "child number ";
      result += std::to_string(i+1);
      result += " (";
      result += dom2Std (parentNode->getNodeName());
      result += ") of ";
      parentNode = grandParent;
      grandParent = parentNode->getParentNode();
    }
    result += dom2Std (parentElement->getNodeName());
    result += " ";
    result += parentName;
  } else {
    // else just give the name of the parent
    parentNode = element->getParentNode();
    if (parentNode != 0) {
      result += "tag ";
      result += dom2Std (parentNode->getNodeName());
    } else {
      result += "top node";
    }
  }
  return result;
} // end locateElement


// -----------------------------------------------------------------------
// Create the name of a physical volume
// -----------------------------------------------------------------------
std::string XmlLVolumeCnv::createPvName (PVolumeItem* pv) {
  if (0 == pv) {
    return "";
  }
  if (!pv->indexed) {
    return pv->physvolName;
  }
  // builds the actual name of the volume
  std::ostringstream ost;
  ost << pv->physvolName << ":" << pv->tag;
  return ost.str();
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::internalCreateObj (xercesc::DOMElement* element,
                                             DataObject*&       refpObject ,
                                             IOpaqueAddress* /* address */ ) 
{
  // gets the attributes
  std::string materialName = dom2Std (element->getAttribute (materialString));
  std::string magFieldName = dom2Std (element->getAttribute (magfieldString));
  std::string sensDetName = dom2Std (element->getAttribute (sensdetString));
  std::string volName = dom2Std (element->getAttribute (nameString));

  m_numeral = dom2Std (element->getAttribute ( serialNumber ) );
  //  if ( 0 != m_numeral.size() ) replaceTagInString( volName );
  if ( 0 != m_numeral.size() ) {
    replaceTagInString( volName );
    replaceTagInString( materialName);
    
  }
  
  // processes the children. The dtd says we should find
  // ((%solid;, %transformation;?)?, (physvol | paramphysvol | surf)*)
  xercesc::DOMNodeList* childNodes = element->getChildNodes();
  // get the first one, it should be a solid, a physical volume, a
  // parametrized physical volume, a surface or nothing
  unsigned int i = 0;
  while (i < childNodes->getLength() && 
         childNodes->item(i)->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) {
    i += 1;
  }

  // if no child, return
  if (i >= childNodes->getLength()) {
    return StatusCode::SUCCESS;
  }

  // if there is a child, gets it as an element and gets its name
  xercesc::DOMNode* childNode = childNodes->item(i);
  xercesc::DOMElement* childElement = (xercesc::DOMElement *) childNode;
  const XMLCh* tagName = childElement->getNodeName();

  
  // try to see if it is a solid and deal with it and a possible
  // transformation if yes
  ISolid* solid = 0;
  if (isSolid (tagName)) {
    // deal with the solid itself
    solid = dealWithSolid (childElement);
    // finds the next child
    i += 1;
    while (i < childNodes->getLength() && 
           childNodes->item(i)->getNodeType() !=
           xercesc::DOMNode::ELEMENT_NODE) {
      i += 1;
    }
    // if there is more children, try to see if it is a transformation
    if (i < childNodes->getLength()) {
      childNode = childNodes->item(i);
      childElement = (xercesc::DOMElement*) childNode;
      const XMLCh* childTagName = childElement->getNodeName();
      if (isTransformation(childTagName)) {
        // deal with the transformation itself
        Gaudi::Transform3D* transformation = dealWithTransformation (element, &i);
        // modifies the solid in consequence
        // TO BE IMPLEMENTED --- TODO
        MsgStream log(msgSvc(), "XmlLVolumeCnv" );
        char* tagNameString = xercesc::XMLString::transcode(tagName);
        log << MSG::WARNING
            << "In logvol " << volName << ", a transformation ("
            << childTagName << ") is applied to the first solid ("
            << tagNameString
            << "). This functionnality is not implemented yet. The "
            << " transformation will be ignored." << endmsg;
        xercesc::XMLString::release(&tagNameString);
        // frees the memory
        delete (transformation);
        transformation = 0;
        // finds the next child
        while (i < childNodes->getLength() && 
               childNodes->item(i)->getNodeType() !=
               xercesc::DOMNode::ELEMENT_NODE) {
          i += 1;
        }
      }
    }
  }
  LogVolBase* dataObj;
  if (0 != solid) {
    // computes the actual material name
    if (materialName.empty() || materialName[0] != '/') {
      materialName.insert(0,"/dd/Materials/");
    }
    // if there is a solid, creates a logical volume and stores the solid inside
    dataObj = new LVolume(volName, 
                          solid, 
                          materialName,
                          sensDetName,
                          magFieldName);
  } else {
    // else create an Assembly volume
    dataObj = new LAssembly(volName, 
                            sensDetName,
                            magFieldName);
    // if materialName was not null, display a warning that it will be ignored
    if (!materialName.empty()) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING << "The logical volume " << volName
          << " has no associated solid. Thus, the material that you "
          << "defined for it (" << materialName << ") will be ignored."
          << endmsg;
    }
  }
  refpObject = dataObj;
    
  // Whatever we found up to now, the next child to process is at index i.
  // The dtd says that we will find (physvol | paramphysvol | surf)* now
  // So let's deal with it.
  while (i < childNodes->getLength()) {
    childNode = childNodes->item(i);
    childElement = (xercesc::DOMElement*) childNode;
    tagName = childElement->getNodeName();
    // dispatches according to the tagname
    if (0 == xercesc::XMLString::compareString(physvolString, tagName)) {
      // deals with a physical volume, adds it to the logical volume
      // and frees the memory
      PVolumeItem* volume = dealWithPhysvol(childElement);
      if (0 == volume->transformation) {
        dataObj->createPVolume (createPvName (volume),
                                volume->logvolName,
                                Gaudi::Transform3D() );
      } else {
        dataObj->createPVolume (createPvName (volume),
                                volume->logvolName,
                                volume->transformation->Inverse());
      }
      if (volume->transformation != 0) {
        delete (volume->transformation);
        volume->transformation = 0;
      }
      
      delete (volume);
      volume = 0;
    } else if (isParamphysvol (tagName)) {
      // deals with a parametrized physical volume, adds all physical
      // volumes created to the logical volume and frees the memory      
      PVolumes* volumes = dealWithParamphysvol(childElement);
      for (PVolumes::iterator it = volumes->begin();
           volumes->end() != it;
           ++it) {
        if (0 == (*it)->transformation) {
          dataObj->createPVolume (createPvName (*it),
                                  (*it)->logvolName,
                                  Gaudi::Transform3D() );
        } else {
          dataObj->createPVolume (createPvName (*it),
                                  (*it)->logvolName,
                                  (*it)->transformation->Inverse());
        }
      }
      while (volumes->size() > 0) {
        PVolumeItem* item = volumes->back();
        volumes->pop_back();
        if (item->transformation != 0) {
          delete (item->transformation);
          item->transformation = 0;
        }
        delete (item);
        item = 0;
      }
      delete (volumes);
      volumes = 0;
    } else if (0 == xercesc::XMLString::compareString(surfString, tagName)) {
      // deals with a surface and adds it to the logical volume
      std::string address = dealWithSurf(childElement);
      long linkID = dataObj->linkMgr()->addLink(address, nullptr);
      SmartRef<Surface> reference (dataObj, linkID);
      dataObj->surfaces().push_back(reference); 
    } else {
      // Something goes wrong, does it?
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      char* tagNameString = xercesc::XMLString::transcode(tagName);
      log << MSG::WARNING
          << "This tag makes no sense to LVolume " << volName
          << " : " << tagNameString
          << ". It will be ignored" << endmsg;
      xercesc::XMLString::release(&tagNameString);
      return StatusCode::FAILURE;
    }

    //XmlLVolumeCnv:: gets the next child
    i += 1;
    while (i < childNodes->getLength() && 
           childNodes->item(i)->getNodeType() !=
           xercesc::DOMNode::ELEMENT_NODE) {
      i += 1;
    }
  }
  return StatusCode::SUCCESS;
} // end internalCreateObj


// -----------------------------------------------------------------------
// Deal with Physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumeItem*
XmlLVolumeCnv::dealWithPhysvol (xercesc::DOMElement* element) {
  // gets attributes
  std::string nameAttribute = dom2Std (element->getAttribute (nameString));
  std::string logvolAttribute = dom2Std (element->getAttribute (logvolString));

  // gets the children
  xercesc::DOMNodeList* childNodes = element->getChildNodes();

  // deal with a possible transformation inside element
  Gaudi::Transform3D* transformation = 0;
  // while there are children, and no transformation found, try
  // to find one
  unsigned int i = 0;
  while ((transformation == 0 ||
          *transformation == Gaudi::Transform3D())
         && i < childNodes->getLength()) {    
    while (i < childNodes->getLength() && 
           childNodes->item(i)->getNodeType() !=
           xercesc::DOMNode::ELEMENT_NODE) {
      i += 1;
    }
    // is there is one, deal with the transformation
    if (i < childNodes->getLength()) {
      transformation = dealWithTransformation (element, &i);
    }
  }
  // if there are still children after the transformation,
  // raise an error message
  while (i < childNodes->getLength() && 
         childNodes->item(i)->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) {
    i += 1;
  }
  if (i < childNodes->getLength()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING
        << "There are too many children in physical volume "
        << nameAttribute << ". The exceeding ones will be ignored."
        << endmsg;
  }

  // deals with the name of the physical volume
  // it could end with ':' and digits. In this case, the name itself is the
  // part before ':' and the digits give an index for this volume
  std::string::size_type columnPos = nameAttribute.find_last_of (':');
  int index = 0;
  bool indexed = false;
  if (columnPos != std::string::npos) {
    std::string digits = nameAttribute.substr (columnPos + 1);
    nameAttribute = nameAttribute.substr (0, columnPos);
    indexed = true;
    if (!digits.empty()) {
      if ( 0 != m_numeral.size() ) {
        replaceTagInString(digits);
      }      
      index = (int) xmlSvc()->eval(digits, false);
    }
  }

  // builds physvol and returns
  PVolumeItem* result = new PVolumeItem;
  result->physvolName = nameAttribute;
  result->tag = index;
  result->indexed = indexed;
  result->logvolName = logvolAttribute;
  result->transformation = transformation;
  if ( 0 != m_numeral.size() ) {
    replaceTagInString( result->logvolName );
    replaceTagInString( result->physvolName );
  }
  return result;
} // end dealWithPhysVol


// -----------------------------------------------------------------------
// Deal with parametrized physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumes*
XmlLVolumeCnv::dealWithParamphysvol (xercesc::DOMElement* element) {
  // gets the element's name
  const XMLCh* tagName = element->getNodeName();
  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(paramphysvolString, tagName)) {
    return dealWithParamphysvol (element, 1);
  } else if (0 == xercesc::XMLString::compareString
             (paramphysvol2DString, tagName)) {
    return dealWithParamphysvol (element, 2);
  } else if (0 == xercesc::XMLString::compareString
             (paramphysvol3DString, tagName)) {
    return dealWithParamphysvol (element, 3);
  } else {
    // unknown tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "In " << locateElement (element)
        << " : the tag " << tagNameString
        << " does no denote a parametrized physical volume. "
        << "It will be ignored" << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }
  return 0;
} // end dealWithParamphysvol


// -----------------------------------------------------------------------
// Deal with parametrized physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumes*
XmlLVolumeCnv::dealWithParamphysvol (xercesc::DOMElement* element,
                                     unsigned int nD) {
  // nD should be positive
  if (0 == nD) {
    return 0;
  }
  
  // gets attributes
  std::vector<std::string> numberAttributes(nD);
  if (1 == nD) {
    numberAttributes[0] = dom2Std (element->getAttribute (numberString));
  } else {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      // builds the actual name of the attribute
      std::ostringstream ost;
      ost << "number" << i + 1;
      const unsigned int len = ost.str().size();
      char *resstr = new char[len+1];
      strncpy(resstr,ost.str().c_str(),len);
      resstr[len] = 0;
      const XMLCh* attrName = xercesc::XMLString::transcode (resstr); 
      delete [] resstr;
      numberAttributes[i] = dom2Std (element->getAttribute (attrName));
      xercesc::XMLString::release((XMLCh**)&attrName);
    }
  }

  // computes the values
  std::vector<int> numbers(nD);
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      numbers[i] = (int) xmlSvc()->eval(numberAttributes[i], false);
    }
  }

  // some checks : numbers should be positive
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      if (numbers[i] < 0) {
        throw XmlCnvException
          (" paramphysvolume : number of copies must be positive ! ",
           CORRUPTED_DATA);
        return 0;
      }
    }
  }
  
  // builds the result
  PVolumes* result = 0;

  // deals with the children of the tag. The dtd says it should be
  // (physvol | paramphysvol), %transformation;*nD
  xercesc::DOMNodeList* childNodes = element->getChildNodes();
  // finds the first child that is an element
  unsigned int i = 0;
  while (i < childNodes->getLength() && 
         childNodes->item(i)->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) {
    i += 1;
  }

  // is there is one, it should be a physvol or a paramphysvol
  if (i < childNodes->getLength()) {
    xercesc::DOMNode* firstChildNode = childNodes->item(i);
    xercesc::DOMElement* firstChildElement =
      (xercesc::DOMElement*) firstChildNode;
    // get the name of the first child
    const XMLCh* tagName = firstChildElement->getNodeName();
    if (0 == xercesc::XMLString::compareString(physvolString, tagName)) {
      // gets the physical volume and add it to the result as the first
      // volume of the vector
      PVolumeItem* volume = dealWithPhysvol (firstChildElement);
      result = new PVolumes (1, volume);
    } else if (isParamphysvol (tagName)) {
      // gets the vector of physical volumes and use it as first result
      // that will be upgraded later for the current parametrisation
      result = dealWithParamphysvol (firstChildElement);
    } else {
      // unknown tag -> display an error, return 0
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      char* tagNameString = xercesc::XMLString::transcode(tagName);
      log << MSG::WARNING << "In " << locateElement (element)
          << " : the tag " << tagNameString
          << " is not a valid tag as first child of <paramphysvol";
      xercesc::XMLString::release(&tagNameString);
      if (0 != nD) {
        log << nD << "D";
      }
      log << ">. It should be either physvol or paramphysvol(nD). "
          << "As a consequence, "
          << "this parametrised physical volume will be ignored." << endmsg;    
      return 0;
    }

    // now looks subsequent child, they will define the transformations
    std::vector<Gaudi::Transform3D*> transformations(nD);

    // while there are still children, and not enough transformations found, try
    // to find more transformations
    unsigned int j = i + 1;
    unsigned int nbOfTransfoFound = 0;
    while (nbOfTransfoFound < nD
           && j < childNodes->getLength()) {
      while (j < childNodes->getLength() && 
             childNodes->item(j)->getNodeType() !=
             xercesc::DOMNode::ELEMENT_NODE) {
        j += 1;
      }
      // is there is one child, deal with the transformation
      if (j < childNodes->getLength()) {
        transformations[nbOfTransfoFound] =
          dealWithTransformation (element, &j);
        nbOfTransfoFound++;
      }
    }
  
    // if there are still children after the transformation,
    // raise an error message
    while (j < childNodes->getLength() && 
           childNodes->item(j)->getNodeType()
           != xercesc::DOMNode::ELEMENT_NODE) {
      j += 1;
    }
    if (j < childNodes->getLength()) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING << "In " << locateElement (element)
          << " : there are too many children in this parametrized physical "
          << "volume. The exceeding ones will be ignored."
          << endmsg;
    }
    
    // last but not least build the final result from the current one
    // by expansion of the vector using transformation and number
    result = expandParamPhysVol (result, numbers, transformations);

    // frees the memory allocated for the transformations
    {
      unsigned int ii = 0;
      for (ii = 0; ii < nD; ii++) {
        if (transformations[ii] != 0) {
          delete (transformations[ii]);
          transformations[ii] = 0;
        }
      }
    }

  } else {
    // no element child -> display an error, return 0
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this <paramphysvol> cannot be empty, it should contain "
        << "either physvol or paramphysvol(nD). As a consequence, this "
        << "parametrised physical volume will be ignored." << endmsg;    
    return 0;
  }

  // returns
  return result;
} // end dealWithParamphysvol


// -----------------------------------------------------------------------
// Expand parametrised physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumes*
XmlLVolumeCnv::expandParamPhysVol
(PVolumes* volumes,
 std::vector<int> numbers,
 std::vector<Gaudi::Transform3D*> transformations) {
  // gets the number of dimensions, check the arguments are ok
  unsigned int nD = numbers.size();
  if (transformations.size() != nD) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::FATAL
        << "In expandParamPhysVol, the dimensions of the arguments are "
        << "not compatible : I got " << nD << " numbers and "
        << transformations.size() << " transformations. "
        << "The parametrized physical volume will be ignored." << endmsg;
    return 0;
  }

  // get the number of volumes in volumes
  unsigned int volNumber = volumes->size();
  // computes the number of replication of a single volume
  unsigned int nV = 1;
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      nV *= numbers[i];
    }
  }
    
  // get the translations and rotations to apply
  std::vector<Gaudi::XYZVector> stepTranslations(nD);
  std::vector<Gaudi::Rotation3D> stepRotations(nD);
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      transformations[i]->GetDecomposition(stepRotations[i],
                                           stepTranslations[i]);
    }
  }
  
  // creates the result
  PVolumes* result = new PVolumes();
  // creates the set of translations and rotations that the volumes will use
  std::vector<Gaudi::XYZVector> translations(nV);
  std::vector<Gaudi::Rotation3D> rotations(nV);
  {
    unsigned int numberOfItems = 1;
    unsigned int dimension, item;
    int i;
    for (dimension = 0; dimension < nD; dimension++) {
      for (item = 0; item < numberOfItems; item++) {
        for (i = 1; i < numbers[dimension]; i++) {
          translations[numberOfItems*i+item] =
            stepTranslations[dimension] +
            translations[numberOfItems*(i-1)+item];
          rotations[numberOfItems*i+item] =
            stepRotations[dimension] *
            rotations[numberOfItems*(i-1)+item];
        }
      }
      numberOfItems *= numbers[dimension];
    }
  }
  
  Gaudi::XYZVector tmpTrans;
  Gaudi::Rotation3D tmpRot;

  // for each volume
  for (PVolumes::iterator it = volumes->begin();
       volumes->end() != it;
       ++it) {
    // add the volume to the result
    result->push_back (*it);
    // replicates the volume
    unsigned int step;
    for (step = 1; step < nV; ++step) {
      // first give it a tag
      unsigned int tag = volNumber * step + (*it)->tag;
      // add a new physical volume to the result
      PVolumeItem* newPvi = new PVolumeItem;
      newPvi->physvolName = (*it)->physvolName;
      newPvi->tag = tag;
      newPvi->indexed = true;
      newPvi->logvolName = (*it)->logvolName;
      if (0 == (*it)->transformation) {
        Gaudi::Transform3D* transformation =
          new Gaudi::Transform3D(translations[step]);
        *transformation = *transformation * 
          Gaudi::Transform3D(rotations[step] );
        newPvi->transformation = transformation;
      } else {
        (*it)->transformation->GetDecomposition(tmpRot, tmpTrans);
        Gaudi::Transform3D *transformation =
          new Gaudi::Transform3D (translations[step] + tmpTrans );
        *transformation = *transformation * 
          Gaudi::Transform3D( rotations[step]*tmpRot );
        newPvi->transformation = transformation;
      }
      result->push_back (newPvi);
    }
  }
  delete (volumes);
  volumes = 0;
  return result;
} // end expandParamphysvol


// -----------------------------------------------------------------------
// Deal with Surface
// -----------------------------------------------------------------------
std::string XmlLVolumeCnv::dealWithSurf (xercesc::DOMElement* element) {
  return dom2Std (element->getAttribute (addressString));
} // end dealWithSurf


// -----------------------------------------------------------------------
// Deal with solid
// -----------------------------------------------------------------------
ISolid* XmlLVolumeCnv::dealWithSolid (xercesc::DOMElement* element) {
  // gets the element's name
  const XMLCh* tagName = element->getNodeName();
  // dispatches, based on the name
  if (isSimpleSolid (tagName)) {
    return dealWithSimpleSolid (element);
  } else if (isBooleanSolid(tagName)) {
    return dealWithBoolean (element);
  } else {
    // unknown tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this should be a Solid but is a : "
        << tagName << ". It will be ignored" << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }
  return 0;
} // end dealWithSolid


// -----------------------------------------------------------------------
// Deal with boolean
// -----------------------------------------------------------------------
SolidBoolean* XmlLVolumeCnv::dealWithBoolean (xercesc::DOMElement* element) {
  // gets the element's name
  std::string nameAttribute = dom2Std (element->getAttribute (nameString));
  const XMLCh* tagName = element->getNodeName();

  // Checks it's a boolean solid
  if (!isBooleanSolid (tagName)) {
    // unknow tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this sould be a boolean solid but is a : "
        << tagNameString << ". It will be ignored" << endmsg;
    xercesc::XMLString::release(&tagNameString);
    return 0;
  }
  
  // builds the list of the children
  PlacedSolidList* solids = dealWithBooleanChildren (element);

  // checks that there are at least two solids
  if (solids->size() < 2) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this boolean solid contains less than 2 solids. "
        << "It will be ignored" << endmsg;
    if (solids->size() > 0) {
      PlacedSolid solid = solids->back();
      solids->pop_back();
      delete (solid.solid);
      solid.solid = 0;
      if (solid.transformation != 0) {
        delete (solid.transformation);
        solid.transformation = 0;
      }
    }
    delete (solids);
    solids = 0;
    return 0;
  }

  // creates the object
  SolidBoolean* result = 0;
  PlacedSolid placedSolid = solids->front();
  solids->pop_front();
  if (0 == xercesc::XMLString::compareString(unionString, tagName)) {
    SolidUnion* unionResult = new SolidUnion (nameAttribute, placedSolid.solid);
    result = unionResult;
    // TO BE IMPLEMENTED -- TODO
    if (0 != placedSolid.transformation) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING
          << "In union " << nameAttribute << ", a transformation"
          << " is applied to the first solid."
          << " This functionnality is not implemented yet. The "
          << " transformation will be ignored." << endmsg;
      // what about placedSolid.transformation ?
      delete (placedSolid.transformation);
      placedSolid.transformation = 0;
    }
    // add every child to the boolean solid
    while (!solids->empty()) {
      placedSolid = solids->front();
      solids->pop_front();
      StatusCode sc = unionResult->unite (placedSolid.solid,
                                          placedSolid.transformation);
      if( !sc.isSuccess() ) {
        MsgStream log(msgSvc(), "XmlLVolumeCnv" );
        log << MSG::WARNING
            << "unionResult->unite failed. Don't know what to do..." << endmsg;
      }
      if (placedSolid.transformation != 0) {
        delete (placedSolid.transformation);
        placedSolid.transformation = 0;
      }
    }
  } else if (0 == xercesc::XMLString::compareString
             (subtractionString, tagName)) {
    SolidSubtraction* subtractionResult =
      new SolidSubtraction (nameAttribute, placedSolid.solid);
    result = subtractionResult;
    // TO BE IMPLEMENTED -- TODO
    if (0 != placedSolid.transformation) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING
          << "In subtraction " << nameAttribute << ", a transformation"
          << " is applied to the first solid."
          << " This functionnality is not implemented yet. The "
          << " transformation will be ignored." << endmsg;
      // what about placedSolid.transformation ?
      delete (placedSolid.transformation);
      placedSolid.transformation = 0;
    }
    // add every child to the boolean solid
    while (!solids->empty()) {
      placedSolid = solids->front();
      solids->pop_front();
      StatusCode sc = subtractionResult->subtract (placedSolid.solid,
                                                   placedSolid.transformation);
      if( !sc.isSuccess() ) {
        MsgStream log(msgSvc(), "XmlLVolumeCnv" );
        log << MSG::WARNING
            << "unionResult->subtract failed. Don't know what to do.." << endmsg;
      }
      if (placedSolid.transformation != 0) {
        delete (placedSolid.transformation);
        placedSolid.transformation = 0;
      }
    }
  } else if (0 == xercesc::XMLString::compareString
             (intersectionString, tagName)) {
    SolidIntersection* intersectionResult =
      new SolidIntersection (nameAttribute, placedSolid.solid);
    result = intersectionResult;
    // TO BE IMPLEMENTED -- TODO
    if (0 != placedSolid.transformation) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING
          << "In intersection " << nameAttribute << ", a transformation"
          << " is applied to the first solid."
          << " This functionnality is not implemented yet. The "
          << " transformation will be ignored." << endmsg;
      // what about placedSolid.transformation ?
      delete (placedSolid.transformation);
      placedSolid.transformation = 0;
    }
    // add every child to the boolean solid
    while (!solids->empty()) {
      placedSolid = solids->front();
      solids->pop_front();
      StatusCode sc = intersectionResult->intersect (placedSolid.solid,
                                                     placedSolid.transformation);
      if( !sc.isSuccess() ) {
        MsgStream log(msgSvc(), "XmlLVolumeCnv" );
        log << MSG::WARNING
            << "unionResult->intersect failed. Don't know what to do!" << endmsg;
      }
      if (placedSolid.transformation != 0) {
        delete (placedSolid.transformation);
        placedSolid.transformation = 0;
      }
    }
  }

  // frees some memory
  delete solids;
  solids = 0;
  
  // returns
  return result;
} // end dealWithBoolean


// -----------------------------------------------------------------------
// Deal with boolean children
// -----------------------------------------------------------------------
XmlLVolumeCnv::PlacedSolidList*
XmlLVolumeCnv::dealWithBooleanChildren (xercesc::DOMElement* element) {
  // builds an empty list
  PlacedSolidList* result = new PlacedSolidList();
  
  // the dtd says ((%solid; %transformation;?), (%solid; %transformation;?)+)
  // but we do here (%solid; %transformation;?)*
  xercesc::DOMNodeList* childNodes = element->getChildNodes();
  unsigned int i = 0;
  while (i < childNodes->getLength()) {
    if (childNodes->item(i)->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      // gets the current child
      xercesc::DOMNode* childNode = childNodes->item(i);
      xercesc::DOMElement* childElement = (xercesc::DOMElement*) childNode;
      // gets its name
      const XMLCh* tagName = childElement->getNodeName();
      // checks it's a solid
      if (isSolid (tagName)) {
        // get the C++ object from it
        ISolid* solid = dealWithSolid(childElement);
        // see if there is a transformation afterwards
        Gaudi::Transform3D* transformation = 0;
        i += 1;
        while (i < childNodes->getLength() && 
               childNodes->item(i)->getNodeType() !=
               xercesc::DOMNode::ELEMENT_NODE) {
          i += 1;
        }
        if (i < childNodes->getLength()) {
          childNode = childNodes->item(i);
          childElement = (xercesc::DOMElement*) childNode;
          tagName = childElement->getNodeName();
          if (isTransformation (tagName)) {
            transformation = dealWithTransformation (element, &i);
            *transformation = transformation->Inverse();
          }
        }
        PlacedSolid newPs = { solid, transformation };
        result->push_back (newPs);
      } else {
        // we should have a solid here
        MsgStream log(msgSvc(), "XmlLVolumeCnv" );
        char* tagNameString = xercesc::XMLString::transcode(tagName);
        log << MSG::WARNING << "In " << locateElement (childElement)
            << " : this " << tagName << " makes no sense to a boolean solid. "
            << "A solid is needed here. It will be ignored" << endmsg;
        xercesc::XMLString::release(&tagNameString);
        i += 1;
      }
    } else {
      i += 1;
    }
  }
  // returns
  return result;
} // end dealWithBooleanChildren


// -----------------------------------------------------------------------
// Deal with simple solid
// -----------------------------------------------------------------------
ISolid* XmlLVolumeCnv::dealWithSimpleSolid (xercesc::DOMElement* element) {
  // gets the element's name
  const XMLCh* tagName = element->getNodeName();

  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(boxString, tagName)) {
    return dealWithBox (element);
  } else if (0 == xercesc::XMLString::compareString(trdString, tagName)) {
    return dealWithTrd (element);
  } else if (0 == xercesc::XMLString::compareString(trapString, tagName)) {
    return dealWithTrap (element);
  } else if (0 == xercesc::XMLString::compareString(consString, tagName)) {
    return dealWithCons (element);
  } else if (0 == xercesc::XMLString::compareString(polyconeString, tagName)) {
    return dealWithPolycone (element);
  } else if (0 == xercesc::XMLString::compareString(tubsString, tagName)) {
    return dealWithTubs (element);
  } else if (0 == xercesc::XMLString::compareString(sphereString, tagName)) {
    return dealWithSphere (element);
  } else {
    // unknow tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this should be a SimpleSolid but is a : "
        << tagNameString << ". It will be ignored" << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }
  return 0;
} // end dealWithSimpleSolid
 

// -----------------------------------------------------------------------
// Deal with box
// -----------------------------------------------------------------------
SolidBox* XmlLVolumeCnv::dealWithBox (xercesc::DOMElement* element) {
  // gets attributes
  std::string sizeXAttribute = dom2Std (element->getAttribute (sizeXString));
  std::string sizeYAttribute = dom2Std (element->getAttribute (sizeYString));
  std::string sizeZAttribute = dom2Std (element->getAttribute (sizeZString));
  std::string solidName = dom2Std (element->getAttribute (nameString));
  if ( 0 != m_numeral.size() ) {
    replaceTagInString( solidName );
  }
  
  // computes the values
  double sizeX = 0.0;
  double sizeY = 0.0;
  double sizeZ = 0.0;
  if (!sizeXAttribute.empty()) {
    sizeX = xmlSvc()->eval(sizeXAttribute);
  }
  if (!sizeYAttribute.empty()) {
    sizeY = xmlSvc()->eval(sizeYAttribute);
  }
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }

  // builds solid
  SolidBox* result = new SolidBox
    (solidName, sizeX / 2.0, sizeY / 2.0, sizeZ / 2.0);

  // checks there are no children
  if (element->hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : a box should not have any child. They will be ignored" << endmsg;
  }

  // returns
  return result;
} // end dealWithBox


// -----------------------------------------------------------------------
// Deal with trd
// -----------------------------------------------------------------------
SolidTrd* XmlLVolumeCnv::dealWithTrd (xercesc::DOMElement* element) {
  // gets attributes
  std::string sizeX1Attribute = 
    dom2Std (element->getAttribute (sizeX1String));
  std::string sizeX2Attribute =
    dom2Std (element->getAttribute (sizeX2String));
  std::string sizeY1Attribute =
    dom2Std (element->getAttribute (sizeY1String));
  std::string sizeY2Attribute =
    dom2Std (element->getAttribute (sizeY2String));
  std::string sizeZAttribute =
    dom2Std (element->getAttribute (sizeZString));
  std::string trdName = dom2Std (element->getAttribute (nameString));

  // computes the values
  double sizeX1 = 0.0;
  double sizeX2 = 0.0;
  double sizeY1 = 0.0;
  double sizeY2 = 0.0;
  double sizeZ = 0.0;
  if (!sizeX1Attribute.empty()) {
    sizeX1 = xmlSvc()->eval(sizeX1Attribute);
  }
  if (!sizeX2Attribute.empty()) {
    sizeX2 = xmlSvc()->eval(sizeX2Attribute);
  }
  if (!sizeY1Attribute.empty()) {
    sizeY1 = xmlSvc()->eval(sizeY1Attribute);
  }
  if (!sizeY2Attribute.empty()) {
    sizeY2 = xmlSvc()->eval(sizeY2Attribute);
  }
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }

  // builds solid
  SolidTrd* result = new SolidTrd
    (trdName, sizeZ / 2.0, sizeX1 / 2.0, sizeY1 / 2.0, sizeX2 / 2.0,
     sizeY2 / 2.0);

  // checks there are no children
  if (element->hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : trd should not have any child. They will be ignored" << endmsg;
  }

  // returns
  return result;
} // end dealWithTrd


// -----------------------------------------------------------------------
// Deal with trap
// -----------------------------------------------------------------------
SolidTrap* XmlLVolumeCnv::dealWithTrap (xercesc::DOMElement* element) {
  // gets attributes
  std::string sizeZAttribute =
    dom2Std (element->getAttribute (sizeZString));
  std::string thetaAttribute =
    dom2Std (element->getAttribute (thetaString));
  std::string phiAttribute =
    dom2Std (element->getAttribute (phiString));
  std::string sizeY1Attribute =
    dom2Std (element->getAttribute (sizeY1String));
  std::string sizeX1Attribute = 
    dom2Std (element->getAttribute (sizeX1String));
  std::string sizeX2Attribute =
    dom2Std (element->getAttribute (sizeX2String));
  std::string alp1Attribute =
    dom2Std (element->getAttribute (alp1String));
  std::string sizeY2Attribute =
    dom2Std (element->getAttribute (sizeY2String));
  std::string sizeX3Attribute = 
    dom2Std (element->getAttribute (sizeX3String));
  std::string sizeX4Attribute =
    dom2Std (element->getAttribute (sizeX4String));
  std::string alp2Attribute =
    dom2Std (element->getAttribute (alp2String));
  std::string trapName = dom2Std (element->getAttribute (nameString));

  // computes the values
  double sizeZ = 0.0;
  double theta = 0.0;
  double phi = 0.0;
  double sizeY1 = 0.0;
  double sizeX1 = 0.0;
  double sizeX2 = 0.0;
  double alp1 = 0.0;
  double sizeY2 = 0.0;
  double sizeX3 = 0.0;
  double sizeX4 = 0.0;
  double alp2 = 0.0;
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }
  if (!thetaAttribute.empty()) {
    theta = xmlSvc()->eval(thetaAttribute, false);
  }
  if (!phiAttribute.empty()) {
    phi = xmlSvc()->eval(phiAttribute, false);
  }
  if (!sizeY1Attribute.empty()) {
    sizeY1 = xmlSvc()->eval(sizeY1Attribute);
  }
  if (!sizeX1Attribute.empty()) {
    sizeX1 = xmlSvc()->eval(sizeX1Attribute);
  }
  if (!sizeX2Attribute.empty()) {
    sizeX2 = xmlSvc()->eval(sizeX2Attribute);
  }
  if (!alp1Attribute.empty()) {
    alp1 = xmlSvc()->eval(alp1Attribute, false);
  }
  if (!sizeY2Attribute.empty()) {
    sizeY2 = xmlSvc()->eval(sizeY2Attribute);
  }
  if (!sizeX3Attribute.empty()) {
    sizeX3 = xmlSvc()->eval(sizeX3Attribute);
  }
  if (!sizeX4Attribute.empty()) {
    sizeX4 = xmlSvc()->eval(sizeX4Attribute);
  }
  if (!alp2Attribute.empty()) {
    alp2 = xmlSvc()->eval(alp2Attribute, false);
  }

  // builds solid
  SolidTrap* result = new SolidTrap
    (trapName, sizeZ / 2.0, theta, phi, sizeY1 / 2.0, sizeX1 / 2.0,
     sizeX2 / 2.0, alp1, sizeY2 / 2.0, sizeX3 / 2.0, sizeX4 / 2.0,
     alp2);

  // checks there are no children
  if (element->hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A trap should not have any child. They will be ignored" << endmsg;
  }

  // returns
  return result;
} // end dealWithTrap


// -----------------------------------------------------------------------
// Deal with cons
// -----------------------------------------------------------------------
SolidCons* XmlLVolumeCnv::dealWithCons (xercesc::DOMElement* element) {
  // gets attributes
  std::string sizeZAttribute =
    dom2Std (element->getAttribute (sizeZString));
  std::string outerRadiusPZAttribute =
    dom2Std (element->getAttribute (outerRadiusPZString));
  std::string outerRadiusMZAttribute =
    dom2Std (element->getAttribute (outerRadiusMZString));
  std::string innerRadiusPZAttribute =
    dom2Std (element->getAttribute (innerRadiusPZString));
  std::string innerRadiusMZAttribute =
    dom2Std (element->getAttribute (innerRadiusMZString));
  std::string startPhiAngleAttribute =
    dom2Std (element->getAttribute (startPhiAngleString));
  std::string deltaPhiAngleAttribute =
    dom2Std (element->getAttribute (deltaPhiAngleString));
  std::string consName = dom2Std (element->getAttribute (nameString));

  // computes the values
  double sizeZ = 0.0;
  double outerRadiusPZ = 0.0;
  double outerRadiusMZ = 0.0;
  double innerRadiusPZ = 0.0;
  double innerRadiusMZ = 0.0;
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * Gaudi::Units::degree;
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }
  if (!outerRadiusPZAttribute.empty()) {
    outerRadiusPZ = xmlSvc()->eval(outerRadiusPZAttribute);
  }
  if (!outerRadiusMZAttribute.empty()) {
    outerRadiusMZ = xmlSvc()->eval(outerRadiusMZAttribute);
  }
  if (!innerRadiusPZAttribute.empty()) {
    innerRadiusPZ = xmlSvc()->eval(innerRadiusPZAttribute);
  }
  if (!innerRadiusMZAttribute.empty()) {
    innerRadiusMZ = xmlSvc()->eval(innerRadiusMZAttribute);
  }
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute, false);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute, false);
  }

  // builds solid
  SolidCons* result = new SolidCons
    (consName, sizeZ / 2.0, outerRadiusMZ, outerRadiusPZ, innerRadiusMZ,
     innerRadiusPZ, startPhiAngle, deltaPhiAngle);

  // checks there are no children
  if (element->hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A cons should not have any child. They will be ignored" << endmsg;
  }

  // returns
  return result;
} // end dealWithCons


// -----------------------------------------------------------------------
// Deal with polycone
// -----------------------------------------------------------------------
SolidPolycone* XmlLVolumeCnv::dealWithPolycone (xercesc::DOMElement* element) {
  // gets attributes
  std::string startPhiAngleAttribute =
    dom2Std (element->getAttribute (startPhiAngleString));
  std::string deltaPhiAngleAttribute =
    dom2Std (element->getAttribute (deltaPhiAngleString));
  std::string polyconeName = dom2Std (element->getAttribute (nameString));

  // computes the values
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * Gaudi::Units::degree;
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute, false);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute, false);
  }

  // this are the zplanes contained by this node
  SolidPolycone::Triplets zplanes;

  // deals with the children
  xercesc::DOMNodeList* children = element->getElementsByTagName (zplaneString);
  for (unsigned int i = 0; i < children->getLength(); i++) {
    xercesc::DOMNode* childNode = children->item(i);
    xercesc::DOMElement* child = (xercesc::DOMElement*) childNode;

    // gets attributes
    std::string zAttribute =
      dom2Std (child->getAttribute (zString));
    std::string outerRadiusAttribute =
      dom2Std (child->getAttribute (outerRadiusString));
    std::string innerRadiusAttribute =
      dom2Std (child->getAttribute (innerRadiusString));
    
    // computes the values
    double z = 0.0;
    double outerRadius = 0.0;
    double innerRadius = 0.0;
    if (!zAttribute.empty()) {
      z = xmlSvc()->eval(zAttribute);
    }
    if (!outerRadiusAttribute.empty()) {
      outerRadius = xmlSvc()->eval(outerRadiusAttribute);
    }
    if (!innerRadiusAttribute.empty()) {
      innerRadius = xmlSvc()->eval(innerRadiusAttribute);
    }
    
    // builds zplane and adds it to the list
    zplanes.push_back (SolidPolycone::Triplet
                       (z, SolidPolycone::Pair (outerRadius, innerRadius)));
    
    // checks there are no children
    if (child->hasChildNodes()) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING << "In " << locateElement (child)
          << "A zplane should not have any child. They will be ignored"
          << endmsg;
    }
  }
    
  // builds solid
  SolidPolycone* result;
  try {
    result = new SolidPolycone
      (polyconeName, zplanes, startPhiAngle, deltaPhiAngle);
  } catch (GaudiException e) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::ERROR << "Was not able to create SolidPolycone "
        << "due to following GaudiException : ";
    e.printOut (log);
    log << endmsg;
    result = 0;
  }
    
  // returns
  return result;
} // end dealWithPolycone


// -----------------------------------------------------------------------
// Deal with tubs
// -----------------------------------------------------------------------
SolidTubs* XmlLVolumeCnv::dealWithTubs (xercesc::DOMElement* element) {
  // gets attributes
  std::string sizeZAttribute =
    dom2Std (element->getAttribute (sizeZString));
  std::string outerRadiusAttribute =
    dom2Std (element->getAttribute (outerRadiusString));
  std::string innerRadiusAttribute =
    dom2Std (element->getAttribute (innerRadiusString));
  std::string startPhiAngleAttribute =
    dom2Std (element->getAttribute (startPhiAngleString));
  std::string deltaPhiAngleAttribute =
    dom2Std (element->getAttribute (deltaPhiAngleString));
  std::string tubsName = dom2Std (element->getAttribute (nameString));

  // computes the values
  double sizeZ = 0.0;
  double outerRadius = 0.0;
  double innerRadius = 0.0;
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * Gaudi::Units::degree;
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }
  if (!outerRadiusAttribute.empty()) {
    outerRadius = xmlSvc()->eval(outerRadiusAttribute);
  }
  if (!innerRadiusAttribute.empty()) {
    innerRadius = xmlSvc()->eval(innerRadiusAttribute);
  }
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute, false);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute, false);
  }

  // builds solid
  SolidTubs* result = new SolidTubs
    (tubsName, sizeZ / 2.0, outerRadius, innerRadius,
     startPhiAngle, deltaPhiAngle);

  // checks there are no children
  if (element->hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A tubs should not have any child. They will be ignored" << endmsg;
  }

  // returns
  return result;
} // end dealWithTubs


// -----------------------------------------------------------------------
// Deal with sphere
// -----------------------------------------------------------------------
SolidSphere* XmlLVolumeCnv::dealWithSphere (xercesc::DOMElement* element) {
  // gets attributes
  std::string outerRadiusAttribute =
    dom2Std (element->getAttribute (outerRadiusString));
  std::string innerRadiusAttribute =
    dom2Std (element->getAttribute (innerRadiusString));
  std::string startPhiAngleAttribute =
    dom2Std (element->getAttribute (startPhiAngleString));
  std::string deltaPhiAngleAttribute =
    dom2Std (element->getAttribute (deltaPhiAngleString));
  std::string startThetaAngleAttribute =
    dom2Std (element->getAttribute (startThetaAngleString));
  std::string deltaThetaAngleAttribute =
    dom2Std (element->getAttribute (deltaThetaAngleString));
  std::string sphereName = dom2Std (element->getAttribute (nameString));

  // computes the values
  double outerRadius = 0.0;
  double innerRadius = 0.0;
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * Gaudi::Units::degree;
  double startThetaAngle = 0.0;
  double deltaThetaAngle = 180.0 * Gaudi::Units::degree;  
  if (!outerRadiusAttribute.empty()) {
    outerRadius = xmlSvc()->eval(outerRadiusAttribute);
  }
  if (!innerRadiusAttribute.empty()) {
    innerRadius = xmlSvc()->eval(innerRadiusAttribute);
  }
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute, false);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute, false);
  }
  if (!startThetaAngleAttribute.empty()) {
    startThetaAngle = xmlSvc()->eval(startThetaAngleAttribute, false);
  }
  if (!deltaThetaAngleAttribute.empty()) {
    deltaThetaAngle = xmlSvc()->eval(deltaThetaAngleAttribute, false);
  }

  // take care that if startThetaAngle+deltaThetaAngle = 180 degree,
  // it could be 180 + epsilon in case of a round error.
  // In this case, we correct deltaThetaAngle
  if ((startThetaAngle + deltaThetaAngle > M_PI) &&
      (startThetaAngle + deltaThetaAngle <= M_PI + 2*DBL_EPSILON)) {
    deltaThetaAngle = M_PI - startThetaAngle;
  }
  
  // builds the solid
  SolidSphere* result = new SolidSphere
    (sphereName, outerRadius, innerRadius, startPhiAngle,
     deltaPhiAngle, startThetaAngle, deltaThetaAngle);

  // checks there are no children
  if (element->hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A sphere should not have any child. They will be ignored" << endmsg;
  }

  // returns
  return result;
} // end dealWithSphere


// -----------------------------------------------------------------------
// Deal with transformation
// -----------------------------------------------------------------------
Gaudi::Transform3D*
XmlLVolumeCnv::dealWithTransformation(xercesc::DOMElement* element,
                                      unsigned int* index) {
  // gets children of element
  xercesc::DOMNodeList* childNodes = element->getChildNodes();

  // gets the first node of the transformation
  xercesc::DOMNode* childNode = childNodes->item (*index);
  xercesc::DOMElement* childElement = (xercesc::DOMElement*) childNode;

  // put index on the next node
  *index += 1;
  while (*index < childNodes->getLength() && 
         childNodes->item(*index)->getNodeType() !=
         xercesc::DOMNode::ELEMENT_NODE) {
    *index += 1;
  }

  // the result
  Gaudi::Transform3D* result = 0;

#ifdef __INTEL_COMPILER        // Disable ICC remark
  #pragma warning(disable:177) // handler parameter was declared but never referenced
#endif

  // gets the tag name for childElement
  const XMLCh* tagName = childElement->getNodeName();
  // gets the transformation corresponding to the first child
  if (0 == xercesc::XMLString::compareString(posXYZString, tagName)) {
    result = dealWithPosXYZ(childElement);
  } else if (0 == xercesc::XMLString::compareString(posRPhiZString, tagName)) {
    // catches an exception in case the r attribute is negative
    try {
      result = dealWithPosRPhiZ(childElement);
    } catch (XmlCnvException e) {
      result = new Gaudi::Transform3D();
    }
  } else if (0 == xercesc::XMLString::compareString(posRThPhiString, tagName)) {
    // catches an exception in case the r attribute is negative
    try {
      result = dealWithPosRThPhi(childElement);
    } catch (XmlCnvException e) {
      result = new Gaudi::Transform3D();
    }
  } else if (0 == xercesc::XMLString::compareString
             (transformationString, tagName)) {
    return dealWithTransformation (childElement);
  } else {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "In " << locateElement (childElement)
        << " : this should be a rotation or a translation but is a : "
        << tagNameString << ". It will be ignored." << endmsg;
    xercesc::XMLString::release(&tagNameString);
    return new Gaudi::Transform3D();
  }

  // We are here because we found a translation. Try now to see if
  // it is followed by a rotation
  if (*index < childNodes->getLength()) {
    childNode = childNodes->item (*index);
    childElement = (xercesc::DOMElement*) childNode;
    bool foundRotation = false;
    
    // gets the tag name for the new childElement
    tagName = childElement->getNodeName();
    if (0 == xercesc::XMLString::compareString(rotXYZString, tagName)) {
      Gaudi::Transform3D* rotation = dealWithRotXYZ(childElement);
      *result = *result * *rotation;
      delete (rotation);
      rotation = 0;
      foundRotation = true;
    } else if (0 == xercesc::XMLString::compareString(rotAxisString, tagName)) {
      // catches an exception in case the theta or phi value is out of range
      try {
        Gaudi::Transform3D* rotation = dealWithRotAxis(childElement);
        *result = *result * *rotation;
        delete (rotation);
        rotation = 0;
      } catch (XmlCnvException e) { 
      }
      foundRotation = true;
    }

    // if needed, put index on the next node
    if (foundRotation) {
      *index += 1;
      while (*index < childNodes->getLength() && 
             childNodes->item(*index)->getNodeType() !=
             xercesc::DOMNode::ELEMENT_NODE) {
        *index += 1;
      }
    }
  }
  
  // returns
  return result;
} // end dealWithTransformation


// -----------------------------------------------------------------------
// Deal with transformation
// -----------------------------------------------------------------------
Gaudi::Transform3D*
XmlLVolumeCnv::dealWithTransformation(xercesc::DOMElement* element) {
  // gets the children
  xercesc::DOMNodeList* childNodes = element->getChildNodes();

  // position of the current child processed
  unsigned int i = 0;

  // list of the transformation already found
  std::deque<Gaudi::Transform3D*> tList;

  // scans the children and builds the transformations
  while (i < childNodes->getLength()) {
    Gaudi::Transform3D* transformation = dealWithTransformation (element, &i);
    if (transformation != 0) {
      tList.push_back (transformation);
    }
  }

  // checks there are at least 2 transformations
  if (tList.size() < 2) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A transformation should have at least two children." << endmsg;
  }

  // computes the result
  Gaudi::Transform3D* result = 0;
  if (!tList.empty()) {
    result = tList.front();
    tList.pop_front();
    while (!tList.empty()) {
      Gaudi::Transform3D* transformation = tList.front();
      tList.pop_front();
      *result = *transformation * *result;
      delete transformation;
      transformation = 0;
    }
  } else {
    result = new Gaudi::Transform3D();
  }
  // returns
  return result;
} // end dealWithTransformation


// -----------------------------------------------------------------------
// Deal with posXYZ
// -----------------------------------------------------------------------
Gaudi::Transform3D* XmlLVolumeCnv::dealWithPosXYZ (xercesc::DOMElement* element) {
  // gets attributes
  std::string xAttribute = dom2Std (element->getAttribute (xString));
  std::string yAttribute = dom2Std (element->getAttribute (yString));
  std::string zAttribute = dom2Std (element->getAttribute (zString));

  // computes the values
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  if (!xAttribute.empty()) {
    x = xmlSvc()->eval(xAttribute);
  }
  if (!yAttribute.empty()) {
    y = xmlSvc()->eval(yAttribute);
  }
  if (!zAttribute.empty()) {
    z = xmlSvc()->eval(zAttribute);
  }

  // builds the translation.
  return new Gaudi::Transform3D(Gaudi::XYZVector(x, y, z) );
} // end dealWithPosXYZ


// -----------------------------------------------------------------------
// Deal with posRPhiZ
// -----------------------------------------------------------------------
Gaudi::Transform3D* XmlLVolumeCnv::dealWithPosRPhiZ (xercesc::DOMElement* element) {
  // gets attributes
  std::string rAttribute = dom2Std (element->getAttribute (rString));
  std::string phiAttribute = dom2Std (element->getAttribute (phiString));
  std::string zAttribute = dom2Std (element->getAttribute (zString));

  // computes the values
  double rho = 0.0;
  double z = 0.0;
  double phi = 0.0;

  if (!rAttribute.empty()) {
    rho = xmlSvc()->eval(rAttribute);
  }
  if (!phiAttribute.empty()) {
    phi = xmlSvc()->eval(phiAttribute, false);
  }
  if (!zAttribute.empty()) {
    z = xmlSvc()->eval(zAttribute);
  }
  // checks the validity of the value of r
  if (0.0 > rho) {
    throw XmlCnvException
      (" RPhiZTranslation : r must be non-negative value !", CORRUPTED_DATA);
  }
  
  // builds the translation.

  return new Gaudi::Transform3D(Gaudi::RhoZPhiVector( rho, z, phi ) );
} // end dealWithPosRPhiZ


// -----------------------------------------------------------------------
// Deal with posRThPhi
// -----------------------------------------------------------------------
Gaudi::Transform3D* XmlLVolumeCnv::dealWithPosRThPhi (xercesc::DOMElement* element) {
  // gets attributes
  std::string rAttribute = dom2Std (element->getAttribute (rString));
  std::string thetaAttribute = dom2Std (element->getAttribute (thetaString));
  std::string phiAttribute = dom2Std (element->getAttribute (phiString));

  // computes the values
  double r = 0.0;
  double theta = 0.0;
  double phi = 0.0;
  if (!rAttribute.empty()) {
    r = xmlSvc()->eval(rAttribute);
  }
  if (!thetaAttribute.empty()) {
    theta = xmlSvc()->eval(thetaAttribute, false);
  }
  if (!phiAttribute.empty()) {
    phi = xmlSvc()->eval(phiAttribute, false);
  }
  // checks the validity of the value of r
  if (0.0 > r) {
    throw XmlCnvException
      (" RThPhiTranslation : r must be non-negative value !", CORRUPTED_DATA);
  }
  
  // builds the translation.

  return new Gaudi::Transform3D(Gaudi::Polar3DVector(r, theta, phi) );
} // end dealWithPosRThPhi


// -----------------------------------------------------------------------
// Deal with rotXYZ
// -----------------------------------------------------------------------
Gaudi::Transform3D* XmlLVolumeCnv::dealWithRotXYZ (xercesc::DOMElement* element) {
  // get attributes
  std::string rotXAttribute = dom2Std (element->getAttribute (rotXString));
  std::string rotYAttribute = dom2Std (element->getAttribute (rotYString));
  std::string rotZAttribute = dom2Std (element->getAttribute (rotZString));

  // computes the values
  double rx = 0.0;
  double ry = 0.0;
  double rz = 0.0;
  if (!rotXAttribute.empty()) {
    rx = xmlSvc()->eval(rotXAttribute, false);
  }
  if (!rotYAttribute.empty()) {
    ry = xmlSvc()->eval(rotYAttribute, false);
  }
  if (!rotZAttribute.empty()) {
    rz = xmlSvc()->eval(rotZAttribute, false);
  }

  // computes the rotation
  Gaudi::Rotation3D result = Gaudi::RotationX(rx) * 
    Gaudi::RotationY(ry) *
    Gaudi::RotationZ(rz);

  // Recover precision in cases when rotation is by halfpi or pi
  double xx, xy, xz, yx, yy, yz, zx, zy, zz;
  result.GetComponents( xx, xy, xz, yx, yy, yz, zx, zy, zz );
  if( fabs(xx) < 1.e-15 ) xx = 0.;
  if( fabs(xy) < 1.e-15 ) xy = 0.;
  if( fabs(xz) < 1.e-15 ) xz = 0.;
  if( fabs(yx) < 1.e-15 ) yx = 0.;
  if( fabs(yy) < 1.e-15 ) yy = 0.;
  if( fabs(yz) < 1.e-15 ) yz = 0.;
  if( fabs(zx) < 1.e-15 ) zx = 0.;
  if( fabs(zy) < 1.e-15 ) zy = 0.;
  if( fabs(zz) < 1.e-15 ) zz = 0.;
  result.SetComponents( xx, xy, xz, yx, yy, yz, zx, zy, zz );

  return new Gaudi::Transform3D(result);
} // end dealWithRotXYZ


// -----------------------------------------------------------------------
// Deal with rotAxis
// -----------------------------------------------------------------------
Gaudi::Transform3D* XmlLVolumeCnv::dealWithRotAxis (xercesc::DOMElement* element) {
  // get attributes
  std::string axThetaAttribute =
    dom2Std (element->getAttribute (axThetaString));
  std::string axPhiAttribute =
    dom2Std (element->getAttribute (axPhiString));
  std::string angleAttribute =
    dom2Std (element->getAttribute (angleString));

  // computes the values
  double axTheta = 0.0;
  double axPhi = 0.0;
  double angle = 0.0;
  if (!axThetaAttribute.empty()) {
    axTheta = xmlSvc()->eval (axThetaAttribute, false);
  }
  if (!axPhiAttribute.empty()) {
    axPhi = xmlSvc()->eval (axPhiAttribute, false);
  }
  if (!angleAttribute.empty()) {
    angle = xmlSvc()->eval (angleAttribute, false);
  }
  // checks the validity of the theta angle
  if (axTheta < 0 || axTheta > 180 * Gaudi::Units::degree) {
    throw XmlCnvException
      (" AxisRotation : axTheta must be inside 0*degree"
       " and 180*degree ! ", CORRUPTED_DATA);
  }
  // checks the validity of the phi angle
  if (axPhi < 0 || axPhi > 360 * Gaudi::Units::degree) { 
    throw XmlCnvException 
      (" AxisRotation : axPhi  must be inside 0*degree"
       " and 360*degree ! ", CORRUPTED_DATA);
  }
  // Construction of vector with input of theta and phi
  Gaudi::XYZVector axis( sin(axTheta)*cos(axPhi),
                         sin(axTheta)*sin(axPhi),
                         cos(axTheta)             );

  return new Gaudi::Transform3D(Gaudi::AxisAngle(axis, angle), 
                                Gaudi::XYZVector());


} // end dealWithRotAxis

//=========================================================================
//  Method to handle the 'tag' string and replace by the numeral.
//=========================================================================
void XmlLVolumeCnv::replaceTagInString ( std::string& string ) {
  std::string::size_type indx = string.find( "-KEY-" );
  if ( std::string::npos != indx ) {
    if ( indx < string.size()-5 ) {
      string = string.substr(0,indx) + m_numeral + string.substr( indx+5 );
    } else {
      string = string.substr(0,indx) + m_numeral;
    }
  }
}
// ============================================================================
// End 
// ============================================================================
