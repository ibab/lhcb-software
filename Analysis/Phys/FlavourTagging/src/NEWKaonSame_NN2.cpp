#include "NEWKaonSame_NN2.h"

// hack, otherwise: redefinitions...
//
namespace MyNN2Space 
{
#include "TMVAClassification/NN2_SSK.dat/TMVAClassification_MLPBNN.class.C"
}


NN2ReaderCompileWrapper::NN2ReaderCompileWrapper(std::vector<std::string> & names) :
	nn2reader(new MyNN2Space::ReadMLPBNN(names))
{}

NN2ReaderCompileWrapper::~NN2ReaderCompileWrapper() {delete nn2reader; }

double NN2ReaderCompileWrapper::GetMvaValue(std::vector<double> const & values) 
{
	return nn2reader->GetMvaValue(values);
}
