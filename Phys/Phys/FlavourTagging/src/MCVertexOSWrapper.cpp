#include "MCVertexOSWrapper.h"

#include "compileSettings.h"

// hack, otherwise: redefinitions...
//
namespace MyMCVertexOSSpace{
#ifndef SKIP_TMVA_COMPILE
#include "NeuralNet/weights/vtx__vtxMLPBNN_MC.class.C"
#endif
}

MCVertexOSWrapper::MCVertexOSWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  reader = new MyMCVertexOSSpace::Read_vtxMLPBNN_MC(names);
#endif
}

MCVertexOSWrapper::~MCVertexOSWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete reader;
#endif
}


double MCVertexOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
