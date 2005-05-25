#include <iostream>
#include "ProcessorKernel/Unit.h"


L0Muon::Unit::Unit() {

  m_parent =0;
  m_debug = false;
  m_name = "Unknown";  
}
    
L0Muon::Unit::~Unit() {} 

void L0Muon::Unit::setParent(L0Muon::Unit * unit)
{
  m_parent = unit;
}


std::string L0Muon::Unit::getProperty(std::string name) {
  std::map<std::string,std::string>::iterator im;
  im = m_properties.find(name);
  if (im!=m_properties.end()) {
    return (*im).second;
  } else {
    return "Unknown";
  }
}


void L0Muon::Unit::addInputRegister(L0Muon::Register* in, std::string rname) {

  m_inputs[rname] = in ;
}

void L0Muon::Unit::addInputRegister(L0Muon::Register* in) {
  std::string nm = in->name();
  m_inputs[nm] = in ;
}

void L0Muon::Unit::addOutputRegister(L0Muon::Register* in, std::string rname) {

  m_outputs[rname] = in ;
}

void L0Muon::Unit::addOutputRegister(L0Muon::Register* out) {
  std::string nm = out->name();
  m_outputs[nm] = out ;
}

void L0Muon::Unit::addUnit(L0Muon::Unit* punit, std::string uname) {

  m_units[uname] = punit ;
}

void L0Muon::Unit::addUnit(L0Muon::Unit* punit) {
  int nreg = m_units.size();
  char  strbuf[256];
  sprintf(strbuf,"%d",nreg);
  std::string iss = strbuf;
  m_units[iss] = punit ;
}

void L0Muon::Unit::releaseRegisters() {

  std::map<std::string,L0Muon::Register*>::iterator ir;
  if ( ! m_inputs.empty()){
    
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {

    L0Muon::Register* reg = (*ir).second;
    reg->reset();    
  }
  }
  if ( ! m_outputs.empty()){
  for ( ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
  L0Muon::Register* reg = (*ir).second;
    reg->reset();    
  }
  }
  
}  

void L0Muon::Unit::dumpRegisters() {

  std::map<std::string,L0Muon::Register*>::iterator ir;
  
  
  int ic = 1;
  std::cout << "Input Registers" << std::endl;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){
      std::cout << bits[i] << std::endl;
    }
    std::cout << std::endl;
    ic++;
  }
 
  ic = 1;
  std::cout << "Output Registers" << std::endl;
  for ( ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
   
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){
      std::cout << bits[i] << std::endl;    
    }
    std::cout << std::endl; 
    ic++; 
  }
}

void L0Muon::Unit::setDebugMode() {

  m_debug = true;
  if (m_debug) std::cout <<"*** "<< type() <<"::setDebugMode" << std::endl;
  // set debug level for all the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->setDebugMode();
    }
  } 
}

void L0Muon::Unit::initialize() {

  if (m_debug) std::cout <<"*** "<< type() <<"::initialize" << std::endl;
  // initialize the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->initialize();
    }
  } 
}

void L0Muon::Unit::preexecute() {

  if (m_debug) std::cout <<"*** "<< type() <<"::preexecute" << std::endl;
  // preexecute the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->preexecute();
    }
  } 
}

void L0Muon::Unit::execute() {
  if (m_debug) std::cout <<"*** "<< type() <<"::execute" << std::endl;
  // execute the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->execute();

    }
  }  
}

  
void L0Muon::Unit::postexecute() {
  if (m_debug) std::cout <<"*** "<< type() <<"::postexecute" << std::endl;
  // postexecute the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->postexecute();
    }
  } 
}

void L0Muon::Unit::finalize() {
  if (m_debug) std::cout <<"*** "<< type() <<"::finalize" << std::endl;
  // finalize the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->finalize();
    }
  } 
}

L0Muon::Unit* L0Muon::Unit::parentByType(std::string utype) {

  L0Muon::Unit* uparent = parent();
  if (uparent) {
    if (m_debug) std::cout << "parentByType: <"<<uparent->type() << ">\n";
    if ( uparent->type() != utype ) {
      uparent = uparent->parentByType(utype);
    }
  }    
  return uparent;
}

void L0Muon::Unit::dump(int offset) {

  std::string blanc(offset,' ');

  std::cout << blanc << "+--------- Unit -----------" << std::endl;
  std::cout << blanc << "| Unit name '" << m_name << "' of type " << type() << std::endl;
  if (m_parent) {
    std::cout << blanc << "| Parent name '" << m_parent->name() << "' of type " 
              << m_parent->type() << std::endl;
  } else {
    std::cout << blanc << "| No Parent" << std::endl;
  }
  
  if (m_units.size() > 0) {
    std::cout << blanc << "| Subunits:" << std::endl;  
    std::map<std::string,L0Muon::Unit*>::iterator isu;
    for ( isu = m_units.begin(); isu != m_units.end(); isu++) {
      std::cout << blanc << "| '" << (*isu).first << "' of type " << (*isu).second->type() << std::endl;
    }  
  }
  std::cout << blanc << "+--------------------------" << std::endl;    
}

void L0Muon::Unit::dumpUnitTree(int offset) {

  dump(offset);

  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->dumpUnitTree(offset+4);
    }
  } 
}
