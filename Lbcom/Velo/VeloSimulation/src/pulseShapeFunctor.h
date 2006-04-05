// $Id: pulseShapeFunctor.h,v 1.2 2006-04-05 10:13:47 szumlat Exp $
#ifndef VELOSIMULATION_PULSESHAPEFUNCTOR_H 
#define VELOSIMULATION_PULSESHAPEFUNCTOR_H 1

// Include files
#include <vector>

/** @class pulseShapeFunctor pulseShapeFunctor.h VeloSimulation/pulseShapeFunctor.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-01-25
 *  @ functor object to calculate values of the pulse shape
 */
class pulseShapeFunctor {
public: 
  typedef std::vector<double> parVec;
  //
  double operator()(double TDCTime, const parVec& fitPar)
  {
    double value=0.;
    int size=fitPar.size();
    //
    if(!size){
      return (value);
    }else{
      for(int i=size-1; i>=0; i--){
        value=value*TDCTime+fitPar[i];
      }
    }
    return (value);
  }  
    
protected:

private:

};
#endif // VELOSIMULATION_PULSESHAPEFUNCTOR_H
