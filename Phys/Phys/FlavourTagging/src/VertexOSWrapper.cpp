#include "VertexOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyVertexOSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/vtx__vtxMLPBNN.class.C"
#endif
}

VertexOSWrapper::VertexOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyVertexOSSpace::Read_vtxMLPBNN(names);
#endif
}

VertexOSWrapper::~VertexOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double VertexOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
