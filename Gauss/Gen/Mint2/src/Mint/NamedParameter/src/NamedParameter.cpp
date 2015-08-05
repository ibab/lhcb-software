// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/NamedParameter.h"

using namespace MINT;
/*
  not much to do - it's a template
  and it likes to live in the header.
  Specialiations go here, though.
*/

template<>
MINT::NamedParameter<std::string>& 
MINT::NamedParameter<std::string>::operator-=(const std::string& rhs){
  std::cout << "WARNING in MINT::NamedParameter<std::string>::operator-=:"
	    << "\n\t OPERATOR \"-=\" undefined for strings" 
	    << "\n\t you tried: " << (std::string)(*this)
	    << " -= " << rhs
	    << std::endl;
  return *this;
}

template<>
MINT::NamedParameter<std::string>& 
MINT::NamedParameter<std::string>::operator*=(const std::string& rhs){
  std::cout << "WARNING in MINT::NamedParameter<std::string>::operator-=:"
	    << "\n\t OPERATOR \"*=\" undefined for strings" 
	    << "\n\t you tried: " << (std::string)(*this)
	    << " *= " << rhs
	    << std::endl;
  return *this;
}

template<>
MINT::NamedParameter<std::string>& 
MINT::NamedParameter<std::string>::operator/=(const std::string& rhs){
  std::cout << "WARNING in MINT::NamedParameter<std::string>::operator-=:"
	    << "\n\t OPERATOR \"/=\" undefined for strings" 
	    << "\n\t you tried: " << (std::string)(*this)
	    << " /= " << rhs
	    << std::endl;
  return *this;
}


template<>
bool MINT::NamedParameter<std::string>::setFromParsedLine(const ParsedParameterLine& line){
  const std::vector<std::string>& vsl = line.parsedStrings();
  if(vsl.size() < 2) return false; // first element is parameter name
  if(vsl[0] != _name){
    std::cout << "ERROR IN NamedParameter<string>::setFromParsedLine"
	      << " wrong parameter name. My name is " << _name
	      << " the line's first element is " << vsl[0]
	      << std::endl;
    return false;
  }
  for(unsigned int i=1; i< vsl.size(); i++){
    setVal(vsl[i], i-1);
  }
  return true;
}

template<>
void MINT::NamedParameter<std::string>::print(std::ostream& os) const{
  os << "\"" << name() << "\" ";
  if(size()==0){
    return;
  }else{
    for(int i=0; i< size(); i++){
      os << "\"" << getVal(i) << "\"  ";
      if(i != size()) os << "  ";
    }
  }
};

template class MINT::NamedParameter<std::string>;
//
