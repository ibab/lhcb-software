#ifndef IGENFCT_HH
#define IGENFCT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Mint/DalitzCoordinate.h"
//#include "Mint/counted_ptr.h"

class TRandom;

class IGenFct{
 public:
  virtual void setLimits(double sMin, double sMax)=0;
  virtual double getSMi()const=0;
  virtual double getSMa()const=0;
  virtual double generate(TRandom* rnd) const=0;
  virtual double generateRho(TRandom* rnd) const=0;

  virtual DalitzCoordinate getCoordinate() const=0;
  virtual void setCoordinate(const DalitzCoordinate& c)=0;
  virtual double generatingPDFValue(double sij) const=0; 
  virtual double generatingFctValue(double sij) const=0; 
  virtual double integral() const=0; 

  //  virtual MINT::counted_ptr<IGenFct> Clone() const=0;

  virtual bool flat()const{return false;}

  // co-ordinate transformation stuff for efficient integration
  // default implementation cover case where there is no
  // coordinate tranformation implemented.
  virtual double coordTransformFromS(double s)const{return s;}
  virtual double coordTransformToS(double rho)const{return rho;}
  virtual double transformedFctValue(double rho)const{
    return generatingFctValue(rho); // no transformation
  };
  virtual double transformedFctMax()const=0;
  virtual double getRhoMi()const{return coordTransformFromS(getSMi());}
  virtual double getRhoMa()const{return coordTransformFromS(getSMa());}

  virtual ~IGenFct(){};
};

#endif
//
