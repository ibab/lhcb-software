#include "MCVertexOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMCVertexOSSpace{
#include "NeuralNet/weights/vtx__vtxMLPBNN_MC.class.C"

}

MCVertexOSWrapper::MCVertexOSWrapper(std::vector<std::string> & names) {
	reader = new MyMCVertexOSSpace::Read_vtxMLPBNN_MC(names);
}

MCVertexOSWrapper::~MCVertexOSWrapper() {delete reader; }


double MCVertexOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
