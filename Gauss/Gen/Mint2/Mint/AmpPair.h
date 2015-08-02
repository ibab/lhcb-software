#ifndef AMP_PAIR_HH
#define AMP_PAIR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "Mint/counted_ptr.h"
#include <complex>
#include <string>
#include <map>
#include <iostream>

#include "Mint/DalitzHistoSet.h"
#include "Mint/Amplitude.h"
#include "Mint/FitParDependent.h"
#include "Mint/IFitParRegister.h"

//class FitAmplitude;
class IDalitzEvent;

class AmpPair : public MINT::FitParDependent{

  Amplitude* _a1;
  Amplitude* _a2;

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

  std::complex<double> ampValue(IDalitzEvent* evtPtr);
  int oneOrTwo() const;

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

  bool isCompatibleWith(const AmpPair& other) const;
 public:
  AmpPair(Amplitude* a1=0, Amplitude* a2=0);
  AmpPair(const AmpPair& other);

  bool add(const AmpPair& other);

  const std::string& name() const;
  const std::string& name();
  const std::string& dirName()const;
  const std::string& dirName();

  bool save(const std::string& asSubdirOf=".") const;
  bool retrieve(const std::string& asSubdirOf=".");

  bool isSingleAmp() const;

  std::complex<double> add(IDalitzEvent* evt, double weight=1, double efficiency=1);
  std::complex<double> add(MINT::counted_ptr<IDalitzEvent> evt, double weight=1, double efficiency=1);

  // real part of the above.
  std::complex<double> integral() const;
  std::complex<double> variance() const; // not a "real" variance, but something to calculate it from
  double weightSum() const;
  long int N() const;
  std::complex<double> lastEntry() const;

  DalitzHistoSet histoSet() const;

  Amplitude* amp1(){return _a1;}
  Amplitude* amp2(){return _a2;}
  const Amplitude* amp1() const{return _a1;}
  const Amplitude* amp2() const{return _a2;}
  
  virtual void print(std::ostream& os=std::cout) const;
  
  virtual ~AmpPair(){}

  AmpPair& operator+=(const AmpPair& other);
  AmpPair operator+(const AmpPair& other) const;

  //  friend class AmpPairCovariance;
};

class lessByAmpPairIntegral{
 public:
  bool operator()(const AmpPair& a, const AmpPair& b) const;
};

class lessByAmpPairIntegral_ptr{
 public:
  bool operator()(const AmpPair* a, const AmpPair* b) const;
};


std::ostream& operator<<(std::ostream& os, const AmpPair& fap);

class lessByAmpPairIntegral_ptr_int_pair{
 public:
  bool operator()(const std::pair<AmpPair*, int>& a, const std::pair<AmpPair*, int>& b) const;
};


#endif

