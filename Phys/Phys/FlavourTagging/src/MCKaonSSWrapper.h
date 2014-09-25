#ifndef MCKAONSSWRAPPER_H 
#define MCKAONSSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMCKaonSSSpace { class Read_kaonSMLPBNN_MC; }

class MCKaonSSWrapper : public TMVAWrapper {
public:
	MCKaonSSWrapper(std::vector<std::string> &);
	virtual ~MCKaonSSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMCKaonSSSpace::Read_kaonSMLPBNN_MC * reader;

};

#endif
