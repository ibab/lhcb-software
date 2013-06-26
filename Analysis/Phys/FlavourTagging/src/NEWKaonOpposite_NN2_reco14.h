#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace MyDataSpace_reco14 { class ReadMLPBNN; }

struct DataReaderCompileWrapper_reco14{

	DataReaderCompileWrapper_reco14(std::vector<std::string> &);

	~DataReaderCompileWrapper_reco14();

	double GetMvaValue(std::vector<double> const &);

	private:
	MyDataSpace_reco14::ReadMLPBNN * datareader_reco14;

};
