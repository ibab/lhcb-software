#ifndef CHARMOSWRAPPER_H 
#define CHARMOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyCharmOSSpace { class Read_charmMLPBNN; }

class CharmOSWrapper : public TMVAWrapper {
public:
	CharmOSWrapper(std::vector<std::string> &);
	virtual ~CharmOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyCharmOSSpace::Read_charmMLPBNN * reader;

};

#endif
