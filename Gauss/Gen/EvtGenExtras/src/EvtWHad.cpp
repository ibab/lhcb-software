//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtWHad.cc
//
// Description: Routine to calculate W -> (n pi) + (m K) current
//			according to [Kuhn, Was, Acta.Phys.Polon B39 (2008) 147]
//
// Modification history:
//	AVL	20 Jan, 2013	Module created
//
//------------------------------------------------------------------------
// 

#include "EvtGenModels/EvtWHad.hh"

using namespace std;

// =================== W+ -> pi_ current ========================================
EvtVector4C EvtWHad::WCurrent(EvtVector4R q1) { 
  return q1;
}

//====================== W+ -> pi+ pi0 current =========================================
EvtVector4C EvtWHad::WCurrent(EvtVector4R q1, EvtVector4R q2) { 
  return BWr(q1+q2)*(q1-q2);
}

//========================= W+ -> pi+ pi+ pi- current ==============================================
EvtVector4C EvtWHad::WCurrent(EvtVector4R q1, EvtVector4R q2, EvtVector4R q3) {
 EvtVector4R Q=q1+q2+q3;
 double Q2=Q.mass2();
 return BWa(Q)*( (q1-q3) - (Q*(Q*(q1-q3))/Q2)*BWr(q2+q3) +
	      (q2-q3) - (Q*(Q*(q2-q3))/Q2)*BWr(q1+q3) ); 
}

// ================= W+ -> pi+ pi+ pi- pi- pi+ current with symmetrization ================================
EvtVector4C EvtWHad::WCurrent(EvtVector4R q1, EvtVector4R q2, EvtVector4R q3, EvtVector4R q4, EvtVector4R q5) {
  EvtVector4C V = JB(q1, q2, q3, q4, q5) + JB(q5, q2, q3, q4, q1) + JB(q1, q5, q3, q4, q2) +
 		  JB(q1,q2,q4,q3,q5)+JB(q5,q2,q4,q3,q1)+JB(q1,q5,q4,q3,q2); 
  return V;
}


// =========================W+ -> K+ K- pi+ current ==================================================
EvtVector4C EvtWHad::WCurrent_KKP(EvtVector4R pKplus, EvtVector4R pKminus, EvtVector4R pPiPlus) {
  const double mK_892 = 0.892, gammaK_892 = 0.051;
  const double mA1 = 1.239, gammaA1 = 0.600;
  
  EvtVector4R q = pKplus + pKminus + pPiPlus;
  double q2=q.mass2();
  EvtVector4R pK = pKminus + pPiPlus;
  double pK2 = pK.mass2();

  EvtComplex I(0., 1.), den1, den2;

  den1=1/(q2-mA1*mA1+I*mA1*gammaA1);
  den2=1/(pK2-mK_892*mK_892+I*mK_892*gammaK_892);
  
  EvtTensor4C ten = EvtTensor4C::g()-(1./q2)*EvtGenFunctions::directProd(q,q);
  EvtVector4C vec=den1*den2*(pKminus-pPiPlus);
  vec = ten.cont2(vec);
  return vec;

}


// hadronic current W+ -> K+ pi+ pi-
EvtVector4C EvtWHad::WCurrent_KPP(EvtVector4R pKplus, EvtVector4R pPiPlus, EvtVector4R pPiMinus) {
  static double const cK1p=0.210709, cK1r=-0.0152997, cK2p=0.0945309, cK2r=0.504315;
  const double mK1_1270 = 1.270, gammaK1_1270 = 0.090, gK1270_Krho = 2.71, gK1270_KsPi = 0.792;
  const double mK1_1400 = 1.400, gammaK1_1400 = 0.174, gK11400_Krho = 	0.254, gK11400_KsPi = 2.509;
  const double mK_892 = 0.892, gammaK_892 = 0.051, gK892_Kpi = 3.26;
  const double mRho = 0.770, gammaRho = 0.150, gRho_PiPi = 6.02;
  
  EvtVector4R q = pKplus + pPiPlus + pPiMinus;
  double q2 = q.mass2(), pp2;
  
  EvtVector4C curr(0,0,0,0),curr1;
  EvtComplex I(0., 1.), den1, den2;
  
  // W+ -> K1+(1270) -> K+ rho0 -> K+ pi+ pi-
  den1 = gK1270_Krho/(q2 - mK1_1270*mK1_1270 + I*mK1_1270*gammaK1_1270);
  pp2 = (pPiPlus+pPiMinus).mass2();
  den2 = gRho_PiPi/(pp2 - mRho*mRho + I*mRho*gammaRho);
  curr1 = (pPiPlus - pPiMinus)*den1*den2;
  curr = curr + cK1r*curr1;
  
  // W+ -> K1+(1270) -> K*(892)0 pi+ -> K+ pi- pi-
  den1 = gK1270_KsPi/(q2 - mK1_1270*mK1_1270 + I*mK1_1270*gammaK1_1270);
  pp2 = (pKplus+pPiMinus).mass2();
  den2 = gK892_Kpi/(pp2 - mK_892*mK_892 + I*mK_892*gammaK_892);
  curr1 = (pKplus - pPiMinus)*den1*den2;
  curr = curr + cK1p*curr1;

  // W+ -> K1+(1400) -> K+ rho0 -> K+ pi+ pi-
  den1 = gK11400_Krho/(q2 - mK1_1400*mK1_1400 + I*mK1_1400*gammaK1_1400);
  pp2 = (pPiMinus+pPiPlus).mass2();
  den2 = gRho_PiPi/(pp2 - mRho*mRho + I*mRho*gammaRho);
  curr1 = (pPiPlus - pPiMinus)*den1*den2;
  curr = curr + cK2r*curr1;
  
  // W+ -> K1+(1400) -> K*(892)0 pi+ -> K+ pi- pi+
  den1 = gK11400_KsPi/(q2 - mK1_1400*mK1_1400 + I*mK1_1400*gammaK1_1400);
  pp2 = (pKplus+pPiMinus).mass2();
  den2 = gK892_Kpi/(pp2 - mK_892*mK_892 + I*mK_892*gammaK_892);
  curr1 = (pKplus - pPiPlus)*den1*den2;
  curr = curr + cK2p*curr1;

  EvtTensor4C ten = EvtTensor4C::g()-(1./q2)*EvtGenFunctions::directProd(q,q);
  curr = ten.cont2(curr);

  return curr;
}


