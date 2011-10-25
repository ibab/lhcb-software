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
// Module: EvtGen/EvtbTosllVectorAmpNewExt.hh
//
// Description:
//
// Modification history:
//
//  N.Nikitin   July 07, 2011  Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLVECTORAMPNEWEXT_HH
#define EVTBTOSLLVECTORAMPNEWEXT_HH

#include "EvtGenModels/EvtbTosllAmpNewExt.hh"

class EvtId;
class EvtAmp;
class EvtParticle;
class EvtbTosllFFNew;
class EvtbTosllWilsCoeffNLO;

class EvtbTosllVectorAmpNewExt : public EvtbTosllAmpNewExt{

     public:

     EvtbTosllVectorAmpNewExt(){}

    //Daughters are initialized and have been added to the parent.
    //No need to carry around the daughters seperately!
    void CalcAmp( EvtParticle *parent, EvtAmp& amp,
 	  	  EvtbTosllFFNew *formFactors,
                  EvtbTosllWilsCoeffNLO *WilsCoeff,  
                  double mu, int Nf,   
                  int res_swch, int ias, 
                  double CKM_A, double CKM_lambda, 
                  double CKM_barrho, double CKM_bareta, 
                  double ReA7, double ImA7, double ReA10, double ImA10);

    double CalcMaxProb(EvtId parnum, EvtId mesnum, EvtId l1num,  EvtId l2num,
		       EvtbTosllFFNew *formFactors, EvtbTosllWilsCoeffNLO *WilsCoeff, 
                       double mu, int Nf, int res_swch, int ias, 
                       double CKM_A, double CKM_lambda, 
                       double CKM_barrho, double CKM_bareta, 
                       double ReA7, double ImA7, double ReA10, double ImA10);

   double lambda(double a, double b, double c);

};

#endif

