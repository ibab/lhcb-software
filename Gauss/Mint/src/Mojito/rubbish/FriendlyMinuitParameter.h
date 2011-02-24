#ifndef FRIENDLY_MINUIT_PARAMETER_HH
#define FRIENDLY_MINUIT_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "TMinuit.h"

class FriendlyMinuitParameter{
  TMinuit* _myMinuit;
  int _pN;
  int _iFixInit;
  double _meanInit, _stepInit, _minInit, _maxInit;
 public:
  FriendlyMinuitParameter();
  
  const TMinuit* getMinuit() const;
  TMinuit* getMinuit();
  int iFixInit() const;
  int meanInit() const;
  int stepInit() const;
  int minInit() const;
  int maxInit() const;

  int parNumber() const;
  void setParNumber(int i);
  void associate(TMinuit* tm);

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


}
#endif
//
