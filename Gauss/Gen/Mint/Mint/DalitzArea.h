#ifndef DALITZAREA_HH
#define DALITZAREA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"

#include "TLorentzVector.h"
#include "TRandom.h"

#include "Mint/counted_ptr.h"

#include <vector>
#include <iostream>

class DalitzEvent;
class IDalitzEvent;

class DalitzArea{

  DalitzEventPattern _pat;

  TRandom* _rnd;

  //  MINT::counted_ptr<DalitzEvent> _evt;

  mutable bool _madeCMap;
  void makeCoordinateMap()const;
  void makeMiMa();

  bool compatiblePattern(const DalitzEventPattern& p)const;
  MINT::counted_ptr<DalitzEvent> make3Event() const;
  MINT::counted_ptr<DalitzEvent> make4Event() const;

  std::vector<DalitzArea> split_in_all_dim_3body(int n=2) const;
  std::vector<DalitzArea> split_in_all_dim_4body(int n=2) const;

 public:
  // for now keep them public:

  // currently storing the same info twice, once as
  // a (flexible) map, once again as seperate variables
  // - the latter mainly for passing it to 
  // the DalitzEvent constructor. 
  // Can remove the latter copy once the DalitzEvent
  // constructor can take some 'coordinate set' or so.

  mutable std::map<DalitzCoordKey, DalitzCoordinate*> _coords;
  mutable DalitzCoordinate _t01, _s12, _s23, _s34, _t40;
  // mutable for debug - remove this and const in makeCoordinate map.

  DalitzArea();
  DalitzArea(const DalitzEventPattern& pat, TRandom* rnd = gRandom);
  DalitzArea(const DalitzArea& other);
  ~DalitzArea();
  
  DalitzArea& operator=(const DalitzArea& other);

  bool setCoordinate(const DalitzCoordinate& c);

  void setPattern(const DalitzEventPattern& pat);

  bool isInside(const IDalitzEvent& evt) const;
  //  bool isInside(const DalitzEvent& evt) const;
  bool isInside(const DalitzCoordinate& dc) const;
  bool isInside(const std::vector<DalitzCoordinate>& dcList) const;

  double size() const;

  MINT::counted_ptr<DalitzEvent> makeEventForOwner() const;
  MINT::counted_ptr<DalitzEvent> makeEventForOwner(double scale0,
					     double scale1,
					     double scale2,
					     double scale3,
					     double scale4
					     ) const;

  /* one day...(3 body)
  MINT::counted_ptr<DalitzEvent> makeEventForOwner(double scale0
					     double scale1
					     );
  */

  void encloseInPhaseSpaceArea(double safetyFactor=1.0);

  void setAllLimitsToPhaseSpaceArea(double safetyFactor=1.0);

  std::vector<DalitzArea> split_in_all_dimensions(int n=2) const;

  void print(std::ostream& os = std::cout) const;

  bool setRnd(TRandom* rnd=gRandom);
};

std::ostream& operator<<(std::ostream& os, const DalitzArea& da);

#endif
//
