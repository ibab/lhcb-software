
#include "Mint/FitAmpPairCovariance.h"
#include "Mint/FitAmpPairList.h"
#include "Mint/Utils.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace MINT;

unsigned int FitAmpPairCovariance::_maxSize = 1500;

FitAmpPairCovariance::FitAmpPairCovariance(const FitAmpPairList* myList)
  : _myList(myList)
  , _Nevents(0)
  , _size()
  , _needToRecalculate(true)
  , _sum_x(1, 1)
  , _sum_xy(1)
  , _cov2N(2)
  , _corr2N(2)
  , _cov(1)
  , _corr(1)
  , _fractionCov(1)
  , _fractionSumCov(1)
  , _integralCov(1)
  , _dN_by_d2N(1, 2)
  , _dFraction_by_dN(1, 1)
  , _dFractionSum_by_dFraction(1, 1)
  , _dIntegral_by_dN(1, 1)
{  
  if(0 != _myList && _myList->size() >=1) resize();
}
FitAmpPairCovariance::FitAmpPairCovariance(const FitAmpPairCovariance& other)
  : _myList(other._myList)
  , _Nevents(other._Nevents)
  , _size(other._size)
  , _needToRecalculate(true)
  , _sum_x(other._sum_x)
  , _sum_xy(other._sum_xy)
  , _cov2N(other._cov2N)
  , _corr2N(other._corr2N)
  , _cov(other._cov)
  , _corr(other._corr)
  , _fractionCov(other._fractionCov)
  , _fractionSumCov(other._fractionSumCov)
  , _integralCov(other._integralCov)
  , _dN_by_d2N(other._dN_by_d2N)
  , _dFraction_by_dN(other._dFraction_by_dN)
  , _dFractionSum_by_dFraction(other._dFractionSum_by_dFraction)
  , _dIntegral_by_dN(other._dIntegral_by_dN)
{
}
FitAmpPairCovariance::FitAmpPairCovariance(const FitAmpPairCovariance& other
					   , const FitAmpPairList* newList)
  : _myList(newList)
  , _Nevents(other._Nevents)
  , _size(other._size)
  , _needToRecalculate(true)
  , _sum_x(other._sum_x)
  , _sum_xy(other._sum_xy)
  , _cov2N(other._cov2N)
  , _corr2N(other._corr2N)
  , _cov(other._cov)
  , _corr(other._corr)
  , _fractionCov(other._fractionCov)
  , _fractionSumCov(other._fractionSumCov)
  , _integralCov(other._integralCov)
  , _dN_by_d2N(other._dN_by_d2N)
  , _dFraction_by_dN(other._dFraction_by_dN)
  , _dFractionSum_by_dFraction(other._dFractionSum_by_dFraction)
  , _dIntegral_by_dN(other._dIntegral_by_dN)
{  
}

bool FitAmpPairCovariance::isCompatibleWith(const FitAmpPairCovariance& 
					    other) const{
  if(0 == _myList && 0 == other._myList) return true;
  if(0 == _myList || 0 == other._myList) return false;
  if(_myList->size() != other._myList->size()) return false;

  if(! _myList->isCompatibleWith(*(other._myList))) return false;

  // the following check should be redundant:
  for(unsigned int i=0; i < this->size(); i++){
    if(realName(i) != other.realName(i)) return false;
    if(imagName(i) != other.imagName(i)) return false;
  }
  return true;
}
bool FitAmpPairCovariance::add(const FitAmpPairCovariance& other){
  if(0 == _myList || 0 == other._myList) {
    cout << "WARNING in FitAmpPairCovariance::add: "
	 << "trying to add incompatible FitAmpPairCovariances"
	 << "\n\t _myListPtr = " << _myList << " other's: " << other._myList
	 << endl;
    return false;
  }
  if(! isCompatibleWith(other)){
     cout << "WARNING in FitAmpPairCovariance::add: "
	 << "trying to add incompatible FitAmpPairCovariances"
	 << endl;   
    return false;
  }
  _needToRecalculate = true;
  _Nevents += other._Nevents;
  _sum_x   += other._sum_x;
  _sum_xy  += other._sum_xy;

  return true;
}

