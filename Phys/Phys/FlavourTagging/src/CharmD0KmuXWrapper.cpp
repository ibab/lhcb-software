// Include files 

// local
#include "CharmD0KmuXWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kmuX.C"

namespace MyD0KmuXSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KmuX_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KmuXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KmuXWrapper::CharmD0KmuXWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  mcreader = new MyD0KmuXSpace::ReadBDT(names);
  purtable = new MyD0KmuXSpace::PurityTable();
#endif
}

CharmD0KmuXWrapper::~CharmD0KmuXWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KmuXWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
  
}

//=============================================================================
