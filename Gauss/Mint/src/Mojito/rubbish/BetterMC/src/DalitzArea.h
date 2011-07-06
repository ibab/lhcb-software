#ifndef DALITZAREA_HH
#define DALITZAREA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT

#include "DalitzEventPattern.h"
#include "DalitzCoordinate.h"

#include "TLorentzVector.h"
#include "TRandom.h"

#include "counted_ptr.h"

#include <vector>
#include <iostream>

class DalitzEvent;

class DalitzArea{

  DalitzEventPattern _pat;

  TRandom* _rnd;

  //  counted_ptr<DalitzEvent> _evt;

  mutable bool _madeCMap;
  void makeCoordinateMap()const;
  void makeMiMa();

  counted_ptr<DalitzEvent> make3Event() const;
  counted_ptr<DalitzEvent> make4Event() const;
 public:
  // for now keep them public:

  // currently storing the same info twice, once as
  // a (flexible) map, once again as seperate variables
  // - the latter mainly for passing it to 
  // the DalitzEvent constructor. 
  // Can remove the latter copy once the DalitzEvent
  // constructor can take some 'coordinate set' or so.

  mutable std::map<std::vector<int>, DalitzCoordinate*> _coords;
  mutable DalitzCoordinate _t01, _s12, _s23, _s34, _t40;
  // mutable for debug - remove this and const in makeCoordinate map.

  DalitzArea();
  DalitzArea(const DalitzEventPattern& pat, TRandom* rnd = gRandom);
  DalitzArea(const DalitzArea& other);
  ~DalitzArea();
  
  DalitzArea& operator=(const DalitzArea& other);

  bool setCoordinate(const DalitzCoordinate& c);

  void setPattern(const DalitzEventPattern& pat);
  void setRnd(TRandom* rnd = gRandom);

  bool isInside(const DalitzEvent& evt) const;
  bool isInside(const DalitzCoordinate& dc) const;
  bool isInside(const std::vector<DalitzCoordinate>& dcList) const;

  double size() const;

  counted_ptr<DalitzEvent> makeEventForOwner() const;
  counted_ptr<DalitzEvent> makeEventForOwner(double scale0,
					     double scale1,
					     double scale2,
					     double scale3,
					     double scale4
					     ) const;

  /* one day...(3 body)
  counted_ptr<DalitzEvent> makeEventForOwner(double scale0
					     double scale1
					     );
  */

  void encloseInPhaseSpaceArea();

  void setAllLimitsToPhaseSpaceArea();

  void print(std::ostream& os = std::cout) const;

};

std::ostream& operator<<(std::ostream& os, const DalitzArea& da);

#endif
//
