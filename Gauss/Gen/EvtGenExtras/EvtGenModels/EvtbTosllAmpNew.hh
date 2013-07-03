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
// Module: EvtGen/EvtbTosllAmpNew.hh
//
// Description:
//
// Modification history:
//
//    N.Nikitin           March 27 2008         Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLAMPNEW_HH
#define EVTBTOSLLAMPNEW_HH
class EvtAmp;
class EvtId;
class EvtbTosllFFNew;
class EvtParticle;
class EvtbTosllWilsCoeffNLO;

// all definicions !
class EvtbTosllAmpNew{

 public:
  virtual ~EvtbTosllAmpNew( ) { } ;

  virtual void CalcAmp( EvtParticle *parent, EvtAmp& amp,
                        EvtbTosllFFNew *formFactors, 
                        EvtbTosllWilsCoeffNLO *WilsCoeff,
                        double /* mu */, int /* Nf */,   
                        int /* res_swch */, int /* ias */, 
                        double /* CKM_A */, double /* CKM_lambda */, 
                        double /* CKM_barrho */, double /* CKM_bareta */)=0;
  
  virtual double CalcMaxProb(EvtId /* parnum */, EvtId /* mesnum */,
                             EvtId /* l1num */,  EvtId /* l2num */,
                             EvtbTosllFFNew * /* formFactors */,
                             EvtbTosllWilsCoeffNLO * /*WilsCoeff */,
                             double /* mu */, int /* Nf */,
                             int /* res_swch */, int /* ias */,
                             double /* CKM_A */, double /* CKM_lambda */,
                             double /* CKM_barrho */, double /* CKM_bareta */) {return 0.0;};

  virtual double lambda(double /* a */, double /* b */, double /* c */) {return 0.0;};

};

#endif
