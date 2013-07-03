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
// Module: EvtGen/EvtbTosllAmpNewExt.hh
//
// Description:
//
// Modification history:
//
//    N.Nikitin           July 07 2011         Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLAMPNEWEXT_HH
#define EVTBTOSLLAMPNEWEXT_HH
class EvtAmp;
class EvtId;
class EvtbTosllFFNew;
class EvtParticle;
class EvtbTosllWilsCoeffNLO;

// all definicions !
class EvtbTosllAmpNewExt{

 public:
  virtual ~EvtbTosllAmpNewExt( ) { } ;
 
  virtual void CalcAmp( EvtParticle *parent, EvtAmp& amp,
	 	        EvtbTosllFFNew *formFactors, 
                        EvtbTosllWilsCoeffNLO *WilsCoeff,
                        double /* mu */,         int /* Nf */,   
                        int /* res_swch */,      int /* ias */, 
                        double /* CKM_A */,      double /* CKM_lambda */, 
                        double /* CKM_barrho */, double /* CKM_bareta */, 
                        double /* ReA7 */,       double /* ImA7 */,
                        double /* ReA10 */,      double /*ImA10 */)=0;
  
  virtual double CalcMaxProb(EvtId /* parnum */, EvtId /* mesnum */,
                             EvtId /* l1num */,  EvtId /* l2num */,
                             EvtbTosllFFNew * /* formFactors */,
                             EvtbTosllWilsCoeffNLO * /*WilsCoeff */,
                             double /* mu */,         int /* Nf */,
                             int /* res_swch */,      int /* ias */,
                             double /* CKM_A */,      double /* CKM_lambda */,
                             double /* CKM_barrho */, double /* CKM_bareta */, 
                             double /* ReA7 */,       double /* ImA7 */,
                             double /* ReA10 */,      double /*ImA10 */) {return 0.0;};

  virtual double lambda(double /* a */, double /* b */, double /* c */) {return 0.0;};

};

#endif
