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
// Module: EvtGen/EvtXLL.hh
//
// Description: {B,Lb}->{Kpi,KK,pK,pipi,...}ell ell, w/ optimized dihadron, cos_theta, spectra 
//
// Modification history:
//
//    Biplab Dey, 4/09/2015    Module created
//
//------------------------------------------------------------------------

#ifndef EVTXLL_HH
#define EVTXLL_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtXLL: public  EvtDecayProb  {

public:

    EvtXLL() {}
    virtual ~EvtXLL();

    std::string getName();
    EvtDecayBase* clone();

    void init();
    void initProbMax();

    void decay(EvtParticle *p);

private:

    // to get the m(hh) spectra pdf
    // -1 (flat), 0 (Kpi), 1 (KK), 2 (pK)
    int _chan;
    // masses of the (up to four) daughters
    double _masses[4];
    // limits on the m(hh) mass range
    double _MX_LO, _MX_HI;
    // calculate the maximum prob channel by channel
    double _MY_MAX_PROB; 
    // Maximum number of phase space iterations
    int _nLoop;

    double fn_p_Mm1m2(double M, double m1, double m2);
    double fn_cth(double cth);
    double fn_wt_dihadron(double mX, int chan);
    bool genValidPhaseSpace(EvtParticle* p);

};

#endif

