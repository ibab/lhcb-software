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
//  N.Nikitin   Avril 21, 2008  Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLVECTORAMPNEW_HH
#define EVTBTOSLLVECTORAMPNEW_HH

#include "EvtGenModels/EvtbTosllAmpNew.hh"

class EvtId;
class EvtAmp;
class EvtParticle;
class EvtbTosllFFNew;
class EvtbTosllWilsCoeffNLO;

class EvtbTosllVectorAmpNew : public EvtbTosllAmpNew{

     public:

     EvtbTosllVectorAmpNew(){}

    //Daughters are initialized and have been added to the parent.
    //No need to carry around the daughters seperately!
    void CalcAmp( EvtParticle *parent, EvtAmp& amp,
 	  	  EvtbTosllFFNew *formFactors,
                  EvtbTosllWilsCoeffNLO *WilsCoeff,  
                  double mu, int Nf,   
                  int res_swch, int ias, 
                  double CKM_A, double CKM_lambda, 
                  double CKM_barrho, double CKM_bareta);

    double CalcMaxProb(EvtId parnum, EvtId mesnum, EvtId l1num,  EvtId l2num,
		       EvtbTosllFFNew *formFactors, EvtbTosllWilsCoeffNLO *WilsCoeff, 
                       double mu, int Nf, int res_swch, int ias, 
                       double CKM_A, double CKM_lambda, 
                       double CKM_barrho, double CKM_bareta);

   double lambda(double a, double b, double c);

};

#endif

