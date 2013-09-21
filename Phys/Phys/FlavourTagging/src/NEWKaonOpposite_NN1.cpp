#include "NEWKaonOpposite_NN1.h"

// hack, otherwise: redefinitions...
//
namespace MyMCSpace 
{
#include "TMVAClassification/NN1_OSK.dat/TMVAClassification_MLPBNN.class.C"
}

MCReaderCompileWrapper::MCReaderCompileWrapper(std::vector<std::string> & names) :
	mcreader(new MyMCSpace::ReadMLPBNN(names))
{}

MCReaderCompileWrapper::~MCReaderCompileWrapper() {delete mcreader; }

double MCReaderCompileWrapper::GetMvaValue(std::vector<double> const & values)
{
	return mcreader->GetMvaValue(values);
}

