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
// Module: EvtModel.cc
//
// Description:
//
// Modification history:
//
//    RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "EvtGenBase/EvtModel.hh"

#include "EvtGenModels/EvtTauVectornu.hh"
#include "EvtGenModels/EvtVVP.hh"
#include "EvtGenModels/EvtSLN.hh"
#include "EvtGenModels/EvtISGW2.hh"
#include "EvtGenModels/EvtMelikhov.hh"
#include "EvtGenModels/EvtSLPole.hh"
#include "EvtGenModels/EvtISGW.hh"
#include "EvtGenModels/EvtBHadronic.hh"
#include "EvtGenModels/EvtVSS.hh"
#include "EvtGenModels/EvtVSSMix.hh"
#include "EvtGenModels/EvtVSSBMixCPT.hh"
#include "EvtGenModels/EvtVSPPwave.hh"
#include "EvtGenModels/EvtGoityRoberts.hh"
#include "EvtGenModels/EvtSVS.hh"
#include "EvtGenModels/EvtTSS.hh"
#include "EvtGenModels/EvtTVSPwave.hh"
#include "EvtGenModels/EvtSVVHelAmp.hh"
#include "EvtGenModels/EvtSVPHelAmp.hh"
#include "EvtGenModels/EvtVVSPwave.hh"
#include "EvtGenModels/EvtDDalitz.hh"
#include "EvtGenModels/EvtOmegaDalitz.hh"
#include "EvtGenModels/EvtPi0Dalitz.hh"
#include "EvtGenModels/EvtEtaDalitz.hh"
#include "EvtGenModels/EvtPhsp.hh"
#include "EvtGenModels/EvtBtoXsgamma.hh"
#include "EvtGenModels/EvtBtoXsll.hh"
#include "EvtGenModels/EvtBtoXsEtap.hh"
#include "EvtGenModels/EvtSSSCP.hh"
#include "EvtGenModels/EvtSSSCPpng.hh"
#include "EvtGenModels/EvtSTSCP.hh"
#include "EvtGenModels/EvtSTS.hh"
#include "EvtGenModels/EvtSSSCPT.hh"
#include "EvtGenModels/EvtSVSCP.hh"
#include "EvtGenModels/EvtSSDCP.hh"
#include "EvtGenModels/EvtSVSNONCPEIGEN.hh"
#include "EvtGenModels/EvtSVVNONCPEIGEN.hh"
#include "EvtGenModels/EvtSVVCP.hh"
#include "EvtGenModels/EvtSVVCPLH.hh"
#include "EvtGenModels/EvtSVSCPLH.hh"
#include "EvtGenModels/EvtSll.hh"
#include "EvtGenModels/EvtVll.hh"
#include "EvtGenModels/EvtTaulnunu.hh"
#include "EvtGenModels/EvtTauHadnu.hh"
#include "EvtGenModels/EvtTauScalarnu.hh"
#include "EvtGenModels/EvtKstarnunu.hh"
#include "EvtGenModels/EvtbTosllBall.hh"
#include "EvtGenModels/EvtSingleParticle.hh"
#include "EvtGenModels/EvtVectorIsr.hh"

#include "EvtGenModels/EvtHQET.hh"
#include "EvtGenModels/EvtHQET2.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtHelAmp.hh"
#include "EvtGenModels/EvtPartWave.hh"

#include "EvtGenModels/EvtBto2piCPiso.hh"
#include "EvtGenModels/EvtBtoKpiCPiso.hh"
#include "EvtGenModels/EvtSVSCPiso.hh"
#include "EvtGenModels/EvtVVpipi.hh"

#include "EvtGenModels/EvtBTo4piCP.hh"
#include "EvtGenModels/EvtBTo3piCP.hh"
#include "EvtGenModels/EvtCBTo3piP00.hh"
#include "EvtGenModels/EvtCBTo3piMPP.hh"
#include "EvtGenModels/EvtBToKpipiCP.hh"
#include "EvtGenModels/EvtBsquark.hh"
#include "EvtGenModels/EvtPhiDalitz.hh"

#include "EvtGenModels/EvtLNuGamma.hh"

#include "EvtGenModels/EvtVub.hh"
#include "EvtGenModels/EvtPto3P.hh"
#include "EvtGenModels/EvtKstarstargamma.hh"

#include "EvtGenModels/EvtBToDDalitzCPK.hh"

