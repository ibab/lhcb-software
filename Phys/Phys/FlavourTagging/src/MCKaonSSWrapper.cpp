#include "MCKaonSSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyMCKaonSSSpace{
#ifndef SKIP_TMVA
  #include "NeuralNet/weights/kaonS__kaonSMLPBNN_MC.class.C"
#endif
}

MCKaonSSWrapper::MCKaonSSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyMCKaonSSSpace::Read_kaonSMLPBNN_MC(names);
#endif
}

MCKaonSSWrapper::~MCKaonSSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double MCKaonSSWrapper::GetMvaValue(std::vector<double> const & values) {
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
