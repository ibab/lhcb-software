#ifndef DALITZ_BOX_SET_METHOD_TWO_HH
#define DALITZ_BOX_SET_METHOD_TWO_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzEventPattern.h"
#include "TRandom.h"

#include "Mint/IEventGenerator.h"

class DalitzBoxSet_Method2 
: public DalitzBoxSet
, virtual public MINT::IEventGenerator<IDalitzEvent>
{

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
  DalitzBoxSet_Method2(const DalitzBoxSet_Method2& other);

  double maxHeight() const;

  virtual MINT::counted_ptr<DalitzEvent> tryEvent();
  virtual MINT::counted_ptr<DalitzEvent> tryWeightedEvent();
  virtual MINT::counted_ptr<IDalitzEvent> newEvent();
 
  //  virtual MINT::counted_ptr<DalitzEvent> generateEventForOwner();
  //  virtual DalitzEvent generateEvent();
  
  virtual bool exhausted()const {return false;}
  virtual ~DalitzBoxSet_Method2(){}
};

#endif
//
