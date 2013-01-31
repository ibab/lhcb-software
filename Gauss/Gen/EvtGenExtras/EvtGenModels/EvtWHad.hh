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
// Module: EvtWHad.cc
//
// Description: Routine to calculate W -> (n pi) + (m K) current
//			according to [Kuhn, Was, Acta.Phys.Polon B39 (2008) 147]
//
// Modification history:
//	AVL	20 Jan, 2013	Module created
//
//------------------------------------------------------------------------
// 


#ifndef EvtWHad_HH
#define EvtWHad_HH

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenModels/EvtTauHadnu.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenBase/EvtParser.hh"


class EvtWHad {
public:
  EvtVector4C WCurrent(EvtVector4R q1);
  EvtVector4C WCurrent(EvtVector4R q1, EvtVector4R q2);
  EvtVector4C WCurrent(EvtVector4R q1, EvtVector4R q2, EvtVector4R q3);
  EvtVector4C WCurrent(EvtVector4R q1, EvtVector4R q2, EvtVector4R q3, EvtVector4R q4, EvtVector4R q5);
  EvtVector4C WCurrent_KKP(EvtVector4R pKplus, EvtVector4R pKminus, EvtVector4R pPiPlus);
  EvtVector4C WCurrent_KPP(EvtVector4R pKplus, EvtVector4R pPiPlus, EvtVector4R pPiMinus);
protected:
  EvtVector4C JB(EvtVector4R q1, EvtVector4R q2, EvtVector4R q3, EvtVector4R q4, EvtVector4R q5); 
  EvtComplex BWa( EvtVector4R q);
  EvtComplex BWf( EvtVector4R q);
  EvtComplex BWr( EvtVector4R q);
  double pi3G(double Q2);
};

#endif
