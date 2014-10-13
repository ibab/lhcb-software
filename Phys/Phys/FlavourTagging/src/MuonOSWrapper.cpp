#include "MuonOSWrapper.h"


// hack, otherwise: redefinitions...
//
namespace MyMuonOSSpace{
#ifndef SKIP_TMVA
  #include "NeuralNet/weights/muon__muonMLPBNN.class.C"
#endif
}

MuonOSWrapper::MuonOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyMuonOSSpace::Read_muonMLPBNN(names);
#endif
}

MuonOSWrapper::~MuonOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double MuonOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
