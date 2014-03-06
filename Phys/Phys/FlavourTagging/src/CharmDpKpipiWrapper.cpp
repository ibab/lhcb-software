// Include files 

// local
#include "CharmDpKpipiWrapper.h"

namespace MyDpKpipiSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_Kpipi_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKpipiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKpipiWrapper::CharmDpKpipiWrapper(std::vector<std::string> & names) {
	mcreader = new MyDpKpipiSpace::ReadBDT(names);
}

CharmDpKpipiWrapper::~CharmDpKpipiWrapper() {
  delete mcreader;
}

double CharmDpKpipiWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
