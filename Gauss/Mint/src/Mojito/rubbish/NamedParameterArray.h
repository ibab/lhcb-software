#ifndef NAMED_PARAMETER_ARRAY_HH
#define NAMED_PARAMETER_ARRAY_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include <string>
#include <vector>
#include <sstream>

#include "NamedParameterBase.h"

template<typename T>
class NamedParameterArray : public NamedParameterBase{

 public:
  NamedParameterArray(std::string name)
    : NamedParameterBase(name)
    {}
  virtual ~NamedParameterArray(){}

  virtual int size() const{
    return _valueArray.size();
  }
  virtual const T& getVal(int i=0) const{
    if(i < 0 || i >= (int) _valueArray.size()){
      std::cout << "ERROR IN NamedParameterArray::getVal: "
		<< " index: " << i << " out of range ["
		<< 0 << " , " << _valueArray.size()
		<< " ] - I will crash"
		<< endl;
      throw "array index out of bounds";
    }
    return _valueArray[i];
  }
  virtual void setVal(const T& val, int i=0){
    if(i < 0) return;
    if(i >= (int) _valueArray.size()){
      _valueArray.resize(i+1);
    }
    _valueArray[i] = val;
  }
  virtual bool setFromParsedLine(const ParsedParameterLine& line){
    const std::vector<std::string>& vsl = line.parsedStrings();
    if(vsl.size() < 2) return false; // first element is parameter name
    if(vsl[0] != _name){
      std::cout << "ERROR IN NamedParameterArray::setFromParsedLine"
		<< " wrong parameter name. My name is " << _name
		<< " the line's first element is " << vsl[0]
		<< endl;
      return false;
    }
    for(unsigned int i=1; i< vsl.size(); i++){
      stringstream strm(vsl[i]);
      T tmpVal;
      strm >> tmpVal;
      setVal(tmpVal, i-1);
    }
    return true;
  }
};

template<>
bool NamedParameterArray<std::string>::setFromParsedLine(const ParsedParameterLine& line){
  const std::vector<std::string>& vsl = line.parsedStrings();
  if(vsl.size() < 2) return false; // first element is parameter name
  if(vsl[0] != _name){
    std::cout << "ERROR IN NamedParameterArray<string>::setFromParsedLine"
	      << " wrong parameter name. My name is " << _name
	      << " the line's first element is " << vsl[0]
	      << endl;
    return false;
  }
  for(unsigned int i=1; i< vsl.size(); i++){
    setVal(vsl[i], i-1);
  }
  return true;
}


#endif
//
