#ifndef DALITZ_BW_BOX_HH
#define DALITZ_BW_BOX_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Mint/MappedDalitzBWArea.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/DalitzEventList.h"

#include "Mint/IGenFct.h"

#include "Mint/counted_ptr.h"

#include "TRandom.h"

#include <vector>
#include <string>
#include <iostream>

class DalitzBWBox{
  std::string _name;

  MappedDalitzBWArea _area;

  DalitzEventPattern _pat;
  MINT::IReturnRealForEvent<IDalitzEvent>* _amps;

  TRandom* _rnd;
  double _height;

  MINT::counted_ptr<DalitzEvent> tryNewEvent();

 public:
  double& height(){ return _height;}
  const double& height() const{ return _height;}
    

  DalitzBWBox(TRandom* rnd = gRandom);
  DalitzBWBox(const DalitzEventPattern& pat
	    , MINT::IReturnRealForEvent<IDalitzEvent>* amps = 0
	    , TRandom* rnd = gRandom
	    );

  DalitzBWBox(const DalitzEventPattern& pat
	    , const MINT::counted_ptr<IGenFct>& fct
	    , MINT::IReturnRealForEvent<IDalitzEvent>* amps =0
	    , TRandom* rnd = gRandom);

  DalitzBWBox(const DalitzEventPattern& pat
	    , const std::vector<MINT::counted_ptr<IGenFct> >& limits
	    , MINT::IReturnRealForEvent<IDalitzEvent>* amps =0
	    , TRandom* rnd = gRandom);

  DalitzBWBox(const DalitzBWBox& other);
  ~DalitzBWBox();

  DalitzEventPattern& pattern(){return _pat;}
  const DalitzEventPattern& pattern()const{return _pat;}

  void setName(const std::string& name){  _name = name;}
  const std::string& name() const{return _name;}

  const MappedDalitzBWArea& area()const{return _area;}
  MappedDalitzBWArea& area(){return _area;}

  double volume() const;

  bool checkIntegration() const{
    return _area.checkIntegration();}

  double genValue(const DalitzEvent& evt) const;

  void setUnWeightPs(bool doSo=true){_area.setUnWeightPs(doSo);}
  bool unWeightPs(){return _area.unWeightPs();}


  bool setAmps(MINT::IReturnRealForEvent<IDalitzEvent>* amps);

  bool insideArea(const DalitzEvent& evt) const;

  MINT::counted_ptr<DalitzEvent> tryEventForOwner();
  MINT::counted_ptr<DalitzEvent> makeEventForOwner();
  MINT::counted_ptr<DalitzEvent> makeEventForOwner(int& nTries);

  bool setRnd(TRandom* rnd=gRandom);

  void print(std::ostream& os = std::cout) const;

};

ostream& operator<<(ostream& os, const DalitzBWBox& box);


#endif
//
