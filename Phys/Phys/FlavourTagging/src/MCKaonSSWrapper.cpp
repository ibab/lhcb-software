#include "MCKaonSSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMCKaonSSSpace{
#include "NeuralNet/weights/kaonS__kaonSMLPBNN_MC.class.C"

}

MCKaonSSWrapper::MCKaonSSWrapper(std::vector<std::string> & names) {
	reader = new MyMCKaonSSSpace::Read_kaonSMLPBNN_MC(names);
}

MCKaonSSWrapper::~MCKaonSSWrapper() {delete reader; }


double MCKaonSSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
