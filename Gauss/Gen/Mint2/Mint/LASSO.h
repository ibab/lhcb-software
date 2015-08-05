#ifndef LASSO_HH
#define LASSO_HH
// author: Philippe d'Argent (p.dargent@cern.ch)

#include "TMath.h"
#include "Mint/Minimisable.h"
#include "Mint/DalitzPdfBaseFastInteg.h"
#include <vector>

namespace MINT{

class LASSO: public Minimisable{
    protected:
    DalitzPdfBaseFastInteg* _pdf;
    double _lambda;
    
    public:
        LASSO(DalitzPdfBaseFastInteg* pdf, double lambda = 1.)
        : _pdf(pdf), _lambda(lambda) {};
                
        virtual void beginFit(){};
        virtual void parametersChanged(){};
        virtual void endFit(){};
        
        virtual double getVal();
    
        virtual double getNewVal(){ 
            parametersChanged();
            return getVal();
        }
    
        virtual void Gradient(Double_t* grad);
        virtual bool useAnalyticGradient() {return _pdf->useAnalyticGradient();}

        int numberOfFitFractionsLargerThanThreshold(double threshold);
    
        virtual ~LASSO(){}
        
};

}// namespace MINT
#endif
//
