#include "MCElectronOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMCElectronOSSpace{
#include "NeuralNet/weights/ele__eleMLPBNN_MC.class.C"

}

MCElectronOSWrapper::MCElectronOSWrapper(std::vector<std::string> & names) {
	reader = new MyMCElectronOSSpace::Read_eleMLPBNN_MC(names);
}

MCElectronOSWrapper::~MCElectronOSWrapper() {delete reader; }


double MCElectronOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
