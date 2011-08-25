#ifndef MINT_DALITZ_FITFRACTION_HH
#define MINT_DALITZ_FITFRACTION_HH

#include <string>
#include <iostream>

class FitFraction{
 protected:
  std::string _name;
  double _frac, _sigmaFit, _sigmaInteg;
 public:
  FitFraction();
  FitFraction(const std::string& name
	      , double frac=0
	      , double sigmaFit=-9999.0
	      , double sigmaInteg=-9999.0);
  FitFraction(const FitFraction& other);

  const std::string& name() const{return _name;}

  double& frac(){return _frac;}
  const double& frac()const {return _frac;}

  double& sigmaFit(){return _sigmaFit;}
  const double& sigmaFit() const{return _sigmaFit;}
  
  double& sigmaInteg(){return _sigmaInteg;}
  double sigmaInteg() const{return _sigmaInteg;}

  void print(std::ostream& os) const;
  
};

std::ostream& operator<<(std::ostream& os, const FitFraction& ff);

#endif
//
