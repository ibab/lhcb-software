#include "Mint/FitFractionList.h"
#include <algorithm>



FitFractionList::FitFractionList()
  : std::vector<FitFraction>()
  , _sum("sum", 0.0)
{
}

FitFractionList::FitFractionList(const FitFractionList& other)
  : std::vector<FitFraction>(other)
  , _sum(other._sum)
{
}

void FitFractionList::add(const FitFraction& f){
  _sum.frac() += f.frac();
  this->push_back(f);
}
void FitFractionList::setSumFitError(double sfe){
  _sum.sigmaFit()=sfe;
}
void FitFractionList::setSumIntegError(double sie){
  _sum.sigmaInteg()=sie;
}

void FitFractionList::print(std::ostream& os)const{
  for(unsigned int i=0; i < this->size(); i++){
    if(i > 0) os << '\n';
    os << "\t(" << i+1 << ") " << (*this)[i];
  }
  if(_sum.frac() > -9998){
    os << "\n=========================================================="
       << "\n\t " << _sum
       << "\n (can differ from 1 due to interference effects)";
  }
}

void FitFractionList::sortBySizeAscending(){
  FitFractionList::smallerFitFractionBySize sorter;
  sort(this->begin(), this->end(), sorter);
}
void FitFractionList::sortBySizeDecending(){
  FitFractionList::antiSmallerFitFractionBySize sorter;
  sort(this->begin(), this->end(), sorter);
}
void FitFractionList::sortByMagnitudeAscending(){
  FitFractionList::smallerFitFractionByMagnitude sorter;
  sort(this->begin(), this->end(), sorter);
}
void FitFractionList::sortByMagnitudeDecending(){
  FitFractionList::antiSmallerFitFractionByMagnitude sorter;
  sort(this->begin(), this->end(), sorter);
}
void FitFractionList::sortByName(){
  FitFractionList::smallerFitFractionByName sorter;
  sort(this->begin(), this->end(), sorter);
}

std::ostream& operator<<(std::ostream& os, const FitFractionList& f){
  f.print(os);
  return os;
}


//

