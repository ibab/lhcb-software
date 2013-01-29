#include "NEWKaonOpposite_NN2.h"

// hack, otherwise: redefinitions...
//
namespace MyDataSpace {
#include "TMVAClassification/NN2_data_cut_04_NN1.dat/TMVAClassification_MLPBNN.class.C"
}
//
//
// to get faster to the errors... replacing the struct
//namespace MyDataSpace {
//	struct ReadMLPBNN {
//		double GetMvaValue(std::vector<double> const & ) { return 0.0; }
//	};
//}


DataReaderCompileWrapper::DataReaderCompileWrapper(std::vector<std::string> & names) :
	datareader(new MyDataSpace::ReadMLPBNN(names))
{}

DataReaderCompileWrapper::~DataReaderCompileWrapper() {delete datareader; }

double DataReaderCompileWrapper::GetMvaValue(std::vector<double> const & values) {
	return datareader->GetMvaValue(values);
}

