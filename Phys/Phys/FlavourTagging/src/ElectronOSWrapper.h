#ifndef ELECTRONOSWRAPPER_H 
#define ELECTRONOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyElectronOSSpace { class Read_eleMLPBNN; }

class ElectronOSWrapper : public TMVAWrapper {
public:
	ElectronOSWrapper(std::vector<std::string> &);
	virtual ~ElectronOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyElectronOSSpace::Read_eleMLPBNN * reader;

};

#endif
