#include "KaonOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyKaonOSSpace{
#include "NeuralNet/weights/kaon__kaonMLPBNN.class.C"

}

KaonOSWrapper::KaonOSWrapper(std::vector<std::string> & names) {
	reader = new MyKaonOSSpace::Read_kaonMLPBNN(names);
}

KaonOSWrapper::~KaonOSWrapper() {delete reader; }


double KaonOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
