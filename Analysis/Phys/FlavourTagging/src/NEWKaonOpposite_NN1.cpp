#include "NEWKaonOpposite_NN1.h"

// hack, otherwise: redefinitions...
//
namespace MyMCSpace {
#include "TMVAClassification/NN1_MC.dat/TMVAClassification_MLPBNN.class.C"
}
//
//
// to get faster to the errors... replacing the struct
//namespace MyMCSpace {
//	struct ReadMLPBNN {
//		double GetMvaValue(std::vector<double> const & ) { return 0.0; }
//	};
//}


MCReaderCompileWrapper::MCReaderCompileWrapper(std::vector<std::string> & names) :
	mcreader(new MyMCSpace::ReadMLPBNN(names))
{}

MCReaderCompileWrapper::~MCReaderCompileWrapper() {delete mcreader; }

double MCReaderCompileWrapper::GetMvaValue(std::vector<double> const & values) {
	return mcreader->GetMvaValue(values);
}

