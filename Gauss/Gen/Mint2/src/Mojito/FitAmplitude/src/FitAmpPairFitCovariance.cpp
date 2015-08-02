
#include "Mint/FitAmpPairFitCovariance.h"
#include "Mint/FitAmpPairList.h"
#include "Mint/Utils.h"

#include <iostream>

using namespace std;
using namespace MINT;

unsigned int FitAmpPairFitCovariance::_maxSize = 1500;

FitAmpPairFitCovariance::FitAmpPairFitCovariance(const FitAmpPairList* myList
						 , const TMatrixTSym<double>& fc
						 )
  : _myList(myList)
  , _fitCov(fc)
  , _Nevents(0)
  , _size(1)
  , _needToRecalculate(true)
  , _pairCov(1)
  , _pairCorr(1)
  , _fractionCov(1)
  , _fractionSumCov(1)
  , _interferenceFracSumCov(1)
  , _totalFractionSumCov(1)
  , _dFitAmpPair_by_dFitParameters(1, fc.GetNcols())
  , _dFraction_by_dFitAmpPair(1, 1)
  , _dFractionSum_by_dFraction(1, 1)
  , _dInterferenceFracSum_by_dFraction(1, 1)
  , _dTotalFractionSum_by_dFraction(1, 1)
{  
  bool dbThis=false;
  if(0 != _myList && _myList->size() >=1) resize();

  if(dbThis) {
    cout << "FitAmpPairFitCovariance constructor called with this cov matrix:"
	 << endl;
    _fitCov.Print();
  }
}
FitAmpPairFitCovariance::FitAmpPairFitCovariance(const FitAmpPairFitCovariance& other)
  : _myList(other._myList)
  , _fitCov(other._fitCov)
  , _Nevents(other._Nevents)
  , _size(other._size)
  , _needToRecalculate(false)
  , _pairCov(other._pairCov)
  , _pairCorr(other._pairCorr)
  , _fractionCov(other._fractionCov)
  , _fractionSumCov(other._fractionSumCov)
  , _interferenceFracSumCov(other._interferenceFracSumCov)
  , _totalFractionSumCov(other._totalFractionSumCov)
  , _dFitAmpPair_by_dFitParameters(other._dFitAmpPair_by_dFitParameters)
  , _dFraction_by_dFitAmpPair(other._dFraction_by_dFitAmpPair)
  , _dFractionSum_by_dFraction(other._dFractionSum_by_dFraction)
  , _dInterferenceFracSum_by_dFraction(other._dInterferenceFracSum_by_dFraction)
  , _dTotalFractionSum_by_dFraction(other._dTotalFractionSum_by_dFraction)
{
}

unsigned int FitAmpPairFitCovariance::size() const{
  return _size;
}
bool FitAmpPairFitCovariance::resize(){
  bool dbThis=false;
  if(dbThis){
    cout << "FitAmpPairFitCovariance::resize()"
	 << " resizing from " << size()
	 << " to " << _myList->size() 
	 << " with maxSize() = " << maxSize() << "."
	 << endl;
  }

  _needToRecalculate=true;

  _size = _myList->size();

  if(size() > maxSize()){
    cout << "FitAmpPairFitCovariance::resize size() = " << size()
	 << " that's too big! won't do it!" << endl;
    return false;
  }

  _pairCov.   ResizeTo(size(), size());
  _pairCorr.  ResizeTo(size(), size());

  _fractionCov.     ResizeTo(size(), size());
  _fractionSumCov.  ResizeTo(1,1);
  _interferenceFracSumCov.  ResizeTo(1,1);
  _totalFractionSumCov.  ResizeTo(1,1);
  _dFitAmpPair_by_dFitParameters.       ResizeTo(size(), _fitCov.GetNcols());
  _dFraction_by_dFitAmpPair. ResizeTo(size(), size());

  _dFractionSum_by_dFraction.ResizeTo(1, size());
  _dInterferenceFracSum_by_dFraction.ResizeTo(1, size());
  _dTotalFractionSum_by_dFraction.ResizeTo(1, size());

  return true;
}

std::string FitAmpPairFitCovariance::realName(int i) const{
  return (*_myList)[i].name() + "_real";
}
std::string FitAmpPairFitCovariance::imagName(int i) const{
  return (*_myList)[i].name() + "_imag";
}

