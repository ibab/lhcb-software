// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/Calculate4BodyProps.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TLorentzVector.h"

#include "Mint/CLHEPPhysicalConstants.h"

#include "Mint/Utils.h"

using namespace std;

Calculate4BodyProps::Calculate4BodyProps(double t01_in, double s12_in
					 , double s23_in, double s34_in
					 , double t40_in
					 , double m0_in
					 , double m1_in
					 , double m2_in
					 , double m3_in
					 , double m4_in
					 ){
  set(t01_in, s12_in, s23_in, s34_in, t40_in
      , m0_in, m1_in, m2_in, m3_in, m4_in);
}

Calculate4BodyProps::Calculate4BodyProps( const Calculate4BodyProps& other){
  _t01 = other._t01;
  _s12 = other._s12;
  _s23 = other._s23;
  _s34 = other._s34;
  _t40 = other._t40;

  for(int i=0; i<5; i++){
    m[i]=other.m[i];
    for(int j=0; j<5; j++){
      stij[i][j] = other.stij[i][j];
      xij[i][j]  = other.xij[i][j];
    }
  }

  _K4    = other._K4;
  _K3_12 = other._K3_12;
  _s124  = other._s124;
  _K3_23 = other._K3_23;
  _s14   = other._s14;

  _K3_34 = other._K3_34;
  _s134  = other._s134;
  _s13_plus_s24 = other._s13_plus_s24;

  _s13_minus_s24 = other._s13_minus_s24;
  _s13   = other._s13;
  _s24   = other._s24;

  _BDet   = other._BDet;
  _Delta4 = other._Delta4;
  _Delta3 = other._Delta3;
  _Delta2 = other._Delta2;
  
  _p1 = other._p1;
  _p2 = other._p2;
  _p3 = other._p3;
  _p4 = other._p4;
  _p0 = other._p0;
  
  knowK4     = other.knowK4;
  knowK3_12  = other.knowK3_12;
  knows124   = other.knows124;
  knowK3_23  = other.knowK3_23;
  knows14    = other.knows14;

  knowK3_34  = other.knowK3_34;
  knows134   = other.knows134;
  knows13_plus_s24 = other.knows13_plus_s24;

  knows13_minus_s24 = other.knows13_minus_s24;
  knows13    = other.knows13;
  knows24    = other.knows24;

  knowBDet   = other.knowBDet;
  knowstij   = other.knowstij;

  knowDelta4 = other.knowDelta4;
  knowDelta3 = other.knowDelta3;
  knowDelta2 = other.knowDelta2;
  
  knowp1     = other.knowp1;
  knowp2     = other.knowp2;
  knowp3     = other.knowp3;
  knowp4     = other.knowp4;
  knowp0     = other.knowp0;

}


