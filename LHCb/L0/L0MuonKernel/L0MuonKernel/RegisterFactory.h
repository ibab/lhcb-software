// $Id: RegisterFactory.h,v 1.2 2004-03-11 15:57:55 ltocco Exp $

#ifndef L0MUONKERNEL_REGISTERFACTORY_H
#define L0MUONKERNEL_REGISTERFACTORY_H     1

/** @class RegisterFactory RegisterFactory L0MuonKernel/RegisterFactory.h

   Class representing factory for creating Registers
   of the L0Muon Trigger for hardware simulations
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include <string>
#include <map>
#include "L0mConf/L0MPuNodeBase.h"
#include "L0MuonKernel/Register.h"
#include "L0MuonKernel/TileRegister.h"

namespace L0Muon {

class RegisterFactory {

protected:
  RegisterFactory();
public:
  virtual ~RegisterFactory();
  
  static RegisterFactory* instance();
  
  Register* createRegister(std::string name, int size);
  TileRegister* createTileRegister(std::string name, int size);
  TileRegister* searchTileRegister(std::string name);
 int createAlias(std::string name, std::string alias);
  void deleteRegister(std::string name);
  std::map<std::string,Register*> getRegisters() { return m_registers ; }   
  bool found(){ return m_found;}
  
    

private:

  static RegisterFactory* m_instance;
  std::map<std::string,Register*> m_registers;    // registers repository    
  bool m_found;

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_REGISTERFACTORY_H  