// a1 -> pi+ pi+ pi- BW
EvtComplex EvtWHad::BWa(EvtVector4R q) {
  double const _mA1=1.26, _GA1=0.4;
  EvtComplex I(0,1);
  double Q2 = q.mass2();
  double GA1=_GA1*pi3G(Q2)/pi3G(_mA1*_mA1);
  EvtComplex denBA1(_mA1*_mA1 - Q2,-1.*_mA1*GA1);
  return _mA1*_mA1 / denBA1; 
}


EvtComplex EvtWHad::BWf(EvtVector4R q) {
  double const mf=0.8, Gf=0.6;
  EvtComplex I(0,1);
  double Q2 = q.mass2();
  return mf*mf/(mf*mf-Q2-I*mf*Gf); 
}

EvtComplex EvtWHad::BWr(EvtVector4R q) {
	double _mRho = 0.775, _gammaRho=0.149, _mRhopr=1.364,  _gammaRhopr=0.400, _beta=-0.108;
	double m1=EvtPDL::getMeanMass(EvtPDL::getId("pi+")), m2=EvtPDL::getMeanMass(EvtPDL::getId("pi+"));
	double mQ2=q.mass2();
	
	// momenta in the rho->pipi decay
	double dRho= _mRho*_mRho - m1*m1 - m2*m2;
	double pPiRho = (1.0/_mRho)*sqrt((dRho*dRho)/4.0 - m1*m1*m2*m2);
	
	double dRhopr= _mRhopr*_mRhopr - m1*m1 - m2*m2;
	double pPiRhopr = (1.0/_mRhopr)*sqrt((dRhopr*dRhopr)/4.0 - m1*m1*m2*m2);
	
	double dQ= mQ2 - m1*m1 - m2*m2;
	double pPiQ = (1.0/sqrt(mQ2))*sqrt((dQ*dQ)/4.0 - m1*m1*m2*m2);
	
	
	double gammaRho = _gammaRho*_mRho/sqrt(mQ2)*pow((pPiQ/pPiRho),3);
	EvtComplex BRhoDem(_mRho*_mRho - mQ2,-1.0*_mRho*gammaRho);
	EvtComplex BRho= _mRho*_mRho / BRhoDem;
	
	double gammaRhopr = _gammaRhopr*_mRhopr/sqrt(mQ2)*pow((pPiQ/pPiRhopr),3);
	EvtComplex BRhoprDem(_mRhopr*_mRhopr - mQ2,-1.0*_mRho*gammaRhopr);
	EvtComplex BRhopr= _mRhopr*_mRhopr / BRhoprDem;
	
	return (BRho + _beta*BRhopr)/(1+_beta);
}

double EvtWHad::pi3G(double m2) {
 double mPi = EvtPDL::getMeanMass(EvtPDL::getId("pi+"));
 double _mRho = 0.775;
  if ( m2 > (_mRho+mPi) ) {
    return m2*(1.623 + 10.38/m2 - 9.32/(m2*m2) + 0.65/(m2*m2*m2));
  }
  else {
    double t1=m2-9.0*mPi*mPi;
    return 4.1*pow(t1,3.0)*(1.0 - 3.3*t1+5.8*t1*t1);
  };
}

EvtVector4C EvtWHad::JB( EvtVector4R p1, EvtVector4R p2, EvtVector4R p3, EvtVector4R p4, EvtVector4R p5) {
  EvtVector4R Qtot = p1+p2+p3+p4+p5, Qa=p1+p2+p3;
  EvtTensor4C T= (1/Qtot.mass2())*EvtGenFunctions::directProd(Qtot,Qtot) - EvtTensor4C::g();
  EvtVector4R V13 = Qa*( p2*(p1-p3) )/Qa.mass2() - (p1-p3);
  EvtVector4R V23 = Qa*( p1*(p2-p3) )/Qa.mass2() - (p2-p3);
  return BWa(Qtot)*BWa(Qa)*BWf(p4+p5)*(
    T.cont1(V13)*BWr(p1+p3) + T.cont1(V23)*BWr(p2+p3)
  );
}

