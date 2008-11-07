#ifndef PROPERTY_H
#define PROPERTY_H 1

#include <iostream>
#include <string>
#include <vector>

#include "Kernel/MuonTileID.h" 

namespace L0Muon {

  /** @class Property Property.h ProcessorKernel/Property.h
      
      Class holding a property of a L0MuonKernel Unit.
      
      It stores a property as a string and defines operators 
      to convert it to a integer, a string, a MuonTileID or a L0MTileType. 
      
      @author Julien Cogan
      @date 2005-07-01
  */
  
  class Property{
  private:
    std::string m_value;
    
  public:
    
    /**
       Construct an empty Property.
    */
    Property():m_value("Undefined"){}
    
    /**
       Construct an Property from a string.
    */
    Property(const std::string &value):m_value(value){}
    
    /**
     Destruct the Property.
    */
    ~Property(){};
    
    /// Returns the property value (string)
    const std::string value() const {return m_value;}
  
    /// Build vector of int
    const std::vector<int> getVectorOfInt() const;

    /// Build vector of long
    const std::vector<long> getVectorOfLong() const; 

    /// Build vector of float
    const std::vector<float> getVectorOfFloat() const; 

    /// Build vector of double
    const std::vector<double> getVectorOfDouble() const; 
    
    /**
       Convert to bool.
    */
    operator bool() const {return (( atoi(m_value.c_str()) != 0 ) ? true : false );}
    
    /**
       Convert to int.
    */
    operator int() const {return atoi(m_value.c_str());}
    
    /**
       Convert to long.
    */
    operator long() const {return atol(m_value.c_str());}
    
    /**
       Convert to float.
    */
    operator float() const {return float(atof(m_value.c_str()));}
    
    /**
       Convert to double.
    */
    operator double() const {return atof(m_value.c_str());}
    
    /**
     Convert to vector of int.
    */
    operator const std::vector<int>() const {return getVectorOfInt();}
    
    /**
       Convert to vector of int.
    */
    operator const std::vector<long>() const {return getVectorOfLong();}
    
    /**
       Convert to vector of float.
    */
    operator const std::vector<float>() const {return getVectorOfFloat();}
    
    /**
       Convert to vector of double.
    */
    operator const std::vector<double>() const {return getVectorOfDouble();}
    
    /**
       Convert to char.
    */
    //operator const char*() const {return m_value.c_str();}
    
    /**
       Convert to string.
    */
    operator const std::string() const {return m_value;}
    
    /**
       Convert to MuonTileID.
    */
    operator const LHCb::MuonTileID() const {return LHCb::MuonTileID(atol(m_value.c_str()));}
    
    
  };
  
  /**
     Define ouput operator.
     Output the internal string.
  */
  std::ostream& operator<< (std::ostream& os, const Property  &fv);
};  // namespace L0Muon

#endif // PROPERTY
