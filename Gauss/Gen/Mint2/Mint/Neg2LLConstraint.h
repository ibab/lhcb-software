#ifndef NEG_TWO_LL_Constraint_HH
#define NEG_TWO_LL_Constraint_HH
// author: Philippe d'Argent (p.dargent@cern.ch)

#include "TMath.h"
#include "Mint/Minimisable.h"
#include "Mint/Neg2LLConstraint.h"

#include <vector>

namespace MINT{

class Neg2LLConstraint: public Minimisable{
    protected:
    IMinuitParameter* _mp;
    double _mean;
    double _sigma;
        
    public:
        Neg2LLConstraint(IMinuitParameter* mp, double mean, double sigma, MinuitParameterSet* mps=0)
        : Minimisable(mps), _mp(mp), _mean(mean), _sigma(sigma) {};
                
        virtual void beginFit(){};
        virtual void parametersChanged(){};
        virtual void endFit(){};
        
        virtual double getVal(){
            if(0 == _sigma) return 0;
            double d=((double) *(_mp) -_mean)/_sigma;
            return d*d;
            //return -2.* log(gauss((double) *(_mp)) ); 
        }
    
        virtual double getNewVal(){ 
            parametersChanged();
            return getVal();
        }
    
        inline double gauss(double x){
            if(0 == _sigma) return 0;
            double d=(x-_mean)/_sigma;
            double n=1./(sqrt(2.0*TMath::Pi())*_sigma);
            return n  * exp(-0.5*d*d);
        }
    
        virtual ~Neg2LLConstraint(){}
        
};

}// namespace MINT
#endif
//
