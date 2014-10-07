// Include files 

// local
#include "CharmD0KpiXWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kpiX.C"

namespace MyD0KpiXSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KpiX_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpiXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpiXWrapper::CharmD0KpiXWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  mcreader = new MyD0KpiXSpace::ReadBDT(names);
  purtable = new MyD0KpiXSpace::PurityTable();
#endif
}

CharmD0KpiXWrapper::~CharmD0KpiXWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KpiXWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
  
}

//=============================================================================
