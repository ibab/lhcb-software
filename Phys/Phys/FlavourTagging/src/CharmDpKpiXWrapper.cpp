// Include files 

// local
#include "CharmDpKpiXWrapper.h"

namespace MyDpKpiXSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_KpiX_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKpiXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKpiXWrapper::CharmDpKpiXWrapper(std::vector<std::string> & names) {
	mcreader = new MyDpKpiXSpace::ReadBDT(names);
}

CharmDpKpiXWrapper::~CharmDpKpiXWrapper() {
  delete mcreader;
}

double CharmDpKpiXWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
