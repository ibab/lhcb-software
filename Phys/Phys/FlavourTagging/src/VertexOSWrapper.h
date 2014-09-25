#ifndef VERTEXOSWRAPPER_H 
#define VERTEXOSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyVertexOSSpace { class Read_vtxMLPBNN; }

class VertexOSWrapper : public TMVAWrapper {
public:
	VertexOSWrapper(std::vector<std::string> &);
	virtual ~VertexOSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyVertexOSSpace::Read_vtxMLPBNN * reader;

};

#endif
