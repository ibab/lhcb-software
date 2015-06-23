#include "Mint/VBW.h"
//#include "Mint/CLHEPPhysicalConstants.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "Mint/Utils.h"
#include "Mint/DalitzEventList.h"
#include <cmath>
#include <complex>

using namespace std;
using namespace MINT;

VBW::VBW( const AssociatedDecayTree& decay)
: BW_BW(decay), _scaleQ("scaleQ",0) {         
    cout << "scale q ?" << _scaleQ << endl;
}


double VBW::effectiveMass(int i, double mumsMass) const{
   
    double m = daughterRecoMass(i);
    double mB=0.;
    if(i==0)mB=daughterRecoMass(1);
    else mB=daughterRecoMass(0);
    const_counted_ptr<AssociatedDecayTree> dgtr = _theDecay.getDgtrTreePtr(i);

    if(dgtr->isFinalState())return m;
    
    std::vector<int> asi = dgtr->getVal().asi();
    double min = sqrt(getEvent()->eventPattern().sijMin(asi));
    double max = mumsMass-mB;//sqrt(getEvent()->eventPattern().sijMax(asi));
    return min + (max-min)*(1+TMath::TanH((m-(min+max)/2.)/(max-min)))/2.;      
}

double VBW::twoBody_recodgtPsq_in_MumsPDGFrame() const{

    double mumsMass = this->mumsMass();
    double mA= effectiveMass(0,mumsMass);
    double mB= effectiveMass(1,mumsMass);
    
    return twoBody_dgtPsq_in_MumsFrame(mumsMass, mA, mB);
}

double VBW::twoBody_dgtPsq_in_MumsRecoFrame() {
    
    double mumsMass = mumsRecoMass();
    double mA= daughterRecoMass(0);
    double mB= daughterRecoMass(1);
    
    if(_scaleQ){
        mA= effectiveMass(0,mumsMass);
        mB= effectiveMass(1,mumsMass);
    }
    return twoBody_dgtPsq_in_MumsFrame(mumsMass, mA, mB);
}

double VBW::twoBody_dgtPsq_in_MumsPDGFrame() const{
    double mumsMass = mumsRecoMass();
    double mA= daughterRecoMass(0);
    double mB= daughterRecoMass(1);
    
    if(_scaleQ){
        mA= effectiveMass(0,mumsMass);
        mB= effectiveMass(1,mumsMass);
    }
    return twoBody_dgtPsq_in_MumsFrame(mumsMass, mA, mB);
}

//
