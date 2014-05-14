#ifndef DALITZ_BOX_HH
#define DALITZ_BOX_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/MappedDalitzArea.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/DalitzEventList.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/IReturnRealForEvent.h"

#include "Mint/counted_ptr.h"

#include "TRandom.h"

#include <vector>
#include <string>
#include <iostream>

class DalitzBox{
  double _max_s0, _max_s1, _max_s2, _max_s3, _max_s4;
  double _maxPhaseSpace;

  int _number;
  std::string _name;

  bool _ready;

  MappedDalitzArea _area;

  DalitzEventPattern _pat;
  MINT::IReturnRealForEvent<IDalitzEvent>* _amps;

  TRandom* _rnd;
  double _height;
  DalitzEventList _eventList;

  int _nTries;
  int _nEventsForTries;
  const DalitzBox* _daddy;

  double _guessedHeight;
  unsigned int _heightProblems;

  bool _flatPhaseSpace;

  int makeFlatEventsKeepAll(int N);

  bool estimateHeight(std::vector<double>& vals);
  bool estimateHeight_old(std::vector<double>& vals);
  bool estimateHeightMinuit();

  bool makeStarterSet();
  int throwAwayData(const std::vector<double>& vals);

  MINT::counted_ptr<DalitzEvent> tryNewEvent();
  MINT::counted_ptr<DalitzEvent> tryEventFromList();
  MINT::counted_ptr<DalitzEvent> popEventFromList();

  double getEventsPdf(DalitzEvent& evt);
  
  //  void getReady();
 public:
  void getReady();
  
  void setFlatPhaseSpace(bool flat=true){
    _flatPhaseSpace = flat;
  }
  double amps(IDalitzEvent& ep);
  double phaseSpace(IDalitzEvent& ep);
  double ampsWithPhaseSpace(IDalitzEvent& ep);

  DalitzBox();
  DalitzBox(const DalitzEventPattern& pat
	    , MINT::IReturnRealForEvent<IDalitzEvent>* amps = 0
	    , TRandom* rnd = gRandom
	    );
  
  DalitzBox(const DalitzEventPattern& pat
	    , const DalitzCoordinate& limit
	    , MINT::IReturnRealForEvent<IDalitzEvent>* amps =0
	    , TRandom* rnd = gRandom);

  DalitzBox(const DalitzEventPattern& pat
	    , const std::vector<DalitzCoordinate>& limits
	    , MINT::IReturnRealForEvent<IDalitzEvent>* amps =0
	    , TRandom* rnd = gRandom);

  DalitzBox(const DalitzBox& other);
  ~DalitzBox();

  void setNumber(int num){_number = num;}
  int number() const{return _number;}

  void setName(const std::string& name){  _name = name;}
  const std::string& name() const{return _name;}

  void setGuessedHeight(double h){_guessedHeight = h;}
  double guessedHeight() const{return _guessedHeight;}

  const MappedDalitzArea& area()const{return _area;}
  MappedDalitzArea& area(){return _area;}

  double height() const{
    return _height;
  }
  double setHeight(double h){
    return _height = h;
  }
  DalitzEventList& eventList(){ return _eventList;}
  const DalitzEventList& eventList() const{ return _eventList;}
  
  double volume() const;

  void setDaddy(const DalitzBox* daddy);
  bool setAmps(MINT::IReturnRealForEvent<IDalitzEvent>* amps);

  bool insideArea(const DalitzEvent& evt) const;
  bool insideDaddysArea(const DalitzEvent& evt) const;
  bool insideMyOrDaddysArea(const DalitzEvent& evt) const;

  MINT::counted_ptr<DalitzEvent> tryEventForOwner();

  MINT::counted_ptr<DalitzEvent> tryWeightedEventForOwner();

  void encloseInPhaseSpaceArea(){
    area().encloseInPhaseSpaceArea();
  }

  std::vector<DalitzBox> split(unsigned int nWays) const;
  std::vector<DalitzBox> splitIfWiderThan(double maxWidth) const;

  bool setRnd(TRandom* rnd=gRandom);

  void print(std::ostream& os = std::cout) const;
};

std::ostream& operator<<(std::ostream& os, const DalitzBox& box);


#endif
//
