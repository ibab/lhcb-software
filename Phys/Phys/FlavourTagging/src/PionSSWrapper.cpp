// Include files

// local
#include "PionSSWrapper.h"


namespace MyPionSSSpace {
#ifndef SKIP_TMVA
#include "TMVAClassification/BDT_SSpion/BDT_SSpion_Reco14.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : PionSSWrapper
//
// 2014-02-19 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

PionSSWrapper::PionSSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyPionSSSpace::ReadssPion(names);
#endif
}

PionSSWrapper::~PionSSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}

double PionSSWrapper::GetMvaValue(std::vector<double> const & values) {
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