bool FitAmpPairFitCovariance::calculateAll(){
  bool dbThis=false;
  bool sc=true;
  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making dFitAmpPair_by_dFitParameters" << endl;
  sc &= make_dFitAmpPair_by_dFitParameters();

  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making dFraction_by_dFitAmpPair" << endl;
  sc &= make_dFraction_by_dFitAmpPair();

  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making dFractionSum_by_dFraction" << endl;
  sc &= make_dFractionSum_by_dFraction();

  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making dInterferenceFrac_by_dFraction" << endl;
  sc &= make_dInterferenceFracSum_by_dFraction();

  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making dTotalFractionSum_by_dFraction" << endl;
  sc &= make_dTotalFractionSum_by_dFraction();

  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making FitAmpPairCov" << endl;
  sc &= makeFitAmpPairCov();
  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making FractionCovariance" << endl;
  sc &= makeFractionCovariance();
  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making FractionSumCovariance" << endl;
  sc &= makeFractionSumCovariance();
  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making InterferenceFracSumCovariance" << endl;
  sc &= makeInterferenceFracSumCovariance();
  if(dbThis) cout << "FitAmpPairFitCovariance::calculateAll"
		  << " making TotalFractionSumCovariance" << endl;
  sc &= makeTotalFractionSumCovariance();

  _needToRecalculate=false;
  return sc;
}
bool FitAmpPairFitCovariance::make_dFitAmpPair_by_dFitParameters(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairFitCovariance::"
		  << "make_dFitAmpPair_by_dFitParameters called" << endl;
  /*
    each FitAmpPair is evaluated as the real part of
    the product of three complex numbers, times other factors (f)

    p = z1 z2* a * f, p* = z1* z2 a f*

    The paramters are:
    o The product of fit parameters z1  z2* = fitParValue()
    o The integral: a

    Each FitAmpPair returns real(p) = 0.5(p + p*);
    The total integral is the sum of these.

    z1 is either given as   
    o z1 = (x1 + iy1) or as 
    o z1 = r1  exp(i phi1)  

    We want to transform from the fit parameters to p. We'll need

    dp/d(x1)   = z2*  a f                  =   p/z1
    dp/d(y1)   = iz2* a f                  = i p/z1
    dp/d(r1)   = exp(i  degFac phi1) z2* a  f      =   p/r1
    dp/d(phi1) = i degFac r1 exp(i phi1) z2* a f  = i p

    dp/d(x2)   =   z1 a f                  =   p/z2*
    dp/d(y2)   = -iz1 a f                  = -i p/z2*
    dp/d(r2)   = exp(-i degFac phi2) z1 a f       =   p/r2
    dp/d(phi2) = -i degFac r2 exp(-i degFac phi2) z1 a f = -i p
    */
  for(int i=0; i < _dFitAmpPair_by_dFitParameters.GetNrows(); i++){
    for(int j=0; j < _dFitAmpPair_by_dFitParameters.GetNcols(); j++){
      _dFitAmpPair_by_dFitParameters(i,j)=0;
    }
  }
  for(unsigned int i=0; i < _myList->size(); i++){
    complex<double> p  =  (*_myList)[i].complexVal();
    complex<double> z1 =  (*_myList)[i].fitAmp1().FitAmpPhase();
    complex<double> z2 =  (*_myList)[i].fitAmp2().FitAmpPhase();
    complex<double> z2star(conj(z2));
    complex<double> number_i(0, 1);

    FitComplex::TYPE t1 = (*_myList)[i].fitAmp1().FitAmpPhase().type();

    int index_1a = (*_myList)[i].fitAmp1().FitAmpPhase().p1().parSetIndex();
    int index_1b = (*_myList)[i].fitAmp1().FitAmpPhase().p2().parSetIndex();

    if(FitComplex::CARTESIAN == t1){
      complex<double> dpdx1 = p/z1;
      complex<double> dpdy1 = number_i * p / z1;
      
      _dFitAmpPair_by_dFitParameters(i, index_1a) += dpdx1.real();
      _dFitAmpPair_by_dFitParameters(i, index_1b) += dpdy1.real();
    }else if(FitComplex::POLAR == t1){

      double degFac = 
	((const FitComplexPolar&) ((*_myList)[i].fitAmp1().FitAmpPhase()) 
	 ).degFac();
      
      complex<double> r1   = abs(z1);

      complex<double> dpdr1    = p/r1;
      complex<double> dpdphi1  = number_i * p * degFac;
      _dFitAmpPair_by_dFitParameters(i, index_1a) += dpdr1.real();
      _dFitAmpPair_by_dFitParameters(i, index_1b) += dpdphi1.real();
    }else{
      cout << "ERROR in FitAmpPairFitCovariance::"
	   << "make_dFitAmpPair_by_dFitParameters()"
	   << " don't know type of FitComplex in " 
	   << (*_myList)[i].fitAmp1()
	   << endl;
      throw "shouldn't be possible";
    }
    FitComplex::TYPE t2 = (*_myList)[i].fitAmp2().FitAmpPhase().type();

    int index_2a = (*_myList)[i].fitAmp2().FitAmpPhase().p1().parSetIndex();
    int index_2b = (*_myList)[i].fitAmp2().FitAmpPhase().p2().parSetIndex();

    /*    
	  dp/d(x2)   = z1  a                   =   p/z2*
	  dp/d(y2)   = -iz1 a                  = -i p/z2*
	  dp/d(r2)   = exp(-i degFac phi2) z1 a       =   p/r2
	  dp/d(phi2) = -i degFac r2 exp(-i phi2) z2 a = -i p
    */

    if(FitComplex::CARTESIAN == t2){
      complex<double> dpdx2 = p/z2star;
      complex<double> dpdy2 = - number_i * p / z2star;
      
      _dFitAmpPair_by_dFitParameters(i, index_2a) += dpdx2.real();
      _dFitAmpPair_by_dFitParameters(i, index_2b) += dpdy2.real();
    }else if(FitComplex::POLAR == t2){
      double degFac = 
	((const FitComplexPolar&) ((*_myList)[i].fitAmp1().FitAmpPhase())
	 ).degFac();

      complex<double> r2   = abs(z2);

      complex<double> dpdr2    = p/r2;
      complex<double> dpdphi2  = - number_i * p *degFac;
      _dFitAmpPair_by_dFitParameters(i, index_2a) += dpdr2.real();
      _dFitAmpPair_by_dFitParameters(i, index_2b) += dpdphi2.real();
    }else{
      cout << "ERROR in FitAmpPairFitCovariance::"
	   << "make_dFitAmpPair_by_dFitParameters()"
	   << " don't know type of FitComplex in " << (*_myList)[i].fitAmp2()
	   << endl;
      throw "shouldn't be possible";
    }
  }
  return true;
}

