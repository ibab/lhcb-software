#ifndef DALITZ_EVENT_BASE_HH
#define DALITZ_EVENT_BASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <iostream>
#include <string>
#include <vector>

#include "TLorentzVector.h"
#include "TVector3.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/Calculate4BodyProps.h"

#include "Mint/RememberAnything.h"

#include "Mint/Permutator.h"

class TRandom;
class TNtupleD;

class DalitzEventBase : virtual public IDalitzEvent{
 protected:

  static const char ntpNameChars[];
  static const char prtNameChars[];
  static std::string prtToNtpName(const std::string& s_in);
  static std::string ntpToPrtName(const std::string& s_in);

  static int singleParticleNtpArraySize();

  // begin all data members:
  DalitzEventPattern          _pat;
  std::vector<TLorentzVector> _p;

  mutable double _rememberPhaseSpace;

  RememberAnything<std::complex<double> >  _rememberAmps;

  double _aValue;
  double _weight;
  double _generatorPdfRelativeToPhaseSpace;

  mutable std::vector< std::vector<double> > _s;
  mutable std::vector< std::vector<double> > _t;

  int _permutationIndex;
  Permutator _perm;

  // end all data members;


  const TLorentzVector& p_intern(unsigned int i) const; // 4-vectors
  double s_intern(unsigned int i, unsigned int j) const;       // sij
  double t_intern(unsigned int i, unsigned int j) const;       // tij

  bool shoutAndKill();
  double BDet() const;
  double Delta4() const;
  double phaseSpace3(double epsilon = 1.e-9) const;
  double phaseSpace4() const;
  bool setMomenta(const std::vector<TLorentzVector>& mumAndDgtr_p4);
  bool setMomenta(const std::vector<TVector3>& mumAndDgtr_p3);
  void setP(unsigned int i, const TLorentzVector& p4);
public:
  DalitzEvent();
  DalitzEvent(const DalitzEventPattern& pat
	      , const std::vector<TLorentzVector> mumAndDgtr_p4
	      );
  DalitzEvent(const DalitzEventPattern& pat
	      , double t01_in
	      , double s12_in, double s23_in, double s34_in
	      , double t40_in
	      );
  DalitzEvent(const DalitzEventPattern& pat
	      , const std::vector<TVector3> mumAndDgtr_p4
	      );
  DalitzEvent(const DalitzEventPattern& pat
	      );
  DalitzEvent(const DalitzEventPattern& pat
	      , TRandom* rnd
	      );
  DalitzEvent(const DalitzEvent& other);

  virtual double getWeight() const;
  virtual void   setWeight(double w);

  virtual void   setGeneratorPdfRelativeToPhaseSpace(double gpdf);
  virtual double getGeneratorPdfRelativeToPhaseSpace()const;

  bool resetST();

  const Permutation& currentPermutation() const{
    return _perm[_permutationIndex];
  }

  void setPermutationIndex(int i);
  int numPermutations() const;
  int permutationIndex() const{
    return _permutationIndex;
  }


  DalitzEvent(TNtupleD* ntp);

  bool fromNtuple(TNtupleD* ntp);

  // required
  virtual void setAValue(double a){_aValue=a;}
  virtual double getAValue()const{return _aValue;}

  virtual const DalitzEventPattern& eventPattern() const{return _pat;}
  virtual const TLorentzVector& p(unsigned int i) const; // 4-vectors
  virtual double s(unsigned int i, unsigned int j) const;       // sij
  virtual double t(unsigned int i, unsigned int j) const;       // tij
  virtual double sij(const std::vector<int>& indices) const;

  virtual double phaseSpace() const;

  virtual bool retrieveComplex(void* key, std::complex<double>& value){
    return _rememberAmps.find(key, value);
  }

  virtual void setComplex(void* key, const std::complex<double>& value){
    _rememberAmps.set(key, value);
  }
 
  // helpful
  double sijMin(const std::vector<int>& indices) const;
  double sijMax(const std::vector<int>& indices) const;

  int nDgtr() const{
    return _pat.size()-1;
  }
  double m(unsigned int i) const;  // nominal masses (from _pat)
  double m2(unsigned int i) const; // nominal masses squared
  TLorentzVector p_dgtr_sum() const;
  bool kinematicallyAllowed(double epsilon = 1.e-9) const;

  void print(std::ostream& os = std::cout) const;
  // generate yourself:
  void generateThisToPhaseSpace(TRandom* rnd = 0);
  void generateThisToPhaseSpace(double p_of_D, TRandom* rnd = 0);
  
  void generateThisToPhaseSpace(const TVector3& pVec_of_D
				, TRandom* rnd = 0);


  std::string makeNtupleVarnames()const;
  bool fillNtupleVarArray(Double_t* array, unsigned int arraySize) const;
  unsigned int ntupleVarArraySize() const;

  // mainly for debugging and x-check with prev version
  Calculate4BodyProps makeCalculate4BodyProps() const;
  

};

std::ostream& operator<<(std::ostream& os, const DalitzEvent& de);

#endif
//
