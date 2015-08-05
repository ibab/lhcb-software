#ifndef NEG_TWO_LL_SUM_HH
#define NEG_TWO_LL_SUM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/Minimisable.h"

#include <vector>

namespace MINT{

class IMinimisable;
class MinuitParameterSet;

class Neg2LLSum : public Minimisable{

  std::vector<IMinimisable*> _likList;
 public:
  Neg2LLSum(MinuitParameterSet* mps = 0);

  Neg2LLSum(const std::vector<IMinimisable*>& likList_in
	    , MinuitParameterSet* mps = 0);
  
  Neg2LLSum(IMinimisable* ll_1
	    , MinuitParameterSet* mps = 0);

  Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
	    , MinuitParameterSet* mps = 0);

  Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
	    , IMinimisable* ll_3
	    , MinuitParameterSet* mps = 0);
  
  Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
	    , IMinimisable* ll_3, IMinimisable* ll_4
	    , MinuitParameterSet* mps = 0);
  
  Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
	    , IMinimisable* ll_3, IMinimisable* ll_4
	    , IMinimisable* ll_5
	    , MinuitParameterSet* mps = 0);
  
  Neg2LLSum(IMinimisable* ll_1, IMinimisable* ll_2
	    , IMinimisable* ll_3, IMinimisable* ll_4
	    , IMinimisable* ll_5, IMinimisable* ll_6
	    , MinuitParameterSet* mps = 0);
  
  Neg2LLSum(const Neg2LLSum& other);
  
  bool add(IMinimisable* llPtr);

  virtual void beginFit();

  virtual void parametersChanged();

  virtual void endFit();

  double getVal();
    
  bool addConstraints();
    
  virtual void Gradient(Double_t* grad);
  virtual bool useAnalyticGradient();
  virtual void setUseAnalyticGradient(bool useAnalyticGradient);

};
}// namespace MINT
#endif
//
