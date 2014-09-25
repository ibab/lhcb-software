#ifndef MUONOSWRAPPER_H 
#define MUONOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMuonOSSpace { class Read_muonMLPBNN; }

class MuonOSWrapper : public TMVAWrapper {
public:
	MuonOSWrapper(std::vector<std::string> &);
	virtual ~MuonOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMuonOSSpace::Read_muonMLPBNN * reader;

};

#endif
