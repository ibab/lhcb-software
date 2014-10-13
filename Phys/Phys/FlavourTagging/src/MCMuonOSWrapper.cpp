#include "MCMuonOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyMCMuonOSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/muon__muonMLPBNN_MC.class.C"
#endif
}

MCMuonOSWrapper::MCMuonOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
	reader = new MyMCMuonOSSpace::Read_muonMLPBNN_MC(names);
#endif
}

MCMuonOSWrapper::~MCMuonOSWrapper() 
{ 
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double MCMuonOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
