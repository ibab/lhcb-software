// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT

#include "rhoOmega.h"

#include <iostream>
using namespace std;

std::complex<Double_t> rhoOmega::omegaFactor() const{
  const DcyAmplitude* rhoAmpPtr 
    = dynamic_cast<const DcyAmplitude*>(& rhoAmp.arg());

  const DcyAmplitude* omegaAmpPtr 
    = dynamic_cast<const DcyAmplitude*>(& omegaAmp.arg());

  Double_t eps_coh = 1.14;

  Double_t deg = 180.0/TMath::Pi();
  Double_t beta=-4.3/deg;
  std::complex<Double_t> exp_ibeta(cos(beta), sin(beta));

  Double_t phi = 100.7/deg;
  std::complex<Double_t> exp_iphi(cos(phi), sin(phi));

  Double_t delta = 2.5e-3; // units: GeV

  Double_t mrho   =   rhoAmpPtr->nominalMass();
  Double_t momega = omegaAmpPtr->nominalMass();
  
  Double_t Grho   =   rhoAmpPtr->GofM();
  Double_t Gomega = omegaAmpPtr->GofM();

  std::complex<Double_t> numerator
    =  eps_coh 
    *  exp_ibeta
    *  exp_iphi 
    *  delta
    *  (mrho + momega);

  Double_t RDen = momega*momega - mrho*mrho;
  Double_t IDen = - (momega * Gomega - mrho * Grho);
  std::complex<Double_t> denominator(RDen, IDen);

  std::complex<Double_t> returnVal = numerator/denominator;

  return returnVal;
}

Double_t rhoOmega::getVal() const{
  return evaluate();
}
Double_t rhoOmega::evaluate() const{ 
  std::complex<Double_t> M(amp*cos(phase),amp*sin(phase));
  const AbsComplex* rhoAmpPtr 
    = dynamic_cast<const AbsComplex*>(& rhoAmp.arg());

  const AbsComplex* omegaAmpPtr 
    = dynamic_cast<const AbsComplex*>(& omegaAmp.arg());

  Double_t p = 1, q=1; // I think I've included these via the
  //                      the spin factors... not sure.

  _result 
    = p*q * M
    * ( rhoAmpPtr->getCVal() 
	+   omegaFactor() * omegaAmpPtr->getCVal()
	);

  return _result.real()*_result.real() + _result.imag()*_result.imag();
}
std::complex<Double_t> rhoOmega::getCVal() const{
  getVal();
 

  if(false)std::cout << " for " << rhoAmp.arg().GetName() 
		     << " and " << omegaAmp.arg().GetName()
		     << " I, " << this->GetName() 
		     << ", return "  << _result 
		     << " (" << abs(_result) << ")" 
		     << std::endl;
  return _result;
}


ClassImp(rhoOmega)

//