unsigned int FitAmpPairCovariance::size() const{
  return _size;
}
bool FitAmpPairCovariance::resize(){
  bool dbThis=false;
  if(dbThis){
    cout << "FitAmpPairCovariance::resize()"
	 << " resizing from " << size()
	 << " to " << _myList->size() 
	 << " with maxSize() = " << maxSize()
	 << endl;
  }

  _needToRecalculate=true;

  _size = _myList->size();

  if(size() > maxSize()){
    cout << "FitAmpPairCovariance::resize size() = " << size()
	 << " that's too big! won't do it!" << endl;
    return false;
  }

  _sum_x. ResizeTo(size()*2, 1);
  _sum_xy.ResizeTo(size()*2, size()*2);
  _cov2N. ResizeTo(size()*2, size()*2);
  _corr2N.ResizeTo(size()*2, size()*2);

  _cov.   ResizeTo(size(), size());
  _corr.  ResizeTo(size(), size());

  _fractionCov.     ResizeTo(size(), size());
  _fractionSumCov.  ResizeTo(1,1);
  _integralCov.     ResizeTo(1,1);

  _dN_by_d2N.       ResizeTo(size(), size()*2);
  _dFraction_by_dN. ResizeTo(size(), size());

  _dFractionSum_by_dFraction.ResizeTo(1, size());

  _dIntegral_by_dN. ResizeTo(1, size());

  if(dbThis){
    cout << "FitAmpPairCovariance::resize() to "
	 << size() << " done." << endl;
  }
  return true;
}

std::string FitAmpPairCovariance::realName(int listPosition) const{
  return (*_myList)[listPosition].name() + "_real";
}
std::string FitAmpPairCovariance::imagName(int listPosition) const{
  return (*_myList)[listPosition].name() + "_imag";
}

bool FitAmpPairCovariance::addLastEventFromList(){
  bool dbThis=false;
  if(size() != _myList->size()) {
    if(dbThis) cout << "FitAmpPairCovariance::addLastEventFromList() size() " 
		    << size() << " == " <<  _myList->size() << endl;
    if(! resize()) return false;
  }
  _needToRecalculate = true;
  _Nevents++;
  if(dbThis)cout << "FitAmpPairCovariance::addLastEventFromList()"
		 << " adding event to cov matrix." << endl;

  if(! isValid()) return false;
  for(unsigned int i=0; i < _myList->size(); i++){
    if(dbThis) cout << " i= " << i << endl;
    double re_i =  (*_myList)[i].lastEntry().real();
    double im_i =  (*_myList)[i].lastEntry().imag();
    _sum_x(indexReal(i), 0) += re_i;
    _sum_x(indexImag(i), 0) += im_i;

    for(unsigned int j=0; j < _myList->size(); j++){
      double re_j =  (*_myList)[j].lastEntry().real();
      double im_j =  (*_myList)[j].lastEntry().imag();
 
      _sum_xy(indexReal(i), indexReal(j)) += re_i * re_j;
      _sum_xy(indexReal(i), indexImag(j)) += re_i * im_j;
      _sum_xy(indexImag(i), indexReal(j)) += im_i * re_j;
      _sum_xy(indexImag(i), indexImag(j)) += im_i * im_j;
    }
  }
  return true;
}

bool FitAmpPairCovariance::calculateAll(){
  bool dbThis=false;
  if(! isValid()) return false;
  bool sc=true;
  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making dN_by_d2N" << endl;
  sc &= make_dN_by_d2N();

  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making dFraction_by_dN" << endl;
  sc &= make_dFraction_by_dN();

  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making dFractionSum_by_dFraction" << endl;
  sc &= make_dFractionSum_by_dFraction();

  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making dIntegral_by_dN" << endl;
  sc &= make_dIntegral_by_dN();

  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making 2NCovariance" << endl;
  sc &= make2NCovariance();
  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making NCovariance" << endl;
  sc &= makeNCovariance();
  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making FractionCovariance" << endl;
  sc &= makeFractionCovariance();
  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making FractionSumCovariance" << endl;
  sc &= makeFractionSumCovariance();
  if(dbThis) cout << "FitAmpPairCovariance::calculateAll"
		  << " making IntegralCovariance" << endl;
  sc &= makeIntegralCovariance();

  

  //_needToRecalculate=false;
  return sc;
}
bool FitAmpPairCovariance::make_dN_by_d2N(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairCovariance::make_dN_by_d2N() called" << endl;

  /*
    each FitAmpPair is evaluated as the real part of
    the product of two complex numbers.
    The product of fit parameters: oneOrTwo * (x_1 + iy_1)*(x_2 - iy_2)
    The integral: a + ib
    So we want to transfrom from (a, b) to c = xa - yb.

    Here we do the covariance due to the statistical uncertainty
    in the MC integration, not the uncertainy in the fit parameters x,y.

    So we calculate d(c) by d(a,b) = (x, -y) 
   */
  for(unsigned int i=0; i < _myList->size(); i++){
    complex<double> fpv = 
      (*_myList)[i].fitParValue()
      * (double)((*_myList)[i].oneOrTwo());

    _dN_by_d2N(i, indexReal(i)) =   fpv.real();
    _dN_by_d2N(i, indexImag(i)) = - fpv.imag();
  }
  return true;
}

