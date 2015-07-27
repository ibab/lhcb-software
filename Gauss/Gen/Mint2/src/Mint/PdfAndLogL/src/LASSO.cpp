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



//
