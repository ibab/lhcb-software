#include "MCElectronOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyMCElectronOSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/ele__eleMLPBNN_MC.class.C"
#endif
}

MCElectronOSWrapper::MCElectronOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyMCElectronOSSpace::Read_eleMLPBNN_MC(names);
#endif
}

MCElectronOSWrapper::~MCElectronOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double MCElectronOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
