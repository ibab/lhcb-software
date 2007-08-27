// $Id: UnitFactory.h,v 1.2 2007-08-27 09:32:24 jucogan Exp $

#ifndef L0MUONKERNEL_UNITFACTORY_H
#define L0MUONKERNEL_UNITFACTORY_H     1

/** @class UnitFactory UnitFactory.h L0MuonKernel/UnitFactory.h

   Class representing a factory for creating and 
   owning registers
   
   
*/ 

#include <string>
#include <map>
#include "ProcessorKernel/Unit.h"
//#include "L0MuonKernel/L0MUnit.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {

class UnitFactory {

protected:

  /// Constructor
  UnitFactory();
public:

  /// Destructor
  virtual ~UnitFactory();
  
  /// single instance for the factory
  static UnitFactory* instance();

  /// Fill the factory from the XML file describing the registers
  Unit* createUnit(DOMNode* pNode, std::string type);
//   Unit* fromXML(DOMNode* pNode);
  void fromXML(DOMNode* pNode);
  
  /// Empty the factory
  void reset();

  Unit * topUnit(){return m_topUnit;}
  

private:

  int getAttributeInt(DOMNamedNodeMap* di, const char* key);
  std::string getAttributeStr(DOMNamedNodeMap* di, const char* key);

private:

  static UnitFactory* m_instance;
  
  Unit * m_topUnit;

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_UNITFACTORY_H  
