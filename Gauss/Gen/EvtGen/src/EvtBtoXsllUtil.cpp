//--------------------------------------------------------------------------
//
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtBtoXsllUtil.cc
//
// Description: Routine to generate non-resonant B -> Xs l+ l- decays.
// It generates a dilepton mass spectrum according to
// F.Kruger and L.M.Sehgal, Phys. Lett. B380, 199 (1996)
// and then generates the two lepton momenta according to
// A.Ali, G.Hiller, L.T.Handoko and T.Morozumi, Phys. Rev. D55, 4105 (1997).
// The resultant Xs particles may be decayed by JETSET.
//
// Modification history:
//
//    Stephane Willocq        Jan  19, 2001       Module created
//------------------------------------------------------------------------
//
#ifdef WIN32
extern "C" double __stdcall DDILOG(const double & sh ) ;
#else
extern "C" double ddilog_(const double & sh);
#endif
//
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtBtoXsllUtil.hh"
#include <string>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtConst.hh"

EvtComplex EvtBtoXsllUtil::coeff9(double mb, double sh)
{
  // Compute the complex effective Wilson coefficient c9

  // First set the value of Wilson coefficients that do not depend
  // on the scaled dilepton invariant mass sh
  // These numbers and the following expressions can be found in
  // A.Ali and G.Hiller, Phys. Rev. D58, 074001 (1998)

  double c1 = -0.240;
  double c2 =  1.103;
  double c3 =  0.011;
  double c4 = -0.025;
  double c5 =  0.007;
  double c6 = -0.030;
  double c9ndr = 4.153;

  double mu =  4.8; // energy scale in GeV
  double mc =  1.4; // charm-quark mass in GeV
  double lambda_msbar = 0.225;
  double beta0 =  23.0 / 3.0;
  double beta1 = 116.0 / 3.0;

  double hlog = log(pow(mu/lambda_msbar,2));
  double alpha_s_mu = 4.0*EvtConst::pi*(1.0-(beta1/(beta0*beta0))
                         *(log(hlog)/hlog)) / (beta0*hlog);
  double zh = mc/mb;
#ifdef WIN32
  double omesh = - 2.0/9.0*EvtConst::pi*EvtConst::pi - 4.0/3.0*DDILOG(sh)
                 - 2.0/3.0*log(sh)*log(1.0-sh)
                 - (5.0+4.0*sh)/(3.0*(1.0+2.0*sh)) * log(1.0-sh)
                 - 2.0*sh*(1.0+sh)*(1.0-2.0*sh)
                 /(3.0*pow(1.0-sh,2)*(1.0+2.0*sh)) * log(sh)
                 + (5.0+9.0*sh-6.0*sh*sh)/(6.0*(1.0-sh)*(1.0+2.0*sh));
#else
  double omesh = - 2.0/9.0*EvtConst::pi*EvtConst::pi - 4.0/3.0*ddilog_(sh)
                 - 2.0/3.0*log(sh)*log(1.0-sh)
                 - (5.0+4.0*sh)/(3.0*(1.0+2.0*sh)) * log(1.0-sh)
                 - 2.0*sh*(1.0+sh)*(1.0-2.0*sh)
                 /(3.0*pow(1.0-sh,2)*(1.0+2.0*sh)) * log(sh)
                 + (5.0+9.0*sh-6.0*sh*sh)/(6.0*(1.0-sh)*(1.0+2.0*sh));
#endif
  double etash = 1.0 + alpha_s_mu*omesh/EvtConst::pi;

  double y = 4.*zh*zh/sh;
  double tmp1 = sqrt(fabs(1.0-y));
  double tmp2 = 2.0/9.0*(2.0+y)*tmp1;

  double gRe, gIm;
  if (y <= 1.0)
  {
    gRe = -8./9.*log(mb/mu)-8./9.*log(zh)+8./27.+4./9.*y
          -tmp2*log(fabs((tmp1+1.)/(tmp1-1.)));
    gIm = tmp2*EvtConst::pi;
  }
  else
  {
    gRe = -8./9.*log(mb/mu)-8./9.*log(zh)+8./27.+4./9.*y
          -tmp2*2.*atan(1./tmp1);
    gIm = 0.;
  }
  EvtComplex gmcsh(gRe,gIm);

  y = 4./sh;
  tmp1 = sqrt(fabs(1.-y));
  tmp2 = 2./9.*(2.+y)*tmp1;
  if (y < 1.0)
  {
    gRe = -8./9.*log(mb/mu)+8./27.+4./9.*y
          -tmp2*log(fabs((tmp1+1.)/(tmp1-1.)));
    gIm = tmp2*EvtConst::pi;
  }
  else
  {
    gRe = -8./9.*log(mb/mu)+8./27.+4./9.*y
          -tmp2*2.*atan(1./tmp1);
    gIm = 0.;
  }
  EvtComplex g1sh(gRe,gIm);

  EvtComplex g0sh(8./27.-8./9.*log(mb/mu)-4./9.*log(sh), 4./9.*EvtConst::pi);

  EvtComplex z1(c9ndr*etash, 0.);
  EvtComplex z2(3.*c1 + c2 + 3.*c3 +    c4 + 3.*c5 + c6, 0.);
  EvtComplex z3(             4.*c3 + 4.*c4 + 3.*c5 + c6, 0.);
  EvtComplex z4(                c3 + 3.*c4,              0.);
  EvtComplex z5(             3.*c3 +    c4 + 3.*c5 + c6, 0.);

  EvtComplex coeff;
  coeff = z1 + gmcsh*z2 - 0.5*g1sh*z3 - 0.5*g0sh*z4 + (2./9.)*z5;

  return coeff;
}

