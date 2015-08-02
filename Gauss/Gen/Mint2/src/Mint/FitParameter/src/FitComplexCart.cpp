// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include <cmath>
#include <string>

#include "Mint/CLHEPPhysicalConstants.h"

#include "Mint/FitComplexCart.h"
#include "Mint/IFitParRegister.h"


using namespace std;
using namespace MINT;

std::string FitComplexCart::makeRealName(const std::string& varName){
  return varName + "_Re";
}
std::string FitComplexCart::makeImagName(const std::string& varName){
  return varName + "_Im";
}

FitComplexCart::FitComplexCart(const std::string& varName
			       , const char* fname
			       , MinuitParameterSet* pset
			       , MINT::IFitParRegister* daddy
			       , FitParameter::FIX_OR_WHAT fow
			       , NamedParameterBase::VERBOSITY vb
			       )
  : FitComplex(daddy)
  , _real(makeRealName(varName), fname, pset, fow, vb)
  , _imag(makeImagName(varName), fname, pset, fow, vb)
{
  // cout << "pset pointer in FitComplexCart " << pset << endl;
}
FitComplexCart::FitComplexCart(const std::string& varName
			       , MinuitParameterSet* pset
			       , MINT::IFitParRegister* daddy
			       , FitParameter::FIX_OR_WHAT fow
			       , NamedParameterBase::VERBOSITY vb
			       )
  : FitComplex(daddy)
  , _real(makeRealName(varName), pset, fow, vb)
  , _imag(makeImagName(varName), pset, fow, vb)
{
}

FitComplexCart::~FitComplexCart(){
}
/*
FitComplexCart::FitComplexCart(const FitComplexCart& other){
  : _real(makeRealName(varName), fname, pset)
  , _imag(makeImagName(varName), fname, pset)
{
}
*/

std::complex<double> FitComplexCart::getVal()const{
  return complex<double>((double)real(), (double) imag() );
}
std::complex<double> FitComplexCart::getValInit()const{
  return complex<double>( real().meanInit(), imag().meanInit());
}

bool FitComplexCart::gotInitialised()const{
  return real().gotInitialised() && imag().gotInitialised();
}

void FitComplexCart::defaultInit(){
  real().defaultInit();
  imag().defaultInit();
}
void FitComplexCart::print(std::ostream& os) const{
  os << real() << "\n"
     << imag();
}

void FitComplexCart::set(std::complex<double> z){
  _real = z.real();
  _imag = z.imag();
}


//
