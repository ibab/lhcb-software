#ifndef DALITZ_BW_BOX_SET_HH
#define DALITZ_BW_BOX_SET_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "DalitzBWBox.h"

#include "IDalitzEvent.h"
#include "IUnweightedEventGenerator.h"

#include "IGetRealEventWithSmoothy.h"
#include "DalitzEventPtrList.h"

#include "counted_ptr.h"

#include <vector>
#include <iostream>

#include "TRandom.h"

class DalitzBWBoxSet : public std::vector<DalitzBWBox>
, virtual public MINT::IUnweightedEventGenerator<IDalitzEvent>{
 protected:
  DalitzEventPtrList _eventPtrList;
  double _maxWeightEstimate;
  double _maxWeightInSample;

  MINT::IGetRealEventWithSmoothy<IDalitzEvent>* _ampSum;

  bool _ready;
  std::vector<double> _volumeProbs;
  TRandom* _rnd;

  double _phaseSpaceProb;
  mutable double _phaseSpaceIntegral;
  
  void getReady();
  void findMax();

  MINT::counted_ptr<DalitzEvent> popEventFromList();

  double VolumeSum()const;
  void makeVolumeProbIntervals();
  int pickRandomVolume();
  
  bool checkIntegration()const;

  bool ready()const {return _ready;}

  double findMaxInList(double& sampleMax);
 public:
  DalitzBWBoxSet(MINT::IGetRealEventWithSmoothy<IDalitzEvent>* amps=0
		 , TRandom* r=gRandom);
  DalitzBWBoxSet(TRandom* r);
  DalitzBWBoxSet(const DalitzBWBoxSet& other);
  void add(DalitzBWBox& box);
  void add(DalitzBWBoxSet& boxes);

  double phaseSpaceProb() const{return _phaseSpaceProb;}

  void setUnWeightPs(bool doSo=true);

  virtual MINT::counted_ptr<DalitzEvent> makeEventForOwner();
  virtual MINT::counted_ptr<DalitzEvent> makeEventForOwner(int& nTries);
  virtual MINT::counted_ptr<DalitzEvent> makeWeightedEventForOwner();
  virtual MINT::counted_ptr<DalitzEvent> makeWeightedEventForOwner(int& nTries);
  virtual MINT::counted_ptr<DalitzEvent> tryEventForOwner();

  virtual MINT::counted_ptr<DalitzEvent> phaseSpaceEvent();

  virtual MINT::counted_ptr<IDalitzEvent> newEvent();
  virtual MINT::counted_ptr<IDalitzEvent> newUnweightedEvent();

  virtual bool exhausted() const{return false;}

  double phaseSpaceIntegral() const;

  double fullPdf(DalitzEvent& evt);

  double genValueNoPs(const DalitzEvent& evt) const;
  double genValuePs(const DalitzEvent& evt) const;
  double genValueWithLoop(DalitzEvent& evt) const;
  double genValue(const DalitzEvent& evt) const;

  void print(std::ostream& os)const;

  bool ensureFreshEvents();
  bool setRnd(TRandom* rnd=gRandom);
};

ostream& operator<<(std::ostream& os, const DalitzBWBoxSet& box);
#endif
//
