#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace MyNN1Space { class ReadMLPBNN; }

struct NN1ReaderCompileWrapper{

	NN1ReaderCompileWrapper(std::vector<std::string> &);

	~NN1ReaderCompileWrapper();

	double GetMvaValue(std::vector<double> const &);

	private:
	MyNN1Space::ReadMLPBNN * nn1reader;

};
