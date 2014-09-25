#include "MCPionSSWrapper.h"

// hack, otherwise: redefinitions...
//
namespace MyMCPionSSSpace{
#include "NeuralNet/weights/pionS__pionSMLPBNN_MC.class.C"

}

MCPionSSWrapper::MCPionSSWrapper(std::vector<std::string> & names) {
	reader = new MyMCPionSSSpace::Read_pionSMLPBNN_MC(names);
}

MCPionSSWrapper::~MCPionSSWrapper() {delete reader; }


double MCPionSSWrapper::GetMvaValue(std::vector<double> const & values) {
  return reader->GetMvaValue(values);
}
