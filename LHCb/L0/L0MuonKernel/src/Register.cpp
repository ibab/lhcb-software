#include "L0MuonKernel/Register.h"

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
   m_bitset = r.m_bitset;
}

  
std::ostream& L0Muon::operator<<(std::ostream ost,L0Muon::Register& r) {
  return (ost << r.getBitset());
}

