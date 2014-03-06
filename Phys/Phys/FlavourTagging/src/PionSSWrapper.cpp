// Include files

// local
#include "PionSSWrapper.h"

namespace MyPionSSSpace {
#include "TMVAClassification/BDT_SSpion/BDT_SSpion_Reco14.class.C"
}


//-----------------------------------------------------------------------------
// Implementation file for class : PionSSWrapper
//
// 2014-02-19 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

PionSSWrapper::PionSSWrapper(std::vector<std::string> & names) {
  reader = new MyPionSSSpace::ReadssPion(names);
}

PionSSWrapper::~PionSSWrapper() {
  delete reader;
}

double PionSSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}

//=============================================================================
