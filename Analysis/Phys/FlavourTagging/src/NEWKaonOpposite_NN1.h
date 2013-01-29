#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace MyMCSpace { class ReadMLPBNN; }

struct MCReaderCompileWrapper{

	MCReaderCompileWrapper(std::vector<std::string> &);

	~MCReaderCompileWrapper();

	double GetMvaValue(std::vector<double> const &);

	private:
	MyMCSpace::ReadMLPBNN * mcreader;

};
