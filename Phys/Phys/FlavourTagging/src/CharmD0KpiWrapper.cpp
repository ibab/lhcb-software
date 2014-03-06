// Include files 

// local
#include "CharmD0KpiWrapper.h"

namespace MyD0KpiSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_Kpi_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpiWrapper::CharmD0KpiWrapper(std::vector<std::string> & names) {
	mcreader = new MyD0KpiSpace::ReadBDT(names);
}

CharmD0KpiWrapper::~CharmD0KpiWrapper() {
  delete mcreader;
}

double CharmD0KpiWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
