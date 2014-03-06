// Include files 

// local
#include "CharmD0KpipipiWrapper.h"

namespace MyD0KpipipiSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_Kpipipi_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpipipiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpipipiWrapper::CharmD0KpipipiWrapper(std::vector<std::string> & names) {
	mcreader = new MyD0KpipipiSpace::ReadBDT(names);
}

CharmD0KpipipiWrapper::~CharmD0KpipipiWrapper() {
  delete mcreader;
}

double CharmD0KpipipiWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
