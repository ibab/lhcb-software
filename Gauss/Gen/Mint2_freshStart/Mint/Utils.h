#ifndef UHTILS_HH
#define UHTILS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <iostream>
#include "TLorentzVector.h"
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "TMatrixT.h"
#include "TMatrixTSym.h"

namespace MINT{

enum OWNERSHIP{CALLER_PASSES_OWNERSHIP, CALLER_KEEPS_OWNERSHIP};

template <typename Key, typename Val> 
  const Val &  keyFinder( const Key& k
                          , const std::map< Key, Val > & m
                          , const Val& dummy
			  , bool& successFlag
                          ){
  typename std::map< Key , Val >::const_iterator it = m.find(k);
  if(it == m.end()){
    successFlag = false;
    return dummy;
  }else{
    successFlag = true;
    return it->second;
  }
}

template <typename Key, typename Val> 
  const Val &  keyFinder( const Key& k
                          , const std::map< Key, Val > & m
                          , const Val& dummy
                          ){
  bool successFlag;
  return keyFinder<Key, Val>(k, m, dummy, successFlag);
}
 
template<typename T>
TMatrixTSym<T> makeTMatrixTSym(const TMatrixT<T>& m){
  TMatrixTSym<T> ms(m.GetNcols());
  for(int i=0; i < ms.GetNcols(); i++){
    for(int j=i; j < ms.GetNcols(); j++){    
      ms(i,j) = ms(j,i) = m(i,j);
    }
  }
  return ms;
}
  
template<typename T>
void stringToAnything(const std::string& str, T& anything){
  std::stringstream strm(str);
  strm >> anything;
}

template<typename T>
std::string anythingToString(const T& anything){
  std::stringstream strm;
  strm << anything;
  std::string ing;
  strm >> ing;
  return ing;
}

template<typename T>
std::string anyVectorToString(const std::vector<T>& anyV){
  std::stringstream strm;
  for(unsigned int i=0; i<anyV.size(); i++){
    strm << anyV[i];
  }
  std::string ing;
  strm >> ing;
  return ing;
}

std::string stringtime(double dt);

int nearestInt(double f);

bool A_is_in_B(const std::string& a, const std::string& b);

// need to move these to Dalitz Fitter
int LeviCita(int a, int b);
int LeviCita(int a, int b, int c);
int LeviCita(int a, int b, int c, int d);
int LeviCita(int a, int b, int c, int d, int e);
int LeviCita(const std::vector<int>& v);

} // namespace MINT

std::ostream& operator<<(std::ostream& os, const TLorentzVector& v);

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
  os << "(";
  for(unsigned int i=0; i<v.size(); i++){
    os << v[i];
    if(i + 1 < v.size()) os << ", ";
  }
  os << ")";
  return os;
}

#endif
//
