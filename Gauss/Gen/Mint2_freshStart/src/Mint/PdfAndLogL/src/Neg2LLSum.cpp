// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/Neg2LLSum.h"
#include "Mint/IMinimisable.h"

using namespace MINT;

Neg2LLSum::Neg2LLSum(MinuitParameterSet* mps)
  : Minimisable(mps){}

Neg2LLSum::Neg2LLSum(const std::vector<IMinimisable*>& likList_in
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
  , _likList(likList_in)
{}

Neg2LLSum::Neg2LLSum(IMinimisable* ll_1
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
{
  add(ll_1);
}
Neg2LLSum::Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
{
  add(ll_1); add(ll_2);
}
Neg2LLSum::Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
		     , IMinimisable* ll_3
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
{
  add(ll_1); add(ll_2); add(ll_3);
}
Neg2LLSum::Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
		     , IMinimisable* ll_3, IMinimisable* ll_4
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
{
  add(ll_1); add(ll_2); add(ll_3); add(ll_4);
}
Neg2LLSum::Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
		     , IMinimisable* ll_3, IMinimisable* ll_4
		     , IMinimisable* ll_5
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
{
  add(ll_1); add(ll_2); add(ll_3); add(ll_4); add(ll_5);
}

Neg2LLSum::Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
		     , IMinimisable* ll_3, IMinimisable* ll_4
		     , IMinimisable* ll_5, IMinimisable* ll_6
		     , MinuitParameterSet* mps)
  : Minimisable(mps)
{
  add(ll_1); add(ll_2); add(ll_3); add(ll_4); add(ll_5); add(ll_6);
}

Neg2LLSum::Neg2LLSum(const Neg2LLSum& other)
  : IMinimisable()
  , Minimisable(other)
  , _likList(other._likList)
{}


bool Neg2LLSum::add(IMinimisable* llPtr){
  if(0 == llPtr){
    std::cout << "ERROR in Neg2LLSum::add: trying to add null pointer"
	      << std::endl;
    return false;
  }
  _likList.push_back(llPtr);
  return true;
}

void Neg2LLSum::beginFit(){
  for(unsigned int i=0; i < _likList.size(); i++){
    _likList[i]->beginFit();
  }
};
void Neg2LLSum::parametersChanged(){
  for(unsigned int i=0; i < _likList.size(); i++){
    _likList[i]->parametersChanged();
  }
};

void Neg2LLSum::endFit(){
  for(unsigned int i=0; i < _likList.size(); i++){
    _likList[i]->endFit();
  }
};


double Neg2LLSum::getVal(){
  double sum=0;
  for(unsigned int i=0; i < _likList.size(); i++){
    sum += _likList[i]->getVal();
  }
  return sum;
}

//
