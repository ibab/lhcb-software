#include "Mint/FitFraction.h"

using namespace std;

FitFraction::FitFraction()
  : _name("FractionWithNoName")
  , _frac(0)
  , _sigmaFit(-9999.0)
  , _sigmaInteg(-9999.0)
{}

FitFraction::FitFraction(const std::string& name
			 , double frac
			 , double sigmaFit
			 , double sigmaInteg)
  : _name(name)
  , _frac(frac)
  , _sigmaFit(sigmaFit)
  , _sigmaInteg(sigmaInteg)
{
}
FitFraction::FitFraction(const FitFraction& other)
  : _name(other._name)
  , _frac(other._frac)
  , _sigmaFit(other._sigmaFit)
  , _sigmaInteg(other._sigmaInteg)
{
}
void FitFraction::print(std::ostream& os) const{
  os << name() << " = " << frac();
  if(sigmaFit() >= 0.0)  os << " +/- " << sigmaFit() << "(fit)";
  if(sigmaInteg() >=0.0) os << " +/- " << sigmaInteg() << "(integ)";
}

std::ostream& operator<<(std::ostream& os, const FitFraction& ff){
  ff.print(os);
  return os;
}