void Calculate4BodyProps::set(double t01_in, double s12_in
			      , double s23_in, double s34_in
			      , double t40_in
			      , double m0_in
			      , double m1_in
			      , double m2_in
			      , double m3_in
			      , double m4_in
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

void Calculate4BodyProps::set(double t01_in, double s12_in
			      , double s23_in, double s34_in
			      , double t40_in
			      , double m_in[5]
			      ){
  _t01=t01_in;
  _s12=s12_in;
  _s23=s23_in;
  _s34=s34_in;
  _t40=t40_in;
  for(int i=0; i<5; i++) m[i]=m_in[i];
  
  knowNothing();
}

void Calculate4BodyProps::setWithSameMass(double t01_in, double s12_in
					  , double s23_in, double s34_in
					  , double t40_in
					  ){
  _t01=t01_in;
  _s12=s12_in;
  _s23=s23_in;
  _s34=s34_in;
  _t40=t40_in;
  
  knowNothing();
}
void Calculate4BodyProps::knowNothing(){
  knowK4=knowK3_12=knows124=knowK3_23=knows14=false;
  knowK3_34=knows134=knows13_plus_s24=false;
  knows13_minus_s24=knows13=knows24=false;
  knowBDet=knowstij=false;
  knowDelta4=knowDelta3=knowDelta2=false;
  
  knowp1=knowp2=knowp3=knowp4=knowp0=false;
}

bool Calculate4BodyProps::boundaries3Body(double& s12Min
					  , double& s12Max
					  , double _s23
					  , double _mumM
					  , double _d1M
					  , double _d2M
					  , double _d3M
					  ){
  
  s12Min = s12Max = -9999;
  double s23AbsMin = (_d2M + _d3M)*(_d2M + _d3M);
  double s23AbsMax = (_mumM - _d1M)*(_mumM - _d1M);
  
  if(_s23 < 0) return 0;
  if(_s23 < s23AbsMin) return false;
  if(_s23 > s23AbsMax) return false;
  
  
  double mumMsq = _mumM * _mumM;
  
  if(_s23 > mumMsq) return false;
  
  double m1sq=_d1M*_d1M, m2sq=_d2M*_d2M, m3sq=_d3M*_d3M;
  
  double m23 = sqrt(_s23);
  double E3 = (_s23 - m2sq + m3sq)/(2*m23);
  if(E3 < _d3M) return false;
  
  double E1 = (mumMsq - _s23 - m1sq)/(2*m23);
  if(E1 < _d1M) return false;
  
  double E3_E1sq = (E3+E1)*(E3+E1);
  double p3_23   = sqrt(E3*E3 - m3sq); // p2 in m12 restframe
  double p1_23   = sqrt(E1*E1 - m1sq);
  double P_minSq = (p3_23 - p1_23)*(p3_23 - p1_23);
  double P_maxSq = (p3_23 + p1_23)*(p3_23 + p1_23);
  
  s12Max = E3_E1sq - P_minSq;
  s12Min = E3_E1sq - P_maxSq;
  if(false)cout << "s12Max s12Min " 
		<< s12Max << ", " 
		<< s12Min << endl;
  return true;
}

  // for 4 body decays:
  // K4 := m0sq + 2(m1sq + m2sq + m3sq +m4q) 
  //      = s12 + s13 + s14 + s23 + s24 + s34
  // K4 - s12 - s23 - s34 = s13 + s14 + s24;
  //
  // for 3 body decays (there's no missing factor 2).
  // K3 := m0sq + m1sq + m2sq + m3sq = s12 + s23 + s13
  // so s13 = K3 - s12 - s23.
  //

double Calculate4BodyProps::pij(int i, int j){
  if(! knowstij) fill_stij();
  return xij[i][j];
}

double Calculate4BodyProps::G(int p, int q){
  if(! knowstij) fill_stij();
  return xij[p][q];
}

double Calculate4BodyProps::G(int p1, int p2, int q1, int q2){
  TMatrixD m(2,2);
  if(! knowstij) fill_stij();
  m(0,0) = xij[p1][q1];
  m(0,1) = xij[p1][q2];
  m(1,0) = xij[p2][q1];
  m(1,1) = xij[p2][q2];
  
  return m.Determinant();
}

double Calculate4BodyProps::G(int p1, int p2, int p3
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

double Calculate4BodyProps::G(int p1, int p2, int p3, int p4
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

double Calculate4BodyProps::Delta(int p1){
  return G(p1,  p1);
}
double Calculate4BodyProps::Delta(int p1, int p2){
  return G(p1, p2,   p1, p2);
}
double Calculate4BodyProps::Delta(int p1, int p2, int p3){
  return G(p1, p2, p3,   p1, p2, p3);
}
double Calculate4BodyProps::Delta(int p1, int p2, int p3, int p4){
  return G(p1, p2, p3, p4,   p1, p2, p3, p4);
}

double Calculate4BodyProps::K4(){
  if(! knowK4) calcK4();
  return _K4;
}
void Calculate4BodyProps::calcK4(){
  _K4 = m0sq() + 2*(m1sq() + m2sq() + m3sq() + m4sq());
  knowK4=true;
}
double Calculate4BodyProps::K3_12(){
  if(! knowK3_12) calcK3_12();
  return _K3_12;
}
void Calculate4BodyProps::calcK3_12(){
  //    K3_12 := m0sq + m{12}sq + m3sq + m4sq 
  //           = s{12}3 + s34 + {s12}4
  _K3_12 =  m0sq() + m12sq() + m3sq() + m4sq();
  knowK3_12 = true;
}
double Calculate4BodyProps::s124(){
  if(! knows124) calcs124();
  return _s124;
}
void Calculate4BodyProps::calcs124(){
  _s124 = K3_12() - s123() - s34();
  knows124 = true;
}

double Calculate4BodyProps::K3_23(){
  if(! knowK3_23) calcK3_23();
  return _K3_23;
}
void Calculate4BodyProps::calcK3_23(){
  //    K3_23 := m0sq + m1sq + m{23}sq + m4sq 
  //           = s1{23} + s{23}4 + s14
  _K3_23 = m0sq() + m1sq() + m23sq() + m4sq();
  knowK3_23 = true;
}

double Calculate4BodyProps::s14(){
  if(! knows14) calcs14();
  return _s14;
}
void Calculate4BodyProps::calcs14(){
  _s14 = K3_23() - s123() - s234();
  knows14 = true;
}
double Calculate4BodyProps::K3_34(){
  if(! knowK3_34) calcK3_34();
  return _K3_34;
}
void Calculate4BodyProps::calcK3_34(){
  //    K3_34 := m0sq + m1sq + m2sq + m{34}sq 
  //           = s12 + s2{34} + s1{34}
  _K3_34 = m0sq() + m1sq() + m2sq() + m34sq();
  knowK3_34 = true;
}
double Calculate4BodyProps::s134(){
  if(! knows134) calcs134();
  return _s134;
}
void Calculate4BodyProps::calcs134(){
  _s134 = K3_34() - s12() - s234();
  knows134 = true;
}

double Calculate4BodyProps::s13_plus_s24(){
  if(! knows13_plus_s24) calcs13_plus_s24();
  return _s13_plus_s24;
}
void Calculate4BodyProps::calcs13_plus_s24(){
  // K4 - s12 - s23 - s34 = s13 + s14 + s24;
  _s13_plus_s24 = K4() - s12() -s23() - s34() - s14();
  knows13_plus_s24 = true;
}
double Calculate4BodyProps::s13_minus_s24(){
  if(! knows13_minus_s24) calcs13_minus_s24();
  return _s13_minus_s24;
}
void Calculate4BodyProps::calcs13_minus_s24(){
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
double Calculate4BodyProps::s13(){
  if(! knows13) calcs13();
  return _s13;
}
void Calculate4BodyProps::calcs13(){
  _s13 = (s13_plus_s24() + s13_minus_s24())/2.0;
  knows13 = true;
}
double Calculate4BodyProps::s24(){
  if(! knows24) calcs24();
  return _s24;
}
void Calculate4BodyProps::calcs24(){
  _s24 = (s13_plus_s24() - s13_minus_s24())/2.0;
  knows24 = true;
}

double Calculate4BodyProps::sij(int i, int j){
  int a = (i <  j ? i: j);
  int b = (i >= j ? i: j);
  return sij(10*a + b);
}
double Calculate4BodyProps::sij(const std::vector<int>& v_in){
  std::vector<int> v = v_in;
  sort(v.begin(), v.end());
  int index  = 0;
  int facTen = 1;
  for(int i= v.size() -1; i>=0; i--){
    index  += facTen * v[i];
    facTen *= 10;
  }
  return sij(index);  
}


double Calculate4BodyProps::sijk(int i, int j, int k){
  std::vector<int> v;
  v.push_back(i); v.push_back(j); v.push_back(k);
  sort(v.begin(), v.end());

  return sij(100*v[0] + 10*v[1] + v[2]);
}

double Calculate4BodyProps::sij(int index){
  switch ( index ){ // VERY! clumsy
  case 1:
    return m[1]*m[1];
    break;
  case 2:
    return m[2]*m[2];
    break;
  case 3:
    return m[3]*m[3];
    break;
  case 4:
    return m[4]*m[4];
    break;
  case 11:
    return 4*m[1]*m[1];
    break;
  case 22:
    return 4*m[2]*m[2];
    break;
  case 33:
    return 4*m[3]*m[3];
    break;
  case 44:
    return 4*m[4]*m[4];
    break;
  case 12:
    return s12();
    break;
  case 13:
    return s13();
    break;
  case 14:
    return s14();
    break;
  case 23:
    return s23();
    break;
  case 24:
    return s24();
    break;
  case 34:
    return s34();
    break;
  case 123:
    return s123();
    break;
  case 124:
    return s124();
    break;
  case 134:
    return s134();
    break;
  case 234:
    return s234();
    break;
  case 1234:
    return m[0]*m[0]; // assumes constrained fit.
    break;
  default:
    cout << "Error from Calculate4BodyProps::sij(" << index << ")"
	 << ": Don't understand index " << index << endl;
    return -9999;
  }
}


double Calculate4BodyProps::BDet(){
  if(! knowBDet) calcBDet();
  return _BDet;
}
void Calculate4BodyProps::calcBDet(){
  /*
  cout << " for sij " << t01() << ", " << s12() << ", " << s23()
       << ", " << s34() << ", " << t40() << endl;
  cout << "\n " << m0sq() << ", " << m1sq() << ", " << m2sq()
       << ", " << m3sq() << m4sq() << endl;
  */
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
  //  cout << "B  " << _BDet << endl;
  knowBDet = true;
}
double Calculate4BodyProps::Delta4_without_xij(){ 
    return -BDet()/16.0;
}

void Calculate4BodyProps::fill_stij(){
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

double Calculate4BodyProps::Delta4_with_xij(){
  if(! knowDelta4) calcDelta4();
  return _Delta4;
}

double Calculate4BodyProps::Delta4(){
  //  cout << " compare: " << Delta4_with_xij() << " , " << Delta4_without_xij() << endl;
  return Delta4_without_xij();
}

void Calculate4BodyProps::calcDelta4(){
  TMatrixDSym B(4);
  if(! knowstij)fill_stij();
  
  B(0,0)=xij[1][1];
  B(1,0)=B(0,1)=xij[2][1]; B(1,1)=xij[2][2];
  B(2,0)=B(0,2)=xij[3][1]; B(2,1)=B(1,2)=xij[3][2]; B(2,2)=xij[3][3];
  B(3,0)=B(0,3)=xij[4][1]; B(3,1)=B(1,3)=xij[4][2]; B(3,2)=B(2,3)=xij[4][3]; B(3,3)=xij[4][4];
  
  _Delta4 = B.Determinant();
  
  knowDelta4 = true;
}
  
double Calculate4BodyProps::Delta3(){
  if(! knowDelta3) calcDelta3();
  return _Delta3;
}
void Calculate4BodyProps::calcDelta3(){
  TMatrixDSym B(3);
  if(! knowstij)fill_stij();
  B(0,0)=xij[1][1];
  B(1,0)=B(0,1)=xij[2][1]; B(1,1)=xij[2][2];
  B(2,0)=B(0,2)=xij[3][1]; B(2,1)=B(1,2)=xij[3][2]; B(2,2)=xij[3][3];
  _Delta3 = B.Determinant();
  knowDelta3 = true;
}
double Calculate4BodyProps::Delta2(){
  if(! knowDelta2) calcDelta2();
  return _Delta2;
}
void Calculate4BodyProps::calcDelta2(){
  TMatrixDSym B(2);
  if(! knowstij)fill_stij();
  B(0,0)=xij[1][1];
  B(1,0)=B(0,1)=xij[2][1]; B(1,1)=xij[2][2];
  _Delta2 = B.Determinant();
  knowDelta2 = true;
}
double Calculate4BodyProps::Delta1(){
  if(! knowstij)fill_stij();
  return xij[1][1];
}

double Calculate4BodyProps::phaseSpaceFactor(){
  double D1=Delta1(), D2=Delta2(), D3=Delta3(), D4=Delta4();
  if(D1 <=0 || -D2*D3 <=0 || -D4/D3 < 0 || -D1*D2 <=0){
    return 0;
  }

  double invps2 = -Delta4();
  if(invps2 <= 0) return 0;
  return  pi*pi/(32.0*m[0]*m[0]) *  1.0/sqrt(invps2);
}

double Calculate4BodyProps::showPhaseSpaceFactorCalculation(){
  double D1=Delta1(), D2=Delta2(), D3=Delta3(), D4=Delta4();
  cout << "Calculate4BodyProps::showPhaseSpaceFactorCalculation()" << endl;
  cout << "\n\t D1 " << D1 << ", D2 " << D2 
       << ", D3 " << D3 << ", D4 " << D4 << endl;
  cout << "\n\t D1 <=0 ? " << D1;
  cout << "\n\t -D2*D3 <=0 ? " << "- " << D2 << " * " << D3 
       << " = " << -D2*D3;
  cout << "\n\t -D4/D3 < 0 ? " << "- " << D4 << " / " << D3;
  if(D3 != 0) cout << " = " << -D4/D3;
  cout << "\n\t -D1*D2 <=0 ? " << "- " << D1 << " * " << D2 
       << " = " <<  -D1*D2
       << endl;

  if(D1 <=0 || -D2*D3 <=0 || -D4/D3 < 0 || -D1*D2 <=0){
    return 0;
  }

  double invps2 = -Delta4();
  cout << "\n\t invps2 = " << invps2 << endl;
  if(invps2 <= 0) return 0;
  double returnVal = pi*pi/(32.0*m[0]*m[0]) *  1.0/sqrt(invps2);
  cout << "\n\t returning " << returnVal << endl;
  return  returnVal;
}


void Calculate4BodyProps::s12MinMax(double& mi, double& ma){
  // pseudoparticle: 34
  // p0 = p1 + p2 + (p3 + p4)
  boundaries3Body(mi, ma
		  , s234()
		  , m0(), m1(), m2()
		  , sqrt(s34()));
}

void Calculate4BodyProps::s23MinMax(double& mi, double& ma){
  // pseudoparticle: 14
  // p0 = p2 + p3 + (p1 + p4)
  boundaries3Body(mi, ma
		  , s134()
		  , m0(), m2(), m3()
		  , sqrt(s14()));
}

void Calculate4BodyProps::s34MinMax(double& mi, double& ma){
  // pseudoparticle: 12
  // p0 = p3 + p4 + (p1 + p2)
  boundaries3Body(mi, ma
		  , s124()
		  , m0(), m3(), m4()
		  , sqrt(s12()));
}

void Calculate4BodyProps::t40MinMax(double& mi, double& ma){
  // = s123MinMax
  // pseudoparticle: 23
  // p0 = p1 + (p2 + p3) + p4
  boundaries3Body(mi, ma
		  , s234()
		  , m0(), m1(), sqrt(s23())
		  , m4());
}

void Calculate4BodyProps::t01MinMax(double& mi, double& ma){
  // = s234MinMax
  // pseudoparticle: 34
  // p0 = p2 + (p3 + p4) + p1
  boundaries3Body(mi, ma
		  , s134()
		  , m0(), m2(), sqrt(s34())
		  , m1());
}


// the actual 4-momenta - see page 208 of THE BOOK:
TLorentzVector Calculate4BodyProps::p1(){
  if(! knowp1) calcp1();
  return _p1;
}
void Calculate4BodyProps::calcp1(){
  double D1=Delta1();
  if(D1 < 0){
    TLorentzVector p(-9999,-9999,-9999,-9999);
    _p1 = p;
  }else{
    TLorentzVector p(0, 0, 0, sqrt(D1));
    _p1 = p;
  }
  knowp1 = true;
}

TLorentzVector Calculate4BodyProps::p2(){
  if(! knowp2) calcp2();
  return _p2;
}
void Calculate4BodyProps::calcp2(){
  double D1=Delta1(), D2=Delta2();
  if(D1 <= 0 || -D2/D1 < 0){
    TLorentzVector p(-9999,-9999,-9999,-9999);
    _p2 = p;
  }else{
    TLorentzVector p(0, 0, sqrt(-D2/D1), G(1,2)/sqrt(D1));
    _p2 = p;
  }
  knowp2 = true;
}
TLorentzVector Calculate4BodyProps::p3(){
  if(! knowp3) calcp3();
  return _p3;
}
void Calculate4BodyProps::calcp3(){
  bool dbThis =false;
  double D1=Delta1(), D2=Delta2(), D3=Delta3();
  if(D1 <= 0 || (- D1 * D2) <= 0 || -D3/D2 < 0 ){
    if(dbThis) cout << "calcp3, failing because  "
		    << D1 <<" <= " << 0 << " || " 
		    << (- D1 * D2) << " <= " << 0
		    << " || " << -D3/D2 << " < " << 0 <<endl;
    if(dbThis) cout << " Delta3 = " << D3 << endl;
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

TLorentzVector Calculate4BodyProps::p4(){
  if(! knowp4) calcp4();
  return _p4;
}
void Calculate4BodyProps::calcp4(){
  double D1=Delta1(), D2=Delta2(), D3=Delta3(), D4=Delta4();
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

TLorentzVector Calculate4BodyProps::p0(){
  if(! knowp0) calcp0();
  return _p0;
}
void Calculate4BodyProps::calcp0(){
  double D1=Delta1(), D2=Delta2(), D3=Delta3(), D4=Delta4();
  if(D1 <=0 || -D2*D3 <=0 || -D4/D3 < 0 || -D1*D2 <=0){
    TLorentzVector p(-9999,-9999,-9999,-9999);
    _p0 = p;
  }else{
    _p0 = p1() + p2() + p3() + p4();
  }
  knowp0 = true;
}

TLorentzVector Calculate4BodyProps::pVec(int i){
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
//

