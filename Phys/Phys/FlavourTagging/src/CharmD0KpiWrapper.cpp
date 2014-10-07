// Include files 

// local
#include "CharmD0KpiWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kpi.C"

namespace MyD0KpiSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_Kpi_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpiWrapper::CharmD0KpiWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
	mcreader = new MyD0KpiSpace::ReadBDT(names);
  purtable = new MyD0KpiSpace::PurityTable();
#endif
}

CharmD0KpiWrapper::~CharmD0KpiWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KpiWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
}

//=============================================================================
