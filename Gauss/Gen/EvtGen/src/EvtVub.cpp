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
// Module: EvtVub.cc
//
// Description: Routine to decay a particle according th phase space
//
// Modification history:
//
//    Sven Menke       January 17, 2001       Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtVub.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtVector4R.hh"
#include "EvtGen/EvtHepRandomEngine.hh"
#include "EvtGen/EvtPFermi.hh"
#include "EvtGen/EvtVubdGamma.hh"
#include "CLHEP/Random/RandGeneral.h"
#include "EvtGen/EvtRandom.hh"

EvtVub::~EvtVub() {
  delete _dGamma;
  delete _pFermi;
  delete [] _masses;
  delete [] _weights;
}

void EvtVub::getName(EvtString& model_name){

  model_name="VUB";     

}

EvtDecayBase* EvtVub::clone(){

  return new EvtVub;

}


void EvtVub::init(){

  // check that there are at least 6 arguments

  if (getNArg()<6) {

    report(ERROR,"EvtGen") << "EvtVub generator expected "
                           << " at least 6 arguments (mb,a,alpha_s,Nbins,m1,w1,...) but found: "
			   <<getNArg()<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();

  }
  
  _mb      	= getArg(0);
  _a       	= getArg(1);
  _alphas  	= getArg(2);
  _nbins        = abs((int)getArg(3));
  _storeQplus   = (getArg(3)<0?1:0);
  _masses       = new double[_nbins];
  _weights      = new double[_nbins];
 
  if (getNArg()-4 != 2*_nbins) {
    report(ERROR,"EvtGen") << "EvtVub generator expected " 
                           << _nbins << " masses and weights but found: "
			   <<(getNArg()-4)/2 <<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  }
  int i,j = 4;
  double maxw = 0.;
  for (i=0;i<_nbins;i++) {
    _masses[i] = getArg(j++);
    if (i>0 && _masses[i] <= _masses[i-1]) {
      report(ERROR,"EvtGen") << "EvtVub generator expected " 
			     << " mass bins in ascending order!"
			     << "Will terminate execution!"<<endl;
      ::abort();
    }
    _weights[i] = getArg(j++);
    if (_weights[i] < 0) {
      report(ERROR,"EvtGen") << "EvtVub generator expected " 
			     << " weights >= 0, but found: " 
			     <<_weights[i] <<endl;
      report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
      ::abort();
    }
    if ( _weights[i] > maxw ) maxw = _weights[i];
  }
  if (maxw == 0) {
    report(ERROR,"EvtGen") << "EvtVub generator expected at least one " 
			   << " weight > 0, but found none! " 
			   << "Will terminate execution!"<<endl;
    ::abort();
  }
  for (i=0;i<_nbins;i++) _weights[i]/=maxw;

  // the maximum dGamma*p2 value depends on alpha_s only:

  const double dGMax0 = 3.;
  _dGMax = 0.21344+8.905*_alphas;
  if ( _dGMax < dGMax0 ) _dGMax = dGMax0;

  // for the Fermi Motion we need a B-Meson mass - but it's not critical
  // to get an exact value; in order to stay in the phase space for
  // B+- and B0 use the smaller mass

  EvtId BP=EvtPDL::getId("B+");
  EvtId B0=EvtPDL::getId("B0");
  
  double mB0 = EvtPDL::getMaxMass(B0);
  double mBP = EvtPDL::getMaxMass(BP);

  double mB = (mB0<mBP?mB0:mBP);
  
  const double xlow = -_mb;
  const double xhigh = mB-_mb;
  const int aSize = 10000;

  EvtPFermi pFermi(_a,mB,_mb);
  HepDouble pf[aSize];

  for(i=0;i<aSize;i++){
    double kplus = xlow + (double)(i+0.5)/((double)aSize)*(xhigh-xlow);
    pf[i] = pFermi.getFPFermi(kplus);
  }

    static EvtHepRandomEngine myEngine;
//  static EvtHepRandomEngine *myEngine;

  _pFermi = new RandGeneral(myEngine,pf,aSize,0);
  _dGamma = new EvtVubdGamma(_alphas);
  
  checkNDaug(3);

}

