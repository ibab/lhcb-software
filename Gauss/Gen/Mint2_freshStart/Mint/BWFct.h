#ifndef BW_FCT_HH
#define BW_FCT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "TRandom.h"

//#include "Mint/counted_ptr.h"
#include "Mint/IGenFct.h"

class BWFct : virtual public IGenFct{
  DalitzCoordinate _coord;
  double _M, _G;

  double rhoMi() const;
  double rhoMa() const;
  double norm() const;

  double unitFactor() const;

 public:
  BWFct(const DalitzCoordinate& c, double M, double Gamma);
  BWFct(const BWFct& other);
  //  virtual MINT::counted_ptr<IGenFct> Clone() const;

  virtual void setLimits(double sMin, double sMax);
  virtual double getSMi()const{return _coord.min();}
  virtual double getSMa()const{return _coord.max();}
  virtual double generateRho(TRandom* rnd=gRandom) const;
  virtual double generate(TRandom* rnd=gRandom) const;

  virtual DalitzCoordinate getCoordinate() const;
  virtual void setCoordinate(const DalitzCoordinate& c);
 
  virtual double generatingPDFValue(double sij) const;
  virtual double generatingFctValue(double sij) const;
  virtual double integral() const;
  // normalised to limits passed via DalitzCoordinate!!!

  virtual bool flat() const{return false;}

  // co-ordinate transformation stuff (by default there is none)
  virtual double coordTransformFromS(double s)const;
  virtual double coordTransformToS(double rho)const;
  virtual double transformedPDFValue(double rho)const;
  virtual double transformedFctValue(double rho)const;
  virtual double transformedFctMax()const;
  virtual double transformedIntegral()const;
  virtual double getRhoMi()const{return rhoMi();}
  virtual double getRhoMa()const{return rhoMa();}

  virtual ~BWFct(){};
};

#endif
//
