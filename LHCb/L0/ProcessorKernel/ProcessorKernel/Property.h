#ifndef PROPERTY_H
#define PROPERTY_H 1

#include <iostream>
#include <string>
#include <vector>

#include "Kernel/MuonTileID.h" 

//! Property contains the value of a L0MConf database field.
/**
    It defines operators to output the field value and to convert 
    it to a integer, a string, a MuonTileID or a L0MTileType. 
*/
namespace L0Muon {

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


  const std::string value() const {return m_value;}
  
  /** 
      Build vectors
  */
  const std::vector<int> getVectorOfInt() const;
  const std::vector<long> getVectorOfLong() const; 
  const std::vector<float> getVectorOfFloat() const; 
  const std::vector<double> getVectorOfDouble() const; 
  

   /**
     Convert to bool.
  */
  operator const bool() const {return (( atoi(m_value.c_str()) != 0 ) ? true : false );}

  /**
     Convert to int.
  */
  operator const int() const {return atoi(m_value.c_str());}

  /**
     Convert to long.
  */
  operator const long() const {return atol(m_value.c_str());}

  /**
     Convert to float.
  */
  operator const float() const {return float(atof(m_value.c_str()));}

  /**
     Convert to double.
  */
  operator const double() const {return atof(m_value.c_str());}

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
