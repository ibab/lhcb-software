#include "NEWKaonOpposite_NN2.h"

// hack, otherwise: redefinitions...
//
namespace MyDataSpace
{
#include "TMVAClassification/NN2_data_cut_04_NN1.dat/TMVAClassification_MLPBNN.class.C"
}

DataReaderCompileWrapper::DataReaderCompileWrapper(std::vector<std::string> & names) :
	datareader(new MyDataSpace::ReadMLPBNN(names))
{ }

DataReaderCompileWrapper::~DataReaderCompileWrapper() {delete datareader; }

double DataReaderCompileWrapper::GetMvaValue(std::vector<double> const & values)
{
	return datareader->GetMvaValue(values);
}
