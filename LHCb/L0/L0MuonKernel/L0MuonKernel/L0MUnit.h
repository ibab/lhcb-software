#ifndef L0MUONKERNEL_L0MUNIT_H
#define L0MUONKERNEL_L0MUNIT_H     1

/** @class L0MUnit L0MUnit.h L0MuonKernel/L0MUnit.h

*/ 
#include "ProcessorKernel/Unit.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "Kernel/MuonTileID.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {

  /**
     The XML keywords.
  */
  static const std::string XMLProperty        = "Property";
  static const std::string XMLRegisterRecord  = "RegisterRecord";
  static const std::string XMLInputRegisters  = "InputRegisters";
  static const std::string XMLOutputRegisters = "OutputRegisters";
  static const std::string XMLUnit            = "Unit";

  class L0MUnit : public Unit {
    
  public:
    
    /** Constructor

    */
    L0MUnit();

    L0MUnit(LHCb::MuonTileID id);

    /// Constructor from XML (DOM)
    L0MUnit(DOMNode* pNode);

    /// Destructor
    ~L0MUnit();

    LHCb::MuonTileID mid(){return m_mid;}

    /// Save the Unit in XML format
    std::string toXML(std::string tab="");

  protected:
    LHCb::MuonTileID m_mid;
    /// Utilities for XML decoding: get the attribute key in the list di 
    /// and convert it into an integer, a string,...
    int         getAttributeInt(DOMNamedNodeMap* di, const char* key);
    std::string getAttributeStr(DOMNamedNodeMap* di, const char* key);

    void propertyFromNode(DOMNode* pNode);
    void registersFromNode(DOMNode* pNode, int io);
    void unitFromNode(DOMNode* pNode);
  };
 


};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUNIT_H  
