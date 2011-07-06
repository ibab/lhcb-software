#ifndef DALITZCALCULATEFOURBODYPROPS_HH
#define DALITZCALCULATEFOURBODYPROPS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TLorentzVector.h"

#include "Roo3BodyPhaseSpace.h"

class Calculate4BodyProps{
  bool boundaries3Body(Double_t& s12Min
		       , Double_t& s12Max
		       , Double_t _s23
		       , Double_t _mumM
		       , Double_t _d1M
		       , Double_t _d2M
		       , Double_t _d3M
		       ){
    
    s12Min = s12Max = -9999;
    double s23AbsMin = (_d2M + _d3M)*(_d2M + _d3M);
    double s23AbsMax = (_mumM - _d1M)*(_mumM - _d1M);
    
    if(_s23 < 0) return 0;
    if(_s23 < s23AbsMin) return false;
    if(_s23 > s23AbsMax) return false;
    
    
    Double_t mumMsq = _mumM * _mumM;
    
    if(_s23 > mumMsq) return false;
    
    Double_t m1sq=_d1M*_d1M, m2sq=_d2M*_d2M, m3sq=_d3M*_d3M;
    
    Double_t m23 = sqrt(_s23);
    Double_t E3 = (_s23 - m2sq + m3sq)/(2*m23);
    if(E3 < _d3M) return false;
    
    Double_t E1 = (mumMsq - _s23 - m1sq)/(2*m23);
    if(E1 < _d1M) return false;
    
    Double_t E3_E1sq = (E3+E1)*(E3+E1);
    Double_t p3_23   = sqrt(E3*E3 - m3sq); // p2 in m12 restframe
    Double_t p1_23   = sqrt(E1*E1 - m1sq);
    Double_t P_minSq = (p3_23 - p1_23)*(p3_23 - p1_23);
    Double_t P_maxSq = (p3_23 + p1_23)*(p3_23 + p1_23);
    
    s12Max = E3_E1sq - P_minSq;
    s12Min = E3_E1sq - P_maxSq;
    if(false)cout << "s12Max s12Min " 
		  << s12Max << ", " 
		  << s12Min << endl;
    return true;
  }



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

 public:

  Double_t pij(int i, int j){
    if(! knowstij) fill_stij();
    return xij[i][j];
  }

  Double_t G(int p, int q){
    if(! knowstij) fill_stij();
    return xij[p][q];
  }

  Double_t G(int p1, int p2, int q1, int q2){
    TMatrixD m(2,2);
    if(! knowstij) fill_stij();
    m(0,0) = xij[p1][q1];
    m(0,1) = xij[p1][q2];
    m(1,0) = xij[p2][q1];
    m(1,1) = xij[p2][q2];

    return m.Determinant();
  }

  Double_t G(int p1, int p2, int p3
	     , int q1, int q2, int q3){
    TMatrixD m(3,3);
    if(! knowstij) fill_stij();
    m(0,0) = xij[p1][q1];
    m(0,1) = xij[p1][q2];
    m(0,2) = xij[p1][q3];
    m(1,0) = xij[p2][q1];
    m(1,1) = xij[p2][q2];
    m(1,2) = xij[p2][q3];
    m(2,0) = xij[p3][q1];
    m(2,1) = xij[p3][q2];
    m(2,2) = xij[p3][q3];

    return m.Determinant();
  }

  Double_t G(int p1, int p2, int p3, int p4
	     , int q1, int q2, int q3, int q4){
    TMatrixD m(4,4);
    if(! knowstij) fill_stij();
    m(0,0) = xij[p1][q1];
    m(0,1) = xij[p1][q2];
    m(0,2) = xij[p1][q3];
    m(0,3) = xij[p1][q4];
    m(1,0) = xij[p2][q1];
    m(1,1) = xij[p2][q2];
    m(1,2) = xij[p2][q3];
    m(1,3) = xij[p2][q4];
    m(2,0) = xij[p3][q1];
    m(2,1) = xij[p3][q2];
    m(2,2) = xij[p3][q3];
    m(2,3) = xij[p3][q4];
    m(3,0) = xij[p4][q1];
    m(3,1) = xij[p4][q2];
    m(3,2) = xij[p4][q3];
    m(3,3) = xij[p4][q4];

    return m.Determinant();
  }

  Double_t Delta(int p1){
    return G(p1,  p1);
  }
  Double_t Delta(int p1, int p2){
    return G(p1, p2,   p1, p2);
  }
  Double_t Delta(int p1, int p2, int p3){
    return G(p1, p2, p3,   p1, p2, p3);
  }
  Double_t Delta(int p1, int p2, int p3, int p4){
    return G(p1, p2, p3, p4,   p1, p2, p3, p4);
  }

