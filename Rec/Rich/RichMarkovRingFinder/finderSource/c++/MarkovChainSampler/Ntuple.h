#ifndef LESTER_NTUPLE_H
#define LESTER_NTUPLE_H

#include <iostream>
#include <vector>

namespace Lester {

template <int dim, class DataType>
class Ntuple {
public:
  class BadConstructorChoice {
  public:
    BadConstructorChoice(const int _argsAttemptedToUse) :
      argsAttemptedToUse(_argsAttemptedToUse),
      dims(dim) {
    };
    int argsAttemptedToUse;
    int dims;
  };
public:
  std::vector<DataType> vec;
  inline int size() const {
    //std::cerr << "Size = "<<vec.size()<<std::endl;
    assert(vec.size()==dim);
    return dim;
  };
  Ntuple() :
    vec(dim) {
    // Deliberately left empty!!
  };
  Ntuple(const DataType a) :
    vec(dim) {
    if (dim>=1) {
      vec[0]=a;
    } else {
      throw BadConstructorChoice(1);
    };
  };
  Ntuple(const DataType a,
	const DataType b) :
    vec(dim) {
    if (dim>=2) {
      vec[0]=a;
      vec[1]=b;
    } else {
      throw BadConstructorChoice(2);
    };
  };
  Ntuple(const DataType a,
	const DataType b,
	const DataType c) :
    vec(dim) {
    if (dim>=3) {
      vec[0]=a;
      vec[1]=b;
      vec[2]=c; 
    } else {
      throw BadConstructorChoice(3);
    };
  };
  Ntuple(const std::vector<DataType> & otherVec) {
    if (otherVec.size()!=dim) {
      throw BadConstructorChoice(otherVec.size());
    };
    vec=otherVec;
  };
  DataType & operator[] (const int n) { return vec[n]; };
  const DataType & operator[] (const int n) const { return vec[n]; };
  Ntuple<dim, DataType> operator+(const Ntuple<dim, DataType> & other) const {
    assert(vec.size()==other.vec.size());
    //{
    //  std::cout << "Error!  Vector size mismatch when adding "<<*this<<" to "<<other<<"!!!" <<std::endl;
    //  exit(2);
    //};
    std::vector<DataType> ret;
    typename std::vector<DataType>::const_iterator ita;
    typename std::vector<DataType>::const_iterator itb;
    for (
	 ita=vec.begin(),
	   itb=other.vec.begin();
	 ita!=vec.end() && 
	   itb!=other.vec.end();
	 ita++,
	   itb++
	 ) {
      ret.push_back((*ita)+(*itb));
    };
    return Ntuple(ret);
  };
  Ntuple<dim, DataType> operator-(const Ntuple<dim, DataType> & other) const {
    assert(vec.size()==other.vec.size());
    //{
    //  std::cout << "Error!  Vector size mismatch when adding "<<*this<<" to "<<other<<"!!!" <<std::endl;
    //  exit(2);
    //};
    std::vector<DataType> ret;
    typename std::vector<DataType>::const_iterator ita;
    typename std::vector<DataType>::const_iterator itb;
    for (
	 ita=vec.begin(),
	   itb=other.vec.begin();
	 ita!=vec.end() && 
	   itb!=other.vec.end();
	 ita++,
	   itb++
	 ) {
      ret.push_back((*ita)-(*itb));
    };
    return Ntuple(ret);
  };
  //  Ntuple<dim, DataType> & operator+=(const Ntuple<dim, DataType> & other) const {
  //  operator+(other);
  //  return *this;
  //};
  DataType dot(const Ntuple<dim, DataType> & other) const {
    assert(vec.size()==other.vec.size());
    DataType ans=DataType();
    //if (ans!=0) {
    //  std::cout << " JKJJ "<<ans<<std::endl;
    //};
    typename std::vector<DataType>::const_iterator ita;
    typename std::vector<DataType>::const_iterator itb;
    for (
	 ita=vec.begin(),
	   itb=other.vec.begin();
	 ita!=vec.end() && 
	   itb!=other.vec.end();
	 ita++,
	   itb++
	 ) {
      ans+=((*ita)*(*itb));
    };
    return ans;
  };
  DataType modSq() const {
    return dot(*this);
  };
  DataType mod() const {
    return sqrt(modSq());
  };
  Ntuple<dim, DataType> operator*(const double scale) const {
    std::vector<DataType> ret;
    for (
	typename  std::vector<DataType>::const_iterator ita=vec.begin();
	 ita!=vec.end();
	 ita++) {
      ret.push_back((*ita)*scale);
    };
    return Ntuple(ret);
  };
};

};


template<class T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & ntuple) {
  for (typename  std::vector<T>::const_iterator it=ntuple.begin();
       it!=ntuple.end();
       it++) {
    if ( it!=ntuple.begin()) {
      os << " ";
    }; 
    os << (*it);
  };
  return os;
};

template<int dim, class DataType>
std::ostream & operator<<(std::ostream & os, const Lester::Ntuple<dim, DataType> & ntuple) {
  for (typename  std::vector<DataType>::const_iterator it=ntuple.vec.begin();
       it!=ntuple.vec.end();
       it++) {
    if ( it!=ntuple.vec.begin()) {
      os << " ";
    }; 
    os << (*it);
  };
  return os;
};

template<int dim, class DataType>
std::istream & operator>>(std::istream & is, Lester::Ntuple<dim, DataType> & ntuple) {
  for (typename  std::vector<DataType>::iterator it=ntuple.vec.begin();
       it!=ntuple.vec.end();
       it++) {

    is >> (*it);

  };
  return is;
};


#endif
