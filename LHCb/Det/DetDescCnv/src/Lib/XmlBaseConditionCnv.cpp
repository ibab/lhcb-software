// $Id: XmlBaseConditionCnv.cpp,v 1.10 2007-11-09 17:06:05 marcocle Exp $

// include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/Condition.h"
#include "DetDescCnv/XmlBaseConditionCnv.h"
#include "XmlTools/IXmlSvc.h"

#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <strstream> 
#else
#include <sstream>
#endif
#include <string>
#include <vector>

class ISvcLocator;

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::XmlBaseConditionCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, XmlBaseConditionCnv::classID()) {
  m_doGenericCnv = false;
  specificString = xercesc::XMLString::transcode("specific");
  paramString = xercesc::XMLString::transcode("param");
  paramVectorString = xercesc::XMLString::transcode("paramVector");
  mapString = xercesc::XMLString::transcode("map");
  entryString = xercesc::XMLString::transcode("entry");
  
  typeString = xercesc::XMLString::transcode("type");
  nameString = xercesc::XMLString::transcode("name");
  commentString = xercesc::XMLString::transcode("comment");
  keytypeString = xercesc::XMLString::transcode("keytype");
  valuetypeString = xercesc::XMLString::transcode("valuetype");
  keyString = xercesc::XMLString::transcode("key");
  valueString = xercesc::XMLString::transcode("value");
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::XmlBaseConditionCnv (ISvcLocator* svc,
                                          const CLID& clsID ) :
  XmlGenericCnv (svc, clsID) {
  m_doGenericCnv = false;
  specificString = xercesc::XMLString::transcode("specific");
  paramString = xercesc::XMLString::transcode("param");
  paramVectorString = xercesc::XMLString::transcode("paramVector");
  mapString = xercesc::XMLString::transcode("map");
  entryString = xercesc::XMLString::transcode("entry");
  
  typeString = xercesc::XMLString::transcode("type");
  nameString = xercesc::XMLString::transcode("name");
  commentString = xercesc::XMLString::transcode("comment");
  keytypeString = xercesc::XMLString::transcode("keytype");
  valuetypeString = xercesc::XMLString::transcode("valuetype");
  keyString = xercesc::XMLString::transcode("key");
  valueString = xercesc::XMLString::transcode("value");
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::~XmlBaseConditionCnv () {
  xercesc::XMLString::release((XMLCh**)&specificString);
  xercesc::XMLString::release((XMLCh**)&paramString);
  xercesc::XMLString::release((XMLCh**)&paramVectorString);
  xercesc::XMLString::release((XMLCh**)&mapString);
  xercesc::XMLString::release((XMLCh**)&entryString);
  
  xercesc::XMLString::release((XMLCh**)&typeString);
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&commentString);
  xercesc::XMLString::release((XMLCh**)&keytypeString);
  xercesc::XMLString::release((XMLCh**)&valuetypeString);
  xercesc::XMLString::release((XMLCh**)&keyString);
  xercesc::XMLString::release((XMLCh**)&valueString);
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::initialize() {
  StatusCode sc = XmlGenericCnv::initialize();
  if (sc.isSuccess()) {
    MsgStream log (msgSvc(), "XmlBaseConditionCnv");
    log << MSG::VERBOSE << "Initializing converter for class ID " << classID() << endreq;
    if (0 != m_xmlSvc) {
      m_doGenericCnv = m_xmlSvc->allowGenericCnv();
      log << MSG::DEBUG << "Generic conversion status: "
          << (unsigned int)m_doGenericCnv << endreq;
    }
  }
  return sc;
}


// -----------------------------------------------------------------------
// Resolve the references of the created transient object.
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::fillObjRefs (IOpaqueAddress* /*childElement*/,
                                             DataObject* refpObject) {
  // gets the object
  Condition* dataObj = dynamic_cast<Condition*> (refpObject);

  // initialise it and return the status
  return dataObj->initialize();

}


// -----------------------------------------------------------------------
// Resolve the references of the just updated transient object.
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::updateObjRefs (IOpaqueAddress* /*childElement*/,
                                               DataObject* refpObject) {
  // gets the object
  Condition* dataObj = dynamic_cast<Condition*> (refpObject);

  // initialise it and return the status
  return dataObj->initialize();

}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::i_createObj (xercesc::DOMElement* /*element*/,
                                             DataObject*& refpObject) {
  MsgStream log(msgSvc(), "XmlBaseConditionCnv" );
  
  // creates an object for the node found
  log << MSG::DEBUG << "Normal generic condition conversion" << endreq;
  // const XMLCh* elementName = element->getAttribute(nameString);
  // Since the name is never used afterwars, we just don't pass it to
  // the condition object
  refpObject = new Condition (/*elementName*/);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj

// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::i_fillObj (xercesc::DOMElement* childElement,
                                           DataObject* refpObject,
                                           IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "XmlBaseConditionCnv" );

  // gets the object
  Condition* dataObj = dynamic_cast<Condition*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(tagName, specificString)) {
    // this is the place where the user will put new elements he wants
    // to add to the default condition
    // So we just go through the children of this element and call
    // i_fillSpecificObj on them
    xercesc::DOMNodeList* specificChildren = childElement->getChildNodes();
    unsigned int i;
    for (i = 0; i < specificChildren->getLength(); i++) {
      if (specificChildren->item(i)->getNodeType() ==
          xercesc::DOMNode::ELEMENT_NODE) {
        // gets the current child
        xercesc::DOMNode* childNode = specificChildren->item(i);
        StatusCode sc = i_fillSpecificObj ((xercesc::DOMElement*) childNode,
                                           dataObj,
                                           address);
        if (sc.isFailure()) {
          const XMLCh* childNodeName =
            ((xercesc::DOMElement*) childNode)->getNodeName();
          char* nameString = xercesc::XMLString::transcode(childNodeName);
          log << MSG::WARNING << "parsing of specific child "
              << nameString << " raised errors." << endreq;
          xercesc::XMLString::release(&nameString);
        }
      }
    }
  } else if (0 == xercesc::XMLString::compareString(paramString, tagName) ||
             0 == xercesc::XMLString::compareString(paramVectorString,
                                                    tagName)) {
    // get the attributes
    std::string type = dom2Std (childElement->getAttribute (typeString));
    std::string name = dom2Std (childElement->getAttribute (nameString));
    std::string comment = dom2Std (childElement->getAttribute (commentString));
    
    // gets the value
    std::string value;
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    for (i = 0; i < nodeChildren->getLength(); i++) {
      if (nodeChildren->item(i)->getNodeType() == xercesc::DOMNode::TEXT_NODE) {
        std::string newVal = dom2Std (nodeChildren->item(i)->getNodeValue());
        unsigned int begin = 0;
        while (begin < newVal.length() && isspace(newVal[begin])) {
          begin++;
        }
        if (begin < newVal.length()) {
          unsigned int end = newVal.length() - 1;
          while (isspace(newVal[end])) {
            end--;
          }
          if (i>0) value += " ";
          value += newVal.substr(begin, end - begin + 1);
        }
      }
    }

    if (0 == xercesc::XMLString::compareString(paramString, tagName)) {
      // adds the new parameter to the detectorElement
      log << MSG::DEBUG << "Adding user parameter " << name << " with value "
          << value << ", type " << type << " and comment \"" << comment
          << "\"" << endreq;
      if (type == "int") {
        dataObj->addParam<int>(name,(int)xmlSvc()->eval(value, false),comment);
      } else if(type == "double") {
        dataObj->addParam<double>(name,xmlSvc()->eval(value, false),comment);
      } else {
        dataObj->addParam<std::string>(name,value,comment);
      }
    } else if (0 == xercesc::XMLString::compareString
               (paramVectorString, tagName)) {
      // parses the value
      std::vector<std::string> vect;
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
      const char *textValue = value.c_str();
      std::istrstream cstr (textValue, value.length());
#else
      std::istringstream cstr (value);
#endif
      std::string val;
      while (cstr >> val) {
        vect.push_back (val);
      }
      
      // depending on the type, evaluates the value
      std::vector<double> d_vect;
      std::vector<int> i_vect;
      std::vector<std::string>::iterator it;
      for (it = vect.begin();
           vect.end() != it;
           ++it) {
        if ("int" == type) {
          double dd = xmlSvc()->eval(*it, false);
          i_vect.push_back ((int)dd);
        } else if ("double" == type) {
          double dd = xmlSvc()->eval(*it, false);
          d_vect.push_back (dd);
        }
      }
      // adds the new parameterVector to the detectorElement
      log << MSG::DEBUG << "Adding user parameter vector " << name
          << " with values ";
      std::vector<std::string>::iterator it2;
      for (it2 = vect.begin();
           vect.end() != it2;
           ++it2) {
        log << *it2 << " ";
      }
      log << ", type " << type << " and comment \""
          << comment << "\"." << endreq;
      if ("int" == type) {
        dataObj->addParam(name,i_vect,comment);
      } else if ("double" == type) {
        dataObj->addParam(name,d_vect,comment);
      } else {
        dataObj->addParam(name,vect,comment);
      }
    }
  } else if (0 == xercesc::XMLString::compareString(mapString, tagName)) {
    std::string name = dom2Std(childElement->getAttribute(nameString));
    std::string comment = dom2Std(childElement->getAttribute(commentString));
    std::string keytype = dom2Std(childElement->getAttribute(keytypeString));
    std::string valuetype = dom2Std(childElement->getAttribute(valuetypeString));
    
    xercesc::DOMNodeList* entries = childElement->getChildNodes();
    if ( keytype == "string" ) {
      if ( valuetype == "int" ) {
        dataObj->addParam(name,i_makeMap<std::string,int>(entries),comment);
      }
      else if ( valuetype == "double" ) {
        dataObj->addParam(name,i_makeMap<std::string,double>(entries),comment);
      }
      else {
        dataObj->addParam(name,i_makeMap<std::string,std::string>(entries),comment);
      }
    }
    else if ( keytype == "int" ) {
      if ( valuetype == "int" ) {
        dataObj->addParam(name,i_makeMap<int,int>(entries),comment);
      }
      else if ( valuetype == "double" ) {
        dataObj->addParam(name,i_makeMap<int,double>(entries),comment);
      }
      else {
        dataObj->addParam(name,i_makeMap<int,std::string>(entries),comment);
      }
    }
    else {
      log << MSG::WARNING << "Type " << keytype <<  " not supported for map keys";
    }
  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "This tag makes no sense to element : "
        << tagNameString << endreq;
    xercesc::XMLString::release(&tagNameString);
  }
  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Accessor to the classID
// -----------------------------------------------------------------------
const CLID& XmlBaseConditionCnv::classID() {
  return CLID_Condition;
}

