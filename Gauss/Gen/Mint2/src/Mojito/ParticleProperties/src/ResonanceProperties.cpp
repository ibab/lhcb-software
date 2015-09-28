// author: Philippe d'Argent (p.dargent@cern.ch)
#include "Mint/ResonanceProperties.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/NamedParameter.h"
#include "Mint/Utils.h"
#include "Mint/FitParameter.h"
#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace MINT;

std::string ResonanceProperties::nameFromPid(int pdg_id){
  const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  const ParticleProperties* pp = PPL->get(pdg_id);
  if(0 == pp){
    cout << "ResonanceProperties::nameFromPid error!" << endl;
    cout << "Can't find ParticleProperties for pdg_id = " << pdg_id << endl;
    return "fuck!";
  }
  cout << "got name from PID: " << endl;
  cout << PPL->get(pdg_id)->name() << endl;
  return PPL->get(pdg_id)->name();
}

int ResonanceProperties::pidFromName(const std::string& name){
  const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  const ParticleProperties* pp = PPL->get(name);
  if(0 == pp){
    cout << "ResonanceProperties::nameFromPid error!" << endl;
    cout << "Can't find ParticleProperties for name = " << name << endl;
    throw "ResonanceProperties::pidFromName: cannot find name";
  }
  return pp->pdg_id();
}

std::string ResonanceProperties::nameFromName(const std::string& name){
  return nameFromPid(abs(pidFromName(name)));
}

double ResonanceProperties::massInit(int pdg_id){
  return ParticlePropertiesList::getMe()->get(pdg_id)->mass();
}
double ResonanceProperties::widthInit(int pdg_id){
  return ParticlePropertiesList::getMe()->get(pdg_id)->width();
}
double ResonanceProperties::radiusInit(int pdg_id){
  return ParticlePropertiesList::getMe()->get(pdg_id)->radius();
}


