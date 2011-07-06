// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT
#ifndef DALITZ_BOX_SET
#define DALITZ_BOX_SET

#include "DalitzBox.h"
#include "IGetRealEvent.h"

#include "counted_ptr.h"

#include "TRandom.h"

#include <vector>
#include <iostream>

class DalitzBoxSet : public std::vector<DalitzBox>{

 protected:
  bool _ready;
  std::vector<double> _volumeProbs;
  IGetRealEvent<IDalitzEvent>* _amps;

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
  void setPDF(IGetRealEvent<IDalitzEvent>* amps);

  DalitzBoxSet split(unsigned int nWays)const;
  DalitzBoxSet splitIfWiderThan(double maxWidth)const;
  
  virtual counted_ptr<DalitzEvent> tryEvent();
  virtual counted_ptr<DalitzEvent> generateEventForOwner();
  virtual DalitzEvent generateEvent();

  virtual void print(std::ostream& os=std::cout) const;

  virtual ~DalitzBoxSet(){}
};

std::ostream& operator<<(std::ostream& os, const DalitzBoxSet& db);

#endif
//
