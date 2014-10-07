#include "MCKaonOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyMCKaonOSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/kaon__kaonMLPBNN_MC.class.C"
#endif
}

MCKaonOSWrapper::MCKaonOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyMCKaonOSSpace::Read_kaonMLPBNN_MC(names);
#endif
}

MCKaonOSWrapper::~MCKaonOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}

double MCKaonOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
