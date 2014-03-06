// Include files 

// local
#include "CharmD0Kpipi0Wrapper.h"

namespace MyD0Kpipi0Space {
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_Kpipi0_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0Kpipi0Wrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0Kpipi0Wrapper::CharmD0Kpipi0Wrapper(std::vector<std::string> & names) {
	mcreader = new MyD0Kpipi0Space::ReadBDT(names);
}

CharmD0Kpipi0Wrapper::~CharmD0Kpipi0Wrapper() {
  delete mcreader;
}

double CharmD0Kpipi0Wrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
