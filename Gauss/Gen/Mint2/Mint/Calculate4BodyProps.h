#ifndef DALITZCALCULATEFOURBODYPROPS_HH
#define DALITZCALCULATEFOURBODYPROPS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "TLorentzVector.h"
#include <vector>

class Calculate4BodyProps{
 protected:
  bool boundaries3Body(double& s12Min
		       , double& s12Max
		       , double _s23
		       , double _mumM
		       , double _d1M
		       , double _d2M
		       , double _d3M
		       );
  double _t01, _s12, _s23, _s34, _t40;
  //  double _m0, _m1, _m2, _m3, _m4; // m0 is mum's weight

  double m[5];
  double stij[5][5];
  double xij[5][5];

  // t01 = (p0 - p1)^2 = s234
  // s12 = (p1 + p2)^2
  // s23 = (p2 + p3)^2
  // s34 = (p3 + p4)^2
  // t40 = (p4 - p0)^2 = s123


  // for 4 body decays:
  // K4 := m0sq + 2(m1sq + m2sq + m3sq +m4q) 
  //      = s12 + s13 + s14 + s23 + s24 + s34
  // K4 - s12 - s23 - s34 = s13 + s14 + s24;
  //
  // for 3 body decays (there's no missing factor 2).
  // K3 := m0sq + m1sq + m2sq + m3sq = s12 + s23 + s13
  // so s13 = K3 - s12 - s23.
  //


  double _K4, _K3_12, _s124, _K3_23, _s14;
  double _K3_34, _s134, _s13_plus_s24;
  double _s13_minus_s24, _s13, _s24;
  double _BDet;
  double _Delta4, _Delta3, _Delta2;
  
  TLorentzVector _p1, _p2, _p3, _p4, _p0;
  
  bool knowK4, knowK3_12, knows124, knowK3_23, knows14;
  bool knowK3_34, knows134, knows13_plus_s24;
  bool knows13_minus_s24, knows13, knows24;
  bool knowBDet, knowstij;
  bool knowDelta4, knowDelta3, knowDelta2;
  
  bool knowp1, knowp2, knowp3, knowp4, knowp0;

 public:

  // constructor
  Calculate4BodyProps(double t01_in, double s12_in
		      , double s23_in, double s34_in
		      , double t40_in
		      , double m0_in
		      , double m1_in, double m2_in, double m3_in, double m4_in
		      );

  Calculate4BodyProps(){}
  // copy constructor
  Calculate4BodyProps( const Calculate4BodyProps& other);

  void set(double t01_in, double s12_in
	   , double s23_in, double s34_in
	   , double t40_in
	   , double m0_in
	   , double m1_in, double m2_in, double m3_in, double m4_in
	   );
  void set(double t01_in, double s12_in
	   , double s23_in, double s34_in
	   , double t40_in
	   , double m_in[5]
	   );
  void setWithSameMass(double t01_in, double s12_in
		       , double s23_in, double s34_in
		       , double t40_in
		       );
  void knowNothing(); // initialises know... variables
  // called with constructor. If called, forces re-calculation
  // of everything.

  double pij(int i, int j); // products of 4-vectors.
  double G(int p, int q);   // Grahm determinants
  double G(int p1, int p2, int q1, int q2);
  double G(int p1, int p2, int p3
	     , int q1, int q2, int q3);
  double G(int p1, int p2, int p3, int p4
	     , int q1, int q2, int q3, int q4);

  double Delta(int p1);
  double Delta(int p1, int p2);
  double Delta(int p1, int p2, int p3);
  double Delta(int p1, int p2, int p3, int p4);


  double m0() const{ return m[0];}
  double m1() const{ return m[1];}
  double m2() const{ return m[2];}
  double m3() const{ return m[3];}
  double m4() const{ return m[4];}

  double s12() const{ return _s12;}
  double s23() const{ return _s23;}
  double s34() const{ return _s34;}
  double t01() const{ return _t01;}
  double t40() const{ return _t40;}
  double s123() const{ return t40();}
  double s234() const{ return t01();}
  
  double m0sq() const{ return m0()*m0();}
  double m1sq() const{ return m1()*m1();}
  double m2sq() const{ return m2()*m2();}
  double m3sq() const{ return m3()*m3();}
  double m4sq() const{ return m4()*m4();}

  // sometimes it is helpful to give things
  // just a different name (masses of "pseudo particles"):
  double m12sq() const{ return s12();}
  double m23sq() const{ return s23();}
  double m34sq() const{ return s34();}

  // sij and quantities needed to calculate sij
  double K4();
  void calcK4();
  double K3_12();
  void calcK3_12();
  double s124();
  void calcs124();
  double K3_23();
  void calcK3_23();
  double s14();
  void calcs14();
  double K3_34();
  void calcK3_34();
  double s134();
  void calcs134();
  double s13_plus_s24();
  void calcs13_plus_s24();
  double s13_minus_s24();
  void calcs13_minus_s24();
  double s13();
  void calcs13();
  double s24();
  void calcs24();
  double sij(int index);
  double sij(int i, int j);
  double sij(const std::vector<int>& indices);
  double sijk(int i, int j, int k);

  // determinants
  double BDet();
  void calcBDet();
  double Delta4_without_xij();
  double Delta4_with_xij();
  
  void fill_stij(); // needed for deltas

  double Delta4();
  void calcDelta4();
  double Delta3();
  void calcDelta3();
  double Delta2();
  void calcDelta2();
  double Delta1();

  // the phase space factor...
  double phaseSpaceFactor(); // =sqrt(-Delta4()) 
  double showPhaseSpaceFactorCalculation();
 
  void s12MinMax(double& mi, double& ma);
  void s23MinMax(double& mi, double& ma);
  void s34MinMax(double& mi, double& ma);
  void t40MinMax(double& mi, double& ma);
  void t01MinMax(double& mi, double& ma);

  // the actual 4-momenta - see page 208 of THE BOOK:
  TLorentzVector p1();
  void calcp1();
  TLorentzVector p2();
  void calcp2();
  TLorentzVector p3();
  void calcp3();
  TLorentzVector p4();
  void calcp4();
  TLorentzVector p0();
  void calcp0();

  // same 4-momenta as above, but by index:
  TLorentzVector pVec(int i);
};

#endif
//
