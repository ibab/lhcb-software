#ifndef MCELECTRONOSWRAPPER_H 
#define MCELECTRONOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMCElectronOSSpace { class Read_eleMLPBNN_MC; }

class MCElectronOSWrapper : public TMVAWrapper {
public:
	MCElectronOSWrapper(std::vector<std::string> &);
	virtual ~MCElectronOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMCElectronOSSpace::Read_eleMLPBNN_MC * reader;

};

#endif
