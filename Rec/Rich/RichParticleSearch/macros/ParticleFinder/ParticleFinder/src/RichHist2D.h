#ifndef RICHHIST2D_H
#define RICHHIST2D_H

#include "RichHist.h"
#include "TH2D.h"
#include <string>
#include <map>
class RichHist2D : public RichHist
{
public:
	RichHist2D(std::string histName, std::string x_varName, std::string y_varName,
				int xbins, double xlow, double xhigh, int ybins, double ylow, double yhigh);
	
	RichHist2D(const RichHist2D&);
	const RichHist2D& operator= (const RichHist2D& copy);
	~RichHist2D()
	{
		delete m_hist;
	}
	void Fill(std::map<std::string,double>& vars);
	void Write();
	std::string VariableName();
	

private:
	std::string m_xvarName;
	std::string m_yvarName;
	TH2D* m_hist;
};

#endif
