// $Id: L0MuonKernelFromXML.h,v 1.1 2005-07-01 12:42:42 jucogan Exp $

#ifndef L0MUONKERNEL_L0MUONKERNELFROMXML_H
#define L0MUONKERNEL_L0MUONKERNELFROMXML_H     1

/** @class L0MuonKernelFromXML L0MuonKernelFromXML.h L0MuonKernel/L0MuonKernelFromXML.h

Class representing a factory for creating and 
owning registers
   
   
*/ 

#include <string>
#include <map>
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/L0MUnit.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {

  Unit * L0MuonKernelFromXML(std::string XmlFileName);

  int getAttributeInt(DOMNamedNodeMap* di, const char* key);
  std::string getAttributeStr(DOMNamedNodeMap* di, const char* key);

/*   class L0MUONKERNELFROMXML { */

/*   protected: */

/*     /// Constructor */
/*     L0MUONKERNELFROMXML(); */

/*     /// Constructor */
/*     L0MUONKERNELFROMXML(std::string XmlFileName); */
/*   public: */

/*     /// Destructor */
/*     virtual ~L0MUONKERNELFROMXML(); */
  
/*     /// Get the pointer to the top unit */
/*     Unit* top(){return m_top;} */

/*   private: */

/*     Unit* m_top; */

/*     DOMDocument*  m_doc;      //The DOM document */
/*     DOMElement*   m_root;     //The root element */
/*     DOMBuilder*   m_parser;   //The XML parser */


/*   }; */

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUONKERNELFROMXML_H  
