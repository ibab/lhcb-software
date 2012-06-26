#ifndef IRICHHIST_H
#define IRICHHIST_H

#include <string>
#include <map>
class IRichHist
{
public:
	virtual void Fill(std::map<std::string,double>&) = 0;
	virtual void Write() = 0;
};



#endif
