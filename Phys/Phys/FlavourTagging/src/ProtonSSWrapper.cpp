// Include files

// local
#include "ProtonSSWrapper.h"

namespace MyProtonSSSpace {
#include "TMVAClassification/BDT_SSproton/BDT_SSproton_Reco14.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : ProtonSSWrapper
//
// 2014-02-19 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

ProtonSSWrapper::ProtonSSWrapper(std::vector<std::string> & names) {
  reader = new MyProtonSSSpace::ReadssProton(names);
}

ProtonSSWrapper::~ProtonSSWrapper() {
  delete reader;
}

double ProtonSSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}

//=============================================================================
