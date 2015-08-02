// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include <cmath>
#include <string>

#include "Mint/CLHEPPhysicalConstants.h"

#include "Mint/FitComplexPolar.h"
#include "Mint/NamedParameter.h"
#include "Mint/Utils.h"

using namespace std;
using namespace MINT;

double FitComplexPolar::_degFac = -9999;
void FitComplexPolar::calculateDegFac(){
  NamedParameter<std::string> phaseOpt("Phases given in"
				       , (std::string) "degrees");
  if(phaseOpt.getVal() == (std::string) "degrees"){
    _degFac = pi/180.0;
  }else if(phaseOpt.getVal() == (std::string) "rad"){
    _degFac = 1.0;
  }else{
    cout << " ERROR in FitComplexPolar::calculateDegFac()"
	 << " Unknown option " << phaseOpt
	 << " Allowed values are are: \"degrees\" and \"rad\"."
	 << " Assuming you mean rad."
	 << endl;
    _degFac=1.0;
  }
  return;
}

std::string FitComplexPolar::makeAmpName(const std::string& varName){
  return varName + "_Amp";
}
std::string FitComplexPolar::makePhaseName(const std::string& varName){
  return varName + "_Phase";
}

FitComplexPolar::FitComplexPolar(const std::string& varName
				 , const char* fname
				 , MinuitParameterSet* pset
				 , IFitParRegister* daddy
				 , FitParameter::FIX_OR_WHAT fow
				 , FitParameter::VERBOSITY vb

				 )
  : FitComplex(daddy)
  , _amp  (makeAmpName(  varName), fname, pset, fow, vb)
  , _phase(makePhaseName(varName), fname, pset, fow, vb)
{
}
FitComplexPolar::FitComplexPolar(const std::string& varName
				 , MinuitParameterSet* pset
				 , IFitParRegister* daddy
				 , FitParameter::FIX_OR_WHAT fow
				 , FitParameter::VERBOSITY vb
				 )
  : FitComplex(daddy)
  , _amp  (makeAmpName(  varName), pset, fow, vb)
  , _phase(makePhaseName(varName), pset, fow, vb)
{
}

FitComplexPolar::~FitComplexPolar(){
}
bool FitComplexPolar::gotInitialised()const{
  return amp().gotInitialised() && phase().gotInitialised();
}

/*
FitComplexPolar::FitComplexPolar(const FitComplexPolar& other){
  : _amp(varName + "_Amp", other)
  , phase()(varName + "_Phase", other)
{
}
*/

std::complex<double> FitComplexPolar::getVal()const{
  double a(amp());
  if(0.0 == a) return 0;
  double p(phase());
  p*=degFac();
  return std::complex<double>(a*cos(p), a*sin(p));
  /*
  double a(amp());
  if(0.0 == a) return 0;
  double p(phase());
  p*=degFac();
  return std::polar(a,p);
  */
}
std::complex<double> FitComplexPolar::getValInit()const{
  double a=amp().meanInit();
  double p = phase().meanInit() * degFac();

  return std::complex<double>(a*cos(p), a*sin(p));
  //  return std::polar( amp().meanInit(), phase().meanInit() * degFac());
}

void FitComplexPolar::defaultInit(){
  amp().defaultInit();
  phase().defaultInit();
}

void FitComplexPolar::set(std::complex<double> z){
  _amp = abs(z);
  _phase = arg(z);
}

void FitComplexPolar::print(std::ostream& os) const{
  os << amp() << "\n"
     << phase();
}


//