double EvtBtoXsllUtil::dGdsProb(double mb, double ms, double ml,
                                double s)
{
  // Compute the decay probability density function given a value of s
  // according to Ali's paper

  double c7eff = -0.311;
  double c10   = -4.546;

  double delta, lambda, prob;
  double f1, f2, f3, f4;
  double msh, mlh, sh;

  mlh = ml / mb;
  msh = ms / mb;
  sh  = s  / (mb*mb);

  lambda = 1.0 + sh*sh + pow(msh,4) - 2.0*(sh + sh*msh*msh + msh*msh);

  f1 = pow(1.0-msh*msh,2) - sh*(1.0 + msh*msh);
  f2 = 2.0*(1.0 + msh*msh) * pow(1.0-msh*msh,2)
       - sh*(1.0 + 14.0*msh*msh + pow(msh,4)) - sh*sh*(1.0 + msh*msh);
  f3 = pow(1.0-msh*msh,2) + sh*(1.0 + msh*msh) - 2.0*sh*sh
       + lambda*2.0*mlh*mlh/sh;
  f4 = 1.0 - sh + msh*msh;

  EvtComplex c9eff = coeff9(mb, sh);

  delta = ( 12.0*c7eff*real(c9eff)*f1 
            + 4.0*c7eff*c7eff*f2/sh ) * (1.0 + 2.0*mlh*mlh/sh)
            + (pow(abs(c9eff),2) + c10*c10)*f3 
            + 6.0*mlh*mlh*(pow(abs(c9eff),2) - c10*c10)*f4;

  prob =  sqrt(lambda*(1.0 - 4.0*mlh*mlh/sh)) * delta;

  return prob;
}

double EvtBtoXsllUtil::dGdsdupProb(double mb, double ms, double ml,
                                   double s,  double u)
{
  // Compute the decay probability density function given a value of s and u
  // according to Ali's paper

  double c7eff = -0.311;
  double c10   = -4.546;

  double prob/*, prob_max*/;
  double f1sp, f2sp, f3sp;
  //  double u_ext;

  double sh = s / (mb*mb);
  EvtComplex c9eff = coeff9(mb, sh);

  f1sp = ( pow(mb*mb-ms*ms,2) - s*s) * ( pow(abs(c9eff),2) + c10*c10 )
         + 4.0*( pow(mb,4) - ms*ms*mb*mb - pow(ms,4)*(1.0 - ms*ms/(mb*mb))
         - 8.0*s*ms*ms - s*s*(1.0 + ms*ms/(mb*mb) ))*mb*mb*c7eff*c7eff/s
    // kludged mass term
         *(1.0 + 2.0*ml*ml/s)
         - 8.0*(s*(mb*mb + ms*ms) - pow(mb*mb-ms*ms,2)) * c7eff * real(c9eff)
    // kludged mass term
         *(1.0 + 2.0*ml*ml/s);

  f2sp = 4.0*s*real(c9eff)*c10 + 8.0*(mb*mb + ms*ms)*c10*c7eff;
  f3sp = - (pow(abs(c9eff),2) + c10*c10)
         + 4.0*(1.0 + pow(ms/mb,4)) * mb*mb*c7eff*c7eff/s
    // kludged mass term
         *(1.0 + 2.0*ml*ml/s);

  prob = (f1sp + f2sp*u + f3sp*u*u)/ pow(mb,3);

  return prob;
}

double EvtBtoXsllUtil::FermiMomentum(double pf)
{
  // Pick a value for the b-quark Fermi motion momentum
  // according to Ali's Gaussian model

  double pb, pbmax, xbox, ybox;
  pb    = 0.0;
  pbmax = 5.0 * pf;

  while (pb == 0.0)
  {
    xbox = EvtRandom::Flat(pbmax);
    ybox = EvtRandom::Flat();
    if (ybox < FermiMomentumProb(xbox, pf)) { pb = xbox;}
  }

  return pb;
}

double EvtBtoXsllUtil::FermiMomentumProb(double pb, double pf)
{
  // Compute probability according to Ali's Gaussian model
  // the function chosen has a convenient maximum value of 1 for pb = pf

  double prsq = (pb*pb)/(pf*pf);
  double prob = prsq * exp(1.0 - prsq);

  return prob;
}
