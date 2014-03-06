// Include files 

// local
#include "CharmD0KeXWrapper.h"

namespace MyD0KeXSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KeX_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KeXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KeXWrapper::CharmD0KeXWrapper(std::vector<std::string> & names) {
	mcreader = new MyD0KeXSpace::ReadBDT(names);
}

CharmD0KeXWrapper::~CharmD0KeXWrapper() {
  delete mcreader;
}

double CharmD0KeXWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
