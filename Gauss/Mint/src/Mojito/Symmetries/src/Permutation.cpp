// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#include "Mint/Permutation.h"

using namespace std;

int Permutation::dummy = -9999;
Permutation* Permutation::__unit(0);

const Permutation& Permutation::unity(){
  if(0 == __unit){
    __unit = new Permutation();
    __unit->makeUnity();
  }
  return *__unit;
}


int& Permutation::y_of_x(int i){
  if(i >= (int) _v.size() || i < 0){
    cout << "Permutation::y_of_x for x = " << i
	 << " out of range [0," << (_v.size()-1) << "]." 
	 << endl;
    return dummy;
  }
  return _v[i];
}
const int& Permutation::y_of_x(int i) const{
  if(i >= (int) _v.size() || i < 0){
    cout << "Permutation::y_of_x for x = " << i
	 << " out of range [0," << (_v.size()-1) << "]." 
	 << endl;
    return dummy;
  }
  return _v[i];
}

void Permutation::reset(int n){
  _v.resize(n);
  makeUnity();
}
void Permutation::makeUnity(){
  for(unsigned int i=0; i < size(); i++){
    _v[i] = i;
  }
}
bool Permutation::isUnity() const{
  for(unsigned int i=0; i < size(); i++){
    if (_v[i] != (int) i) return false;
  }
  return true;
}

void Permutation::swap(int x1, int x2, int sgn){
  int tmp = y_of_x(x1);
  y_of_x(x1) = y_of_x(x2);
  y_of_x(x2) = tmp;
  _sign *= sgn;
}

void Permutation::set(const std::vector<int>& v, int sgn){
  _v=v;
  _sign =sgn;
}

void Permutation::set(int a){
  _v.resize(1); (*this)[0]=a;
}
void Permutation::set(int a, int b){
  _v.resize(2); (*this)[0]=a; (*this)[1]=b;
}
void Permutation::set(int a, int b, int c){
  _v.resize(3); (*this)[0]=a; (*this)[1]=b; (*this)[2]=c;
}
void Permutation::set(int a, int b, int c, int d){
  _v.resize(4); (*this)[0]=a; (*this)[1]=b; (*this)[2]=c; (*this)[3]=d;
}
void Permutation::set(int a, int b, int c, int d, int e){
  _v.resize(4); (*this)[0]=a; (*this)[1]=b; (*this)[2]=c; (*this)[3]=d; (*this)[4]=e;
}
void Permutation::setSign(int s){
  _sign = s;
}
Permutation Permutation::getInverse() const{
  Permutation newP(*this);
  for(unsigned int i=0; i< this->size(); i++){
    int newIndex = (*this)[i];
    if(newIndex < 0 || newIndex >= (int) this->size()){
      cout << "ERROR in  Permutation::getInverse(): "
	   << " found that I am an invalid permutation when trying to"
	   << " invert myself. This is me:\n" << *this << endl;
      cout << "  Will carry on as best as I can..." <<endl;
      continue;
    }
    newP[newIndex] = i;
  }
  return newP;
}

int Permutation::nPermutations() const{
  int n=0;
  for(unsigned int i=0; i< size()-1; ++i){
    for(unsigned int j=i+1; j< size(); j++){
      if( y_of_x(i) > y_of_x(j) ) ++n;
    }
  }
  return n;
}

void Permutation::print(std::ostream& os) const{

  for(unsigned int i=0; i < size(); i++){
    os << '\t' << i;
  }
  os << "\n ->";
  for(unsigned int i=0; i < size(); i++){
    os << '\t' << (*this)[i];
  }

}

Permutation Permutation::operator*(const Permutation& rhs) const{
  unsigned int newSize = this->size();
  if(rhs.size() > newSize) newSize = rhs.size();
  Permutation newP(newSize);

  for(unsigned int i=0; i < newSize; i++){
    int rhsMapping = -1;
    if(i >= rhs.size()){
      rhsMapping = i;
    }else{
      rhsMapping = rhs[i];
    }
    if(rhsMapping > (int) this->size() || rhsMapping < 0){
      newP[i] = rhsMapping;
    }else{
      newP[i] = (*this)[rhsMapping];
    }
  }
  return newP;
}

Permutation Permutation::operator/(const Permutation& rhs) const{
  return (*this)* rhs.getInverse();
}

bool Permutation::operator<(const Permutation& rhs) const{

  if(this->size() < rhs.size()) return true;
  if(this->size() > rhs.size()) return false;

  for(unsigned int i=0; i< this->size(); i++){
    if( (*this)[i] < rhs[i] ) return true;
    if( (*this)[i] > rhs[i] ) return false;
  }
  return false;
}

bool Permutation::operator==(const Permutation& rhs) const{

  if(this->size() != rhs.size()) return false;

  for(unsigned int i=0; i< this->size(); i++){
    if( (*this)[i] != rhs[i] ) return false;
  }
  return true;
}
bool Permutation::operator<=(const Permutation& rhs) const{
  return ( (*this) < rhs || (*this) == rhs);
}
bool Permutation::operator>(const Permutation& rhs) const{
  return ! ((*this) <= rhs);
}
bool Permutation::operator>=(const Permutation& rhs) const{
  return ! ((*this) < rhs);
}

std::ostream& operator<<(std::ostream& os, const Permutation& p){
  p.print(os);
  return os;
}

//
