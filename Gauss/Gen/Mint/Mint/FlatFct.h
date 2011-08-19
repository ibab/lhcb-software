#ifndef FLAT_FCT_HH
#define FLAT_FCT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "TRandom.h"

//#include "Mint/counted_ptr.h"
#include "Mint/IGenFct.h"

class FlatFct : virtual public IGenFct{
 protected:
  DalitzCoordinate _coord;
  double _boxLimit_min, _boxLimit_max;
  double _min, _max;
  void redoLimits();
  // the limits passed through _coord and changed with setLimits
  // are the limits over
  // which the function is defined, the other limits are on top of that
  // it will return non-zero values if within both limits.
  // Important for the way DalitzBWArea interacts with this.
 public:
  virtual double min() const;
  virtual double max() const;
  
  FlatFct(const DalitzCoordinate& c);
  FlatFct(const FlatFct& other);
  //  virtual counted_ptr<IGenFct> Clone() const;

  virtual void setLimits(double sMin, double sMax);
  virtual void setBoxLimits(double sMin, double sMax);
  virtual void resetBoxLimits();
  virtual double getSMi()const{return _coord.min();}
  virtual double getSMa()const{return _coord.max();}
  virtual double generate(TRandom* rnd=gRandom) const;
  virtual double generateRho(TRandom* rnd=gRandom) const;

  virtual DalitzCoordinate getCoordinate() const;
  virtual void setCoordinate(const DalitzCoordinate& c);
 
  virtual double generatingPDFValue(double sij) const;
  virtual double generatingFctValue(double sij) const;
  virtual double transformedFctValue(double sij) const;
  virtual double transformedFctMax()const;
  virtual double integral()const;
  // normalised to limits passed via DalitzCoordinate & boxLimits
  
  virtual bool flat()const{return true;}

  virtual ~FlatFct(){};
};

#endif
//
