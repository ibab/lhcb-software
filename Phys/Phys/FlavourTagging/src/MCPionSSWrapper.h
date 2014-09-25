#ifndef MCPIONSSWRAPPER_H 
#define MCPIONSSWRAPPER_H 1

#include "TMVAWrapper.h"

namespace MyMCPionSSSpace { class Read_pionSMLPBNN_MC; }

class MCPionSSWrapper : public TMVAWrapper {
public:
	MCPionSSWrapper(std::vector<std::string> &);
	virtual ~MCPionSSWrapper();
	double GetMvaValue(std::vector<double> const &);
  
private:
	MyMCPionSSSpace::Read_pionSMLPBNN_MC * reader;

};

#endif
