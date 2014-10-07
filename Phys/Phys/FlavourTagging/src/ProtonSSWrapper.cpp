// Include files

// local
#include "ProtonSSWrapper.h"
#include "compileSettings.h"

namespace MyProtonSSSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/BDT_SSproton/BDT_SSproton_Reco14.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : ProtonSSWrapper
//
// 2014-02-19 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

ProtonSSWrapper::ProtonSSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyProtonSSSpace::ReadssProton(names);
#endif
}

ProtonSSWrapper::~ProtonSSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}

double ProtonSSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE  
  return reader->GetMvaValue(values);
#endif
  return 0;
}

//=============================================================================
