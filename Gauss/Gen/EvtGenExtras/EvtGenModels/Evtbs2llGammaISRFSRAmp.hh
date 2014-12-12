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
// Module: EvtGen/Evtbs2llGammaISRFSRAmp.hh
//
// Description: See the Internal LHCb Note LHCb-INT-2011-011.
//
// Modification history:
//
//  N.Nikitin    December   09, 2014     Module created
//
//------------------------------------------------------------------------

#ifndef EVTBSTOLLGAMMAISRFSR_AMP_HH
#define EVTBSTOLLGAMMAISRFSR_AMP_HH

class EvtId;
class EvtAmp;
class EvtParticle;
class Evtbs2llGammaFF;
class EvtbTosllWilsCoeffNLO;

class Evtbs2llGammaISRFSRAmp {

  public:

    void CalcAmp( EvtParticle *parent, EvtAmp& amp,
		          Evtbs2llGammaFF *formFactors,
                  EvtbTosllWilsCoeffNLO *WilsCoeff, 
                  double mu, int Nf, int sr, int res_swch, int ias,
                  double Egamma_min, 
                  double CKM_A, double CKM_lambda, 
                  double CKM_barrho, double CKM_bareta);

    double CalcMaxProb(EvtId parnum, EvtId photnum, EvtId l1num, EvtId l2num, 
		               Evtbs2llGammaFF *formFactors,
                       EvtbTosllWilsCoeffNLO *WilsCoeff, 
                       double mu, int Nf, int sr, int res_swch, int ias, 
                       double Egamma_min,
                       double CKM_A, double CKM_lambda, 
                       double CKM_barrho, double CKM_bareta);

    double lambda(double a, double b, double c); 

};

#endif

