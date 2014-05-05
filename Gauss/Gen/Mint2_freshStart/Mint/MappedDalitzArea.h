#ifndef MAPPED_DALITZ_AREA_HH
#define MAPPED_DALITZ_AREA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/DalitzArea.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/Permutation.h"
#include "Mint/DalitzEvent.h"

#include "TRandom.h"
#include "TLorentzVector.h"

#include "Mint/counted_ptr.h"

#include <vector>
#include <iostream>

class MappedDalitzArea{
  TRandom* _rnd;

  DalitzEventPattern _pat, _mappedPat;
  
  DalitzArea _area;

  std::vector<DalitzCoordinate> _limits;

  Permutation _mapping, _inverseMapping;

  void setup();
  void findMapping();
  void applyLimits();

  bool allConsecutive() const;
  bool allStandardised() const;
  static std::vector<TLorentzVector> mapP4(const DalitzEvent& evt
					   , const Permutation& mapping
					   );
  static std::vector<TLorentzVector>& mapP4(const DalitzEvent& evt
					    , const Permutation& mapping
					    , std::vector<TLorentzVector>& p4
					    );
 public:
  MappedDalitzArea();
  MappedDalitzArea(const DalitzEventPattern& pat
		   , TRandom* rnd=gRandom);
  MappedDalitzArea(const DalitzEventPattern& pat
		   , const DalitzCoordinate& oneLimit
		   , TRandom* rnd=gRandom);
  MappedDalitzArea(const DalitzEventPattern& pat
		   , const DalitzCoordinate& oneLimit
		   , const DalitzCoordinate& otherLimit
		   , TRandom* rnd=gRandom);
  MappedDalitzArea(const DalitzEventPattern& pat
		   , const std::vector<DalitzCoordinate>& limits
		   , TRandom* rnd=gRandom);

  MappedDalitzArea(const MappedDalitzArea& other);

  virtual ~MappedDalitzArea();

  MINT::counted_ptr<DalitzEvent> makeEventForOwner() const;
  MINT::counted_ptr<DalitzEvent> makeEventForOwner(double scale0,
					     double scale1,
					     double scale2,
					     double scale3,
					     double scale4
					     ) const;

  std::vector<DalitzCoordinate> centre() const;
  bool isInside(const DalitzEvent& evt) const;
  bool isInside(const DalitzCoordinate& dc) const;
  bool isInside(const std::vector<DalitzCoordinate>& dcList) const;

  double size() const{
    return _area.size();}

  void print(std::ostream& os = std::cout) const;

  void encloseInPhaseSpaceArea(){
    _area.encloseInPhaseSpaceArea();
  }
  std::vector<MappedDalitzArea> split(unsigned int nWays) const;
  std::vector<MappedDalitzArea> split(unsigned int limitIndex
				       , unsigned int nWays) const;

  std::vector<MappedDalitzArea> splitIfWiderThan(double maxWidth) const;
  std::vector<MappedDalitzArea> splitIfWiderThan(unsigned int limitIndex
						 , double maxWidth) const;


  DalitzCoordinate limit_s(const std::vector<int>& indices) const;
  DalitzCoordinate limit_s(int i, int j, int k=-1) const;

  bool setRnd(TRandom* rnd=gRandom);
  
  bool similar(const MappedDalitzArea& ma) const;
  bool less(const MappedDalitzArea& ma) const;

  MappedDalitzArea& operator=(const MappedDalitzArea& other);

  bool operator<(const MappedDalitzArea& rhs) const;
  bool operator<=(const MappedDalitzArea& rhs) const;
  bool operator>(const MappedDalitzArea& rhs) const;
  bool operator>=(const MappedDalitzArea& rhs) const;

  bool operator==(const MappedDalitzArea& ma) const;
  bool operator!=(const MappedDalitzArea& ma) const;

};

std::ostream& operator<<(std::ostream& os, const MappedDalitzArea& mda);

#endif
//
