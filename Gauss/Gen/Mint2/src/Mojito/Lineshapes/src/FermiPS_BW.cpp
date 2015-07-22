#include "Mint/FermiPS_BW.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "Mint/Utils.h"
#include "Mint/DalitzEventList.h"
#include "Mint/phaseSpaceIntegrals.h"
#include <cmath>
#include <complex>

using namespace std;
using namespace MINT;

double FermiPS_BW::phaseSpace(double s){
    return sqrt(1.- _threshold*_threshold/s)/(1.+ exp(lambda()/(1000.*1000.) * (s0()*(1000.*1000.)-s) ) );
}

double FermiPS_BW::GofM(){

    double ps_m = phaseSpace(mumsRecoMass2());
    double ps_m0 = phaseSpace(mumsMass()*mumsMass());
    
    double ps_ratio = 0.;
    if(ps_m0>0)ps_ratio= ps_m/ps_m0 ;
    
    return mumsWidth() * ps_ratio;
}

//
