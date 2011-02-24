#ifndef FIT_PARAMETERLI_HH
#define FIT_PARAMETERLI_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "TMinuit.h"
#include "IMinuitParameter.h"
#include "MinuitParameterSet.h"
#include "NamedParameterBase.h"
#include "Utils.h"



class FitParameterli : public NamedParameterBase, public IMinuitParameter{
 protected:
  TMinuit* _myMinuit;
  MinuitParameterSet* _pset;
  int _pN;
  int _iFixInit;
  double _meanInit, _stepInit, _minInit, _maxInit;

  // copying dangersou because a pointer is held in MinuitParameterSet
  // therfore for the time being: private.
  FitParameterli(const FitParameterli& other);
 public:

#ifndef __CINT__
  FitParameterli(const std::string& name
	       , MinuitParameterSet* pset=0);
  virtual ~FitParameterli();

  virtual bool setFromParsedLine(const ParsedParameterLine& line);
  
  const TMinuit* getMinuit() const;
  TMinuit* getMinuit();
  int iFixInit() const;
  virtual const std::string& name() const{
    return NamedParameterBase::name();
  }
  double meanInit() const;
  double stepInit() const;
  double minInit() const;
  double maxInit() const;

  int parNumber() const;
  void associate(TMinuit* tm, int parNum);

  bool MinuitOK() const;
  double mean()const;
  double min()const;
  double max()const;
  double errPos()const;
  double errNeg()const;
  double err()const;

  operator double() const{
    return mean();
  }

  virtual void print(std::ostream& os = std::cout) const;
  virtual void printMore(std::ostream& os = std::cout) const;
#endif

};

#ifndef __CINT__
std::ostream& operator<<(std::ostream& os, const FitParameterli& fp){
  fp.print(os);
  return os;
}
#endif

#endif
//
