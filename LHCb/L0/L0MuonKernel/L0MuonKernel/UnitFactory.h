// $Id: UnitFactory.h,v 1.4 2008-12-15 10:19:47 cattanem Exp $

#ifndef L0MUONKERNEL_UNITFACTORY_H
#define L0MUONKERNEL_UNITFACTORY_H     1

#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {
  class Unit;

  /** @class UnitFactory UnitFactory.h L0MuonKernel/UnitFactory.h

  Class representing a factory for creating Units. 

  The factory only holds only the pointer to the top Unit. The pointers to the 
  other units are stored by their parent in the hierarchy. 
  
  This class contains a method to instanciate the top unit 
  from the xml description of the L0Muon processor

  */ 
  class UnitFactory {
    
  protected:
    
    /// Constructor
    UnitFactory();

  public:
    
    /// Destructor
    virtual ~UnitFactory();
    
    /// single instance for the factory
    static UnitFactory* instance();
    
    /** From the given xml node, creates a Unit of the given type
        and returns a pointer to it
    */
    Unit* createUnit(DOMNode* pNode, std::string type);

    /// Fill the factory from the top unit node in the XML file describing the L0Muon processor
    void fromXML(DOMNode* pNode);
    
    /// Empty the factory
    void reset();

    /// Returns a pointer to the top unit in the hierarchy of Units in the factory
    Unit * topUnit(){return m_topUnit;}
    
    
  private:
    
    /** Utility for XML decoding: get the attribute key in the list di
        and convert it into an integer
    */
    int getAttributeInt(DOMNamedNodeMap* di, const char* key);
    /** Utility for XML decoding: get the attribute key in the list di
        and convert it into a string
    */
    std::string getAttributeStr(DOMNamedNodeMap* di, const char* key);
    
  private:
    
    static UnitFactory* m_instance;  ///< pointer to the factory instance (unique)
    
    Unit * m_topUnit;                ///< pointer to the top unit in the hierarchy of Units in the factory
    
  };
  
};  // namespace L0Muon

#endif      // L0MUONKERNEL_UNITFACTORY_H  
