// $Id: Register.h,v 1.5 2004-12-21 14:33:02 ltocco Exp $

#ifndef L0MUONKERNEL_REGISTER_H
#define L0MUONKERNEL_REGISTER_H     1

/** @class Register Register.h L0MuonKernel/Register.h

   Class representing a level-0 muon  trigger 
   abstract unit of data
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include <boost/dynamic_bitset.hpp>


namespace L0Muon {

class Link;
class Register  {

public:

  /// Default constructor 
  Register();

  /** Constructor
      
      @param bits : number of bits
   */
  Register(int bits);

  /** Constructor
      
      @param bits : number of bits
      @param pattern : bits pattern
   */
  Register(int bits, unsigned long pattern);

  /// Destructor
  virtual ~Register();
  
  /// Unset the register and reset the bits
  void unset();

  /// Set the register r
  void set (const L0Muon::Register& r);

  /// Set the bitset r
  void set (const boost::dynamic_bitset<>& r) { m_bitset = r; }

  /// Set the bits pattern
  void set (unsigned long pattern) { 
    m_bitset = boost::dynamic_bitset<>(size(),pattern); 
  }

  /// Return the number of bits
  int size() { return m_bitset.size(); }

  /// Reset the bitset
  void reset() {m_bitset.reset();}

  /** Return true if a bit is set

      @param ind :  bit position
   */
  bool test(int ind) { return m_bitset.test(ind); }

  /// Return true if the register is set
  bool isSet() { return m_set; }

  /// Set the name of the register
  void setName(std::string name) { m_name=name; }

  /// Return the name of the register
  std::string name() { return m_name; }

  /// Set the type of the register
  void setType(std::string str){m_type = str;}

  /// Return the type of the register
  std::string Type() { return m_type;}

  /// Return the bitset
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
