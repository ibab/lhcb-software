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
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtModel.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDecayParm.hh"
#include "EvtGen/EvtSymTable.hh"
#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtParticleDecayList.hh"
#include "EvtGen/EvtParser.hh"
#include "EvtGen/EvtReport.hh"

#include "EvtGen/EvtTauVectornu.hh"
#include "EvtGen/EvtVVP.hh"
#include "EvtGen/EvtSLN.hh"
#include "EvtGen/EvtISGW2.hh"
#include "EvtGen/EvtMelikhov.hh"
#include "EvtGen/EvtSLPole.hh"
#include "EvtGen/EvtISGW.hh"
#include "EvtGen/EvtWSB.hh"
#include "EvtGen/EvtKS.hh"
#include "EvtGen/EvtBHadronic.hh"
#include "EvtGen/EvtVSS.hh"
#include "EvtGen/EvtVSSMix.hh"
#include "EvtGen/EvtVSSBMix.hh"
#include "EvtGen/EvtVSPPwave.hh"
#include "EvtGen/EvtGoityRoberts.hh"
#include "EvtGen/EvtSVS.hh"
#include "EvtGen/EvtTSS.hh"
#include "EvtGen/EvtTVSPwave.hh"
#include "EvtGen/EvtSVVHelAmp.hh"
#include "EvtGen/EvtSVPHelAmp.hh"
#include "EvtGen/EvtVVSPwave.hh"
#include "EvtGen/EvtDDalitz.hh"
#include "EvtGen/EvtOmegaDalitz.hh"
#include "EvtGen/EvtPi0Dalitz.hh"
#include "EvtGen/EvtEtaDalitz.hh"
#include "EvtGen/EvtPhsp.hh"
#include "EvtGen/EvtBtoXsgamma.hh"
#include "EvtGen/EvtBtoXsll.hh"
#include "EvtGen/EvtBtoXsEtap.hh"
#include "EvtGen/EvtSSSCP.hh"
#include "EvtGen/EvtSSSCPpng.hh"
#include "EvtGen/EvtSTSCP.hh"
#include "EvtGen/EvtSTS.hh"
#include "EvtGen/EvtSSSCPT.hh"
#include "EvtGen/EvtSVSCP.hh"
#include "EvtGen/EvtSSDCP.hh"
#include "EvtGen/EvtSVSNONCPEIGEN.hh"
#include "EvtGen/EvtSVVNONCPEIGEN.hh"
#include "EvtGen/EvtSVVCP.hh"
#include "EvtGen/EvtSVVCPLH.hh"
#include "EvtGen/EvtSVSCPLH.hh"
#include "EvtGen/EvtSll.hh"
#include "EvtGen/EvtUPS4S.hh"
#include "EvtGen/EvtVll.hh"
#include "EvtGen/EvtTaulnunu.hh"
#include "EvtGen/EvtTauScalarnu.hh"
#include "EvtGen/EvtKstarnunu.hh"
#include "EvtGen/EvtKsLLLiCo.hh"
#include "EvtGen/EvtKsLLTrPo.hh"
#include "EvtGen/EvtbTosllBall.hh"
#include "EvtGen/EvtKLL.hh"
#include "EvtGen/EvtSingleParticle.hh"
#include "EvtGen/EvtVectorIsr.hh"

#include "EvtGen/EvtHQET.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtModel.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtHelAmp.hh"
#include "EvtGen/EvtPartWave.hh"

#include "EvtGen/EvtBto2piCPiso.hh"
#include "EvtGen/EvtBtoKpiCPiso.hh"
#include "EvtGen/EvtSVSCPiso.hh"
#include "EvtGen/EvtVVpipi.hh"

#include "EvtGen/EvtJetSet.hh"
#include "EvtGen/EvtJscont.hh"
#include "EvtGen/EvtBTo4piCP.hh"
#include "EvtGen/EvtBTo3piCP.hh"
#include "EvtGen/EvtCBTo3piP00.hh"
#include "EvtGen/EvtCBTo3piMPP.hh"
#include "EvtGen/EvtBToKpipiCP.hh"
#include "EvtGen/EvtBsquark.hh"

#include "EvtGen/EvtLNuGamma.hh"

#include "EvtGen/EvtVub.hh"

#ifndef EVTSTANDALONE
#include "EvtGen/EvtUclaII.hh"
#endif


#define SRegister(M) EvtModel::Register(new M)

