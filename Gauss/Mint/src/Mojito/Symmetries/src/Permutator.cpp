// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#include "Mint/Permutator.h"

#include <algorithm>

using namespace std;

Permutator::Permutator()
  : std::vector<Permutation>()
  , _pat(){
  Permutation p(1);
  this->push_back(p);
}

Permutator::Permutator(const DalitzEventPattern& pat) 
  : std::vector<Permutation>()
  , _pat(pat)
{
  findAllPermutations();
}

void Permutator::setPattern(const DalitzEventPattern& pat){
  _pat = pat;
  this->clear();
  findAllPermutations();
}

int Permutator::findAllPermutations(){

  this->clear();
  Permutation p(_pat.size());
  p.makeUnity();
  this->push_back(p);

  for(unsigned int i=0; i < _pat.size()-1; i++){
    for(unsigned int j=i+1; j < _pat.size(); j++){
      if(_pat[i] == _pat[j]){
	unsigned int n = this->size();
	for(unsigned int k = 0; k < n; k++){
	  Permutation ptemp( (*this)[k] );
	  ptemp.swap(i,j);
	  this->push_back(ptemp);
	}
      }
    }
  }

  sort(this->begin(), this->end());

  std::vector<Permutation>::iterator
    lastUnique = unique(this->begin(), this->end());

  this->erase(lastUnique, this->end());

  return this->size();
}

void Permutator::print(std::ostream& os) const{
  for(unsigned int i=0; i< this->size(); i++){
    os << " " << i << ")\n" << (*this)[i] << endl;
  }
}

ostream& operator<<(ostream& os, const Permutator& p){
  p.print(os);
  return os;
}

//
