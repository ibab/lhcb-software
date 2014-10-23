// Include files 



// local

#include "EvtGenModels/EvtRareLbToLllFFlQCD.hh"
#include "EvtGenModels/EvtWilsonCoefficients.hh"
#include "EvtGenBase/EvtVector4R.hh" 
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenBase/EvtConst.hh"
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Implementation file for class : EvtRareLbToLllFFlQCD
//
// 2014-10-22 : Michal Kreps
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================


EvtRareLbToLllFFlQCD::EvtRareLbToLllFFlQCD() : EvtRareLbToLllFFBase()  
  {};


void EvtRareLbToLllFFlQCD::init()
{
  FiLQCDconsts[0][0] = 3.975;
  FiLQCDconsts[0][1] = 1.776;
  FiLQCDconsts[1][0] = -0.385;
  FiLQCDconsts[1][1] = 1.156;


  double mb = 4.19;
  double mu = mb;
  EvtWilsonCoefficients wc;
  alphaS = wc.alphaS(mu, 5, 0.2167);  // Is this correct or do I need to change Lambda?
  report(INFO,"EvtGen") <<"alphaS="<<alphaS<<std::endl;
  cgamma = 1-alphaS/EvtConst::pi*(4./3+log(mu/mb));
  cv = 2./3*alphaS/EvtConst::pi;
  csigma = 1-alphaS/EvtConst::pi*(4./3+5./3*log(mu/mb));

  report(INFO,"EvtGen") << " EvtRareLbToLll is using form factors from arXiv:1212.4827 " << std::endl;
};

//=============================================================================
// Destructor
//=============================================================================

EvtRareLbToLllFFlQCD::~EvtRareLbToLllFFlQCD() {
  
};

//=============================================================================


void EvtRareLbToLllFFlQCD::getFF( EvtParticle* parent, 
                             EvtParticle* lambda,
                             EvtRareLbToLllFFBase::FormFactors& FF )
{
  // Find the FF information for this particle, start by setting all to zero
  FF.areZero();
  
  double m1 = parent->getP4().mass();
  double m2 = lambda->getP4().mass();
  double q2 = ( parent->getP4() - lambda->getP4() ).mass2();
  double vdotvp = calculateVdotV(parent, lambda);
  double shat = (m1*m1+m2*m2-q2)/2./m1-m2;

  double fLQCD[2];
  for (int i=0;i<2;++i) {
    fLQCD[i] = formFactorParametrization(shat, FiLQCDconsts[i][0], FiLQCDconsts[i][1]);
  }

  // Both v^2==v'^2==1 by definition
  FF.F_[0] = cgamma*(fLQCD[0]-fLQCD[1]);
  FF.F_[1] = cv*fLQCD[0]+(2*cgamma+cv)*fLQCD[1];
  FF.F_[2] = 0; 

  FF.G_[0] = cgamma*(fLQCD[0]+fLQCD[1]);
  FF.G_[1] = -cv*fLQCD[0]+(2*cgamma+cv)*fLQCD[1];
  FF.G_[2] = 0;

  FF.FT_[0] = -csigma*( (m1+m2)*fLQCD[0]+(2*m2*vdotvp+m2-m1)*fLQCD[1] );
  FF.FT_[1] = csigma*( 2*m2*fLQCD[1] + m1*(fLQCD[0]-fLQCD[1]) );
  FF.FT_[2] = csigma*(fLQCD[0]+fLQCD[1])*m2;

  FF.GT_[0] = csigma*( -(m1+m2)*fLQCD[0]+(-m2*-m2*vdotvp+m1)*fLQCD[1] ); 
  FF.GT_[1] = csigma*( 2*m2*fLQCD[1] + m1*(fLQCD[0]-fLQCD[1]) ); 
  FF.GT_[2] = csigma*(fLQCD[0]+fLQCD[1])*m2;

  return ;
}

double EvtRareLbToLllFFlQCD::formFactorParametrization(double var, double N, double X){ 
  return N/( (X+var)*(X+var) );
}


