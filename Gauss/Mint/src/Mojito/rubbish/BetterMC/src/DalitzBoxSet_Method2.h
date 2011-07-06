#ifndef DALITZ_BOX_SET_METHOD_TWO_HH
#define DALITZ_BOX_SET_METHOD_TWO_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "DalitzBoxSet.h"
#include "DalitzEventPattern.h"
#include "TRandom.h"

class DalitzBoxSet_Method2 : public DalitzBoxSet{

  DalitzEventPattern _pat;

  double _maxHeight;
  void setBoxesToFlat();
  void getMaxHeight();
  void setEmptyBoxesHeight();
  virtual void getReady();

  double eventsPDF(DalitzEvent& evt);
 public:
  DalitzBoxSet_Method2(const DalitzEventPattern& pat
		       , TRandom* rnd=gRandom);

  DalitzBoxSet_Method2(const DalitzBoxSet& boxSet
		       , const DalitzEventPattern& pat
		       );

  double maxHeight() const;

  virtual counted_ptr<DalitzEvent> tryEvent();
  //  virtual counted_ptr<DalitzEvent> generateEventForOwner();
  //  virtual DalitzEvent generateEvent();

  virtual ~DalitzBoxSet_Method2(){}
};

#endif
//
