#include "KaonOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyKaonOSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/kaon__kaonMLPBNN.class.C"
#endif
}

KaonOSWrapper::KaonOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyKaonOSSpace::Read_kaonMLPBNN(names);
#endif
}

KaonOSWrapper::~KaonOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double KaonOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
}
