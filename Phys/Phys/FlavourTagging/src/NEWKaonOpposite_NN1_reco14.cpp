#include "NEWKaonOpposite_NN1_reco14.h"

// hack, otherwise: redefinitions...
//
namespace MyMCSpace_reco14
{
#include "TMVAClassification/NN1_OSK_reco14.dat/TMVAClassification_MLPBNN.class.C"
}

MCReaderCompileWrapper_reco14::MCReaderCompileWrapper_reco14(std::vector<std::string> & names) :
	mcreader_reco14(new MyMCSpace_reco14::ReadMLPBNN(names))
{}

MCReaderCompileWrapper_reco14::~MCReaderCompileWrapper_reco14() {delete mcreader_reco14; }

double MCReaderCompileWrapper_reco14::GetMvaValue(std::vector<double> const & values)
{
	return mcreader_reco14->GetMvaValue(values);
}

