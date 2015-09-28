#include "Mint/RhoOmegaGS.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "Mint/Utils.h"
#include "Mint/DalitzEventList.h"
#include "Mint/phaseSpaceIntegrals.h"
#include <cmath>
#include <complex>
#include "Mint/GounarisSakurai.h"
#include "Mint/BW_BW.h"

using namespace std;
using namespace MINT;


const ResonanceProperties* RhoOmegaGS::omegaProperties() const{
    const ResonanceProperties* rp = resonancePropertiesList()->get(omega_pid);
    if(0 == rp){
        ResonanceProperties* rpnew= new ResonanceProperties(omega_pid);
        resonancePropertiesList()->AddToList(rpnew);
        rp=rpnew;
    }
    if(0 == rp){
        cout << "ERROR in RhoOmegaGS::ResonanceProperties()"
        << " can't find properties for first element"
        << " in this decay tree\n" << _theDecay << endl;
        throw "invalid decay tree in RhoOmegaGS::mumsFittableProperties()";
    }
    return rp;
}

ResonancePropertiesFitRef& RhoOmegaGS::omegaFittableProperties() const{
    if(0 == _fittableOmegaPropertiesPtr){
        cout << "something went wrong in RhoOmegaGS::mumsFittableProperties() "
        << " _fittableOmegaPropertiesPtr is 0 although it should be"
        << " set at construction."
        << " Looking at  this decay tree\n" << _theDecay << endl;
        
        throw "error in RhoOmegaGS::mumsFittableProperties()";
    }
    return *_fittableOmegaPropertiesPtr;
}

bool RhoOmegaGS::setOmegaFitParameters(){
    bool s=true;
    if(0 == resonancePropertiesList()){
        cout << "big problem in RhoOmegaGS::setAllFitParameters"
        << ", resonancePropertiesList is zero" << endl;
        throw "RhoOmegaGS::setAllFitParameters can't find resonancePropertiesList()";
    }
    _fittableOmegaPropertiesPtr = new ResonancePropertiesFitRef(*omegaProperties(), this);
    s &= (0 != _fittableOmegaPropertiesPtr);
    return s;
}


std::complex<double> RhoOmegaGS::omegaBW(IDalitzEvent& evt){  
    setEventPtr(evt);
    resetInternals();
    
    double mass_omega = omegaMass();
    double width_omega = omegaWidth();
        
    std::complex<double> invBW(mass_omega*mass_omega - mumsRecoMass2(), - mass_omega * width_omega);
    return 1.*GeV*GeV/invBW;
}

std::complex<double> RhoOmegaGS::getVal(IDalitzEvent& evt){    
    
    double mass_omega = omegaMass();
    complex<double> delta(delta_Re(),delta_Im());
    
    return GounarisSakurai::getVal(evt)*(1. + mumsRecoMass2()/(mass_omega*mass_omega) *  (delta * omegaBW(evt)));
}


//
