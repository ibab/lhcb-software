#ifndef MCKAONOSWRAPPER_H 
#define MCKAONOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMCKaonOSSpace { class Read_kaonMLPBNN_MC; }

class MCKaonOSWrapper : public TMVAWrapper {
public:
	MCKaonOSWrapper(std::vector<std::string> &);
	virtual ~MCKaonOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMCKaonOSSpace::Read_kaonMLPBNN_MC * reader;

};

#endif
