// Include files 

// local
#include "CharmDpKeXWrapper.h"

namespace MyDpKeXSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_KeX_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKeXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKeXWrapper::CharmDpKeXWrapper(std::vector<std::string> & names) {
	mcreader = new MyDpKeXSpace::ReadBDT(names);
}

CharmDpKeXWrapper::~CharmDpKeXWrapper() {
  delete mcreader;
}

double CharmDpKeXWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
