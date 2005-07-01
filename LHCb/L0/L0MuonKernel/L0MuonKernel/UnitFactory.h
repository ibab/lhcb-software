// $Id: UnitFactory.h,v 1.1 2005-07-01 12:42:43 jucogan Exp $

#ifndef L0MUONKERNEL_UNITFACTORY_H
#define L0MUONKERNEL_UNITFACTORY_H     1

/** @class UnitFactory UnitFactory.h L0MuonKernel/UnitFactory.h

   Class representing a factory for creating and 
   owning registers
   
   
*/ 

#include <string>
#include <map>
#include "L0MuonKernel/L0MUnit.h"

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
  
  /**
     Create a unit.
   */
  Unit* createUnit(DOMNode* pNode, std::string type);

  
private:

  static UnitFactory* m_instance;

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_UNITFACTORY_H  