bool FitAmpPairFitCovariance::make_dFraction_by_dFitAmpPair(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairFitCovariance::make_dFraction_by_dFitAmpPair()"
		  << " called" << endl;
  /*
    We are calculating trafo matrix for the following trafo:

    f_a = a/(a + b + c...)


    diagonal terms:

      d(f_a)/d(a) = 1/(a + b + c +...) - a/(a + b + c ...)^2

    off-diagonal terms:

      d(f_a)/d(b) = -a/(a + b + c ...)^2
   */

  double integ = _myList->integral();
  double integ2 = integ*integ;

  for(int i=0; i < _dFraction_by_dFitAmpPair.GetNrows(); i++){
    for(int j=0; j < _dFraction_by_dFitAmpPair.GetNcols(); j++){
      _dFraction_by_dFitAmpPair(i,j)=0;
    }
  }
  for(unsigned int i=0; i < _myList->size(); i++){
    double num = (*_myList)[i].integral();
    for(unsigned int j=0; j < _myList->size(); j++){
      _dFraction_by_dFitAmpPair(i,j) = -num/integ2;
    }
    _dFraction_by_dFitAmpPair(i, i) += 1.0/integ;
  }
  return true;
}

bool FitAmpPairFitCovariance::make_dFractionSum_by_dFraction(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairFitCovariance::make_dFractionSum_by_dFraction()"
		  << " called" << endl;
  /*
    We are calculating trafo matrix for the trafo to the sum 
    of all terms entering the fraction calculation, this 
    excludes the terms for interference effect.

    It's just ones (those included) and zeros (for those not included)
   */

  for(unsigned int i=0; i < _myList->size(); i++){
    if((*_myList)[i].isSingleAmp()){
      _dFractionSum_by_dFraction(0, i) = 1.0;
    }else{
      _dFractionSum_by_dFraction(0, i) = 0.0;
    }
  }
  return true;
}

