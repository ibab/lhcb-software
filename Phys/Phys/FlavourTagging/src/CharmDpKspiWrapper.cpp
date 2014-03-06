// Include files 

// local
#include "CharmDpKspiWrapper.h"

namespace MyDpKspiSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_Kspi_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKspiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKspiWrapper::CharmDpKspiWrapper(std::vector<std::string> & names) {
	mcreader = new MyDpKspiSpace::ReadBDT(names);
}

CharmDpKspiWrapper::~CharmDpKspiWrapper() {
  delete mcreader;
}

double CharmDpKspiWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
