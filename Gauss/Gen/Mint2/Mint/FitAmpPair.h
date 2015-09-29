#ifndef FIT_AMP_PAIR_HH
#define FIT_AMP_PAIR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "Mint/counted_ptr.h"
#include <complex>
#include <string>
#include <map>
#include <iostream>

#include "Mint/DalitzHistoSet.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitParDependent.h"
#include "Mint/IFitParDependent.h"
#include "Mint/AmpPair.h"

//class FitAmplitude;
class IDalitzEvent;

class FitAmpPair : public MINT::FitParDependent{

  FitAmplitude* _fitA1;
  FitAmplitude* _fitA2;

  bool _slow;
  bool _beingIntegrated;
  MINT::FitParDependent _eventDependentParameters;

  std::complex<double> _sum;
  std::string _sumName;

  std::complex<double> _sumsq;
  std::string _sumSqName;

  long int _Nevents;
  std::string _NName;

  double _weightSum;
  std::string _weightSumName;

  DalitzHistoSet _hsRe;
  DalitzHistoSet _hsIm;

  std::string _name;
  std::string _dirName;

  std::complex<double> _lastEntry;

  inline std::complex<double> ampValue(IDalitzEvent& evtPtr){
    std::complex<double> c1 = rawAmp1().getVal(evtPtr);
    if(0.0 == c1) return 0;
    std::complex<double> c2 = rawAmp2().getVal(evtPtr);
    return (c1 * conj(c2));  // c1 x c2*
  }
  
  inline std::complex<double> fitParValue()const{
    std::complex<double> c1 = fitAmp1().AmpPhase();
    if(0.0 == c1) return 0;
    std::complex<double> c2 = fitAmp2().AmpPhase();
    return c1* conj(c2); // c1 x c2*
  }
  
  inline int oneOrTwo()const{
    if(isSingleAmp()) return 1;
    else return 2;
  }  
  
  //  std::complex<double> ampValue(IDalitzEvent& evt);
  //  int oneOrTwo() const;

  const DalitzHistoSet& histosRe() const{return _hsRe;}
  const DalitzHistoSet& histosIm() const{return _hsIm;}
  DalitzHistoSet& histosRe(){return _hsRe;}
  DalitzHistoSet& histosIm(){return _hsIm;}
  /*
  double ReSquared() const;
  double ImSquared() const;
  double ImRe() const;
  */

  void addToHistograms(IDalitzEvent* evtPtr, const std::complex<double>& c);

  const std::string& makeName();
  const std::string& makeDirName();

  bool makeDirectory(const std::string& asSubdirOf = ".")const;
  std::string valueFileName(const std::string& asSubdirOf) const;
  std::string histoReFileName(const std::string& asSubdirOf) const;
  std::string histoImFileName(const std::string& asSubdirOf) const;
  bool saveValues(const std::string& asSubdirOf = ".") const;
  bool retrieveValues(const std::string& fromDirectory = ".");
  bool saveHistos(const std::string& asSubdirOf = ".") const;
  bool retrieveHistos(const std::string& asSubdirOf = ".");

  bool isCompatibleWith(const FitAmpPair& other) const;

  void rememberEventDependentParameters();
 public:
  FitAmpPair();// required by some STL containers
  FitAmpPair(FitAmplitude& a1, FitAmplitude& a2);
  FitAmpPair(const FitAmpPair& other);

  bool add(const FitAmpPair& other);

  const std::string& name() const;
  const std::string& name();
  const std::string& dirName()const;
  const std::string& dirName();

  bool save(const std::string& asSubdirOf=".") const;
  bool retrieve(const std::string& asSubdirOf=".");

  bool isSingleAmp() const;

  double reAdd(IDalitzEvent& evt, double weight=1, double efficiency=1);
  double add(IDalitzEvent& evt, double weight=1, double efficiency=1);
  double add(IDalitzEvent* evt, double weight=1, double efficiency=1);
  double add(MINT::counted_ptr<IDalitzEvent> evt, double weight=1, double efficiency=1);

  // integral of things that depend on position in Dalitz space:
  std::complex<double> valNoFitPars() const;

  inline std::complex<double> complexVal() const{
    return valNoFitPars() * fitParValue();
  }

  // factors that that don't
  //std::complex<double> fitParValue() const;
  // product of the above
  //  std::complex<double> complexVal() const;
  // real part of the above.
  double integral() const;
  double variance() const;
  double weightSum() const;
  long int N() const;

  void reset();
  bool needToReIntegrate() const;
  void startReIntegration();
  void startIntegration();
  void endIntegration();
  bool acceptEvents() const;

  std::complex<double> lastEntry() const;

  DalitzHistoSet histoSetRe() const; // these two are...
  DalitzHistoSet histoSetIm() const; // ... not usually used...
  inline DalitzHistoSet histoSet() const{return histoSetRe();} // ..this is.

  FitAmplitude& fitAmp1(){
    if(0 == _fitA1){
      std::cout << "ERROR in FitAmpPair::fitAmp1: zero pointer " << std::endl;
      throw "zero ptr";
    }
    return *_fitA1;
  }
  FitAmplitude& fitAmp2(){
    if(0 == _fitA2){
      std::cout << "ERROR in FitAmpPair::fitAmp2: zero pointer " << std::endl;
      throw "zero ptr";
    }
    return *_fitA2;
  }
  const FitAmplitude& fitAmp1() const{return *_fitA1;}
  const FitAmplitude& fitAmp2() const{return *_fitA2;}
  
  Amplitude& rawAmp1(){return fitAmp1().amp();}
  Amplitude& rawAmp2(){return fitAmp2().amp();}
  const Amplitude& rawAmp1() const{return fitAmp1().amp();}
  const Amplitude& rawAmp2() const{return fitAmp2().amp();}
  

  void setSlow(){_slow = true;}
  void setFast(){_slow = false;}

  bool slow() const{return _slow;}
  bool fast() const{return !_slow;}

  virtual void print(std::ostream& os=std::cout) const;
  
  virtual ~FitAmpPair(){}

  FitAmpPair& operator+=(const FitAmpPair& other);
  FitAmpPair operator+(const FitAmpPair& other) const;

  friend class FitAmpPairCovariance;
};

class lessByFitAmpPairIntegral{
 public:
  bool operator()(const FitAmpPair& a, const FitAmpPair& b) const;
};

class lessByFitAmpPairIntegral_ptr{
 public:
  bool operator()(const FitAmpPair* a, const FitAmpPair* b) const;
};


std::ostream& operator<<(std::ostream& os, const FitAmpPair& fap);

class lessByFitAmpPairIntegral_ptr_int_pair{
 public:
  bool operator()(const std::pair<FitAmpPair*, int>& a, const std::pair<FitAmpPair*, int>& b) const;
};


#endif

