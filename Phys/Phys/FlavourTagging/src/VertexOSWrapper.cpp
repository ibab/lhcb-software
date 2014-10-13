#include "VertexOSWrapper.h"



// hack, otherwise: redefinitions...
//
namespace MyVertexOSSpace{
#ifndef SKIP_TMVA
#include "NeuralNet/weights/vtx__vtxMLPBNN.class.C"
#endif
}

VertexOSWrapper::VertexOSWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  reader = new MyVertexOSSpace::Read_vtxMLPBNN(names);
#endif
}

VertexOSWrapper::~VertexOSWrapper() {
#ifndef SKIP_TMVA
  delete reader;
#endif
}


double VertexOSWrapper::GetMvaValue(std::vector<double> const & values) {
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
