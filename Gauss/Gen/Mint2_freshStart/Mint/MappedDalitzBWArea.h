#ifndef MAPPED_DALITZ_BW_AREA_HH
#define MAPPED_DALITZ_BW_AREA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Mint/DalitzBWArea.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/Permutation.h"
#include "Mint/DalitzEvent.h"

#include "TRandom.h"
#include "TLorentzVector.h"

#include "Mint/counted_ptr.h"
#include "Mint/IGenFct.h"

#include <vector>
#include <iostream>

class MappedDalitzBWArea{
  TRandom* _rnd;

  DalitzEventPattern _pat, _mappedPat;
  
  DalitzBWArea _area;

  std::vector<MINT::counted_ptr<IGenFct> > _limits;

  Permutation _mapping, _inverseMapping;

  void setup();
  void findMapping();
  void applyLimits();

  bool allConsecutive() const;
  bool allStandardised() const;

  /*
  static std::vector<TLorentzVector> mapP4(const DalitzEvent& evt
					   , const Permutation& mapping
					   );
  static std::vector<TLorentzVector>& mapP4(const DalitzEvent& evt
					    , const Permutation& mapping
					    , std::vector<TLorentzVector>& p4
					    );
  */
  
  std::vector<TLorentzVector>& mapP4(const DalitzEvent& evt
				     , const Permutation& mapping
				     , std::vector<TLorentzVector>& p4
				     ) const{
    unsigned int n = evt.eventPattern().size();
    p4.resize(n); 
    for(unsigned int i=0; i < n; i++){
      int mappedIndex = mapping[i];
      if(mappedIndex < 0 || mappedIndex + 1 > (int) n){
	std::cout << "ERROR in MappedDalitzBWArea::mapP4()"
		  << "\n   Index out of range: " << mappedIndex
		  << " not in [0, " <<   n-1
		  << std::endl;
	throw "index out of range.";
      }
      p4[mappedIndex] = evt.p(i);
    }
    return p4;
  }
  std::vector<TLorentzVector> mapP4(const DalitzEvent& evt
			       , const Permutation& mapping
			       ) const{
    std::vector<TLorentzVector> p4(evt.eventPattern().size());
    return mapP4(evt, mapping, p4);
  }

 public:
  MappedDalitzBWArea();
  MappedDalitzBWArea(const DalitzEventPattern& pat
		   , TRandom* rnd=gRandom);
  MappedDalitzBWArea(const DalitzEventPattern& pat
		   , const MINT::counted_ptr<IGenFct>& oneFunction
		   , TRandom* rnd=gRandom);
  MappedDalitzBWArea(const DalitzEventPattern& pat
		   , const MINT::counted_ptr<IGenFct>& oneFunction
		   , const MINT::counted_ptr<IGenFct>& otherFunction
		   , TRandom* rnd=gRandom);
  MappedDalitzBWArea(const DalitzEventPattern& pat
		   , const std::vector<MINT::counted_ptr<IGenFct> >& functions
		   , TRandom* rnd=gRandom);

  MappedDalitzBWArea(const MappedDalitzBWArea& other);

  virtual ~MappedDalitzBWArea();

  bool checkIntegration() const{
    return _area.checkIntegration();}

  MINT::counted_ptr<DalitzEvent> tryEventForOwner() const;

  bool isInside(const DalitzEvent& evt) const;
  bool isInside(const DalitzCoordinate& dc) const;
  bool isInside(const std::vector<DalitzCoordinate>& dcList) const;

  double genValue(const DalitzEvent& evt) const;

  double size() const{
    return _area.size();}
  double integral() const{
    return _area.integral();
  }
  void setUnWeightPs(bool doSo=true){_area.setUnWeightPs(doSo);}
  bool unWeightPs(){return _area.unWeightPs();}

  void print(std::ostream& os = std::cout) const;

  MINT::counted_ptr<IGenFct> fct_for_s(const std::vector<int>& indices) const;
  MINT::counted_ptr<IGenFct> fct_for_s(int i, int j, int k=-1) const;

  MappedDalitzBWArea& operator=(const MappedDalitzBWArea& other);

  bool setRnd(TRandom* rnd=gRandom);
};

std::ostream& operator<<(std::ostream& os, const MappedDalitzBWArea& mda);

#endif
//
