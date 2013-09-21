#include "NEWKaonOpposite_NN2_reco14.h"

// hack, otherwise: redefinitions...
//
namespace MyDataSpace_reco14
{
#include "TMVAClassification/NN2_OSK_reco14.dat/TMVAClassification_MLPBNN.class.C"
}

DataReaderCompileWrapper_reco14::DataReaderCompileWrapper_reco14(std::vector<std::string> & names) :
	datareader_reco14(new MyDataSpace_reco14::ReadMLPBNN(names))
{ }

DataReaderCompileWrapper_reco14::~DataReaderCompileWrapper_reco14() {delete datareader_reco14; }

double DataReaderCompileWrapper_reco14::GetMvaValue(std::vector<double> const & values)
{
	return datareader_reco14->GetMvaValue(values);
}
