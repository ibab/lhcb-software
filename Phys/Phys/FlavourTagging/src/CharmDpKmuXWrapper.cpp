// Include files 

// local
#include "CharmDpKmuXWrapper.h"

namespace MyDpKmuXSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_KmuX_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKmuXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKmuXWrapper::CharmDpKmuXWrapper(std::vector<std::string> & names) {
	mcreader = new MyDpKmuXSpace::ReadBDT(names);
}

CharmDpKmuXWrapper::~CharmDpKmuXWrapper() {
  delete mcreader;
}

double CharmDpKmuXWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