ResonanceProperties::ResonanceProperties(int pdg_id, MinuitParameterSet* mps)
  : _pid(abs(pdg_id))
  , _fitMass("mass_"     + nameFromPid(abs(pdg_id)), 1, massInit(pdg_id)  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitWidth("width_"   + nameFromPid(abs(pdg_id)), 1, widthInit(pdg_id) , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitRadius("radius_" + nameFromPid(abs(pdg_id)), 1, radiusInit(pdg_id), 0, 0, 0, mps,  NamedParameterBase::QUIET)

  , _fitAlpha("NonRes::alpha_"   + nameFromPid(abs(pdg_id)), 1, 1.0               , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFermiLambda("FermiPS::lambda_"   + nameFromPid(abs(pdg_id)), 1, 1.0  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFermiS0("FermiPS::s0_"   + nameFromPid(abs(pdg_id)), 1, 1.0          , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitRhoOmegaDelta_Re("RhoOmegaGS::Delta_Re_"   + nameFromPid(abs(pdg_id)), 1, 0.00153  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitRhoOmegaDelta_Im("RhoOmegaGS::Delta_Im_"   + nameFromPid(abs(pdg_id)), 1, 0.00034  , 0, 0, 0, mps,  NamedParameterBase::QUIET)

  , _fitFlatte_gPi("Flatte::gPi_"   + nameFromPid(abs(pdg_id)), 1, 0.165*GeV , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFlatte_gK_by_gPi("Flatte::gK_by_gPi_"   + nameFromPid(abs(pdg_id)), 1, 4.21 , 0, 0, 0, mps,  NamedParameterBase::QUIET)

  , _fitBugg_M("Bugg::M_"+ nameFromPid(abs(pdg_id)), 1, 0.935  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_b1("Bugg::b1_"+ nameFromPid(abs(pdg_id)), 1, 1.302  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_b2("Bugg::b2_"+ nameFromPid(abs(pdg_id)), 1, 0.340  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_A("Bugg::A_"+ nameFromPid(abs(pdg_id)), 1, 2.426  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_g_4pi("Bugg::g_4pi_"+ nameFromPid(abs(pdg_id)), 1, 0.011  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_g_KK("Bugg::g_KK_"+ nameFromPid(abs(pdg_id)), 1, 0.6  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_g_etaeta("Bugg::g_etaeta_"+ nameFromPid(abs(pdg_id)), 1, 0.2  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_alpha("Bugg::alpha_"+ nameFromPid(abs(pdg_id)), 1, 1.3  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_sA("Bugg::sA_"+ nameFromPid(abs(pdg_id)), 1, 0.41 , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_4pi_s0("Bugg::4pi_s0_"+ nameFromPid(abs(pdg_id)), 1, 7.082/2.845  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_4pi_lambda("Bugg::4pi_lambda_"+ nameFromPid(abs(pdg_id)), 1, 2.845  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
{ 

        //fix to PDG value if not explicitly initiallised 
  //        const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  //      if(_fitMass.iFixInit() && _fitMass== 0)      _fitMass   = PPL->mass(pdg_id);
  //      if(_fitWidth.iFixInit() && _fitWidth== 0)    _fitWidth  = PPL->width(pdg_id);    
  //      if(_fitRadius.iFixInit() && _fitRadius== 0)  _fitRadius = PPL->get(pdg_id)->radius();   
        //std:: cout << _fitMass << endl;
        //std:: cout << _fitWidth << endl;
}

ResonanceProperties::ResonanceProperties(const std::string& name, MinuitParameterSet* mps): 
  _pid(abs(pidFromName(name)))
  , _fitMass("mass_"     + nameFromPid(_pid), 1, massInit(_pid)  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitWidth("width_"   + nameFromPid(_pid), 1, widthInit(_pid) , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitRadius("radius_" + nameFromPid(_pid), 1, radiusInit(_pid), 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitAlpha("NonRes::alpha_"   + nameFromPid(_pid), 1, 1.0             , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFermiLambda("FermiPS::lambda_"   + nameFromPid(_pid), 1, 1.0  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFermiS0("FermiPS::s0_"   + nameFromPid(_pid), 1, 1.0          , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitRhoOmegaDelta_Re("RhoOmegaGS::Delta_Re_"   + nameFromPid(_pid), 1, 0.00153  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitRhoOmegaDelta_Im("RhoOmegaGS::Delta_Im_"   + nameFromPid(_pid), 1, 0.00034  , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFlatte_gPi("Flatte::gPi_"   + nameFromPid(_pid), 1, 0.165*GeV , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitFlatte_gK_by_gPi("Flatte::gK_by_gPi_"   + nameFromPid(_pid), 1, 4.21 , 0, 0, 0, mps,  NamedParameterBase::QUIET)
  , _fitBugg_M("Bugg::M_"+ nameFromPid(_pid), 1, 0.935  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_b1("Bugg::b1_"+ nameFromPid(_pid), 1, 1.302  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_b2("Bugg::b2_"+ nameFromPid(_pid), 1, 0.340  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_A("Bugg::A_"+ nameFromPid(_pid), 1, 2.426  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_g_4pi("Bugg::g_4pi_"+ nameFromPid(_pid), 1, 0.011  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_g_KK("Bugg::g_KK_"+ nameFromPid(_pid), 1, 0.6  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_g_etaeta("Bugg::g_etaeta_"+ nameFromPid(_pid), 1, 0.2  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_alpha("Bugg::alpha_"+ nameFromPid(_pid), 1, 1.3  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_sA("Bugg::sA_"+ nameFromPid(_pid), 1, 0.41 , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_4pi_s0("Bugg::4pi_s0_"+ nameFromPid(_pid), 1, 7.082/2.845  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
  , _fitBugg_4pi_lambda("Bugg::4pi_lambda_"+ nameFromPid(_pid), 1, 2.845  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
{ 
}



//
