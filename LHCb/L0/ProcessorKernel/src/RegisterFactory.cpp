#include "ProcessorKernel/RegisterFactory.h"

L0Muon::RegisterFactory* L0Muon::RegisterFactory::m_instance=0;

L0Muon::RegisterFactory::RegisterFactory() {

  m_found =0;
  
}

L0Muon::RegisterFactory::~RegisterFactory() {}

L0Muon::RegisterFactory* L0Muon::RegisterFactory::instance() {
  
  if ( m_instance == 0 ) {
    m_instance = new L0Muon::RegisterFactory;
  } 
  return m_instance;
}

L0Muon::Register* L0Muon::RegisterFactory::createRegister
                 (std::string name, int size) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
       
    return (*ind).second;
  } else {
    
    Register* pr= new Register(size);
    pr->setName(name);
    m_registers[name] = pr;
    return pr;
  }  
}

L0Muon::TileRegister* L0Muon::RegisterFactory::createTileRegister
                 (std::string name, int size) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
    return dynamic_cast<L0Muon::TileRegister*>((*ind).second);
  } else {
    TileRegister* pr= new TileRegister(size);
    pr->setName(name);
    m_registers[name] = pr;
   
    return pr;
  }  
}

L0Muon::TileRegister* L0Muon::RegisterFactory::searchTileRegister
                 (std::string name) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
   
    m_found = true;
    
   return dynamic_cast<L0Muon::TileRegister*>((*ind).second);
  } else {
    return 0;
    
  }  
}

void L0Muon::RegisterFactory::deleteRegister(std::string name) {
  
  Register* pr = m_registers[name];
  delete pr;
  m_registers.erase(name);
  
}


int L0Muon::RegisterFactory::
createAlias(std::string name, std::string alias) {
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
    m_registers[alias] = (*ind).second;
    return 0;
  } else {
    return -1;
  }
}

void L0Muon::RegisterFactory::dump() {
  std::cout << "RegisterFactory: " << m_registers.size() << " registers" << std::endl;
}
