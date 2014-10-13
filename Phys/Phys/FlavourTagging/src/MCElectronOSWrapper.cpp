#include "MCElectronOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyMCElectronOSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/ele__eleMLPBNN_MC.class.C"
#endif
}

MCElectronOSWrapper::MCElectronOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyMCElectronOSSpace::Read_eleMLPBNN_MC(names);
#endif
}

MCElectronOSWrapper::~MCElectronOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double MCElectronOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
