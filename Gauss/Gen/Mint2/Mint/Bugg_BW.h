#ifndef Bugg_BW_LINESHAPE_HH
#define Bugg_BW_LINESHAPE_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// This implements the line shape for sigma suggested in D. V. Bugg, J. Phys. G34:151, 2007, arXiv:hep-ph/0608081

#include <complex>
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/NamedParameter.h"
#include "Mint/NamedParameterBase.h"
#include "Mint/ResonancePropertiesList.h"
#include "Mint/FitParDependent.h"
#include "ResonancePropertiesFitRef.h"

using namespace MINT;

class Bugg_BW  : public BW_BW, virtual public ILineshape{
 public:
  
  Bugg_BW( const AssociatedDecayTree& tree): 
    BW_BW(tree)
    , _m_pi(ParticlePropertiesList::mass(211)/GeV)
    , _m_K(ParticlePropertiesList::mass(321)/GeV)
    , _m_eta(ParticlePropertiesList::mass(221)/GeV)
  {
  }

  virtual std::string name() const{
    return "Bugg_BW("+_theDecay.oneLiner() +")";
  }

  virtual ~Bugg_BW(){}

 protected:
    const double _m_pi;
    const double _m_K;
    const double _m_eta;

    double M() const{return mumsFittableProperties().fitBugg_M() ;}
    double b1() const{return mumsFittableProperties().fitBugg_b1() ;}
    double b2() const{return mumsFittableProperties().fitBugg_b2() ;}
    double A() const{return mumsFittableProperties().fitBugg_A() ;}
    double g_4pi() const{return mumsFittableProperties().fitBugg_g_4pi() ;}
    double g_KK() const{return mumsFittableProperties().fitBugg_g_KK() ;}
    double g_etaeta() const{return mumsFittableProperties().fitBugg_g_etaeta() ;}
    double alpha() const{return mumsFittableProperties().fitBugg_alpha() ;}
    double sA() const{return mumsFittableProperties().fitBugg_sA() ;}
    double lambda() const{return mumsFittableProperties().fitBugg_4pi_lambda() ;}
    double s0() const{return mumsFittableProperties().fitBugg_4pi_s0() ;}
    
    virtual std::complex<double> BreitWigner();
    double Gamma_2pi(double s);
    std::complex<double> Gamma_2K(double s);
    std::complex<double> Gamma_2eta(double s);
    double Gamma_4pi(double s);
    std::complex<double> Gamma_tot(double s){
        return Gamma_2pi(s)+Gamma_2eta(s)+Gamma_2K(s)+Gamma_4pi(s);
    }
    std::complex<double> rho_2(double s, double m);
    double rho_4(double s);
    double g_1_square(double s);
    double j1(double s);
    double z(double s);
    
    

};

#endif
//
