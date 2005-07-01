#include "ProcessorKernel/Register.h"
#include <iostream>

L0Muon::Register::Register() {}

L0Muon::Register::Register(int bits, unsigned long pattern) : 
    m_bitset(bits,pattern) {}
    
L0Muon::Register::Register(int bits) :
    m_bitset(bits) {}   
    
L0Muon::Register::~Register() {}    

void L0Muon::Register::unset() { 

  m_set = false; 
  m_bitset.reset();
  
}   

void L0Muon::Register::set(const L0Muon::Register& r) {
   if (r.isSet()) {
     m_set = true; 
     m_bitset = r.m_bitset;
   }  
}


std::string L0Muon::Register::toXML(std::string tab){

  std::string xmlString=tab;

  char buf[8];
  std::string str;

  xmlString +="<"+XMLRegister+" ";
  xmlString +=" name = \""+m_name+"\" ";
  xmlString +=" type = \""+m_type+"\" ";
  sprintf(buf,"%d",size());
  str = buf;
  xmlString +=" size = \""+str+"\" ";
  xmlString +=" />";
  
  

  return xmlString;
}
  
std::ostream& L0Muon::operator<<(std::ostream ost,L0Muon::Register& r) {
  return (ost << r.getBitset());
}

