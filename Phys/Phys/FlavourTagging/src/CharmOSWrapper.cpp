#include "CharmOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyCharmOSSpace{
#include "NeuralNet/weights/charm__charmMLPBNN.class.C"

}

CharmOSWrapper::CharmOSWrapper(std::vector<std::string> & names) {
	reader = new MyCharmOSSpace::Read_charmMLPBNN(names);
}

CharmOSWrapper::~CharmOSWrapper() {delete reader; }


double CharmOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
