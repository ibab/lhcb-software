// Include files 

// local
#include "CharmDstD0KspipiWrapper.h"

namespace MyDstD0KspipiSpace {
#include "TMVAClassification/mva_charmtagger_reco14/train_Dstar_D0_Kspipi_BDT.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDstD0KspipiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDstD0KspipiWrapper::CharmDstD0KspipiWrapper(std::vector<std::string> & names) {
	mcreader = new MyDstD0KspipiSpace::ReadBDT(names);
}

CharmDstD0KspipiWrapper::~CharmDstD0KspipiWrapper() {
  delete mcreader;
}

double CharmDstD0KspipiWrapper::GetMvaValue(std::vector<double> const & values) {
  return mcreader->GetMvaValue(values);
}

//=============================================================================