#ifndef EVTSTANDALONE
#include "EvtGenModels/EvtUclaII.hh"
#include "EvtGenModels/EvtJetSet.hh"
#include "EvtGenModels/EvtJscont.hh"
#else
#include "EvtGenModels/EvtPythia.hh"
//#include "EvtGenModels/EvtPyGaGa.hh"
//#include "EvtGenModels/EvtPycont.hh"
#endif

#include "EvtGenModels/EvtModelReg.hh"

EvtModelReg::EvtModelReg() {

  EvtModel &modelist=EvtModel::instance();


  modelist.Register(new EvtTauHadnu);
  modelist.Register(new EvtTauVectornu);
  modelist.Register(new EvtVVP);
  modelist.Register(new EvtSLN);
  modelist.Register(new EvtISGW2);
  modelist.Register(new EvtMelikhov);
  modelist.Register(new EvtSLPole);
  modelist.Register(new EvtHQET);
  modelist.Register(new EvtHQET2);
  modelist.Register(new EvtISGW);
  modelist.Register(new EvtBHadronic);
  modelist.Register(new EvtVSS);
  modelist.Register(new EvtVSSMix);
  modelist.Register(new EvtVSSBMixCPT);
  modelist.Register(new EvtVSPPwave);
  modelist.Register(new EvtGoityRoberts);
  modelist.Register(new EvtSVS);
  modelist.Register(new EvtTSS);
  modelist.Register(new EvtTVSPwave);
  modelist.Register(new EvtSVVHelAmp);
  modelist.Register(new EvtSVPHelAmp);
  modelist.Register(new EvtVVSPwave);
  modelist.Register(new EvtDDalitz);
  modelist.Register(new EvtOmegaDalitz);
  modelist.Register(new EvtEtaDalitz);
  modelist.Register(new EvtPhsp);
  modelist.Register(new EvtBtoXsgamma);
  modelist.Register(new EvtBtoXsll);
  modelist.Register(new EvtBtoXsEtap);
  modelist.Register(new EvtSSSCP);
  modelist.Register(new EvtSSSCPpng);
  modelist.Register(new EvtSTSCP);
  modelist.Register(new EvtSTS);
  modelist.Register(new EvtSSSCPT);
  modelist.Register(new EvtSVSCP);
  modelist.Register(new EvtSSDCP);
  modelist.Register(new EvtSVSNONCPEIGEN);
  modelist.Register(new EvtSVVNONCPEIGEN);
  modelist.Register(new EvtSVVCP);
  modelist.Register(new EvtSVVCPLH);
  modelist.Register(new EvtSVSCPLH);
  modelist.Register(new EvtSll);
  modelist.Register(new EvtVll);
  modelist.Register(new EvtTaulnunu);
  modelist.Register(new EvtTauScalarnu);
  modelist.Register(new EvtKstarnunu);
  modelist.Register(new EvtbTosllBall);
  modelist.Register(new EvtBto2piCPiso);
  modelist.Register(new EvtBtoKpiCPiso);
  modelist.Register(new EvtSVSCPiso);
  modelist.Register(new EvtSingleParticle);
  modelist.Register(new EvtVectorIsr);
  modelist.Register(new EvtPi0Dalitz);
  modelist.Register(new EvtHelAmp);
  modelist.Register(new EvtPartWave);
  modelist.Register(new EvtVVpipi);
  modelist.Register(new EvtBsquark);
  modelist.Register(new EvtPhiDalitz); 

  modelist.Register(new EvtBTo4piCP);
  modelist.Register(new EvtBTo3piCP);
  modelist.Register(new EvtCBTo3piP00);
  modelist.Register(new EvtCBTo3piMPP);
  modelist.Register(new EvtBToKpipiCP);

  modelist.Register(new EvtLNuGamma);
  modelist.Register(new EvtKstarstargamma);

  modelist.Register(new EvtVub);
  modelist.Register(new EvtPto3P);

  modelist.Register( new EvtBToDDalitzCPK ) ;

#ifndef EVTSTANDALONE
  modelist.Register(new EvtUclaII);
  modelist.Register(new EvtJetSet);
  modelist.Register(new EvtJscont);
#else
  //  modelist.Register(new EvtPycont);
  modelist.Register(new EvtPythia);
  //  modelist.Register(new EvtPyGaGa);
#endif

}








