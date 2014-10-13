#include "MCPionSSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyMCPionSSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/pionS__pionSMLPBNN_MC.class.C"
#endif
}

MCPionSSWrapper::MCPionSSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyMCPionSSSpace::Read_pionSMLPBNN_MC(names);
#endif
}

MCPionSSWrapper::~MCPionSSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double MCPionSSWrapper::GetMvaValue(std::vector<double> const & values) {
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