  Calculate4BodyProps(double t01_in, double s12_in
		      , double s23_in, double s34_in
		      , double t40_in
		      , double m0_in
		      , double m1_in, double m2_in, double m3_in, double m4_in
		      ){
    _t01=t01_in;
    _s12=s12_in;
    _s23=s23_in;
    _s34=s34_in;
    _t40=t40_in;
    m[0] = m0_in;
    m[1] = m1_in;
    m[2] = m2_in;
    m[3] = m3_in;
    m[4] = m4_in;

    knowNothing();

  }
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

  double K4(){
    if(! knowK4) calcK4();
    return _K4;
  }
  void calcK4(){
    _K4 = m0sq() + 2*(m1sq() + m2sq() + m3sq() + m4sq());
    knowK4=true;
  }
  double K3_12(){
    if(! knowK3_12) calcK3_12();
    return _K3_12;
  }
  void calcK3_12(){
    //    K3_12 := m0sq + m{12}sq + m3sq + m4sq 
    //           = s{12}3 + s34 + {s12}4
    _K3_12 =  m0sq() + m12sq() + m3sq() + m4sq();
    knowK3_12 = true;
  }
  double s124(){
    if(! knows124) calcs124();
    return _s124;
  }
  void calcs124(){
    _s124 = K3_12() - s123() - s34();
    knows124 = true;
  }

  double K3_23(){
    if(! knowK3_23) calcK3_23();
    return _K3_23;
  }
  void calcK3_23(){
    //    K3_23 := m0sq + m1sq + m{23}sq + m4sq 
    //           = s1{23} + s{23}4 + s14
    _K3_23 = m0sq() + m1sq() + m23sq() + m4sq();
    knowK3_23 = true;
  }

  double s14(){
    if(! knows14) calcs14();
    return _s14;
  }
  void calcs14(){
    _s14 = K3_23() - s123() - s234();
    knows14 = true;
  }
  double K3_34(){
    if(! knowK3_34) calcK3_34();
    return _K3_34;
  }
  void calcK3_34(){
        //    K3_34 := m0sq + m1sq + m2sq + m{34}sq 
    //           = s12 + s2{34} + s1{34}
    _K3_34 = m0sq() + m1sq() + m2sq() + m34sq();
    knowK3_34 = true;
  }
  double s134(){
    if(! knows134) calcs134();
    return _s134;
  }
  void calcs134(){
    _s134 = K3_34() - s12() - s234();
    knows134 = true;
  }

  double s13_plus_s24(){
    if(! knows13_plus_s24) calcs13_plus_s24();
    return _s13_plus_s24;
  }
  void calcs13_plus_s24(){
    // K4 - s12 - s23 - s34 = s13 + s14 + s24;
    _s13_plus_s24 = K4() - s12() -s23() - s34() - s14();
    knows13_plus_s24 = true;
  }
  double s13_minus_s24(){
    if(! knows13_minus_s24) calcs13_minus_s24();
    return _s13_minus_s24;
  }
  void calcs13_minus_s24(){
    // K3 = s12 + s23 + s13
    // use 1,3 of our 4 particles as "pseudo particle"
    //
    // m0sq + m{13}sq + m2sq + m4sq
    //       = s{13}2 + s24 + s{13}4
    // <=> m{13}sq - s24
    //       = s{13}2 + s{13}4 - (m0sq - m2sq - m4sq)
    // <=> s13 - s24 = s123 + s134 - (m0sq - m2sq - m4sq)

    _s13_minus_s24 = s123() + s134() - m0sq() - m2sq() - m4sq();

    knows13_minus_s24 = true;
  }
  double s13(){
    if(! knows13) calcs13();
    return _s13;
  }
  void calcs13(){
    _s13 = (s13_plus_s24() + s13_minus_s24())/2.0;
    knows13 = true;
  }
  double s24(){
    if(! knows24) calcs24();
    return _s24;
  }
  void calcs24(){
    _s24 = (s13_plus_s24() - s13_minus_s24())/2.0;
    knows24 = true;
  }
  
  double BDet(){
    if(! knowBDet) calcBDet();
    return _BDet;
  }
  void calcBDet(){
    TMatrixDSym B(6);
    B(0,0)=0;
    B(1,0)=B(0,1)=m2sq(); B(1,1) = 0;
    B(2,0)=B(0,2)=s23() ; B(2,1)=B(1,2) = m3sq(); B(2,2)=0;
    B(3,0)=B(0,3)=t01() ; B(3,1)=B(1,3) = s34() ; B(3,2)=B(2,3)=m4sq(); B(3,3)=0;
    B(4,0)=B(0,4)=m1sq(); B(4,1)=B(1,4) = s12() ; B(4,2)=B(2,4)=t40() ; B(4,3)=B(3,4)=m0sq(); B(4,4)=0;
    B(5,0)=B(0,5)=B(5,1)=B(1,5)=B(5,2)=B(2,5)=B(5,3)=B(3,5)=B(5,4)=B(4,5)=1; 
    B(5,5)=0;
    //  B.Print();
    _BDet = B.Determinant();
    knowBDet = true;
  }
  double Delta4_doesntWork(){
    return -BDet()/16.0;
  }
  
