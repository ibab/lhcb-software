#include "NEWKaonSame_NN1.h"

// hack, otherwise: redefinitions...
//
namespace MyNN1Space 
{
#include "TMVAClassification/NN1_SSK.dat/TMVAClassification_MLPBNN.class.C"
}


NN1ReaderCompileWrapper::NN1ReaderCompileWrapper(std::vector<std::string> & names) :
	nn1reader(new MyNN1Space::ReadMLPBNN(names))
{}

NN1ReaderCompileWrapper::~NN1ReaderCompileWrapper() {delete nn1reader; }

double NN1ReaderCompileWrapper::GetMvaValue(std::vector<double> const & values) 
{
	return nn1reader->GetMvaValue(values);
}

