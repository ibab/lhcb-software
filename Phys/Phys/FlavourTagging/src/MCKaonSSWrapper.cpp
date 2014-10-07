#include "MCKaonSSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyMCKaonSSSpace{
#ifndef SKIP_TMVA_COMPILE
  #include "NeuralNet/weights/kaonS__kaonSMLPBNN_MC.class.C"
#endif
}

MCKaonSSWrapper::MCKaonSSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyMCKaonSSSpace::Read_kaonSMLPBNN_MC(names);
#endif
}

MCKaonSSWrapper::~MCKaonSSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double MCKaonSSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
