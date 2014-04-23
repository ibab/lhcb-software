#ifndef EVENT_COUNTER_HH
#define EVENT_COUNTER_HH

#include "TH2D.h"
#include <string>

class EventCounter{
  long int _numEvents;
  double _eventSum;
  double _weightSum;
  double _weightSumSq;

  double _ptmin, _ptmax, _ymin, _ymax;
  int _nbins_pt, _nbins_y;

  std::string _name;

  double _uncertaintyE, _uncertaintyW;

  TH2D _h, _hw;


  std::string rootFname() const{
    return _name + ".root";
  }
  double uncertaintyEFromEventNumbers() const;
  double uncertaintyWFromEventNumbers() const;

  double sigmaRatio(double N, double sigN, double D, double sigD) const;

 public:
  EventCounter(std::string name);
  EventCounter(const EventCounter& other
	       , const std::string& newName="");
  ~EventCounter();
  void addEvent(double pt, double y, double w=1);
  void divide(const EventCounter& other);
  void drawAndSave();
  void finish();

  double uncertaintyE() const;
  double uncertaintyW() const;


};
#endif
//
