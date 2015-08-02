// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT
#include <cmath>
#include <string>

#include "Mint/CLHEPPhysicalConstants.h"

#include "Mint/FitComplex.h"
#include "Mint/FitComplexPolar.h"
#include "Mint/FitComplexCart.h"
#include "Mint/FitParDependent.h"
#include "Mint/IFitParRegister.h"

using namespace std;
using namespace MINT;

counted_ptr<FitComplex> MINT::FitComplexMaker(const std::string& name
					      , const char* fname
					      , MinuitParameterSet* pset
					      , IFitParRegister* daddy
					      , FitParameter::FIX_OR_WHAT fow
					      , NamedParameterBase::VERBOSITY vb
					      ){
  bool dbThis=false;
  bool verbose=false;
  counted_ptr<FitComplex> 
    ptr_polar(new FitComplexPolar(name, fname, pset, daddy, fow, vb));

  counted_ptr<FitComplex> 
    ptr_euclid(new FitComplexCart(name, fname, pset, daddy, fow, vb));

  int count=0;
  counted_ptr<FitComplex> return_ptr(0);
  counted_ptr<FitComplex> default_ptr(ptr_polar);

  if( ptr_polar->gotInitialised()){
    if(dbThis){
      cout << " FitComplexMaker for " << name
	   << " found polar co-ordinates." << endl;
    }
    return_ptr = ptr_polar;
    count++;
  }
  if( ptr_euclid->gotInitialised()){
    if(dbThis){
      cout << " FitComplexMaker for " << name
	   << " found Cartesian co-ordinates." << endl;
    }
    return_ptr = ptr_euclid;
    count++;
  }
  if(0 == count){
    if(verbose){
      cout << " FitComplexMaker WARNING!: " << name
	   << " did not get initialised!" << endl;
    }
    return default_ptr;
  }else if(2 == count){
    cout << " FitComplexMaker WARNING!: " << name
	 << " got initialised " << count << " times!!" << endl;
    return default_ptr;
  }
  return return_ptr;
}

FitComplex::FitComplex(IFitParRegister* daddy) : FitParDependent(daddy){}

bool FitComplex::isConstant() const{
  return p1().iFixInit() && p2().iFixInit();
}

bool FitComplex::isZero() const{
  return 0 == getAmp();
}
void FitComplex::setParameterSet(MinuitParameterSet* pset){
  if(0 == pset) return;
  p1().addToParSet(pset);
  p2().addToParSet(pset);
}

void FitComplex::print(std::ostream& os) const{
  os << " value " << getVal();
}


std::complex<double> operator*(const std::complex<double>& cplx
			       , const FitComplex& fc){
  return cplx * fc.getVal();
}
std::complex<double> operator*(const FitComplex& fc
			       ,const std::complex<double>& cplx){
  return fc.getVal() * cplx;
}

std::complex<double> operator+(const std::complex<double>& cplx
			       , const FitComplex& fc){
  return cplx + fc.getVal();
}
std::complex<double> operator+(const FitComplex& fc
			       ,const std::complex<double>& cplx){
  return fc.getVal() + cplx;
}

std::complex<double> operator-(const std::complex<double>& cplx
			       , const FitComplex& fc){
  return cplx - fc.getVal();
}
std::complex<double> operator-(const FitComplex& fc
			       ,const std::complex<double>& cplx){
  return fc.getVal() - cplx;
}

std::complex<double> operator/(const std::complex<double>& cplx
			       , const FitComplex& fc){
  return cplx / fc.getVal();
}
std::complex<double> operator/(const FitComplex& fc
			       ,const std::complex<double>& cplx){
  return fc.getVal()/cplx;
}

std::ostream& operator<<(std::ostream& os, const FitComplex& fc){
  fc.print(os);
  return os;
}


//
