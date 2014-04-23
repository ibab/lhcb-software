#include "Mint/symMultiPolyTerm.h"
#include "Mint/Utils.h"
#include <iostream>

using namespace std;
using namespace MINT;

symMultiPolyTerm::symMultiPolyTerm(int dimension, int order)
  : std::vector< symPolyTerm >()
{
  init(dimension, order);
}
symMultiPolyTerm::symMultiPolyTerm(const symMultiPolyTerm& other)
  : std::vector< symPolyTerm >(other)
{
}

bool symMultiPolyTerm::init(int dimension, int order){
  return createTerms(dimension, order);
}

bool symMultiPolyTerm::createTerms(int dimension, int order){
  bool dbThis=false;

  std::vector<std::vector<int> > v;
  createTerms(dimension, order, v);

  // next: remove duplicates, where we treat e.g. 32 as 23, i.e. order
  // does not matter. The symPolyTerm then implements 32 as 32, 23, etc
  // so what we "contract" here is expanded again there - efficient?
  // ... probably not, but it follows the logic of the setup better.
  for(unsigned int i=0; i < v.size(); i++) sort((v[i]).rbegin(), (v[i]).rend());
  sort(v.rbegin(), v.rend());
  std::vector< std::vector<int> >::iterator
    lastUnique = unique(v.begin(), v.end());
  v.erase(lastUnique, v.end());

  for(unsigned int j=0; j < v.size(); j++){
    if(dbThis)cout << v[j] << endl;
    symPolyTerm spt(v[j]);
    this->push_back(spt);
  }

  return true;

}
bool symMultiPolyTerm::createTerms(int dimension, int order
				   , std::vector<std::vector<int> >& v){
  if(dimension <=0) return true;
  
  std::vector<int> p(dimension,0);
  p[0]=order;
  v.push_back(p);

  for(int od = order-1; od > 0; od--){
    std::vector<int> p(dimension,0);
    p[0]=od;
    std::vector<std::vector<int> > v2;
    createTerms(dimension-1, order - od, v2);
    for(unsigned int i=0; i < v2.size(); i++){
      std::vector<int>& p2(p);
      std::vector<int>& tv2(v2[i]);
      for(unsigned int k=0; k < tv2.size() && k + 1 < p2.size(); k++){
	p2[k+1] = tv2[k];
      }
      v.push_back(p2);
    }
  }
  return true;
}

void symMultiPolyTerm::print(std::ostream& os) const{
  for(unsigned int i=0; i < this->size(); i++){
    if(0 != i) os << "\n\t + ";
    else os << "\t";

    os << (*this)[i];
  }
}
std::ostream& operator<<(std::ostream& os, const symMultiPolyTerm& smpt){
  smpt.print(os);
  return os;
}


//
