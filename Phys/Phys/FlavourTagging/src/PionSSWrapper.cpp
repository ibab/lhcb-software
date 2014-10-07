// Include files

// local
#include "PionSSWrapper.h"
#include "compileSettings.h"

namespace MyPionSSSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/BDT_SSpion/BDT_SSpion_Reco14.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : PionSSWrapper
//
// 2014-02-19 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

PionSSWrapper::PionSSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyPionSSSpace::ReadssPion(names);
#endif
}

PionSSWrapper::~PionSSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}

double PionSSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}

//=============================================================================
