#include "ElectronOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyElectronOSSpace{
#ifndef SKIP_TMVA  
#include "NeuralNet/weights/ele__eleMLPBNN.class.C"
#endif
}

ElectronOSWrapper::ElectronOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyElectronOSSpace::Read_eleMLPBNN(names);
#endif
}

ElectronOSWrapper::~ElectronOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double ElectronOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
