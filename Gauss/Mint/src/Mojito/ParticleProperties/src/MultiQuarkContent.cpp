#include "Mint/MultiQuarkContent.h"

using namespace std;

MultiQuarkContent::MultiQuarkContent()
  : vector<QuarkContent>(1)
{
}
MultiQuarkContent::MultiQuarkContent(const MultiQuarkContent& other)
  : vector<QuarkContent>(other)
{
}

void MultiQuarkContent::antiThis(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i].antiThis();
  }
}

bool MultiQuarkContent::initFromString(const string& str){
  unsigned int pos =str.find("non-qQ");
  if(pos < str.size())return true;
  
  unsigned int posP = str.find("p(");
  unsigned int posQ = str.find("q(");

  bool sc=true;
  if(posP >= str.size() || posQ > str.size()){
    sc &= (*this)[0].initFromString(str);
  }else{
    this->resize(2);
    sc &= (*this)[0].initFromString(str.substr(posP, posQ));
    sc &= (*this)[1].initFromString(str.substr(posQ, str.size()));
  }
  return sc;
}

bool MultiQuarkContent::compatible(const MultiQuarkContent& other) const{
  for(unsigned int i=0; i < this->size(); i++){
    for(unsigned int j=0; j < other.size(); j++){
      if( (*this)[i] == other[j] ) return true;
    }
  }
  return false;

}
void MultiQuarkContent::print(std::ostream& os) const{
  os << "[";
  for(unsigned int i=0; i < this->size(); i++){
    if(i > 0 && i+1 < this->size()) os << ", ";
    (*this)[i].print(os);
  }
  os << "]";
}

MultiQuarkContent& MultiQuarkContent::operator+=(const MultiQuarkContent& rhs){
  if(this->size()*rhs.size() <= 1){
    for(unsigned int i=0; i < this->size(); i++){
      (*this)[i] += rhs[i];
    }
  }else{
    MultiQuarkContent oldThis(*this);
    this->clear();
    for(unsigned int i=0; i < oldThis.size(); i++){
      for(unsigned int j=0; j < rhs.size(); j++){
	this->push_back(oldThis[i] + rhs[j]);
      }
    }
  }
  return *this;
}
MultiQuarkContent& MultiQuarkContent::operator-=(const MultiQuarkContent& rhs){
  if(this->size()*rhs.size() <= 1){
    for(unsigned int i=0; i < this->size(); i++){
      (*this)[i] -= rhs[i];
    }
  }else{
    MultiQuarkContent oldThis(*this);
    this->clear();
    for(unsigned int i=0; i < oldThis.size(); i++){
      for(unsigned int j=0; j < rhs.size(); j++){
	this->push_back(oldThis[i] - rhs[j]);
      }
    }
  }
  return *this;
}

MultiQuarkContent MultiQuarkContent::operator+(const MultiQuarkContent& rhs) const{
  MultiQuarkContent returnVal(*this);  
  returnVal += rhs;
  return returnVal;
}
MultiQuarkContent MultiQuarkContent::operator-(const MultiQuarkContent& rhs) const{
  MultiQuarkContent returnVal(*this);  
  returnVal -= rhs;
  return returnVal;
}
std::ostream& operator<<(std::ostream& st, const MultiQuarkContent& qc){
  qc.print(st);
  return st;
}
//
