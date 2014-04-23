#ifndef PERMUTATION_HH
#define PERMUTATION_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include <vector>
#include <iostream>

/*
  naming: is the 'normal' ordering, i.e. 1, 2, 3, 4,...
  whyle y is what it's been mapped to, e.g.
  x = 1, 2, 3, 4, 5
  y = 2, 1, 3, 4, 5

*/

class Permutation{
 protected:
  // begin data members

  std::vector<int> _v;
  int _sign;

  // end data members

  static Permutation* __unit;
  

  static int dummy;
  int& y_of_x(int i);
  const int& y_of_x(int i) const;

 public:

  static const Permutation& unity();

  void swap(int x1, int x2, int sgn=1);
  void set(const std::vector<int>& v, int sgn=1);
  void set(int a);
  void set(int a, int b);
  void set(int a, int b, int c);
  void set(int a, int b, int c, int d);
  void set(int a, int b, int c, int d, int e);
  void setSign(int s);

  void reset(int n);
  void makeUnity();
  bool isUnity() const;

  Permutation getInverse() const;

  unsigned int size() const{ return _v.size();}

  Permutation(int n=1) : _v(n), _sign(1){reset(n);};
  Permutation(const Permutation& other) 
    : _v(other._v), _sign(other._sign)
    {}

  int sign() const{return _sign;}

  int operator[] (int i) const{
    return y_of_x(i);
  }

  int& operator[] (int i){
    return y_of_x(i);
  }

  std::vector<int>& mapValues(const std::vector<int>& in
			      , std::vector<int>& out) const{
    // same as mapValues below, except no new vector needs allocating
    // good when time is precious...
    if(this->isUnity()){
      out = in;
      return out;
    }

    out.resize(in.size());
    for(unsigned int i=0; i < in.size(); i++){
      out[i] = y_of_x(in[i]);
    }
    return out;
  }
  std::vector<int> mapValues(const std::vector<int>& in) const{
    std::vector<int> out(in.size());
    return mapValues(in, out);
  }

  template<typename T>
    std::vector<T>& mapOrder(const std::vector<T>& in
			    , std::vector<T>& out) const{
    out.resize(in.size());
    unsigned int to = in.size();
    if(in.size() > this->size()){
      to = this->size();
      for(unsigned int i = this->size(); i < in.size(); i++) out[i] = in[i];
    }
    
    for(unsigned int i=0; i < to; i++){
      out[y_of_x(i)] = in[i];
    }
    return out;    
  }
  template<typename T>
    std::vector<T> mapOrder(const std::vector<T>& in) const{
    std::vector<T> out(in.size());
    return mapOrder(in, out);
  }

  int nPermutations() const;

  void print(std::ostream& os = std::cout) const;

  Permutation operator*(const Permutation& rhs) const;
  Permutation operator/(const Permutation& rhs) const;
  
  bool operator<(const Permutation& rhs)const;
  bool operator==(const Permutation& rhs)const;
  bool operator<=(const Permutation& rhs)const;
  bool operator>(const Permutation& rhs)const;
  bool operator>=(const Permutation& rhs)const;
};

std::ostream& operator<<(std::ostream& os, const Permutation& p);

#endif
