#ifndef NAMED_PARAMETER_HH
#define NAMED_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include <string>
#include <vector>
#include <iostream>

#include "Mint/NamedParameterBase.h"
#include "Mint/NamedParameterStream.h"

namespace MINT{

template<typename T>
class NamedParameter : public NamedParameterBase{
 protected:
  std::vector<T> _valueArray;
  
  virtual bool setFromParsedLine(const ParsedParameterLine& line){
    _valueArray.clear();
    _valueArray.resize(1);
    const std::vector<std::string>& vsl = line.parsedStrings();
    if(vsl.size() < 2) return false; // first element is parameter name
    if(vsl[0] != _name){
      std::cout << "ERROR in NamedParameter::setFromParsedLine"
		<< " wrong parameter name. My name is " << _name
		<< " the line's first element is " << vsl[0]
		<< std::endl;
      return false;
    }
    for(unsigned int i=1; i< vsl.size(); i++){
      //      std::stringstream strm(vsl[i]);
      NamedParameterStream strm(vsl[i]);
      T tmpVal;
      strm >> tmpVal;
      setVal(tmpVal, i-1);
    }
    return true;
  }

 public:
  NamedParameter(const std::string& name
		 , NamedParameterBase::VERBOSITY vb
		 )
    : NamedParameterBase(name, (const char*)0, vb)
    {  
      setFromParsedFile();
    }
  NamedParameter(const std::string& name
		 , const char* fname=0
		 , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		 )
    : NamedParameterBase(name, fname, vb)
    {  
      setFromParsedFile();
    }
  NamedParameter(const std::string& name
		 , const T& def
		 , const char* fname=0
		 , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		 )
    : NamedParameterBase(name, fname, vb)
    {
      setVal(def);
      _gotInitialised = true;
      setFromParsedFile();
    }
  NamedParameter(const std::string& name
		 , const T& v0, const T& v1
		 , const char* fname=0
		 , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		 )
    : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		 , const char* fname=0		 
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1); setVal(v2,2);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		  , const T& v3
		 , const char* fname=0
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1); setVal(v2,2); setVal(v3,3);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		  , const T& v3, const T& v4
		  , const char* fname=0
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1); setVal(v2,2); setVal(v3,3); setVal(v4,4);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		  , const T& v3, const T& v4, const T& v5
		  , const char* fname=0
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {
      setVal(v0, 0); setVal(v1,1); setVal(v2,2); setVal(v3,3); setVal(v4,4); 
      setVal(v5,5);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		  , const T& v3, const T& v4, const T& v5
		  , const T& v6
		  , const char* fname=0
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1); setVal(v2,2); setVal(v3,3); 
      setVal(v4,4); setVal(v5,5); setVal(v6,6);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		  , const T& v3, const T& v4, const T& v5
		  , const T& v6, const T& v7
		  , const char* fname=0
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1); setVal(v2,2); setVal(v3,3); 
      setVal(v4,4); setVal(v5,5); setVal(v6,6); setVal(v7,7);
      _gotInitialised = true;
      setFromParsedFile();
    }
   NamedParameter(const std::string& name
		  , const T& v0, const T& v1, const T& v2
		  , const T& v3, const T& v4, const T& v5
		  , const T& v6, const T& v7, const T& v8
		  , const char* fname=0
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  )
     : NamedParameterBase(name, fname, vb)
    {      
      setVal(v0, 0); setVal(v1,1); setVal(v2,2); setVal(v3,3); 
      setVal(v4,4); setVal(v5,5); setVal(v6,6); setVal(v7,7);
      setVal(v8,8);
      _gotInitialised = true;
      setFromParsedFile();
    }
  NamedParameter(const std::string& name
		 , const std::vector<T> & defVec
		 , const char* fname=0
		 , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		 )
    : NamedParameterBase(name, fname, vb)
    {
      setVal(defVec);
      _gotInitialised = true;
      setFromParsedFile();
    }
  NamedParameter(const NamedParameter<T>& other)
    : INamedParameter()
    , NamedParameterBase(other)
    , _valueArray(other._valueArray)
    {  
    }

  virtual ~NamedParameter(){}

  virtual int size() const{
    return _valueArray.size();
  }
  virtual const T& getVal(int i=0) const{
    if(i < 0 || i >= (int) _valueArray.size()){
      std::cout << "FATAL ERROR in NamedParameter::getVal: "
		<< " for parameter name \"" << name()
		<< "\". Index: " << i << " out of range [ "
		<< 0 << " , " << _valueArray.size()
		<< " [ - I will crash."
		<< "\n Maybe the parameter was not initialised?"
		<< "\n Even non-array NamedParameters are stored"
		<< " in an array - in this case of size 1."
		<< " Uninitialised this can end up being 0."
		<< std::endl;
      throw "array index out of bounds";
    }
    return _valueArray[i];
  }
  operator T() const{
    return getVal();
    // meaningful for parameters that aren't arrays
    // otherwise: you'll get the first.
  }

  const std::vector<T>& getVector() const{
    return _valueArray;
  }

  virtual void setVal(const T& val, int i=0){
    if(i < 0) return;
    if(i >= (int) _valueArray.size()){
      _valueArray.resize(i+1);
    }
    _valueArray[i]  = val;
    _gotInitialised = true;
  }
  virtual void setVal(const std::vector<T>& valList){
    _valueArray     = valList;
    _gotInitialised = true;
  }
  virtual void print(std::ostream& os = std::cout) const{
    os << "\"" << name() << "\" ";
    if(size()==0){
      return;
    }else{
      for(int i=0; i< size(); i++){
	os << getVal(i);
	if(i != size()) os << " ";
      }
    }
  }
  operator std::vector<T>()const {return getVector();}
  NamedParameter<T>& operator=(const T& d){
    setVal(d, 0);
    return *this;
  }
  NamedParameter<T>& operator=(const std::vector<T>& v){
    setVal(v);
    return *this;
  }
  NamedParameter<T>& operator+=(const T& rhs){
    _valueArray[0] += rhs;
    return *this;
  }
  NamedParameter<T>& operator-=(const T& rhs){
    _valueArray[0] -= rhs;
    return *this;
  }
  NamedParameter<T>& operator*=(const T& rhs){
    for(unsigned int i=0; i < _valueArray.size(); i++) _valueArray[i] *= rhs;
    return *this;
  }
  NamedParameter<T>& operator/=(const T& rhs){
    for(unsigned int i=0; i < _valueArray.size(); i++) _valueArray[i] /= rhs;
    return *this;
  }

};

  // specialisations for strings:
template<>
  bool MINT::NamedParameter<std::string>::setFromParsedLine(const ParsedParameterLine& line);

template<>
  void MINT::NamedParameter<std::string>::print(std::ostream& os) const;

 template<>
   MINT::NamedParameter<std::string>& 
   MINT::NamedParameter<std::string>::operator-=(const std::string& rhs);
 template<>
   MINT::NamedParameter<std::string>& 
   MINT::NamedParameter<std::string>::operator*=(const std::string& rhs);
 template<>
   MINT::NamedParameter<std::string>& 
   MINT::NamedParameter<std::string>::operator/=(const std::string& rhs);
}//namespace MINT

template<typename T> 
std::ostream& operator<<(std::ostream& os, const MINT::NamedParameter<T>& np){
  np.print(os);
  return os;
}

#endif
//
