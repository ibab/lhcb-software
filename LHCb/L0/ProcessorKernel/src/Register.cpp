#include "ProcessorKernel/Register.h"
#include <iostream>

L0Muon::Register::Register() {}

L0Muon::Register::Register(int bits, unsigned long pattern) : 
    m_bitset(bits,pattern) {m_set = false;}
    
L0Muon::Register::Register(int bits) :
    m_bitset(bits) {m_set = false;}   
    
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

void L0Muon::Register::set (unsigned long pattern) { 
  m_bitset = boost::dynamic_bitset<>(size(),pattern); 
  m_set=true;
}

void L0Muon::Register::set (boost::dynamic_bitset<> bitpattern,int nbits, int shift) { 
  
  //bitpattern.resize(nbits,false);
  int newSize = m_bitset.size() > nbits+shift ?  m_bitset.size() : nbits+shift ;
  bitpattern.resize(newSize,false);
  if (m_bitset.size()<newSize) {
    m_bitset.resize(newSize,false);  
  }

  m_bitset|=bitpattern<<shift;

  m_set=true;
}

void L0Muon::Register::set (unsigned long pattern,int nbits, int shift) { 
  boost::dynamic_bitset<> bitpattern = boost::dynamic_bitset<>(nbits,pattern); 
  set(bitpattern,nbits,shift);
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
  xmlString +=" />\n";

  return xmlString;
}
  
std::ostream& L0Muon::operator<<(std::ostream ost,L0Muon::Register& r) {
  return (ost << r.getBitset());
}

