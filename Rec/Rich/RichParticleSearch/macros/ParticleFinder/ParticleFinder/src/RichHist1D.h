#ifndef RICHHIST1_H
#define RICHHIST1_H

#include "RichHist.h"
#include "TH1D.h"
#include <string>

class RichHist1D : public RichHist
{
public:
	RichHist1D(std::string histName, std::string varName, int bins, double xlow, double xhigh);
	RichHist1D(const RichHist1D&);
	const RichHist1D& operator= (const RichHist1D& copy);
	~RichHist1D()
	{
		delete m_hist;
	}
	void Fill(std::map<std::string,double>& vars);
	void Write();
	std::string VariableName();

	
private:
	std::string m_varName;
	TH1D* m_hist;
	
};

#endif
