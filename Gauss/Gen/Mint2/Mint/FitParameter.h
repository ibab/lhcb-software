#ifndef FIT_PARAMETER_HH
#define FIT_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

//#include "TMinuit.h"
#include "Mint/IMinuitParameter.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/NamedParameterBase.h"
#include "Mint/NamedParameter.h"
#include "Mint/Utils.h"

#include <complex>

/* we assume the following format:

ParName       iFix   Par0        Step   Pmin  Pmax  :: 

Ex            1      0.          1e-4   0.0   0.0
Ey            1      0.          1e-4   0.0   0.0

Where Pmin and Pmax can be left out;

Iff the number of parameters read in after the parameter name
is 6, however, we assume the following format:

ParName       N  iFix   Par0        Step   Pmin  Pmax  :: 

Ex            1  1      0.          1e-4   0.0   0.0
Ey            2  1      0.          1e-4   0.0   0.0

This includes the parameter number N. This is meaningless for us.
But it makes it compatible with Mikhail's input files. However, 
this will not work if you specify N, but leave out any of the other
fields - this is only detected by counting the number of fields
following the parameter name!!
*/

namespace MINT{

class FitParameter : public NamedParameterBase, public IMinuitParameter{
  // copying dangerous because a pointer is held 
  // in MinuitParameterSet
  // therefore for the time being: private.
  // Anyway, what would the copied parameter
  // mean?
  FitParameter(const FitParameter& other);


 protected:
  static const char* _initString;
  
  //  TMinuit* _minPtr;
  MinuitParameterSet* _pset;
  int _psetIndex;

  //  int _pN;
  int _iFixInit;
  double _meanInit, _stepInit, _minInit, _maxInit;

  NamedParameter<double> _scanParameters;

  double _meanResult, _errResult, _errPosResult, _errNegResult;
  double _currentFitVal;

  virtual bool setParSet(MinuitParameterSet* ps);
  virtual bool setParSetIndex(int psetIndex);
  void initToResult();

 public:
  enum FIX_OR_WHAT{FIT=0, HIDE=1, FIX=2};// possible fix options

  static const char* getInitString();

  FitParameter(const std::string& name
	       , const char* fname=0
	       , MinuitParameterSet* pset=0
	       , FIX_OR_WHAT fow=FIX
	       , NamedParameterBase::VERBOSITY fb=VERBOSE
	       );
  
  FitParameter(const std::string& name
	       , int fix
	       , double mean
	       , double step
	       , double mi=0
	       , double ma=0
	       , MinuitParameterSet* pset=0
	       , NamedParameterBase::VERBOSITY fb=VERBOSE
	       , const char* fname = 0
	       );
 // Above: so you can fully initialise the FitParameter in your code, 
  // but have that initialisation been overriden by a parameter file (default: stdin))
  
  FitParameter(const std::string& name
	       , MinuitParameterSet* pset
	       , FIX_OR_WHAT fow=FIX
	       , NamedParameterBase::VERBOSITY fb=VERBOSE
	       );

  virtual ~FitParameter();

  bool addToParSet(MinuitParameterSet* ps);

  virtual const MinuitParameterSet* parSet() const{return _pset;}
  virtual MinuitParameterSet* parSet(){return _pset;}
  virtual int parSetIndex() const;

  virtual bool setFromParsedLine(const ParsedParameterLine& line);

  virtual double getCurrentFitVal() const;
  virtual void setCurrentFitVal(double fv);
  void setResult(double fitMean
		 , double fitErr
		 , double fitErrPos
		 , double fitErrNeg);

  void setCurrentValToInit(); // resets only mean value, leaves errors etc alone
  void resetToInit(); // resets all

  //  virtual bool updateResults();

  //  const TMinuit* getMinuit() const;
  //  TMinuit* getMinuit();
  int iFixInit() const;
  bool hidden() const{return iFixInit() == 1;}
  bool scan() const;
  double scanMin()const;
  double scanMax()const;

  virtual const std::string& name() const{
    return NamedParameterBase::name();
  }
  double meanInit() const;
  double stepInit() const;
  double minInit() const;
  double maxInit() const;
  
