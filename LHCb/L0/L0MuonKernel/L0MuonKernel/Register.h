// $Id: Register.h,v 1.1.1.1 2004-03-10 11:38:15 ltocco Exp $

#ifndef L0MUONKERNEL_REGISTER_H
#define L0MUONKERNEL_REGISTER_H     1

/** @class Register Register L0MuonKernel/Register.h

   Class representing a L0Muon Trigger abstract unit of data
   processing for hardware simulations
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include <boost/dynamic_bitset.hpp>


namespace L0Muon {

class L0Muon::Link;


class Register  {

public:
  Register();
  Register(int bits);
  Register(int bits, unsigned long pattern);
  virtual ~Register();
  
  void unset();
  void set (const L0Muon::Register& r);
  void set (const boost::dynamic_bitset<>& r) { m_bitset = r; }
  void set (unsigned long pattern) { 
    m_bitset = boost::dynamic_bitset<>(size(),pattern); 
  }
  int size() { return m_bitset.size(); }
  void reset() {m_bitset.reset();}
  bool test(int ind) { return m_bitset.test(ind); }
  bool isSet() { return m_set; }
  void setName(std::string name) { m_name=name; }
  std::string name() { return m_name; }
  void setType(std::string str){m_type = str;}
  std::string Type() { return m_type;}
  boost::dynamic_bitset<> getBitset() { return m_bitset; }
    
protected:

  boost::dynamic_bitset<> m_bitset;
  std::string m_name;    // register symbolic name
  bool m_set;            // flag to show if the register is set or not
  
  L0Muon::Link* m_wlink;  // write link
  L0Muon::Link* m_rlink;  // read  link 
  std::string m_type;
};

std::ostream& operator<<(std::ostream, L0Muon::Register& r);

};  // namespace L0Muon

#endif      // L0MUONKERNEL_REGISTER_H  
