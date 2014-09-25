#include "MuonOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMuonOSSpace{
#include "NeuralNet/weights/muon__muonMLPBNN.class.C"

}

MuonOSWrapper::MuonOSWrapper(std::vector<std::string> & names) {
	reader = new MyMuonOSSpace::Read_muonMLPBNN(names);
}

MuonOSWrapper::~MuonOSWrapper() {delete reader; }


double MuonOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
