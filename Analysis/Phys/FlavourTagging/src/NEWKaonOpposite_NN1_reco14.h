#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace MyMCSpace_reco14 { class ReadMLPBNN; }

struct MCReaderCompileWrapper_reco14{

	MCReaderCompileWrapper_reco14(std::vector<std::string> &);

	~MCReaderCompileWrapper_reco14();

	double GetMvaValue(std::vector<double> const &);

	private:
	MyMCSpace_reco14::ReadMLPBNN * mcreader_reco14;

};
