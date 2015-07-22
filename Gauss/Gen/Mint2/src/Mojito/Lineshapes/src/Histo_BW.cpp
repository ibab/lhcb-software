#include "Mint/Histo_BW.h"
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


double Histo_BW::GofM(){

    double ps_m  = _phaseSpaceHist->Interpolate(mumsRecoMass2()/(GeV*GeV));
    double ps_m0 = _phaseSpaceHist->Interpolate(mumsMass()*mumsMass()/(GeV*GeV));
    
    double ps_ratio = 0.;
    if(ps_m0>0)ps_ratio= ps_m/ps_m0 ;
    
    return mumsWidth() * ps_ratio;
}

//
