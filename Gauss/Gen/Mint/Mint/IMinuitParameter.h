#ifndef IMINUIT_PARAMETER_HH
#define IMINUIT_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <iostream>

//class TMinuit;

namespace MINT{

class MinuitParameterSet;

class IMinuitParameter{
 protected:
  IMinuitParameter(){};
  virtual bool setParSet(MinuitParameterSet* ps)=0;
  virtual bool setParSetIndex(int psetIndex)=0;
  virtual double blinding() const=0;
 public:
  virtual int iFixInit() const=0;
  virtual bool hidden() const=0;
  virtual const std::string& name() const=0;
  virtual double meanInit() const=0;
  virtual double stepInit() const=0;
  virtual double minInit() const=0;
  virtual double maxInit() const=0;

  virtual bool scan() const=0;
  virtual double scanMin() const=0;
  virtual double scanMax() const=0;

  virtual double mean() const=0;
  virtual double err() const=0;

  //  virtual void associate(TMinuit* tm, int parNumber)=0;

  virtual const MinuitParameterSet* parSet() const=0;
  virtual int parSetIndex() const=0;

  virtual void setCurrentFitVal(double pval)=0;

  virtual void setResult(double fitMean
			 , double fitErr
			 , double fitErrPos
			 , double fitErrNeg)=0;

  //  virtual void endOfFit()=0;

  virtual operator double() const=0;

  virtual void print(std::ostream& os = std::cout) const=0;
  virtual void printResultVsInput(std::ostream& os = std::cout) const=0;

  virtual ~IMinuitParameter(){}

  friend class MinuitParameterSet;
  friend class Minimiser;

};
}//namespace MINT

#endif
//
