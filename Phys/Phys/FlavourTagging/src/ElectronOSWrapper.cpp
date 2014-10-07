#include "ElectronOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyElectronOSSpace{
#ifndef SKIP_TMVA_COMPILE  
#include "NeuralNet/weights/ele__eleMLPBNN.class.C"
#endif
}

ElectronOSWrapper::ElectronOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyElectronOSSpace::Read_eleMLPBNN(names);
#endif
}

ElectronOSWrapper::~ElectronOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double ElectronOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
