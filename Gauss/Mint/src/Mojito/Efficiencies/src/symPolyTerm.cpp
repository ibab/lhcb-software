#include "Mint/symPolyTerm.h"
#include "Mint/Utils.h"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace MINT;

symPolyTerm::symPolyTerm(const std::vector<int>& pos)
  : _powers(pos)
{
  init();
}
symPolyTerm::symPolyTerm(int a)
  : _powers(1)
{
  _powers[0] = a;
  init();
}
symPolyTerm::symPolyTerm(int a, int b)
  : _powers(2)
{
  _powers[0] = a;
  _powers[1] = b;
  init();
}
symPolyTerm::symPolyTerm(int a, int b, int c)
  : _powers(3)
{
  _powers[0] = a;
  _powers[1] = b;
  _powers[2] = c;
  init();
}
symPolyTerm::symPolyTerm(int a, int b, int c, int d)
  : _powers(4)
{
  _powers[0] = a;
  _powers[1] = b;
  _powers[2] = c;
  _powers[3] = d;
 init();
}
symPolyTerm::symPolyTerm(int a, int b, int c, int d, int e)
  : _powers(4)
{
  _powers[0] = a;
  _powers[1] = b;
  _powers[2] = c;
  _powers[3] = d;
  _powers[4] = e;
  init();
}
symPolyTerm::symPolyTerm(int a, int b, int c, int d, int e, int f)
  : _powers(4)
{
  _powers[0] = a;
  _powers[1] = b;
  _powers[2] = c;
  _powers[3] = d;
  _powers[4] = e;
  _powers[5] = f;
  init();
}

symPolyTerm::symPolyTerm(const symPolyTerm& other)
  : _powers(other._powers)
  , _allPermutations(other._allPermutations)
  , _name(other._name)
{
}

bool symPolyTerm::init(){
  makeName();
  makeAllPermutations();
  return true;
}

void symPolyTerm::makeName(){
  _name = "";
  for(unsigned int i=0; i < _powers.size(); i++){
    _name += anythingToString(_powers[i]);
  }
}

void symPolyTerm::makeAllPermutations(){

  _allPermutations.clear();
  _allPermutations.push_back(_powers);

  for(unsigned int i=0; i < _powers.size()-1; i++){
    for(unsigned int j=i+1; j < _powers.size(); j++){
      unsigned int n = _allPermutations.size();
      for(unsigned int k = 0; k < n; k++){
	std::vector<int> ptemp( _allPermutations[k] );
	swap(ptemp[i],ptemp[j]);
	_allPermutations.push_back(ptemp);
      }
    }
  }

  sort(_allPermutations.begin(), _allPermutations.end());

  std::vector< std::vector<int> >::iterator
    lastUnique = unique(_allPermutations.begin(), _allPermutations.end());

  _allPermutations.erase(lastUnique, _allPermutations.end());

  return;
}


void symPolyTerm::print(std::ostream& os)const{
  bool firstTerm(true);

  //  os << " P(x_j; " << name() << " ) = ";
  for(unsigned int i = 0; i < _allPermutations.size(); i++){
    if(! firstTerm ) os << " + ";
    const std::vector<int>& thisPerm(_allPermutations[i]);
    double thisSum=0;
    for(unsigned int j = 0; j < thisPerm.size(); j++){
      thisSum += thisPerm[j];
      if(0 == j) os << " ";
      if(0 != thisPerm[j]){
	os << "x_" << j;
	if(1 != thisPerm[j]) os << "^" << thisPerm[j];
	os << " ";
	firstTerm = false;
      }
    }
    if(0 == thisSum) os << "1 ";
  }
}

double symPolyTerm::eval(double x, double y)const{
  static std::vector<const double* > ptrs(2);
  ptrs[0] = &x;
  ptrs[1] = &y;
  return eval(ptrs);
}
double symPolyTerm::eval(double x, double y, double z)const{
  std::vector<const double* > ptrs(3);
  ptrs[0] = &x;
  ptrs[1] = &y;
  ptrs[2] = &z;
  return eval(ptrs);
}
double symPolyTerm::eval(double x, double y, double z
			 , double u)const{
  static std::vector<const double* > ptrs(4);
  ptrs[0] = &x;
  ptrs[1] = &y;
  ptrs[2] = &z;
  ptrs[3] = &u;
  return eval(ptrs);
}
double symPolyTerm::eval(double x, double y, double z
			 , double u, double v)const{
  std::vector<const double* > ptrs(5);
  ptrs[0] = &x;
  ptrs[1] = &y;
  ptrs[2] = &z;
  ptrs[3] = &u;
  ptrs[4] = &v;
  return eval(ptrs);
}
double symPolyTerm::eval(double x, double y, double z
			 , double u, double v, double w)const{
  static std::vector<const double* > ptrs(6);
  ptrs[0] = &x;
  ptrs[1] = &y;
  ptrs[2] = &z;
  ptrs[3] = &u;
  ptrs[4] = &v;
  ptrs[5] = &w;
  return eval(ptrs);
}

double symPolyTerm::eval(const std::vector<double>& x)const{
 if(x.size() != _powers.size()){
    cout << "symPolyTerm::eval: size mismatch|!!!"
	 << " me: " << _powers.size() << " vs x: " << x.size()
	 << "\n _powers: " << _powers
	 << "\n x: " << x
	 << endl;
    throw "can't deal with this";
  }

  double sum(0);
  for(unsigned int i = 0; i < _allPermutations.size(); i++){
    const std::vector<int>& thisPerm(_allPermutations[i]);
    double prod(1);
    for(unsigned int j = 0; j < thisPerm.size(); j++){
      prod *= pow((x[j]), thisPerm[j]);
    }
    sum += prod;
  }
  return sum;
}

double symPolyTerm::eval(const std::vector<double* >& x)const{
  if(x.size() != _powers.size()){
    cout << "symPolyTerm::eval: size mismatch|!!!"
	 << " me: " << _powers.size() << " vs x: " << x.size()
	 << "\n _powers: " << _powers
	 << "\n x: " << x
	 << endl;
    throw "can't deal with this";
  }

  double sum(0);
  for(unsigned int i = 0; i < _allPermutations.size(); i++){
    const std::vector<int>& thisPerm(_allPermutations[i]);
    double prod(1);
    for(unsigned int j = 0; j < thisPerm.size(); j++){
      prod *= pow((*(x[j])), thisPerm[j]);
    }
    sum += prod;
  }
  return sum;
}

double symPolyTerm::eval(const std::vector<const double* >& x)const{
  if(x.size() != _powers.size()){
    cout << "symPolyTerm::eval: size mismatch|!!!"
	 << " me: " << _powers.size() << " vs x: " << x.size()
	 << "\n _powers: " << _powers
	 << "\n x: " << x
	 << endl;
    throw "can't deal with this";
  }

  double sum(0);
  for(unsigned int i = 0; i < _allPermutations.size(); i++){
    const std::vector<int>& thisPerm(_allPermutations[i]);
    double prod(1);
    for(unsigned int j = 0; j < thisPerm.size(); j++){
      prod *= pow((*(x[j])), thisPerm[j]);
    }
    sum += prod;
  }
  return sum;
}



std::ostream& operator<<(std::ostream& os, const symPolyTerm& spt){
  spt.print(os);
  return os;
}

//




