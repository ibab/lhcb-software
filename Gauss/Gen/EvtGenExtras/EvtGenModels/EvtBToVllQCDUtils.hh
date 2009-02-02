#ifndef QCDUTILS_HH_
#define QCDUTILS_HH_

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenModels/EvtBToVllWC.hh"

/**
 * Usefull and hopefully general QCD related functions.
 */

namespace qcd {

double alpha_s(const double& mu1, const int& nflav);
double as1(const double mu, const int nflav);
double B0(const int& nflav);
double B1(const int& nflav);
double B2(const int& nflav);
double mb_pole(const double mb);
EvtComplex h(const double& s, const double& mq, const double& mu);
EvtComplex Y(const double& s, const WilsonCoefficients<EvtComplex>& C);
double xt(const int& nfl);
}

#endif /*QCDUTILS_HH_*/