void EvtVub::initProbMax(){

  noProbMax();

}

void EvtVub::decay( EvtParticle *p ){

  int j;
  // B+ -> u-bar specflav l+ nu

  EvtParticle *xuhad, *lepton, *neutrino;
  EvtVector4R p4;

  p->initializePhaseSpace(getNDaug(),getDaugs());

  xuhad=p->getDaug(0);
  lepton=p->getDaug(1);
  neutrino=p->getDaug(2);

  double mB = p->mass();
  double ml = lepton->mass();

  const double xlow = -_mb;
  const double xhigh = mB-_mb;
  const double p2epsilon=1e-10;

  double sh = 0;

  // Fermi motion does not need to be computed inside the
  // tryit loop as m_b in Gamma0 does not need to be replaced by (m_b+kplus).
  // The difference however should be of the Order (lambda/m_b)^2 which is
  // beyond the considered orders in the paper anyway ...

  // for alpha_S = 0 and a mass cut on X_u not all values of kplus are 
  // possible. The maximum value is mB/2-_mb + sqrt(mB^2/4-_masses[0]^2)
  double kplus = 2*xhigh;
  
  while( kplus >= xhigh || kplus <= xlow 
	 || (_alphas == 0 && kplus >= mB/2-_mb 
	     + sqrt(mB*mB/4-_masses[0]*_masses[0]))) {
    kplus = _pFermi->shoot();
    kplus = xlow + kplus*(xhigh-xlow);
  }
  double x;
  int tryit = 1;
  double z;
  double p2;
  double qplus,El,Eh;

  while (tryit) {

    x = EvtRandom::Flat();
    z = EvtRandom::Flat(0,2);
    p2 = pow(10,log10(p2epsilon) - log10(p2epsilon)*EvtRandom::Flat());

    qplus = mB-_mb-kplus;
    El = x*(mB-qplus)/2;
    if ( El > ml && El < mB/2) {

      Eh = z*(mB-qplus)/2+qplus;
      if ( Eh > 0 && Eh < mB ) {

	sh = p2*pow(mB-qplus,2)+2*qplus*(Eh-qplus)+qplus*qplus;
	if ( sh > _masses[0]*_masses[0]
	     && mB*mB + sh - 2*mB*Eh > ml*ml) {

	  double xran = EvtRandom::Flat();
	  double y = _dGamma->getdGdxdzdp(x,z,p2)/_dGMax*p2;
	  j=0;
	  double m = sqrt(sh);
	  while ( j < _nbins && m > _masses[j] ) j++; 
	  double w = _weights[j-1];
	  y *= w;
	  if ( y > 1 ) report(WARNING,"EvtGen")<<"EvtVub decay probability > 1 found: " << y << endl;
	  if ( y >= xran ) tryit = 0;
	}
      }
    }
  }
  // o.k. we have the three kineamtic variables 
  // now calculate a flat cos Theta_H [-1,1] distribution of the 
  // hadron flight direction w.r.t the B flight direction 
  // because the B is a scalar and should decay isotropic.
  // Then chose a flat Phi_H [0,2Pi] w.r.t the B flight direction 
  // and and a flat Phi_L [0,2Pi] in the W restframe w.r.t the 
  // W flight direction.

  double ctH = EvtRandom::Flat(-1,1);
  double phH = EvtRandom::Flat(0,2*M_PI);
  double phL = EvtRandom::Flat(0,2*M_PI);

  // now compute the four vectors in the B Meson restframe
    
  double ptmp,sttmp;
  // calculate the hadron 4 vector in the B Meson restframe

  sttmp = sqrt(1-ctH*ctH);
  ptmp = sqrt(Eh*Eh-sh);
  double pHB[4] = {Eh,ptmp*sttmp*cos(phH),ptmp*sttmp*sin(phH),ptmp*ctH};

  p4.set(pHB[0],pHB[1],pHB[2],pHB[3]);
  xuhad->init( getDaug(0), p4);

  if (_storeQplus ) {
    // cludge to store the hidden parameter q+ with the decay; 
    // the lifetime of the Xu is abused for this purpose.
    // tau = 1 ps corresponds to ctau = 0.3 mm -> in order to
    // stay well below BaBars sensitivity we take q+/(10000 GeV) which 
    // goes up to 0.0005 in the most extreme cases as ctau in mm.
    // To extract q+ back from the StdHepTrk its necessary to get
    // delta_ctau = Xu->anyDaughter->getVertexTime()-Xu->getVertexTime()
    // where these pseudo calls refere to the StdHep time stored at
    // the production vertex in the lab for each particle. The boost 
    // has to be reversed and the result is:
    //
    // q+ = delta_ctau * 10000 GeV/mm * Mass_Xu/Energy_Xu     
    //
    xuhad->setLifetime(qplus/10000.);
  }

  // calculate the W 4 vector in the B Meson restrframe

  double apWB = ptmp;
  double pWB[4] = {mB-Eh,-pHB[1],-pHB[2],-pHB[3]};

  // first go in the W restframe and calculate the lepton and
  // the neutrino in the W frame

  double mW2   = mB*mB + sh - 2*mB*Eh;
  double beta  = ptmp/pWB[0];
  double gamma = pWB[0]/sqrt(mW2);

  double pLW[4];
    
  ptmp = (mW2-ml*ml)/2/sqrt(mW2);
  pLW[0] = sqrt(ml*ml + ptmp*ptmp);

  double ctL = (El - gamma*pLW[0])/beta/gamma/ptmp;
  if ( ctL < -1 ) ctL = -1;
  if ( ctL >  1 ) ctL =  1;
  sttmp = sqrt(1-ctL*ctL);

  // eX' = eZ x eW
  double xW[3] = {-pWB[2],pWB[1],0};
  // eZ' = eW
  double zW[3] = {pWB[1]/apWB,pWB[2]/apWB,pWB[3]/apWB};
  
  double lx = sqrt(xW[0]*xW[0]+xW[1]*xW[1]);
  for (j=0;j<2;j++) 
    xW[j] /= lx;

  // eY' = eZ' x eX'
  double yW[3] = {-pWB[1]*pWB[3],-pWB[2]*pWB[3],pWB[1]*pWB[1]+pWB[2]*pWB[2]};
  double ly = sqrt(yW[0]*yW[0]+yW[1]*yW[1]+yW[2]*yW[2]);
  for (j=0;j<3;j++) 
    yW[j] /= ly;

  // p_lep = |p_lep| * (  sin(Theta) * cos(Phi) * eX'
  //                    + sin(Theta) * sin(Phi) * eY'
  //                    + cos(Theta) *            eZ')
  for (j=0;j<3;j++)
    pLW[j+1] = sttmp*cos(phL)*ptmp*xW[j] 
      +        sttmp*sin(phL)*ptmp*yW[j]
      +          ctL         *ptmp*zW[j];

  double apLW = ptmp;
  // calculate the neutrino 4 vector in the W restframe

  double pNW[4] = {sqrt(mW2)-pLW[0],-pLW[1],-pLW[2],-pLW[3]};
    
  // boost them back in the B Meson restframe
  
  double appLB = beta*gamma*pLW[0] + gamma*ctL*apLW;
 
  ptmp = sqrt(El*El-ml*ml);
  double ctLL = appLB/ptmp;

  if ( ctLL >  1 ) ctLL =  1;
  if ( ctLL < -1 ) ctLL = -1;
    
  double pLB[4] = {El,0,0,0};
  double pNB[4] = {pWB[0]-El,0,0,0};

  for (j=1;j<4;j++) {
    pLB[j] = pLW[j] + (ctLL*ptmp - ctL*apLW)/apWB*pWB[j];
    pNB[j] = pWB[j] - pLB[j];
  }

  p4.set(pLB[0],pLB[1],pLB[2],pLB[3]);
  lepton->init( getDaug(1), p4);

  p4.set(pNB[0],pNB[1],pNB[2],pNB[3]);
  neutrino->init( getDaug(2), p4);

  return ;
}


