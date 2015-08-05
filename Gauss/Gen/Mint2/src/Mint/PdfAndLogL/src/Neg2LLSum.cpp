// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/Neg2LLSum.h"
#include "Mint/IMinimisable.h"
#include "Mint/Neg2LLConstraint.h"
#include "Mint/NamedParameter.h"


using namespace std;
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

bool Neg2LLSum::addConstraints(){

    cout << "addConstraints for " << getParSet()->size() << " paramaters" <<  endl;

    
    for(unsigned int i=0;i< getParSet()->size();i++){
        string name_i= getParSet()->getParPtr(i)->name();
        cout << "paramater " << i << " : " <<  name_i << endl;
        NamedParameter<double> constrain(("Constrain_"+name_i).c_str(),-1,-1,NamedParameterBase::QUIET);
        //cout << constrain << endl;
        if(constrain.getVal(0)>0 && constrain.getVal(1)>0){
            IMinimisable* gaussConstraint = new Neg2LLConstraint(getParSet()->getParPtr(i),constrain.getVal(0),constrain.getVal(1),getParSet());
            add(gaussConstraint);
            cout << "Added gaussian constraint to parameter: " << name_i 
            << " with initial value = " << (double) *(getParSet()->getParPtr(i))  
            << " and mean = " << constrain.getVal(0)
            << " and sigma = " << constrain.getVal(1) << endl  ;
            cout << "neg2LL value = " << gaussConstraint->getVal() << endl; 

        }
    }
        
    return true;
}

void Neg2LLSum::Gradient(Double_t* grad){  
    
    std::vector<std::vector<double> > gradVec;
    
    for(unsigned int i=0; i < _likList.size(); i++){
       
        Double_t grad_tmp[this->getParSet()->size()];  
        for(unsigned int j=0; j < this->getParSet()->size(); j++) grad_tmp[j]= 0.;
        
        _likList[i]->Gradient(grad_tmp);
    
        std:vector<double> tmp;

        for(unsigned int j=0; j < this->getParSet()->size(); j++){
            tmp.push_back(grad_tmp[j]);
        }
        
        gradVec.push_back(tmp);
    }
    
    for(unsigned int i=0;i< this->getParSet()->size();i++){
        grad[i]=0.;
        for(unsigned int j=0; j < _likList.size(); j++){
            grad[i] += gradVec[j][i];
        }
    }
}

bool Neg2LLSum::useAnalyticGradient() {
    bool b= true;
    for(unsigned int i=0; i < _likList.size(); i++){
        b = b && _likList[i]->useAnalyticGradient();
    }
    return b;
}

void Neg2LLSum::setUseAnalyticGradient(bool useAnalyticGradient) {
    for(unsigned int j=0; j < _likList.size(); j++){
        _likList[j]->setUseAnalyticGradient(useAnalyticGradient);
    }
}

//
