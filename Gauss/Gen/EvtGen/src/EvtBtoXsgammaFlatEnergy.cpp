//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtBtoXsgammaKagan.cc
//
// Description:
//       Routine to perform two-body B->Xs,gamma decays with a fixed hadronic 
//       mass. For spectrum measurements.
//       The input parameters are 1: the hadronic mass

// Modification history:
//
//      Jim Libby October 11 2002
//------------------------------------------------------------------------
//

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include "EvtGenModels/EvtBtoXsgamma.hh"
#include "EvtGenModels/EvtBtoXsgammaFlatEnergy.hh"
#include "EvtGenBase/EvtRandom.hh"
#include <fstream>

EvtBtoXsgammaFlatEnergy::~EvtBtoXsgammaFlatEnergy(){
}

void EvtBtoXsgammaFlatEnergy::init(int nArg, double* args){

  if ((nArg) > 3 || (nArg > 1 && nArg <3)){
  
  report(ERROR,"EvtGen") << "EvtBtoXsgamma generator model "
			 << "EvtBtoXsgammaFlatEnergy expected " 
			 << "either 1(default config) or two arguments but found: "<<nArg<<std::endl;
  report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();  
  }
  _mB0=5.2794;  
  double mPi = 0.140;
  double mK = 0.494;
  if(nArg == 1){
    _eMin = 1.7;
    //Invariant mass of Xsd must be greater the m_pi+m_K leads to 
    //Egamma < (m_B**2-(m_pi+m_k)**2)/(2m_B)
    _eMax = (pow(_mB0,2)-pow(mPi+mK,2))/(2.0*_mB0); 
  }else{
    _eMin=args[1];
    _eMax=args[2];
  }
  if (_eMax>(pow(_mB0,2)-pow(mPi+mK,2))/(2.0*_mB0)){
    report(ERROR,"EvtGen") << "Emax greater than Kinematic limit" << std::endl;
    report(ERROR,"EvtGen") << "Reset to the kinematic limit" << std::endl;
    report(ERROR,"EvtGen") << "(m_B**2-(m_pi+m_k)**2)/(2m_B)" << std::endl;
    _eMax = (pow(_mB0,2)-pow(mPi+mK,2))/(2.0*_mB0);
  }
  _eRange=_eMax-_eMin;
}

double EvtBtoXsgammaFlatEnergy::GetMass( int /*Xscode*/ ){
  
  double eGamma = EvtRandom::Flat(_eRange)+_eMin;
  double mH = sqrt(pow(_mB0,2)-2.0*_mB0*eGamma); 
  return mH;
}

