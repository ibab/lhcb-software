// Include files 

// local
#include "CharmD0Kpipi0Wrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kpipiz.C"

namespace MyD0Kpipi0Space {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_Kpipi0_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0Kpipi0Wrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0Kpipi0Wrapper::CharmD0Kpipi0Wrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE  
  mcreader = new MyD0Kpipi0Space::ReadBDT(names);
  purtable = new MyD0Kpipi0Space::PurityTable();
#endif
}

CharmD0Kpipi0Wrapper::~CharmD0Kpipi0Wrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0Kpipi0Wrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
  
}

//=============================================================================
