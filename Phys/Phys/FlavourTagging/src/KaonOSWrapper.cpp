#include "KaonOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyKaonOSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/kaon__kaonMLPBNN.class.C"
#endif
}

KaonOSWrapper::KaonOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyKaonOSSpace::Read_kaonMLPBNN(names);
#endif
}

KaonOSWrapper::~KaonOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double KaonOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
