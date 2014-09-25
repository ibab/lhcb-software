#include "MCMuonOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMCMuonOSSpace{
#include "NeuralNet/weights/muon__muonMLPBNN_MC.class.C"

}

MCMuonOSWrapper::MCMuonOSWrapper(std::vector<std::string> & names) {
	reader = new MyMCMuonOSSpace::Read_muonMLPBNN_MC(names);
}

MCMuonOSWrapper::~MCMuonOSWrapper() {delete reader; }


double MCMuonOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
