// author: Philippe d'Argent (p.dargent@cern.ch) & Jonas Rademacker
#include "Mint/ResonancePropertiesFitRef.h"
#include "Mint/ResonanceProperties.h"

using namespace std;
using namespace MINT;

ResonancePropertiesFitRef::ResonancePropertiesFitRef(const ResonanceProperties& repo
						     , IFitParRegister* daddy)
  : FitParDependent(daddy)
  , _fitMass(repo._fitMass, this)
  , _fitWidth(repo._fitWidth, this)
  , _fitRadius(repo._fitRadius, this)
  , _fitAlpha(repo._fitAlpha, this)
  , _fitFermiLambda(repo._fitFermiLambda, this)
  , _fitFermiS0(repo._fitFermiS0, this)
  , _fitRhoOmegaDelta_Re(repo._fitRhoOmegaDelta_Re, this)
  , _fitRhoOmegaDelta_Im(repo._fitRhoOmegaDelta_Im, this)
  , _fitFlatte_gPi(repo._fitFlatte_gPi,this)
  , _fitFlatte_gK_by_gPi(repo._fitFlatte_gK_by_gPi,this)
  , _fitBugg_M(repo._fitBugg_M, this)
  , _fitBugg_b1(repo._fitBugg_b1, this)
  , _fitBugg_b2(repo._fitBugg_b2, this)
  , _fitBugg_A(repo._fitBugg_A, this)
  , _fitBugg_g_4pi(repo._fitBugg_g_4pi, this)
  , _fitBugg_g_KK(repo._fitBugg_g_KK, this)
  , _fitBugg_g_etaeta(repo._fitBugg_g_etaeta, this)
  , _fitBugg_alpha(repo._fitBugg_alpha, this)
  , _fitBugg_sA(repo._fitBugg_sA, this)
  , _fitBugg_4pi_s0(repo._fitBugg_4pi_s0, this)
  , _fitBugg_4pi_lambda(repo._fitBugg_4pi_lambda, this)
{
}

//