  //  int parNumber() const;
  //  void associate(TMinuit* tm, int parNum);

  //  bool MinuitOK() const;
  double valAtLastFCNCall()const;
  double mean()const;
  double min()const;
  double max()const;
  double errPos(); // not const because mnerrs is non-const
  double errNeg(); // same here
  double err()const;

  // fixing and un-fixing the fit parameters
  // note that this will only come into effect
  // when you start a new fit
  // (i.e. if you use Minimiser, when you 
  // call Minimiser::doFit(), or something
  // equivalent - it won't fix or unfix
  // anything while this command is running)
  void fix();
  void fixToInit();
  void fixAndHide();
  void fixToInitAndHide();
  void unFix();

  operator double() const{
    return mean();
  }
  FitParameter& operator=(double d){
    _meanResult=d;
    return *this;
  }
  template<typename T>
    T operator*(const T& rhs) const{
    return ((double) *this) * rhs;
  }
  template<typename T>
    T operator*(const MINT::NamedParameter<T>& rhs) const{
    return ((double) *this) * rhs;
  }
  double operator*(const MINT::FitParameter& rhs) const{
    return ((double) *this) * ((double) rhs);
  }
  
  template<typename T>
    T operator+(const T& rhs) const{
    return ((double) *this) + rhs;
  }
  template<typename T>
    T operator+(const MINT::NamedParameter<T>& rhs) const{
    return ((double) *this) + rhs;
  }
  double operator+(const MINT::FitParameter& rhs) const{
    return ((double) *this) + ((double) rhs);
  }
  
  template<typename T>
    T operator/(const T& rhs) const{
    return ((double) *this) / rhs;
  }
  template<typename T>
    T operator/(const MINT::NamedParameter<T>& rhs) const{
    return ((double) *this) / rhs;
  }
  double operator/(const MINT::FitParameter& rhs) const{
    return ((double) *this) / ((double) rhs);
  }
  
  template<typename T>
    T operator-(const T& rhs) const{
    return ((double) *this) -  rhs;
  }
  template<typename T>
    T operator-(const MINT::NamedParameter<T>& rhs) const{
    return ((double) *this) -  rhs;
  }
  double operator-(const MINT::FitParameter& rhs) const{
    return ((double) *this) - ((double) rhs);
  }


  /*  operator std::complex<double>() const{
    return std::complex<double>(mean(),0);
    }*/


  void defaultInit();
  // normally better not to use because if you initialise
  // it, it won't complain in the destructor when it didn't find itself
  // in the file.
  
  virtual void print(std::ostream& os = std::cout) const;
  virtual void printVal(std::ostream& os = std::cout) const;
  virtual void printResultVsInput(std::ostream& os = std::cout) const;
  
  static void printFormat(std::ostream& os = std::cout, int pad=0);
  static void printResultFormat(std::ostream& os = std::cout, int pad=0);
};
  
}//namespace MINT

std::ostream& operator<<(std::ostream& os, const MINT::FitParameter& fp);

template<typename T>
double operator*(const MINT::NamedParameter<T>& lhs, const MINT::FitParameter& fp) {
  return ((T) lhs) * ((double) fp);
}
template<typename T>
double operator+(const MINT::NamedParameter<T>& lhs, const MINT::FitParameter& fp) {
  return ((T) lhs) + ((double) fp);
}
template<typename T>
double operator-(const MINT::NamedParameter<T>& lhs, const MINT::FitParameter& fp) {
  return ((T) lhs) - ((double) fp);
}
template<typename T>
double operator/(const MINT::NamedParameter<T>& lhs, const MINT::FitParameter& fp) {
  return ((T) lhs) / ((double) fp);
}

template<typename T>
T operator*(const T& lhs, const MINT::FitParameter& fp) {
  return lhs * ((double) fp);
}

template<typename T>
T operator+(const T& lhs, const MINT::FitParameter& fp) {
  return lhs + ((double) fp);
}

template<typename T>
T operator/(const T& lhs, const MINT::FitParameter& fp) {
  return lhs/((double) fp);
}

template<typename T>
T operator-(const T& lhs, const MINT::FitParameter& fp) {
  return lhs - ((double) fp);
}

#endif
//
