#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace MyNN2Space { class ReadMLPBNN; }

struct NN2ReaderCompileWrapper{

	NN2ReaderCompileWrapper(std::vector<std::string> &);

	~NN2ReaderCompileWrapper();

	double GetMvaValue(std::vector<double> const &);

	private:
	MyNN2Space::ReadMLPBNN * nn2reader;

};
