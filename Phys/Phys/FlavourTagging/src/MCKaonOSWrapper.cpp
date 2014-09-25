#include "MCKaonOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMCKaonOSSpace{
#include "NeuralNet/weights/kaon__kaonMLPBNN_MC.class.C"

}

MCKaonOSWrapper::MCKaonOSWrapper(std::vector<std::string> & names) {
	reader = new MyMCKaonOSSpace::Read_kaonMLPBNN_MC(names);
}

MCKaonOSWrapper::~MCKaonOSWrapper() {delete reader; }

double MCKaonOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
