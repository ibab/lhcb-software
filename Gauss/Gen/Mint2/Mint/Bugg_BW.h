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
    , _M("Bugg_M", 1, 0.935  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _b1("Bugg_b1", 1, 1.302  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _b2("Bugg_b2", 1, 0.340  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _A("Bugg_A", 1, 2.426  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _g_4pi("Bugg_g_4pi", 1, 0.011  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _g_KK("Bugg_g_KK", 1, 0.6  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _g_etaeta("Bugg_g_etaeta", 1, 0.2  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _alpha("Bugg_alpha", 1, 1.3  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _sA("Bugg_sA", 1, 0.41 , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _4pi_s0("Bugg_4pi_s0", 1, 7.082/2.845  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _4pi_lambda("Bugg_4pi_lambda", 1, 2.845  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    , _m_pi(ParticlePropertiesList::mass(211)/GeV)
    , _m_K(ParticlePropertiesList::mass(321)/GeV)
    , _m_eta(ParticlePropertiesList::mass(221)/GeV)
  {
      _fit_M = new FitParRef(_M, this);
      _fit_b1 = new FitParRef(_b1, this);
      _fit_b2 = new FitParRef(_b2, this);
      _fit_A = new FitParRef(_A, this);
      _fit_g_4pi = new FitParRef(_g_4pi, this);
      _fit_g_KK = new FitParRef(_g_KK, this);
      _fit_g_etaeta = new FitParRef(_g_etaeta, this);
      _fit_alpha = new FitParRef(_alpha, this);
      _fit_sA = new FitParRef(_sA, this);
      _fit_4pi_s0 = new FitParRef(_4pi_s0, this);
      _fit_4pi_lambda = new FitParRef(_4pi_lambda, this);

      std::cout << "Bugg_M = " << M() << std::endl;
      std::cout << "GeV = " << GeV << std::endl;
      std::cout << "mumsMass = " << mumsMass()/GeV << std::endl;


  }

  virtual std::string name() const{
    return "Bugg_BW("+_theDecay.oneLiner() +")";
  }

  virtual ~Bugg_BW(){}

 protected:
    double _m_pi;
    double _m_K;
    double _m_eta;

    MINT::FitParameter _M;
    MINT::FitParameter _b1;
    MINT::FitParameter _b2;
    MINT::FitParameter _A;
    MINT::FitParameter _g_4pi;
    MINT::FitParameter _g_KK;
    MINT::FitParameter _g_etaeta;
    MINT::FitParameter _alpha;
    MINT::FitParameter _sA;
    MINT::FitParameter _4pi_lambda;
    MINT::FitParameter _4pi_s0;

    MINT::FitParRef* _fit_M;
    MINT::FitParRef* _fit_b1;
    MINT::FitParRef* _fit_b2;
    MINT::FitParRef* _fit_A;
    MINT::FitParRef* _fit_g_4pi;
    MINT::FitParRef* _fit_g_KK;
    MINT::FitParRef* _fit_g_etaeta;
    MINT::FitParRef* _fit_alpha;
    MINT::FitParRef* _fit_sA;
    MINT::FitParRef* _fit_4pi_lambda;
    MINT::FitParRef* _fit_4pi_s0;

    double M() const{return *_fit_M ;}
    double b1() const{return *_fit_b1 ;}
    double b2() const{return *_fit_b2 ;}
    double A() const{return *_fit_A ;}
    double g_4pi() const{return *_fit_g_4pi ;}
    double g_KK() const{return *_fit_g_KK ;}
    double g_etaeta() const{return *_fit_g_etaeta ;}
    double alpha() const{return *_fit_alpha ;}
    double sA() const{return *_fit_sA ;}
    double lambda() const{return *_fit_4pi_lambda ;}
    double s0() const{return *_fit_4pi_s0 ;}
    
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
