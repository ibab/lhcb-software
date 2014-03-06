// Include files 

// local
#include "CharmD0KpiXWrapper.h"

namespace MyD0KpiXSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KpiX_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpiXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpiXWrapper::CharmD0KpiXWrapper(std::vector<std::string> & names) {
	mcreader = new MyD0KpiXSpace::ReadBDT(names);
}

CharmD0KpiXWrapper::~CharmD0KpiXWrapper() {
  delete mcreader;
}

double CharmD0KpiXWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