bool FitAmpPairFitCovariance::make_dInterferenceFracSum_by_dFraction(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairFitCovariance::make_dInterferenceFracSum_by_dFraction()"
		  << " called" << endl;
  /*
    We are calculating trafo matrix for the trafo to the sum 
    of all terms entering the interference calculation, this 
    excludes all the usual fraction terms.

    It's just ones (those included) and zeros (for those not included)
   */

  for(unsigned int i=0; i < _myList->size(); i++){
    if((*_myList)[i].isSingleAmp()){
      _dInterferenceFracSum_by_dFraction(0, i) = 0.0;
    }else{
      _dInterferenceFracSum_by_dFraction(0, i) = 1.0;
    }
  }
  return true;
}

bool FitAmpPairFitCovariance::make_dTotalFractionSum_by_dFraction(){
  // used for debugging only
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairFitCovariance::"
		  << "make_dTotalFractionSum_by_dFraction()"
		  << " called" << endl;
  /*
    We are calculating trafo matrix for the trafo to the sum 
    of all terms entering the fraction calculation, this 
    excludes the terms for interference effect.

    It's just ones (those included - here all) 
    and zeros (for those not included - here none)
   */

  for(unsigned int i=0; i < _myList->size(); i++){
    _dTotalFractionSum_by_dFraction(0, i) = 1.0;
  }
  return true;
}


bool FitAmpPairFitCovariance::makeFitAmpPairCov(){
  bool dbThis=false;

  bool success = true;

  TMatrixT<double> dFitAmpPair_by_dFitParameters_Transpose(_dFitAmpPair_by_dFitParameters.GetNcols()
				       , _dFitAmpPair_by_dFitParameters.GetNrows());
  dFitAmpPair_by_dFitParameters_Transpose.Transpose(_dFitAmpPair_by_dFitParameters);
  if(dbThis){
    cout << "FitAmpPairFitCovariance::makeFitAmpPairCov()"
	 << " _dFitAmpPair_by_dFitParameters(): " 
	 << endl;
    _dFitAmpPair_by_dFitParameters.Print();
    cout << "and its transpose:" << endl;
    dFitAmpPair_by_dFitParameters_Transpose.Print();
  }

  _pairCov = makeTMatrixTSym(_dFitAmpPair_by_dFitParameters * 
			 _fitCov * 
			 dFitAmpPair_by_dFitParameters_Transpose);

  for(unsigned int i=0; i < _myList->size(); i++){
    if(_pairCov(i, i) <= 0) continue;
    for(unsigned int j=i; j < _myList->size(); j++){
      double densq = _pairCov(i,i)*_pairCov(j,j);
      if(densq > 0){
	_pairCorr(i,j) = _pairCov(i,j)/sqrt(densq);
      }
    }
  }

  if(dbThis){
    cout << "After " << _Nevents << "events" << endl;
    cout << "Covariance Matrix N" << endl;
    _pairCov.Print();
    cout << "Correlation Matrix N" << endl;
    _pairCorr.Print();
  }
  return success;
}

bool FitAmpPairFitCovariance::makeFractionCovariance(){
  bool dbThis=false;

  int cols=_dFraction_by_dFitAmpPair.GetNcols();
  int rows=_dFraction_by_dFitAmpPair.GetNrows();
  TMatrixT<double> dFraction_by_dFitAmpPair_Transpose(cols,rows);
  dFraction_by_dFitAmpPair_Transpose.Transpose(_dFraction_by_dFitAmpPair);

  _fractionCov = makeTMatrixTSym(_dFraction_by_dFitAmpPair * 
				 _pairCov * 
				 dFraction_by_dFitAmpPair_Transpose);

  if(dbThis){
    cout << "Correlation Matrix Fractions" << endl;
    _fractionCov.Print();
  }

  return true;
}
bool FitAmpPairFitCovariance::makeFractionSumCovariance(){
  bool dbThis=false;

  TMatrixT<double> dFractionSum_by_dFraction_Transpose(size(), 1);
  dFractionSum_by_dFraction_Transpose.Transpose(_dFractionSum_by_dFraction);

  _fractionSumCov = makeTMatrixTSym(_dFractionSum_by_dFraction *
				    _fractionCov * 
				    dFractionSum_by_dFraction_Transpose);

  if(dbThis){
    cout << "After " << _Nevents << "events" << endl;
    cout << "Covariance Matrix for FractionSum" << endl;
    _fractionSumCov.Print();
   }

  return true;
}
bool FitAmpPairFitCovariance::makeInterferenceFracSumCovariance(){
  bool dbThis=false;

  TMatrixT<double> dInterferenceFracSum_by_dFraction_Transpose(size(), 1);
  dInterferenceFracSum_by_dFraction_Transpose.Transpose(_dInterferenceFracSum_by_dFraction);

  _interferenceFracSumCov = makeTMatrixTSym(_dInterferenceFracSum_by_dFraction *
					    _fractionCov * 
					    dInterferenceFracSum_by_dFraction_Transpose);

  if(dbThis){
    cout << "After " << _Nevents << "events" << endl;
    cout << "Covariance Matrix for FractionSum" << endl;
    _interferenceFracSumCov.Print();
   }

  return true;
}
bool FitAmpPairFitCovariance::makeTotalFractionSumCovariance(){
  // used for debug only (should make a 1x1 matrix with 0 +/- numerical error)
  bool dbThis=false;

  TMatrixT<double> dTotalFractionSum_by_dFraction_Transpose(size(), 1);
  dTotalFractionSum_by_dFraction_Transpose.Transpose(_dTotalFractionSum_by_dFraction);

  _totalFractionSumCov = makeTMatrixTSym(_dTotalFractionSum_by_dFraction *
					 _fractionCov * 
					 dTotalFractionSum_by_dFraction_Transpose);

  if(dbThis){
    cout << "After " << _Nevents << "events" << endl;
    cout << "Covariance Matrix for TotalFractionSum" << endl;
    _totalFractionSumCov.Print();
   }

  return true;
}

