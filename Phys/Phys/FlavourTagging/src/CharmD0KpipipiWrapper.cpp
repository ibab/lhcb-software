// Include files 

// local
#include "CharmD0KpipipiWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kpipipi.C"

namespace MyD0KpipipiSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_Kpipipi_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpipipiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpipipiWrapper::CharmD0KpipipiWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TVMA_COMPILE	
  mcreader = new MyD0KpipipiSpace::ReadBDT(names);
  purtable = new MyD0KpipipiSpace::PurityTable();
#endif
}

CharmD0KpipipiWrapper::~CharmD0KpipipiWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KpipipiWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
  
}

//=============================================================================
