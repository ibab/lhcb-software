// $Id: Register.h,v 1.3 2005-07-01 12:39:12 jucogan Exp $

#ifndef PROCESSORKERNEL_REGISTER_H
#define PROCESSORKERNEL_REGISTER_H     1

/** @class Register Register.h L0MuonKernel/Register.h

Class representing a level-0 muon  trigger 
abstract unit of data
   
@author  Andrei Tsaregorodtsev
@date  12 January 2002
*/ 

#include <boost/dynamic_bitset.hpp>


namespace L0Muon {

  /**
     The XML keywords.
  */
  static const std::string XMLRegisterFactory   = "RegisterFactory";
  static const std::string XMLRegisterAlias     = "RegisterAlias";
  static const std::string XMLRegister          = "Register";
  static const std::string XMLTileRegister      = "TileRegister";
  static const std::string XMLTile              = "Tile";

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
    void set (const boost::dynamic_bitset<>& r) { m_bitset = r; m_set=true; }

    /// Set the bits pattern
    void set (unsigned long pattern) { 
      m_bitset = boost::dynamic_bitset<>(size(),pattern); 
      m_set=true;
    }

    /// Return the number of bits
    int size() { return m_bitset.size(); }
  
    /// Test if the register is empty
    bool empty() { return m_bitset.none(); }

    /// Reset the bitset
    void reset() {m_bitset.reset();}

    /** Return true if a bit is set

    @param ind :  bit position
    */
    bool test(int ind) { return m_bitset.test(ind); }

    /// Return true if the register is set
    bool isSet() const { return m_set; }

    /// Set the name of the register
    void setName(std::string name) { m_name=name; }

    /// Return the name of the register
    std::string name() { return m_name; }

    /// Set the type of the register
    void setType(std::string str){m_type = str;}

    /// Return the type of the register
    std::string type() { return m_type;}

    /// Return the bitset
    boost::dynamic_bitset<> getBitset() { return m_bitset; }

    virtual std::string toXML(std::string tab="");
    
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

#endif      // PROCESSORKERNEL_REGISTER_H  
