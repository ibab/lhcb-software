#ifndef MCVERTEXOSWRAPPER_H 
#define MCVERTEXOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMCVertexOSSpace { class Read_vtxMLPBNN_MC; }

class MCVertexOSWrapper : public TMVAWrapper {
public:
	MCVertexOSWrapper(std::vector<std::string> &);
	virtual ~MCVertexOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMCVertexOSSpace::Read_vtxMLPBNN_MC * reader;

};

#endif
