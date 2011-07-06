// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#include "RooRandom.h"
#include "Roo3BodyPhaseSpaceAcc.h"
#include <complex>
#include <iostream>
using namespace std;

bool Roo3BodyPhaseSpaceAcc::boundaries(Double_t& s12Min
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


Roo3BodyPhaseSpaceAcc::Roo3BodyPhaseSpaceAcc(const char* name
				       , const char* title
				       , RooAbsReal& _m12sq
				       , RooAbsReal& _m23sq
				       , RooAbsReal& _mumM
				       , RooAbsReal& _d1M
				       , RooAbsReal& _d2M
				       , RooAbsReal& _d3M
				       )
  : RooAbsPhaseSpace(name, title)
  , m12sq("m12sq", "m12sq", this, _m12sq)
  , m23sq("m23sq", "m23sq", this, _m23sq)
  , mumM("mumM", "mumM", this, _mumM)
  , d1M("d1M", "d1M", this, _d1M)
  , d2M("d2M", "d2M", this, _d2M)
  , d3M("d3M", "d3M", this, _d3M)
{
  
}

Roo3BodyPhaseSpaceAcc::Roo3BodyPhaseSpaceAcc(const Roo3BodyPhaseSpaceAcc& other
					   , const char* name
				       )
  : RooAbsPhaseSpace(other, name)
  , m12sq("m12sq", this, other.m12sq)
  , m23sq("m23sq",this, other.m23sq)
  , mumM("mumM",this, other.mumM)
  , d1M("d1M",this, other.d1M)
  , d2M("d2M",this, other.d2M)
  , d3M("d3M",this, other.d3M)
{
  
}

Double_t Roo3BodyPhaseSpaceAcc::m12AbsMin() const{
  double mi = (d1M + d2M)*(d1M + d2M);
  if(m12sq.min() > mi) return m12sq.min();
  return mi;
}
Double_t Roo3BodyPhaseSpaceAcc::m23AbsMin() const{
  double mi = (d2M + d2M)*(d2M + d2M);
  if(m23sq.min() > mi) return m23sq.min();
  return mi;
}
Double_t Roo3BodyPhaseSpaceAcc::m12AbsMax() const{
  double ma = (mumM - d3M) * (mumM - d3M);
  if(m12sq.max() < ma) return m12sq.max();
  return ma;
}
Double_t Roo3BodyPhaseSpaceAcc::m23AbsMax() const{
  double ma = (mumM - d1M) * (mumM - d1M);
  if(m23sq.max() < ma) return m23sq.max();
  return ma;
}

bool Roo3BodyPhaseSpaceAcc::m23Boundaries(Double_t& m23sqMin
				       , Double_t& m23sqMax
				       ) const{
  m23sqMin = m23sqMax = -9999;

  if(m12sq < 0) return false;
  if(m12sq < m12AbsMin()) return false;
  if(m12sq > m12AbsMax()) return false;

  Double_t mumMsq = mumM*mumM;
  if(m12sq > mumMsq) return false;

  Double_t m1sq=d1M*d1M, m2sq=d2M*d2M, m3sq=d3M*d3M;

  Double_t m12 = sqrt(m12sq);
  Double_t E2 = (m12sq - m1sq + m2sq)/(2*m12);
  if(E2 < d2M) return false;

  Double_t E3 = (mumMsq - m12sq - m3sq)/(2*m12);
  if(E3 < d3M) return false;

  Double_t E2_E3sq = (E2+E3)*(E2+E3);
  Double_t p2_12 = sqrt(E2*E2 - m2sq); // p2 in m12 restframe
  Double_t p3_12 = sqrt(E3*E3 - m3sq);
  Double_t P_minSq = (p2_12 - p3_12)*(p2_12 - p3_12);
  Double_t P_maxSq = (p2_12 + p3_12)*(p2_12 + p3_12);
  
  m23sqMax = E2_E3sq - P_minSq;
  m23sqMin = E2_E3sq - P_maxSq;
  if(false)cout << "m23sqMax m23sqMin " 
		<< m23sqMax << ", " << m23sqMin 
		<< endl;
  return true;
}
bool Roo3BodyPhaseSpaceAcc::m12Boundaries(Double_t& m12sqMin
				       , Double_t& m12sqMax
				       ) const{
  m12sqMin = m12sqMax = -9999;

  if(m23sq < 0) return 0;
  if(m23sq < m23AbsMin()) return false;
  if(m23sq > m23AbsMax()) return false;


  Double_t mumMsq = mumM*mumM;

  if(m23sq > mumMsq) return false;

  Double_t m1sq=d1M*d1M, m2sq=d2M*d2M, m3sq=d3M*d3M;

  Double_t m23 = sqrt(m23sq);
  Double_t E3 = (m23sq - m2sq + m3sq)/(2*m23);
  if(E3 < d3M) return false;

  Double_t E1 = (mumMsq - m23sq - m1sq)/(2*m23);
  if(E1 < d1M) return false;

  Double_t E3_E1sq = (E3+E1)*(E3+E1);
  Double_t p3_23 = sqrt(E3*E3 - m3sq); // p2 in m12 restframe
  Double_t p1_23 = sqrt(E1*E1 - m1sq);
  Double_t P_minSq = (p3_23 - p1_23)*(p3_23 - p1_23);
  Double_t P_maxSq = (p3_23 + p1_23)*(p3_23 + p1_23);
  
  m12sqMax = E3_E1sq - P_minSq;
  m12sqMin = E3_E1sq - P_maxSq;
  if(false)cout << "m12sqMax m12sqMin " 
		<< m12sqMax << ", " 
		<< m12sqMin << endl;
  return true;
}
Int_t Roo3BodyPhaseSpaceAcc::getAnalyticalIntegral(RooArgSet& allVars
						, RooArgSet& analVars
						, const char* rangeName
						) const{
  cout << " getAnalyticalIntegral got called" << endl;
  if(matchArgs(allVars, analVars, m12sq)) return 1;
  if(matchArgs(allVars, analVars, m23sq)) return 2;
  cout << " returning 0" << endl;
  return 0;
}
 
Double_t Roo3BodyPhaseSpaceAcc::analyticalIntegral(Int_t code
						, const char* rangeName
						) const{
  //  cout << " analy.. got called " << endl;
  Double_t mi, ma;
  if(code == 1){
    if(! m12Boundaries(mi, ma)) return 0;
    if(ma > m12sq.max()) ma = m12sq.max();
    if(mi < m12sq.min()) mi = m12sq.min();
  }else if(code == 2){
    if(! m23Boundaries(mi, ma)) return 0;
    if(ma > m23sq.max()) ma = m23sq.max();
    if(mi < m23sq.min()) mi = m23sq.min();
  }
  return (ma-mi);
}

Int_t Roo3BodyPhaseSpaceAcc::getGenerator(const RooArgSet &dirVars
				       , RooArgSet &genVars
				       , Bool_t staticInitOK
				       ) const{
  //cout << " getGenerator got called" << endl;
  //cout << " dirVars: " << dirVars << endl;
  //cout << " genVars: " << genVars << endl;
  if(matchArgs(dirVars, genVars, m12sq, m23sq)) return 3;
  if(matchArgs(dirVars, genVars, m23sq)) return 1;
  if(matchArgs(dirVars, genVars, m12sq)) return 2;
  //cout << " getGenerator returning 0" << endl;
  return 0;
}

void Roo3BodyPhaseSpaceAcc::generateEvent(Int_t code){
  //cout << " generating Event with code " << code;
  Double_t mi, ma;
  if(code == 3){
    // first generate m12
    double maxWeight = m23AbsMax() - m23AbsMin();
    double w=-1, rnd=999;
    double wacc=-1, rndacc=999;
    
    do{
      do{
        m12sq = m12AbsMin() + 
          (RooRandom::randomGenerator()->Rndm())*
          (m12AbsMax() - m12AbsMin());
        if(! m23Boundaries(mi, ma)) continue;
        if(ma > m23sq.max()) ma = m23sq.max();
        if(mi < m23sq.min()) mi = m23sq.min();
        w = ma - mi;
        rnd = RooRandom::randomGenerator()->Rndm() * maxWeight;     
      }while( rnd > w );
      // now generate m23
      double genM23 = mi + (RooRandom::randomGenerator()->Rndm())*(ma-mi);
      m23sq = genM23;
      rndacc = RooRandom::randomGenerator()->Rndm();
      wacc = 1.0-0.08*(m23sq+m12sq);
    }while(rndacc > wacc);
    

    }else if(code == 1){
    if(! m12Boundaries(mi, ma)){
      m23sq = -1;
      return;
    }
    if(ma > m12sq.max()) ma = m12sq.max();
    if(mi < m12sq.min()) mi = m12sq.min();
    m23sq = mi + (RooRandom::randomGenerator()->Rndm())*(ma-mi);
  }else if(code == 2){
    if(! m23Boundaries(mi, ma)){
      m12sq = -1;
      return;
    }
    if(ma > m23sq.max()) ma = m23sq.max();
    if(mi < m23sq.min()) mi = m23sq.min();
    m12sq = mi + (RooRandom::randomGenerator()->Rndm())*(ma-mi);
  }
}



Double_t Roo3BodyPhaseSpaceAcc::evaluate() const{
  if(m12sq < 0 || m23sq < 0) return 0;

  Double_t mumMsq = mumM*mumM;
  if(m12sq > mumMsq || m23sq > mumMsq) return 0;

  Double_t m1sq=d1M*d1M, m2sq=d2M*d2M, m3sq=d3M*d3M;
  Double_t K = mumMsq + m1sq + m2sq + m3sq;
  Double_t m13sq = K - m12sq - m23sq;
  if(m13sq < 0 || m13sq > mumMsq) return 0;

  if(false)cout << "m13sq =" << m13sq << endl;

  Double_t m12 = sqrt(m12sq);
  Double_t E2 = (m12sq - m1sq + m2sq)/(2*m12);
  //  if(false)cout << "E2 " << E2 << ", m2 " << m2 << endl;
  if(E2 < d2M) return 0;
  if(false)cout << "E2 > m2" << endl;
  Double_t E3 = (mumMsq - m12sq - m3sq)/(2*m12);
  if(E3 < d3M) return 0;
  if(false)cout << "E3 > m3" << endl;

  Double_t E2_E3sq = (E2+E3)*(E2+E3);
  Double_t p2_12 = sqrt(E2*E2 - m2sq); // p2 in m12 restframe
  Double_t p3_12 = sqrt(E3*E3 - m3sq);
  Double_t P_minSq = (p2_12 - p3_12)*(p2_12 - p3_12);
  Double_t P_maxSq = (p2_12 + p3_12)*(p2_12 + p3_12);
  
  Double_t m23sqMax = E2_E3sq - P_minSq;
  Double_t m23sqMin = E2_E3sq - P_maxSq;
  if(false)cout << "m23sq = " << m23sq 
		<< ", max " << m23sqMax 
		<< ", min " << m23sqMin
		<< endl;

  if(m23sq > m23sqMax) return 0;
  if(m23sq < m23sqMin) return 0;


  double nonflat = 1.0-0.08*(m12sq+m23sq);


  

  return nonflat;
}

//
