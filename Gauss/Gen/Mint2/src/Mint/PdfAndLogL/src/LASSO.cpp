// author: Philippe d'Argent (p.dargent@cern.ch)
#include "Mint/Neg2LLSum.h"
#include "Mint/IMinimisable.h"
#include "Mint/LASSO.h"
#include "Mint/NamedParameter.h"


using namespace std;
using namespace MINT;


double LASSO::getVal(){
    return _lambda * _pdf->sumOfSqrtFitFractions();
}

int LASSO::numberOfFitFractionsLargerThanThreshold(double threshold){
    return _pdf->numberOfFitFractionsLargerThanThreshold(threshold);
}

void LASSO::Gradient(Double_t* grad){
    Double_t grad_pdf[this->getParSet()->size()];  
    
    for(unsigned int j=0; j < this->getParSet()->size(); j++){
        grad_pdf[j]= 0.;   
    }
    
    _pdf->GradientForLasso(grad_pdf);
    
    for(unsigned int j=0; j < this->getParSet()->size(); j++){
        grad[j]= _lambda * grad_pdf[j];   
    }
    
}


//