  double phaseSpaceFactor(){
    double ps2 = -Delta4();
    if(ps2 <= 0) return 0;
    return sqrt(ps2);
  }
  
  void fill_stij(){
    if(knowstij) return;
    stij[0][0] = 4*m0sq();
    stij[0][1] = stij[1][0]=t01();
    stij[0][4] = stij[4][0]=t40(); // not filling any others with 0
    stij[1][1] = 4*m1sq();
    stij[1][2] = stij[2][1] = s12();
    stij[1][3] = stij[3][1] = s13();
    stij[1][4] = stij[4][1] = s14();
    stij[2][2] = 4*m2sq();
    stij[2][3] = stij[3][2] = s23();
    stij[2][4] = stij[4][2] = s24();
    stij[3][3] = 4*m3sq();
    stij[3][4] = stij[4][3] = s34();
    stij[4][4] = 4*m4sq();

    for(int i=0; i<5; i++){
      for(int j=0; j<5; j++){
	xij[i][j] = 0.5*( stij[i][j] - m[i]*m[i] - m[j]*m[j] );
      }
    }
    knowstij = true;
  }
  
  double Delta4(){
    if(! knowDelta4) calcDelta4();
    return _Delta4;
  }
  void calcDelta4(){
    TMatrixDSym B(4);
    if(! knowstij)fill_stij();

    B(0,0)=xij[1][1];
    B(1,0)=B(0,1)=xij[2][1]; B(1,1)=xij[2][2];
    B(2,0)=B(0,2)=xij[3][1]; B(2,1)=B(1,2)=xij[3][2]; B(2,2)=xij[3][3];
    B(3,0)=B(0,3)=xij[4][1]; B(3,1)=B(1,3)=xij[4][2]; B(3,2)=B(2,3)=xij[4][3]; B(3,3)=xij[4][4];
  
    _Delta4 = B.Determinant();

    knowDelta4 = true;
  }
  
  double Delta3(){
    if(! knowDelta3) calcDelta3();
    return _Delta3;
  }
  void calcDelta3(){
    TMatrixDSym B(3);
    if(! knowstij)fill_stij();
    B(0,0)=xij[1][1];
    B(1,0)=B(0,1)=xij[2][1]; B(1,1)=xij[2][2];
    B(2,0)=B(0,2)=xij[3][1]; B(2,1)=B(1,2)=xij[3][2]; B(2,2)=xij[3][3];
    _Delta3 = B.Determinant();
    knowDelta3 = true;
  }
  double Delta2(){
    if(! knowDelta2) calcDelta2();
    return _Delta2;
  }
  void calcDelta2(){
    TMatrixDSym B(2);
    if(! knowstij)fill_stij();
    B(0,0)=xij[1][1];
    B(1,0)=B(0,1)=xij[2][1]; B(1,1)=xij[2][2];
    _Delta2 = B.Determinant();
    knowDelta2 = true;
  }
  double Delta1(){
    if(! knowstij)fill_stij();
    return xij[1][1];
  }

 
  void s12MinMax(Double_t& mi, Double_t& ma){
    // pseudoparticle: 34
    // p0 = p1 + p2 + (p3 + p4)
    boundaries3Body(mi, ma
				   , s234()
				   , m0(), m1(), m2()
				   , sqrt(s34()));
  }

  void s23MinMax(Double_t& mi, Double_t& ma){
    // pseudoparticle: 14
    // p0 = p2 + p3 + (p1 + p4)
    boundaries3Body(mi, ma
				   , s134()
				   , m0(), m2(), m3()
				   , sqrt(s14()));
  }

  void s34MinMax(Double_t& mi, Double_t& ma){
    // pseudoparticle: 12
    // p0 = p3 + p4 + (p1 + p2)
    boundaries3Body(mi, ma
				   , s124()
				   , m0(), m3(), m4()
				   , sqrt(s12()));
  }

  void t40MinMax(Double_t& mi, Double_t& ma){
    // = s123MinMax
    // pseudoparticle: 23
    // p0 = p1 + (p2 + p3) + p4
    boundaries3Body(mi, ma
				   , s234()
				   , m0(), m1(), sqrt(s23())
				   , m4());
  }

