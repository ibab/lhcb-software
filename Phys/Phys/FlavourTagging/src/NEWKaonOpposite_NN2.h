#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace MyDataSpace { class ReadMLPBNN; }

struct DataReaderCompileWrapper{

	DataReaderCompileWrapper(std::vector<std::string> &);

	~DataReaderCompileWrapper();

	double GetMvaValue(std::vector<double> const &);

	private:
	MyDataSpace::ReadMLPBNN * datareader;

};
