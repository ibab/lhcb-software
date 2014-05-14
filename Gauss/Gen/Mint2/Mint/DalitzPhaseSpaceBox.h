#ifndef DALITZ_PHASESPACE_BOX_HH
#define DALITZ_PHASESPACE_BOX_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/DalitzEventList.h"

#include "Mint/IGenFct.h"

#include "Mint/counted_ptr.h"

#include "TRandom.h"

#include <vector>
#include <string>
#include <iostream>

class DalitzPhaseSpaceBox{
  std::string _name;
  DalitzEventPattern _pat;
  TRandom* _rnd;
  double _height;
  double _weight;
  double _phaseSpaceIntegral;

  double doPhaseSpaceIntegral();
  MINT::counted_ptr<DalitzEvent> tryNewEvent();
 public:
  double& height(){ return _height;}
  const double& height() const{ return _height;}

  void setWeight(double w){_weight = w;} // every event gets the same.
  double weight() const{return _weight;}
  double& weight(){return _weight;}

  DalitzPhaseSpaceBox(TRandom* rnd = gRandom);
  DalitzPhaseSpaceBox(const DalitzEventPattern& pat
		      , TRandom* rnd = gRandom
		      );
  DalitzPhaseSpaceBox(const DalitzPhaseSpaceBox& other);
  ~DalitzPhaseSpaceBox();

  const DalitzEventPattern& pattern()const{return _pat;}
  void setPattern(const DalitzEventPattern& pat);

  void setName(const std::string& name){  _name = name;}
  const std::string& name() const{return _name;}

  double volume() const;

  double genValue() const;
  double genValue(const DalitzEvent& evt) const;

  bool insideArea(const DalitzEvent& evt) const;

  MINT::counted_ptr<DalitzEvent> tryEventForOwner();
  MINT::counted_ptr<DalitzEvent> makeEventForOwner();

  bool setRnd(TRandom* rnd=gRandom);

  double phaseSpaceIntegral() const;
  double phaseSpaceIntegral(); // re-evaluates if necessary

  bool checkIntegration() const;

  void print(std::ostream& os = std::cout) const;

};

std::ostream& operator<<(std::ostream& os, const DalitzPhaseSpaceBox& box);


#endif
//