EvtModel::EvtModel() {

  _modelNameHash=new EvtStringHash<EvtDecayBase>(127); 
  _commandNameHash=new EvtStringHash<EvtDecayBase>(127); 

}


void EvtModel::ModelsStored(void) {

  SRegister(EvtTauVectornu);
  SRegister(EvtVVP);
  SRegister(EvtSLN);
  SRegister(EvtISGW2);
  SRegister(EvtMelikhov);
  SRegister(EvtSLPole);
  SRegister(EvtHQET);
  SRegister(EvtISGW);
  SRegister(EvtWSB);
  SRegister(EvtKS);
  SRegister(EvtBHadronic);
  SRegister(EvtVSS);
  SRegister(EvtVSSMix);
  SRegister(EvtVSSBMix);
  SRegister(EvtVSPPwave);
  SRegister(EvtGoityRoberts);
  SRegister(EvtSVS);
  SRegister(EvtTSS);
  SRegister(EvtTVSPwave);
  SRegister(EvtSVVHelAmp);
  SRegister(EvtSVPHelAmp);
  SRegister(EvtVVSPwave);
  SRegister(EvtDDalitz);
  SRegister(EvtOmegaDalitz);
  SRegister(EvtEtaDalitz);
  SRegister(EvtPhsp);
  SRegister(EvtBtoXsgamma);
  SRegister(EvtBtoXsll);
  SRegister(EvtBtoXsEtap);
  SRegister(EvtSSSCP);
  SRegister(EvtSSSCPpng);
  SRegister(EvtSTSCP);
  SRegister(EvtSTS);
  SRegister(EvtSSSCPT);
  SRegister(EvtSVSCP);
  SRegister(EvtSSDCP);
  SRegister(EvtSVSNONCPEIGEN);
  SRegister(EvtSVVNONCPEIGEN);
  SRegister(EvtSVVCP);
  SRegister(EvtSVVCPLH);
  SRegister(EvtSVSCPLH);
  SRegister(EvtSll);
  SRegister(EvtUPS4S);
  SRegister(EvtVll);
  SRegister(EvtTaulnunu);
  SRegister(EvtTauScalarnu);
  SRegister(EvtKstarnunu);
  SRegister(EvtKsLLLC);
  SRegister(EvtKsLLTrPo);
  SRegister(EvtbTosllBall);
  SRegister(EvtKLL);
  SRegister(EvtBto2piCPiso);
  SRegister(EvtBtoKpiCPiso);
  SRegister(EvtSVSCPiso);
  SRegister(EvtSingleParticle);
  SRegister(EvtVectorIsr);
  SRegister(EvtPi0Dalitz);
  SRegister(EvtHelAmp);
  SRegister(EvtPartWave);
  SRegister(EvtVVpipi);
  SRegister(EvtBsquark);

  SRegister(EvtJetSet);
  SRegister(EvtJscont);
  SRegister(EvtBTo4piCP);
  SRegister(EvtBTo3piCP);
  SRegister(EvtCBTo3piP00);
  SRegister(EvtCBTo3piMPP);
  SRegister(EvtBToKpipiCP);

  SRegister(EvtLNuGamma);

  Register(new EvtVub);
#ifndef EVTSTANDALONE
  SRegister(EvtUclaII);
#endif

}




EvtDecayBase* EvtModel::GetFcn(EvtString model_name){

  EvtDecayBase* model=_modelNameHash->get(model_name);

  if (model==0){
    report(ERROR,"EvtGen") << "Did not find the right model:"
			   <<model_name<<"\n";
    return 0;
  }

  return model->clone();

}


void EvtModel::Register(EvtDecayBase* prototype){

  EvtString modelName;
  prototype->getName(modelName);

  _modelNameHash->add(modelName,prototype);

  EvtString commandName=prototype->commandName();
  
  if (commandName!=""){

    //report(DEBUG,"EvtGen") << "Adding command:"<<commandName<<endl;

    _commandNameHash->add(commandName,prototype);

  }

}

int EvtModel::isModel(EvtString model_name){

  return (0!=_modelNameHash->get(model_name));

}


int EvtModel::isCommand(EvtString cmd){

  return (0!=_commandNameHash->get(cmd));

}

void EvtModel::storeCommand(EvtString cmd,EvtString cnfgstr){

  EvtDecayBase* model=_commandNameHash->get(cmd);

  assert(model!=0);

  model->command(cnfgstr);

}




