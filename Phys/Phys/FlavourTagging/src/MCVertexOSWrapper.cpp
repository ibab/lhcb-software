#include "MCVertexOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyMCVertexOSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/vtx__vtxMLPBNN_MC.class.C"
#endif
}

MCVertexOSWrapper::MCVertexOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyMCVertexOSSpace::Read_vtxMLPBNN_MC(names);
#endif
}

MCVertexOSWrapper::~MCVertexOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double MCVertexOSWrapper::GetMvaValue(std::vector<double> const & values) {
#ifdef SKIP_TMVA
  int size = values.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
  return 0.0;
#else
  return reader->GetMvaValue(values);
#endif
  return 0.0;
  
}
