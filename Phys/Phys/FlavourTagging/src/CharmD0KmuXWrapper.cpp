// Include files 

// local
#include "CharmD0KmuXWrapper.h"

namespace MyD0KmuXSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KmuX_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KmuXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KmuXWrapper::CharmD0KmuXWrapper(std::vector<std::string> & names) {
	mcreader = new MyD0KmuXSpace::ReadBDT(names);
}

CharmD0KmuXWrapper::~CharmD0KmuXWrapper() {
  delete mcreader;
}

double CharmD0KmuXWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
