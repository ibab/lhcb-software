#ifndef MINT_DALITZ_FIT_FRACTION_LIST_HH
#define MINT_DALITZ_FIT_FRACTION_LIST_HH

#include "Mint/FitFraction.h"
#include <vector>
#include <iostream>
#include <cmath>

/* this is a dumb class 
   that only stores the result
   of a fit fraction calculation
   but doesn not calculate anytthig 
   by itself
*/

class FitFractionList : public std::vector<FitFraction>{
 protected:
  FitFraction _sum;
 public:
  FitFractionList();
  FitFractionList(const FitFractionList& other);
  
  void add(const FitFraction& f);

  const FitFraction& sum() const{return _sum;}
  void setSumFitError(double sfe);
  void setSumIntegError(double sie);

  void print(std::ostream& os)const;

  void sortBySizeAscending();
  void sortBySizeDecending();
  void sortByMagnitudeAscending();
  void sortByMagnitudeDecending();
  void sortByName();

  // helper classes for sorting:
  class smallerFitFractionBySize{
  public:
    bool operator()(const FitFraction& a, const FitFraction& b){
      return a.frac() < b.frac();
    }
  };
  class antiSmallerFitFractionBySize{
  public:
    bool operator()(const FitFraction& a, const FitFraction& b){
      return (a.frac() > b.frac());
    }
  };
  class smallerFitFractionByMagnitude{
  public:
    bool operator()(const FitFraction& a, const FitFraction& b){
      return fabs(a.frac()) < fabs(b.frac());
    }
  };
  class antiSmallerFitFractionByMagnitude{
  public:
    bool operator()(const FitFraction& a, const FitFraction& b){
      return (fabs(a.frac()) > fabs(b.frac()));
    }
  };
  class smallerFitFractionByName{
  public:
    bool operator()(const FitFraction& a, const FitFraction& b){
      return a.name() < b.name();
    }
  };
  
};

std::ostream& operator<<(std::ostream& os, const FitFractionList& f);
#endif
//
