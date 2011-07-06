#ifndef NAMED_SINGLE_PARAMETER_HH
#define NAMED_SINGLE_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include <string>
#include <vector>
#include <sstream>

#include "NamedParameterBase.h"

template<typename T>
class NamedSingleParameter : public NamedParameterBase{

  T _value;

 public:
  virtual const T& getVal() const{
    return _value;
  }
  virtual void setVal(const T& val){
    _value = val;
  }
  NamedSingleParameter(std::string name)
    : NamedParameterBase(name)
    {}
  virtual ~NamedSingleParameter(){}

  virtual bool setFromLine(const std::string& line){
    std::vector<std::string> vsl = NamedParameterBase::makeValueStrings(line);
    if(vsl.empty()) return false;
    if(vsl.size() > 1){
      std::cout << "WARNING IN: NamedSingleParameter "
		<< " with name: " << _name << ":\n"
		<< " Your parameter string seems to be"
		<< " setting more than one parameter!\n"
		<< " the string is:"
		<< " >" << line << "<"
		<< " \n I'll proceed and use only the first"
		<< " parameter, which is " << vsl[0] << endl;
    }
    stringstream strm(vsl[0]);
    strm >> _value;
    return true;
  }
};

#endif
//
