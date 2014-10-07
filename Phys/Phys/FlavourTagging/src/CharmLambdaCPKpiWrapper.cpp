// Include files 

// local
#include "CharmLambdaCPKpiWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_LambdaC2pkpi.C"

namespace MyLambdaCPKpiSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_LambdaC_PKPi_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmLambdaCPKpiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmLambdaCPKpiWrapper::CharmLambdaCPKpiWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
	mcreader = new MyLambdaCPKpiSpace::ReadBDT(names);
  purtable = new MyLambdaCPKpiSpace::PurityTable();
#endif
}

CharmLambdaCPKpiWrapper::~CharmLambdaCPKpiWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmLambdaCPKpiWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
}

//=============================================================================
