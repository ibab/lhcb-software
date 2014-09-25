#include "VertexOSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyVertexOSSpace{
#include "NeuralNet/weights/vtx__vtxMLPBNN.class.C"

}

VertexOSWrapper::VertexOSWrapper(std::vector<std::string> & names) {
	reader = new MyVertexOSSpace::Read_vtxMLPBNN(names);
}

VertexOSWrapper::~VertexOSWrapper() {delete reader; }


double VertexOSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
