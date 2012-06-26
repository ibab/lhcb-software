#ifndef RICHHIST_H
#define RICHHIST_H

#include <string>
#include <map>
#include "IRichHist.h"

class RichHist : public IRichHist
{
public:
	virtual void Fill(std::map<std::string,double>&) = 0;
	virtual void Write() = 0;
};



#endif