double FitAmpPairFitCovariance::getPairVariance(int i, int j){
  if(_needToRecalculate)  calculateAll();
  if(i < 0 || i >= _pairCov.GetNrows() ) return -9999.0;
  if(j < 0 || j >= _fractionCov.GetNcols() ) return -9999.0;
  return _pairCov(i, j);
}
double FitAmpPairFitCovariance::getPairError(int i, int j){
  double V = getPairVariance(i, j);
  if(V < 0){
    cout << "ERROR in FitAmpPairFitCovariance::getPairError"
	 << " variance is " << V << " which is < 0."
	 << " will return -9999."
	 << endl;
    return 0;
  }
  return sqrt(V);
}

double FitAmpPairFitCovariance::getFractionVariance(int i){
  if(_needToRecalculate)  calculateAll();
  if(i < 0 || i >= _fractionCov.GetNcols() ) return -9999.0;
  if(i < 0 || i >= _fractionCov.GetNrows() ) return -9999.0;
  return _fractionCov(i, i);
}
double FitAmpPairFitCovariance::getFractionError(int i){
  double V = getFractionVariance(i);
  if(V < 0){
    cout << "ERROR in FitAmpPairFitCovariance::getFractionError"
	 << " variance is " << V << " which is < 0."
	 << " will return -9999."
	 << endl;
    return 0;
  }
  return sqrt(V);
}

double FitAmpPairFitCovariance::getTotalFractionSumVariance(){
  // use for debug only - should always return zero (ish)
  if(_needToRecalculate)  calculateAll();
  return _totalFractionSumCov(0,0);
}
double FitAmpPairFitCovariance::getTotalFractionSumError(){
  // use for debug only - should always return zero (ish)
  double V = getTotalFractionSumVariance();
  if(V < 0) return -sqrt(-V);
  else return sqrt(V);
}
double FitAmpPairFitCovariance::getInterferenceFracSumVariance(){
  if(_needToRecalculate)  calculateAll();
  return _interferenceFracSumCov(0,0);
}
double FitAmpPairFitCovariance::getInterferenceFracSumError(){
  double V = getInterferenceFracSumVariance();
  if(V < 0){
    cout << "ERROR in FitAmpPairFitCovariance::getInterferenceFracSumError"
	 << " variance is " << V << " which is < 0."
	 << " will return 0."
	 << endl;
    return 0;
  }
  return sqrt(V);
}
double FitAmpPairFitCovariance::getFractionSumVariance(){
  if(_needToRecalculate)  calculateAll();
  return _fractionSumCov(0,0);
}

double FitAmpPairFitCovariance::getFractionSumError(){
  double V = getFractionSumVariance();
  if(V < 0){
    cout << "ERROR in FitAmpPairFitCovariance::getFractionSumError"
	 << " variance is " << V << " which is < 0."
	 << " will return 0."
	 << endl;
    return 0;
  }
  return sqrt(V);
}
bool FitAmpPairFitCovariance::isValid()const{
  if(size() > maxSize()) return false;
  if(size() <= 0)return false;
  if((unsigned int) _pairCov.GetNcols() != size()) return false;
  return true;
}
//
