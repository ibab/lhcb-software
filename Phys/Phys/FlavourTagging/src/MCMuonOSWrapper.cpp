#include "MCMuonOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyMCMuonOSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/muon__muonMLPBNN_MC.class.C"
#endif
}

MCMuonOSWrapper::MCMuonOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
	reader = new MyMCMuonOSSpace::Read_muonMLPBNN_MC(names);
#endif
}

MCMuonOSWrapper::~MCMuonOSWrapper() 
{ 
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double MCMuonOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
}
