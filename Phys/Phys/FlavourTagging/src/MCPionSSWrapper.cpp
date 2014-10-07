#include "MCPionSSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyMCPionSSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/pionS__pionSMLPBNN_MC.class.C"
#endif
}

MCPionSSWrapper::MCPionSSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyMCPionSSSpace::Read_pionSMLPBNN_MC(names);
#endif
}

MCPionSSWrapper::~MCPionSSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double MCPionSSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;  
}
