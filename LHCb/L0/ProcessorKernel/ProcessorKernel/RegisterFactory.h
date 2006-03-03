// $Id: RegisterFactory.h,v 1.6 2006-03-03 14:11:01 jucogan Exp $

#ifndef PROCESSORKERNEL_REGISTERFACTORY_H
#define PROCESSORKERNEL_REGISTERFACTORY_H     1

/** @class RegisterFactory RegisterFactory.h L0MuonKernel/RegisterFactory.h

Class representing a factory for creating and 
owning registers
   
   
*/ 

#include <string>
#include <map>
#include "ProcessorKernel/Register.h"
#include "ProcessorKernel/TileRegister.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {


  class RegisterFactory {

  protected:

    /// Constructor
    RegisterFactory();
  public:

    /// Destructor
    virtual ~RegisterFactory();
  
    /// single instance for the factory
    static RegisterFactory* instance();
  
    /**
       Create a register.
       If the register was already created, it 
       return the existing one.
     
       @param name : name of the register
       @param size : number of bits
    */
    Register* createRegister(std::string name, int size);
    Register* searchRegister(std::string name);

    /**
       Create a tileregister.
       If the register was alredy created, it return the 
       existing one
     
       @param name : name of the register
       @param size : number of bits
    */
    TileRegister* createTileRegister(std::string name, int size);
    TileRegister* searchTileRegister(std::string name);
 
    /// Crate an alias for the name of the register
    int createAlias(std::string name, std::string alias);

    /// Delete the registers named name 
    void deleteRegister(std::string name);
    std::map<std::string,Register*> getRegisters() { return m_registers ; }   

    /// True if the register is found
    bool found(){ return m_found;}
  
    /// Dump the factory state
    void dump(bool full=false);  

    /// Write out the XML file describing the registers
    std::string toXML(std::string tab="");
    
    /// Fill the factory from the XML file describing the registers
    void fromXML(DOMNode* pNode);
    void registerFromNode(DOMNode* pNode);
    void tileRegisterFromNode(DOMNode* pNode);
    void registerAliasFromNode(DOMNode* pNode);
    void tileFromNode(DOMNode* pNode, 
                      std::vector<LHCb::MuonTileID> *mids, 
                      boost::dynamic_bitset<> *tilestag, 
                      boost::dynamic_bitset<> *stripstag);

  /// Utilities for XML decoding: get the attribute key in the list di 
  /// and convert it into an integer, a string,...
  int         getAttributeInt(DOMNamedNodeMap* di, const char* key);
  std::string getAttributeStr(DOMNamedNodeMap* di, const char* key);


  private:

    static RegisterFactory* m_instance;
    std::map<std::string,Register*> m_registers;    // registers repository    
    bool m_found;


  };

};  // namespace L0Muon

#endif      // PROCESSORKERNEL_REGISTERFACTORY_H  
