//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtbTosllVectorAmpNew.hh
//
// Description:
//
// Modification history:
//
//  A.Popov	October 21, 2008	Module created
//
//------------------------------------------------------------------------

#ifndef EVTBSTOLLGAMMA_AMP_HH
#define EVTBSTOLLGAMMA_AMP_HH

class EvtId;
class EvtAmp;
class EvtParticle;
class Evtbs2llGammaFF;
class EvtbTosllWilsCoeffNLO;

class Evtbs2llGammaAmp {

  public:

    void CalcAmp( EvtParticle *parent, EvtAmp& amp,
		  Evtbs2llGammaFF *formFactors,
                  EvtbTosllWilsCoeffNLO *WilsCoeff, 
                  double mu, int Nf,   
                  int res_swch, int ias,
                  double Egamma_min, 
                  double CKM_A, double CKM_lambda, 
                  double CKM_barrho, double CKM_bareta);

    double CalcMaxProb(EvtId parnum, EvtId photnum, EvtId l1num, EvtId l2num, 
		       Evtbs2llGammaFF *formFactors,
                       EvtbTosllWilsCoeffNLO *WilsCoeff, 
                       double mu, int Nf, int res_swch, int ias, 
                       double Egamma_min,
                       double CKM_A, double CKM_lambda, 
                       double CKM_barrho, double CKM_bareta);

    double lambda(double a, double b, double c); 

};

#endif

