#ifndef IDALITZ_EVENT_HH
#define IDALITZ_EVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "TLorentzVector.h"
#include "TVector3.h"

#include "Mint/DalitzEventPattern.h"

#include <vector>
#include <iostream>
#include <complex>

class IDalitzEvent{
 protected:
  IDalitzEvent(){}
 public:
  virtual void setAValue(double aValue)=0;
  virtual double getAValue()const=0;

  virtual void setWeight(double w)=0;
  virtual double getWeight()const=0;

  virtual void   setGeneratorPdfRelativeToPhaseSpace(double gpdf)=0;
  virtual double getGeneratorPdfRelativeToPhaseSpace()const=0;

  virtual const DalitzEventPattern& eventPattern() const=0;
  virtual const TLorentzVector& p(unsigned int i) const= 0;

  virtual void setMothers3Momentum(const TVector3& mp3)=0;

  virtual double s(unsigned int i, unsigned int j) const= 0;
  virtual double sij(const std::vector<int>& indices) const= 0;
  virtual double t(unsigned int i, unsigned int j) const= 0;
  
  virtual double sijMin(const std::vector<int>& indices) const=0;
  virtual double sijMax(const std::vector<int>& indices) const=0;

  virtual double phaseSpace() const= 0;

  virtual void print(std::ostream& os = std::cout) const=0;

  virtual bool retrieveComplex(void* key, std::complex<double>& value)=0;
  virtual void setComplex(void* key, const std::complex<double>& value)=0;

  virtual int numPermutations() const=0;
  virtual void setPermutationIndex(int i)=0;

  virtual IDalitzEvent* clone() const=0;
  virtual ~IDalitzEvent(){}
};

bool EqualEvent(const IDalitzEvent* a, const IDalitzEvent* b);

std::ostream& operator<<(std::ostream& os, const IDalitzEvent& de);

#endif
//
