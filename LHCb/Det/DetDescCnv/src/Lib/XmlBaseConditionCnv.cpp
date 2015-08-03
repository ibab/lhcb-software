// include files
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/Condition.h"
#include "DetDescCnv/XmlBaseConditionCnv.h"
#include "XmlTools/IXmlSvc.h"

#include "GaudiUtils/HistoParsers.h"
#include "DetDesc/HistoParam.h"

#include <sstream>
#include <string>
#include <vector>

class ISvcLocator;

#include "TH1D.h"
#include "TH2D.h"
#include "TDirectory.h"

// -----------------------------------------------------------------------
namespace {
  /// helper function for converton of 1d or 2d histograms
  template<typename HTYPE>
  StatusCode _addHisto(Condition         *dataObj ,
                       const std::string &name    ,
                             std::string  value   ,
                       const std::string &comment );
}

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
  itemString = xercesc::XMLString::transcode("item");

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
  itemString = xercesc::XMLString::transcode("item");

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
  xercesc::XMLString::release((XMLCh**)&itemString);

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
    if( msgLevel(MSG::VERBOSE) )
      verbose() << "Initializing converter for class ID " << classID() << endmsg;
    if (0 != m_xmlSvc) {
      m_doGenericCnv = m_xmlSvc->allowGenericCnv();
      if( msgLevel(MSG::DEBUG) )
        debug() << "Generic conversion status: "
                << (unsigned int)m_doGenericCnv << endmsg;
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
  Condition* dataObj = static_cast<Condition*> (refpObject);

  // initialise it and return the status
  return dataObj->initialize();

}


// -----------------------------------------------------------------------
// Resolve the references of the just updated transient object.
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::updateObjRefs (IOpaqueAddress* /*childElement*/,
                                               DataObject* refpObject) {
  // gets the object
  Condition* dataObj = static_cast<Condition*> (refpObject);

  // initialise it and return the status
  return dataObj->initialize();

}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::i_createObj (xercesc::DOMElement* /*element*/,
                                             DataObject*& refpObject) {
  // creates an object for the node found
  if( msgLevel(MSG::VERBOSE) )
    verbose() << "Normal generic condition conversion" << endmsg;
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
  // gets the object
  Condition* dataObj = static_cast<Condition*> (refpObject);
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
          warning() << "parsing of specific child "
                    << nameString << " raised errors." << endmsg;
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
      if( msgLevel(MSG::VERBOSE) )
        verbose() << "Adding user parameter " << name << " with value "
                  << value << ", type " << type << " and comment \"" << comment
                  << "\"" << endmsg;
      if (type == "int") {
        dataObj->addParam<int>(name,(int)xmlSvc()->eval(value, false),comment);
      } else if(type == "double") {
        dataObj->addParam<double>(name,xmlSvc()->eval(value, false),comment);
      } else if(type == "Histo1D"){
        StatusCode sc = _addHisto<DetDesc::Params::Histo1D>(dataObj, name, value, comment);
        if ( sc.isFailure() )
          error() << "failed to parse Histo1D name ='" << name
                  << "' value = '" << value << "'" << endmsg;
      } else if(type == "Histo2D"){
        StatusCode sc = _addHisto<DetDesc::Params::Histo2D>(dataObj, name, value, comment);
        if ( sc.isFailure() )
          error() << "failed to parse Histo2D name ='" << name
                  << "' value = '" << value << "'" << endmsg;
      } else {
        dataObj->addParam<std::string>(name,value,comment);
      }
    } else if (0 == xercesc::XMLString::compareString
               (paramVectorString, tagName)) {
      // parses the value
      std::vector<std::string> vect;
      std::istringstream cstr (value);
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
      if( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Adding user parameter vector " << name
                  << " with values ";
        std::vector<std::string>::iterator it2;
        for (it2 = vect.begin();
             vect.end() != it2;
             ++it2) {
          verbose() << *it2 << " ";
        }
        verbose() << ", type " << type << " and comment \""
                  << comment << "\"." << endmsg;
      }

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
      warning() << "Type " << keytype <<  " not supported for map keys";
    }
  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    warning() << "This tag makes no sense to element : "
              << tagNameString << endmsg;
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


#include "boost/algorithm/string/trim.hpp"

namespace {
  /** helper function to convert 1d or 2d histogram from the string
   *  representation (custom or root xml) and add it to the Condition
   *  in the current implementation HTYPE = DetDesc::Params::Histo1D (TH1D)
   *  or DetDesc::Params::Histo2D (TH2D) as defined in DetDesc/HistoParam.h
   *
   *  @param dataObj (UPDATE) - pointer to the Conditon object
   *  @param name    (INPUT)  - name of the param tag
   *  @param value   (INPUT)  - string representation of the histogram
   *  @param comment (INPUT)  - the comment
   *  @returns SUCCESS in case of successful conversion and insertion
   */
  template<typename HTYPE>
  StatusCode _addHisto(Condition *dataObj,
                       const std::string &name    ,
                             std::string  value   ,
                       const std::string &comment )
  {
    if ( !dataObj ) return StatusCode::FAILURE;
    HTYPE histo;

    bool _old_st = histo.AddDirectoryStatus();
    histo.AddDirectory( kFALSE );
    histo.SetDirectory( NULL   );

    boost::algorithm::trim( value ) ;
    StatusCode sc = Gaudi::Parsers::parse( histo, value );

    if ( sc.isSuccess() )
      dataObj->addParam<HTYPE>(name, histo, comment);
    histo.AddDirectory( _old_st );

    return sc;
  }
}
