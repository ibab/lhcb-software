#ifndef DALITZ_BW_AREA_HH
#define DALITZ_BW_AREA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/Permutation.h"

#include "TLorentzVector.h"
#include "TRandom.h"

#include <vector>
#include <iostream>

#include "Mint/counted_ptr.h"
#include "Mint/IGenFct.h"

class DalitzEvent;

class DalitzBWArea{

  DalitzEventPattern _pat;

  TRandom* _rnd;

  //  MINT::counted_ptr<DalitzEvent> _evt;

  mutable bool _madeCMap;

  mutable double _areaIntegral;
  bool _unWeightPs;

  void makeCoords();
  void makeCoord(int i, int j);
  void makeCoord(int i, int j, int k);

  void makeMiMa();

  int ResonanceConfigurationNumber()const;

  MINT::counted_ptr<DalitzEvent> try3Event(const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> try4Event(const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> make4EventWithPhaseSpace(const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> make4EventWithPhaseSpace(double& nTries, const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> try4EventWithPhaseSpace(double& maxWeight, const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> try4EventWithPhaseSpace(const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> try3EventWithPhaseSpace(double& maxWeight, const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> try3EventWithPhaseSpace(const Permutation& mapping=Permutation::unity()) const;

  std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> >& 
    sf(const DalitzCoordinate& c);
  const std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> >& 
    sf(const DalitzCoordinate& c) const;

  std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> >& 
    sf(int i, int j, int k);
  const std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> >& 
    sf(int i, int j, int k) const;

  std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> >& 
    sf(int i, int j);
  const std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> >& 
    sf(int i, int j) const;


  double MC4Integral(double& prec) const;
  double MC4IntegralNoTransform(double& prec) const;
  MINT::counted_ptr<DalitzEvent> tryFlat4EventWithPhaseSpace(double& maxWeight, const Permutation& mapping=Permutation::unity()) const;
  MINT::counted_ptr<DalitzEvent> tryFlat4EventWithPhaseSpace(const Permutation& mapping=Permutation::unity()) const;
  double genValueRho(const IDalitzEvent* evtPtr) const;
  

  std::vector<TLorentzVector>& mapP4(const std::vector<TLorentzVector>& v_in
				     , const Permutation& mapping
				     , std::vector<TLorentzVector>& v_out
				     ) const{
    unsigned int n = v_in.size();
    if(mapping.isUnity()){
      v_out = v_in;
      return v_out;
    }

    v_out.resize(n); 
    for(unsigned int i=0; i < n; i++){
      int mappedIndex = mapping[i];
      if(mappedIndex < 0 || mappedIndex + 1 > (int) n){
	std::cout << "ERROR in DalitzBWArea::mapP4()"
		  << "\n   Index out of range: " << mappedIndex
		  << " not in [0, " <<   n-1
		  << std::endl;
	throw "index out of range.";
      }
      v_out[mappedIndex] = v_in[i];
    }
    return v_out;
  }
  std::vector<TLorentzVector> mapP4(const std::vector<TLorentzVector>& v_in
				    , const Permutation& mapping
				    ) const{
    std::vector<TLorentzVector> v_out(v_in.size());
    return mapP4(v_in, mapping, v_out);
  }
  
 public:
  // for now keep them public:

  // currently storing the same info twice, once as
  // a (flexible) map, once again as seperate variables
  // - the latter mainly for passing it to 
  // the DalitzEvent constructor. 
  // Can remove the latter copy once the DalitzEvent
  // constructor can take some 'coordinate set' or so.

  bool checkIntegration() const;

  mutable std::map<DalitzCoordKey , std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> > >  _coords;
 
  mutable std::map<DalitzCoordKey , std::pair<DalitzCoordinate, MINT::counted_ptr<IGenFct> > >  _mappedCoords;
 

  bool unWeightPs()const{ return _unWeightPs;}
  void setUnWeightPs(bool doSo=true){_unWeightPs = doSo;}

  DalitzCoordinate& t01(){
    return sf(2,3,4).first;
  }
  DalitzCoordinate& s12(){
    return sf(1,2).first;
  }
  DalitzCoordinate& s23(){
    return sf(2,3).first;
  }
  DalitzCoordinate& s34(){
    return sf(3,4).first;
  }
  DalitzCoordinate& t40(){
    return sf(1,2,3).first;
  }

  const DalitzCoordinate& t01() const{
    return sf(2,3,4).first;
  }
  const DalitzCoordinate& s12() const{
    return sf(1,2).first;
  }
  const DalitzCoordinate& s23() const{
    return sf(2,3).first;
  }
  const DalitzCoordinate& s34() const{
    return sf(3,4).first;
  }
  const DalitzCoordinate& t40() const{
    return sf(1,2,3).first;
  }

  MINT::counted_ptr<IGenFct> f_t01(){
    return sf(2,3,4).second;
  }
  MINT::counted_ptr<IGenFct> f_s12(){
    return sf(1,2).second;
  }
  MINT::counted_ptr<IGenFct> f_s23(){
    return sf(2,3).second;
  }
  MINT::counted_ptr<IGenFct> f_s34(){
    return sf(3,4).second;
  }
  MINT::counted_ptr<IGenFct> f_t40(){
    return sf(1,2,3).second;
  }


  DalitzBWArea();
  DalitzBWArea(const DalitzEventPattern& pat, TRandom* rnd = gRandom);
  DalitzBWArea(const DalitzBWArea& other);
  ~DalitzBWArea();
  
  DalitzBWArea& operator=(const DalitzBWArea& other);

  void setFcn(const DalitzCoordinate& c
	      , const MINT::counted_ptr<IGenFct>& fcn);

  double genValue(const IDalitzEvent* evtPtr) const;
  double genValue(const IDalitzEvent* evtPtr, const Permutation& mapping) const;

  void setPattern(const DalitzEventPattern& pat);

  bool isInside(const DalitzEvent& evt) const;
  bool isInside(const DalitzEvent& evt, const Permutation& mapping) const;
  bool isInside(const DalitzCoordinate& dc) const;
  bool isInside(const std::vector<DalitzCoordinate>& dcList) const;

  double size() const;
  double integral() const;
  double integral3()const;
  double integral4()const;
  // actually these integrals are WITHOUT phase space
  // but go with the try...EventWithPhaseSpace routines.

  MINT::counted_ptr<DalitzEvent> tryEventForOwner(const Permutation& mapping=Permutation::unity()) const;

  /* one day...(3 body)
  MINT::counted_ptr<DalitzEvent> makeEventForOwner(double scale0
					     double scale1
					     );
  */

  bool setRnd(TRandom* rnd=gRandom);

  void print(std::ostream& os = std::cout) const;

};

std::ostream& operator<<(std::ostream& os, const DalitzBWArea& da);

#endif
//
