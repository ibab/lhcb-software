#ifndef MCMUONOSWRAPPER_H 
#define MCMUONOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMCMuonOSSpace { class Read_muonMLPBNN_MC; }

class MCMuonOSWrapper : public TMVAWrapper {
public:
	MCMuonOSWrapper(std::vector<std::string> &);
	virtual ~MCMuonOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMCMuonOSSpace::Read_muonMLPBNN_MC * reader;

};

#endif