bool FitAmpPairCovariance::make_dFraction_by_dN(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairCovariance::make_dFraction_by_dN() called" 
		  << endl;
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
  for(unsigned int i=0; i < _myList->size(); i++){
    double num = (*_myList)[i].integral();
    for(unsigned int j=0; j < _myList->size(); j++){
      _dFraction_by_dN(i, j) = -num/integ2;
    }
    _dFraction_by_dN(i, i) += 1.0/integ;
  }
  return true;
}

bool FitAmpPairCovariance::make_dIntegral_by_dN(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairCovariance::make_dIntegral_by_dN() called" 
		  << endl;
  /*
    We are calculating trafo matrix for the following trafo:

    sum = a + b + c...

   */

  for(unsigned int i=0; i < _myList->size(); i++){
    _dIntegral_by_dN(0, i) = 1;
  }
  return true;
}

bool FitAmpPairCovariance::make_dFractionSum_by_dFraction(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpPairCovariance::make_dFractionSum_by_dFraction()"
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


bool FitAmpPairCovariance::make2NCovariance(){
  bool dbThis=false;
  if(dbThis) cout << "making 2N covariance" << endl;

  double dN = _myList->numEvents();
  TMatrixT<float> mean_x(_myList->size()*2, 1);
  TMatrixTSym<float> mean_xy(_myList->size()*2);

  mean_x  = _sum_x;
  mean_x *= 1./dN;

  mean_xy  = _sum_xy;
  mean_xy *= 1./dN;
			    
  for(unsigned int i=0; i < _myList->size()*2; i++){
    for(unsigned int j=0; j < _myList->size()*2; j++){
      _cov2N(i, j) = (mean_xy(i,j) -  mean_x(i,0)*mean_x(j,0));
      // above: cov of the distribution
      // now: .. of the mean
      _cov2N(i, j) /= dN;
      // now taking into account other factors that I muliply the result
      // with.
      int il = listPositionFromIndex(i);
      int jl = listPositionFromIndex(j);
      double sfden = (*_myList)[il].weightSum() * (*_myList)[jl].weightSum();
      if(sfden > 0){
	_cov2N(i, j) *= dN*dN/sfden;
      }
    }
  }
  for(unsigned int i=0; i < _myList->size()*2; i++){
    for(unsigned int j=0; j < _myList->size()*2; j++){
      double den = _cov2N(i,i)*_cov2N(j,j);
      if(den > 0){
	_corr2N(i,j) = _cov2N(i,j)/sqrt(den);
      }
    }
  }

  if(dbThis){
    cout << "After " << _Nevents << "events" << endl;
    cout << "mean matrix" << endl;
    mean_x.Print();
    cout << "Covariance Matrix 2N" << endl;
    _cov2N.Print();
    cout << "Correlation Matrix 2N" << endl;
    _corr2N.Print();
  }
  return true;

}

bool FitAmpPairCovariance::makeNCovariance(){
  bool dbThis=false;

  bool success = true;
  //  success &= make2NCovariance();
  //  success &= make_dN_by_d2N();

  TMatrixT<float> dN_by_d2N_Transpose(2*size(), size());
  dN_by_d2N_Transpose.Transpose(_dN_by_d2N);
  if(dbThis){
    cout << "FitAmpPairCovariance::makeNCovariance()"
	 << " _dN_by_d2N(): " 
	 << endl;
    _dN_by_d2N.Print();
    cout << "and its transpose:" << endl;
    dN_by_d2N_Transpose.Print();
  }

  _cov = makeTMatrixTSym(_dN_by_d2N * _cov2N * dN_by_d2N_Transpose);

  for(unsigned int i=0; i < _myList->size(); i++){
    if(_cov(i, i) <= 0) continue;
    for(unsigned int j=0; j < _myList->size(); j++){
      double den = _cov(i,i)*_cov(j,j);
      if(den > 0){
	_corr(i,j) = _cov(i,j)/sqrt(den);
      }
    }
  }

  if(dbThis){
    cout << "After " << _Nevents << "events" << endl;
    cout << "Covariance Matrix N" << endl;
    _cov.Print();
    cout << "Correlation Matrix N" << endl;
    _corr.Print();
  }
  return success;
}

bool FitAmpPairCovariance::makeFractionCovariance(){
  bool dbThis=false;

  TMatrixT<float> dFraction_by_dN_Transpose(size(), size());
  dFraction_by_dN_Transpose.Transpose(_dFraction_by_dN);

  _fractionCov = makeTMatrixTSym(_dFraction_by_dN * _cov * 
				 dFraction_by_dN_Transpose);

  if(dbThis){
    cout << "Correlation Matrix Fractions" << endl;
    _fractionCov.Print();
  }

  return true;
}
bool FitAmpPairCovariance::makeFractionSumCovariance(){
  bool dbThis=false;

  TMatrixT<float> dFractionSum_by_dFraction_Transpose(size(), 1);
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

bool FitAmpPairCovariance::makeIntegralCovariance(){
  bool dbThis=false;

  TMatrixT<float> dIntegral_by_dN_Transpose(size(), 1);
  dIntegral_by_dN_Transpose.Transpose(_dIntegral_by_dN);

  _integralCov = makeTMatrixTSym(_dIntegral_by_dN * 
				 _cov 
				 * dIntegral_by_dN_Transpose);

  if(dbThis){
    cout << "Correlation Matrix Integral" << endl;
    _fractionCov.Print();
  }

  return true;
}


double FitAmpPairCovariance::getFractionVariance(int i){
  if(_needToRecalculate)  calculateAll();
  if(i < 0 || i >= _fractionCov.GetNcols()) return -9999.0;
  if(i < 0 || i >= _fractionCov.GetNrows()) return -9999.0;
  return _fractionCov(i, i);
}
double FitAmpPairCovariance::getFractionError(int i){
  double V = getFractionVariance(i);
  if(V < 0){
    cout << "ERROR in FitAmpPairCovariance::getFractionError"
	 << " variance is " << V << " which is < 0."
	 << " will return -9999."
	 << endl;
    return -9999.0;
  }
  return sqrt(V);
}

double FitAmpPairCovariance::getFractionSumVariance(){
  if(_needToRecalculate)  calculateAll();
  return _fractionSumCov(0,0);
}

double FitAmpPairCovariance::getFractionSumError(){
  double V = getFractionSumVariance();
  if(V < 0){
    cout << "ERROR in FitAmpPairCovariance::getFractionSumError"
	 << " variance is " << V << " which is < 0."
	 << " will return 0."
	 << endl;
    return 0;
  }
  return sqrt(V);
}

double FitAmpPairCovariance::getIntegralVariance(){
  bool dbThis=false;
  if(dbThis) cout << "need to recalculate ? " << _needToRecalculate << endl;
  if(_needToRecalculate)  calculateAll();
  if(_integralCov.GetNcols() < 1 || _integralCov.GetNrows() < 1) return -9999.0;
  return _integralCov(0,0);
}
double FitAmpPairCovariance::getIntegralError(){
  double V = getIntegralVariance();
  if(V < 0){
    cout << "ERROR in FitAmpPairCovariance::getFractionError"
	 << " variance is " << V << " which is < 0."
	 << " will return -9999."
	 << endl;
    return -9999.0;
  }
  return sqrt(V);
}

bool FitAmpPairCovariance::isValid()const{
  bool dbThis=false;
  static int numWarnings=0;
  const static int maxWarnings=10;
  if(dbThis){
    cout << " FitAmpPairCovariance::isValid() with: "
	 << size() << " == " << _sum_x.GetNcols() << " ? " << endl;
  }
  if(size() > maxSize()){
    if(numWarnings < 10){
      cout << "WARNING: FitAmpPairCovariance::isValid returns false"
	   << " because size (" << size() << ") is too big"
	   << endl;
      cout << " Will print this " << maxWarnings 
	   << " times (" << numWarnings << ")." << endl;
    }
    numWarnings++;
    return false;
  }
  if(size() <= 0)return false;
  //if((unsigned int) _sum_x.GetNcols() != 2*size()) return false;
  return true;
}

bool FitAmpPairCovariance::clearAll(){
  _Nevents=0;
  _needToRecalculate=true;
  resize();
  if(0 != this->size()){
    _sum_x  *= 0;
    _sum_xy *= 0;
  }
  return 0;
}
bool FitAmpPairCovariance::reset(){
  clearAll();
  resize();
  return true;
}


bool FitAmpPairCovariance::makeDirectory(const std::string& asSubdirOf)const{
  /*
    A mode is created from or'd permission bit masks defined
     in <sys/stat.h>:
           #define S_IRWXU 0000700     RWX mask for owner 
           #define S_IRUSR 0000400     R for owner 
           #define S_IWUSR 0000200     W for owner 
           #define S_IXUSR 0000100     X for owner 

           #define S_IRWXG 0000070     RWX mask for group 
           #define S_IRGRP 0000040     R for group 
           #define S_IWGRP 0000020     W for group 
           #define S_IXGRP 0000010     X for group 

           #define S_IRWXO 0000007     RWX mask for other 
           #define S_IROTH 0000004     R for other 
           #define S_IWOTH 0000002     W for other 
           #define S_IXOTH 0000001     X for other 

           #define S_ISUID 0004000     set user id on execution 
           #define S_ISGID 0002000     set group id on execution 
           #define S_ISVTX 0001000     save swapped text even after use
   */

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO 
              | S_ISUID | S_ISGID; 
  // see above for meaning. I want everybody to be allowed to read/write/exec.
  // Not sure about the last two bits.

  int zeroForSuccess = 0;
  zeroForSuccess |= mkdir( (asSubdirOf ).c_str(), mode );
  zeroForSuccess |= mkdir( (dirName(asSubdirOf) ).c_str(), mode );
  return (0 == zeroForSuccess);
}

std::string FitAmpPairCovariance::dirName(const std::string& asSubdirOf) const{
  return asSubdirOf +"/" + "FitAmpPairCovariance";
}
std::string FitAmpPairCovariance::matrix_x_fname(const std::string& asSubdirOf) const{
  return dirName(asSubdirOf) + "/" + "matrix_x.txt";
}
std::string FitAmpPairCovariance::matrix_xy_fname(const std::string& asSubdirOf) const{
  return dirName(asSubdirOf) + "/" + "matrix_xy.txt";
}

bool FitAmpPairCovariance::save(const std::string& inDirectory)const{
  bool dbThis=false;
  if(! isValid()){
    cout << "WARNING FitAmpPairCovariance::save(" << inDirectory <<")"
	 << " I'm not valid, I won't save anything." << endl;
    return false;
  }
  if(dbThis){
    cout << "saving FitAmpPairCovariance in " << inDirectory << "." << endl;
  }
  makeDirectory(inDirectory);
  bool sc=true;
  sc &= save_sum_x(inDirectory);
  sc &= save_sum_xy(inDirectory);

  return true;
}

bool FitAmpPairCovariance::save_sum_x(const std::string & inDirectory)const{
  bool dbThis=false;
  if(! isValid()){
    cout << "WARNING FitAmpPairCovariance::save_sum_x(" << inDirectory <<")"
	 << " I'm not valid, I won't save anything." << endl;
    return false;
  }
  ofstream os(matrix_x_fname(inDirectory).c_str());
  os << "N " << _Nevents << endl;
  if(dbThis){
    cout << "about to save: ";
    _sum_x.Print();
  }
  for(unsigned int i=0; i < _myList->size(); i++){

    os << setprecision(20) << realName(i) << " "
       << _sum_x(indexReal(i), 0) << endl;

    os << setprecision(20) << imagName(i) << " "
       << _sum_x(indexImag(i), 0) << endl;
  }
  os.close();
  return true;
}

bool FitAmpPairCovariance::save_sum_xy(const std::string & inDirectory)const{
  bool dbThis=false;
  if(! isValid()){
    cout << "WARNING FitAmpPairCovariance::save_sum_xy(" << inDirectory <<")"
	 << " I'm not valid, I won't save anything." << endl;
    return false;
  }
  ofstream os(matrix_xy_fname(inDirectory).c_str());
  os << "N " << _Nevents << endl;
  if(dbThis){
    cout << "about to save: ";
    _sum_xy.Print();
  }
  for(unsigned int i=0; i < _myList->size(); i++){
    for(unsigned int j=0; j < _myList->size(); j++){

      os << setprecision(20)<< realName(i) << " " << realName(j) << " "
	 << _sum_xy(indexReal(i), indexReal(j)) << endl;

      os << setprecision(20)<< realName(i) << " " << imagName(j) << " "
	 << _sum_xy(indexReal(i), indexImag(j)) << endl;

      os << setprecision(20)<< imagName(i) << " " << realName(j) << " "
	 << _sum_xy(indexImag(i), indexReal(j)) << endl;

      os << setprecision(20)<< imagName(i) << " " << imagName(j) << " "
	 << _sum_xy(indexImag(i), indexImag(j)) << endl;
    }
  }
  os.close();
  return true;
}

bool FitAmpPairCovariance::retrieve(const std::string& inDirectory){
  if(! resize()) return false;
  if(! retrieve_sum_x(inDirectory)) return false;
  if(! retrieve_sum_xy(inDirectory)) return false;
  _needToRecalculate=true;
  return true;
}

bool FitAmpPairCovariance::retrieve_sum_x(const std::string & inDirectory){
  bool dbThis=false;
  if(size() > maxSize()) return false;
  ifstream is(matrix_x_fname(inDirectory).c_str());
  if(is.bad()) return false;

  if(dbThis) cout << "opened file: "
		  << "\n" << matrix_x_fname(inDirectory)
		  << endl;
  std::string st1;
  double val;
  std::map<string, double> map_x;

  int counter=0;
  while(is){
    if(dbThis) cout << "reading " << counter << ". line" << endl;
    if(0 == counter){
      is >> st1 >> _Nevents;
      if(dbThis){
	cout << "just read " << st1 << " " << _Nevents << endl;
      }
     }else{
      is >> st1 >> val;
      map_x[st1] = val;
      if(dbThis){
	cout << "just read " << st1 << " " << val << endl;
      }
    }
    counter++;
  }
  is.close();
  if(dbThis)cout << "retrieve_sum_x, filling _sum_x for size " 
		 << size() << endl;
  for(unsigned int i=0; i < size(); i++){
    _sum_x(indexReal(i), 0) = map_x[realName(i)];
    _sum_x(indexImag(i), 0) = map_x[imagName(i)];
   if(dbThis){
      cout << "Tryied to retrieve from map entry for " << realName(i)
	   << " \n got: " << map_x[realName(i)] << endl;
    }
 
  }
  if(dbThis){
    cout << " did so, look at what I got for sum_x" << endl;
    _sum_x.Print();
  }
  _needToRecalculate=true;
  return true;
}
bool FitAmpPairCovariance::retrieve_sum_xy(const std::string & inDirectory){
  bool dbThis=false;
  if(size() > maxSize()) return false;
  ifstream is(matrix_xy_fname(inDirectory).c_str());
  if(is.bad()) return false;
  if(dbThis) cout << "opened file: "
		  << "\n" << matrix_xy_fname(inDirectory)
		  << endl;
  std::string st1, st2;
  double val;
  map< string, map<string, double> > map_xy;

  int counter=0;
  while(is){
    if(0 == counter){
      is >> st1 >> _Nevents;
    }else{
      is >> st1 >> st2 >> val;
      map_xy[st1][st2] = val;
      if(dbThis) cout << "just read " << counter << ". line: "
		      << st1 << " " << st2 << " " << val << endl;
    }
    counter++;
  }

  is.close();
  for(unsigned int i=0; i < size(); i++){
    for(unsigned int j=0; j < size(); j++){
      _sum_xy(indexReal(i), indexReal(j)) = map_xy[realName(i)][realName(j)];
      _sum_xy(indexReal(i), indexImag(j)) = map_xy[realName(i)][imagName(j)];
      _sum_xy(indexImag(i), indexReal(j)) = map_xy[imagName(i)][realName(j)];
      _sum_xy(indexImag(i), indexImag(j)) = map_xy[imagName(i)][imagName(j)];
    }
  }
  if(dbThis){
    cout << "got _sum_xy: " << endl;
    _sum_xy.Print();
  }
  _needToRecalculate=true;
  return true;
}

FitAmpPairCovariance& FitAmpPairCovariance::operator+=(const FitAmpPairCovariance& other){
  this->add(other);
  return *this;
}
FitAmpPairCovariance FitAmpPairCovariance::operator+(const FitAmpPairCovariance& other) const{
  FitAmpPairCovariance returnVal(*this);
  return (returnVal += other);
}

//
