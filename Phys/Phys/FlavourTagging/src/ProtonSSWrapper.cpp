// Include files

// local
#include "ProtonSSWrapper.h"


namespace MyProtonSSSpace {
#ifndef SKIP_TMVA
#include "TMVAClassification/BDT_SSproton/BDT_SSproton_Reco14.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : ProtonSSWrapper
//
// 2014-02-19 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

ProtonSSWrapper::ProtonSSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyProtonSSSpace::ReadssProton(names);
#endif
}

ProtonSSWrapper::~ProtonSSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}

double ProtonSSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifdef SKIP_TMVA
  int size = values.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
  return 0.0;
#else
  return reader->GetMvaValue(values);
#endif
  return 0.0;
}

//=============================================================================
