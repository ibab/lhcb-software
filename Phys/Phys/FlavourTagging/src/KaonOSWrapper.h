#ifndef KAONOSWRAPPER_H 
#define KAONOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyKaonOSSpace { class Read_kaonMLPBNN; }

class KaonOSWrapper : public TMVAWrapper {
public:
	KaonOSWrapper(std::vector<std::string> &);
	virtual ~KaonOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyKaonOSSpace::Read_kaonMLPBNN * reader;

};

#endif
