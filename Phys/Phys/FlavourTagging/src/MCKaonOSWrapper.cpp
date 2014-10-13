#include "MCKaonOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyMCKaonOSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/kaon__kaonMLPBNN_MC.class.C"
#endif
}

MCKaonOSWrapper::MCKaonOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyMCKaonOSSpace::Read_kaonMLPBNN_MC(names);
#endif
}

MCKaonOSWrapper::~MCKaonOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}

double MCKaonOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
