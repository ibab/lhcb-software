// Include files 

// local
#include "CharmD0KeXWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02keX.C"

namespace MyD0KeXSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KeX_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KeXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KeXWrapper::CharmD0KeXWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  mcreader = new MyD0KeXSpace::ReadBDT(names);
  purtable = new MyD0KeXSpace::PurityTable();
#endif
}

CharmD0KeXWrapper::~CharmD0KeXWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KeXWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
}

//=============================================================================
