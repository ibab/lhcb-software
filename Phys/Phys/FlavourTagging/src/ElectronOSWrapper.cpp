#include "ElectronOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyElectronOSSpace{
#include "NeuralNet/weights/ele__eleMLPBNN.class.C"

}

ElectronOSWrapper::ElectronOSWrapper(std::vector<std::string> & names) {
	reader = new MyElectronOSSpace::Read_eleMLPBNN(names);
}

ElectronOSWrapper::~ElectronOSWrapper() {delete reader; }


double ElectronOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
