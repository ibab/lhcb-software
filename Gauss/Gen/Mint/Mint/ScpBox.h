#ifndef DALITZ_SCP_BOX_HH
#define DALITZ_SCP_BOX_HH

#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzArea.h"
#include <vector>
#include <iostream>

class IDalitzEvent;
class ScpBox{
  DalitzArea _area;

  int _nData;
  int _nMC;
  double _nWeightedMC;
  double _nWeightedData;

  double _weightMC_Squared;

 public:
  ScpBox();
  ScpBox(const DalitzEventPattern& pat);
  ScpBox(const DalitzArea& area);
  ScpBox(const ScpBox& other);

  std::vector<ScpBox> split(int n=2);

  void resetEventCounts();
  void resetAll(); // changes box size!!
  void enclosePhaseSpace(double safetyFactor=1.2);

  bool addData(const IDalitzEvent& evt, double weight = 1.0);
  bool addData(const IDalitzEvent* evt, double weight = 1.0);
  bool addData(int entries);

  bool addMC(const IDalitzEvent& evt, double weight);
  bool addMC(const IDalitzEvent* evt, double weight);
  bool addMC(int entries);

  bool subtractData(const IDalitzEvent& evt);
  bool subtractData(const IDalitzEvent* evt);
  bool subtractMC(const IDalitzEvent& evt, double weight);
  bool subtractMC(const IDalitzEvent* evt, double weight);

  double scp(double normFactorPassed=1) const;

  int nData() const;
  double weightedData() const;

  int nMC() const;  
  double weightedMC() const;
  
  double weightedMC2() const;
  double rmsMC(int Ntotal) const;

  double t01Min() const{return _area._t01.min();} // = s234
  double t01Max() const{return _area._t01.max();}

  double s12Min() const{return _area._s12.min();}
  double s12Max() const{return _area._s12.max();}

  double s23Min() const{return _area._s23.min();}
  double s23Max() const{return _area._s23.max();}

  double s34Min() const{return _area._s34.min();}
  double s34Max() const{return _area._s34.max();}

  double t40Min() const{return _area._t40.min();} // = s123
  double t40Max() const{return _area._t40.max();}

  bool thisBox(const IDalitzEvent* evt);

  void print(std::ostream& os = std::cout) const;

  DalitzArea area();

};

std::ostream& operator<<(std::ostream& os, const ScpBox& box);

#endif
//