  void t01MinMax(Double_t& mi, Double_t& ma){
    // = s234MinMax
    // pseudoparticle: 34
    // p0 = p2 + (p3 + p4) + p1
    boundaries3Body(mi, ma
				   , s134()
				   , m0(), m2(), sqrt(s34())
				   , m1());
  }
 

  // the actual 4-momenta - see page 208 of THE BOOK:
  TLorentzVector p1(){
    if(! knowp1) calcp1();
    return _p1;
  }
  void calcp1(){
    Double_t D1=Delta1();
    if(D1 < 0){
      TLorentzVector p(-9999,-9999,-9999,-9999);
      _p1 = p;
    }else{
      TLorentzVector p(0, 0, 0, sqrt(D1));
      _p1 = p;
    }
    knowp1 = true;
  }

  TLorentzVector p2(){
    if(! knowp2) calcp2();
    return _p2;
  }
  void calcp2(){
    Double_t D1=Delta1(), D2=Delta2();
    if(D1 <= 0 || -D2/D1 < 0){
      TLorentzVector p(-9999,-9999,-9999,-9999);
      _p2 = p;
    }else{
      TLorentzVector p(0, 0, sqrt(-D2/D1), G(1,2)/sqrt(D1));
      _p2 = p;
    }
    knowp2 = true;
  }
  TLorentzVector p3(){
    if(! knowp3) calcp3();
    return _p3;
  }
  void calcp3(){
    Double_t D1=Delta1(), D2=Delta2(), D3=Delta3();
    if(D1 <= 0 || (- D1 * D2) <= 0 || -D3/D2 < 0 ){
      TLorentzVector p(-9999,-9999,-9999,-9999);
      _p3=p;
    }else{
      TLorentzVector p( sqrt(-D3/D2)
			, 0
			, -G(1,2, 1,3)/sqrt(-D1*D2)
			, G(1,3)/sqrt(D1)
			);
      _p3=p;
    }
    knowp3 = true;
  }

  TLorentzVector p4(){
    if(! knowp4) calcp4();
    return _p4;
  }
  void calcp4(){
    Double_t D1=Delta1(), D2=Delta2(), D3=Delta3(), D4=Delta4();
    if(D1 <=0 || -D2*D3 <=0 || -D4/D3 < 0 || -D1*D2 <=0){
      TLorentzVector p(-9999,-9999,-9999,-9999);
      _p4 = p;
    }else{
      TLorentzVector p(G(1, 2, 3,  1, 2, 4)/sqrt(-D2*D3)
		       , sqrt(-D4/D3)
		       , -G(1, 2,  1, 4)/sqrt(-D1*D2)
		       , G(1,4)/sqrt(D1)
		       );
      _p4 = p;
    }
    knowp4 = true;
  }

  TLorentzVector p0(){
    if(! knowp0) calcp0();
    return _p0;
  }
  void calcp0(){
    Double_t D1=Delta1(), D2=Delta2(), D3=Delta3(), D4=Delta4();
    if(D1 <=0 || -D2*D3 <=0 || -D4/D3 < 0 || -D1*D2 <=0){
      TLorentzVector p(-9999,-9999,-9999,-9999);
      _p0 = p;
    }else{
      _p0 = p1() + p2() + p3() + p4();
    }
    knowp0 = true;
  }

  TLorentzVector pVec(int i){
    switch(i){
    case 0:
      return p0();
      break;
    case 1:
      return p1();
      break;
    case 2:
      return p2();
      break;
    case 3:
      return p3();
      break;
    case 4:
      return p4();
      break;
    default:
      TLorentzVector p(-9999,-9999,-9999,-9999);
      return p;
    }
  }
  Double_t _K4, _K3_12, _s124, _K3_23, _s14;
  Double_t _K3_34, _s134, _s13_plus_s24;
  Double_t _s13_minus_s24, _s13, _s24;
  Double_t _BDet;
  Double_t _Delta4, _Delta3, _Delta2;
  
  TLorentzVector _p1, _p2, _p3, _p4, _p0;
  
  bool knowK4, knowK3_12, knows124, knowK3_23, knows14;
  bool knowK3_34, knows134, knows13_plus_s24;
  bool knows13_minus_s24, knows13, knows24;
  bool knowBDet, knowstij;
  bool knowDelta4, knowDelta3, knowDelta2;
  
  bool knowp1, knowp2, knowp3, knowp4, knowp0;
  
  void knowNothing(){
    knowK4=knowK3_12=knows124=knowK3_23=knows14=false;
    knowK3_34=knows134=knows13_plus_s24=false;
    knows13_minus_s24=knows13=knows24=false;
    knowBDet=knowstij=false;
    knowDelta4=knowDelta3=knowDelta2=false;
    
    knowp1=knowp2=knowp3=knowp4=knowp0=false;
  }

};

#endif
//
