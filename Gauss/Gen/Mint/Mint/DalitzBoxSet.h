// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#ifndef DALITZ_BOX_SET
#define DALITZ_BOX_SET

#include "Mint/DalitzBox.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IEventGenerator.h"

#include "Mint/counted_ptr.h"

#include "TRandom.h"

#include <vector>
#include <iostream>

class DalitzBoxSet 
: public std::vector<DalitzBox>
, virtual public MINT::IEventGenerator<IDalitzEvent>
{

 protected:
  bool _ready;
  std::vector<double> _volumeProbs;
  MINT::IGetRealEvent<IDalitzEvent>* _amps;

  TRandom* _rnd;


  double VolumeSum() const;
  double AreaSum() const;
  void makeVolumeProbIntervals();
  int pickRandomVolume();

  void setBoxPDFs();
  void setBoxRnds();
  void getBoxesReady();
  virtual void getReady();

 public:
  // the next two should be private
  // public for now for debugging:
  void makeRelations();
  void removeDuplicates();

  void sortYourself();
  void callSortYourselfForDebug();

  DalitzBoxSet(TRandom* rnd = gRandom);
  DalitzBoxSet(const DalitzBoxSet& other);

  void add(const DalitzBox& box);
  void add(const std::vector<DalitzBox>& boxes);
  void add(const DalitzBoxSet& boxes);
  void setPDF(MINT::IGetRealEvent<IDalitzEvent>* amps);

  DalitzBoxSet split(unsigned int nWays)const;
  DalitzBoxSet splitIfWiderThan(double maxWidth)const;
  
  virtual MINT::counted_ptr<DalitzEvent> tryEvent();
  virtual MINT::counted_ptr<DalitzEvent> tryWeightedEvent();
  virtual MINT::counted_ptr<DalitzEvent> generateEventForOwner();
  virtual DalitzEvent generateEvent();

  virtual MINT::counted_ptr<IDalitzEvent> newEvent();

  virtual void print(std::ostream& os=std::cout) const;

  
  virtual bool exhausted()const{return false;}
  bool setRnd(TRandom* rnd);
  virtual bool ensureFreshEvents();
  virtual ~DalitzBoxSet(){}
};

std::ostream& operator<<(std::ostream& os, const DalitzBoxSet& db);

#endif
//
