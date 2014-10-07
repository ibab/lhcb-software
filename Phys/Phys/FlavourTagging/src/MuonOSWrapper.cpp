#include "MuonOSWrapper.h"

#include "compileSettings.h"
// hack, otherwise: redefinitions...
//
namespace MyMuonOSSpace{
#ifndef SKIP_TMVA_COMPILE
  #include "NeuralNet/weights/muon__muonMLPBNN.class.C"
#endif
}

MuonOSWrapper::MuonOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyMuonOSSpace::Read_muonMLPBNN(names);
#endif
}

MuonOSWrapper::~MuonOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double MuonOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
