#include "RichHist2D.h"

using namespace std;

RichHist2D::RichHist2D(std::string histName, std::string x_varName, std::string y_varName,
		int xbins, double xlow, double xhigh, int ybins, double ylow, double yhigh)
{
	m_xvarName = x_varName;
	m_yvarName = y_varName;
	m_hist = new TH2D(histName.c_str(),histName.c_str(), xbins, xlow, xhigh, ybins, ylow, yhigh);
}

RichHist2D::RichHist2D(const RichHist2D& copy)
{
	m_xvarName = copy.m_xvarName;
	m_yvarName = copy.m_yvarName;
	m_hist = copy.m_hist;
}

const RichHist2D& RichHist2D::operator= (const RichHist2D& copy)
{
	if (this == &copy) return copy;
	m_xvarName = copy.m_xvarName;
	m_yvarName = copy.m_yvarName;
	m_hist = copy.m_hist;
	return *this;
}

void RichHist2D::Fill(map<string,double>& vars)
{
	m_hist->Fill(vars[m_xvarName],vars[m_yvarName]);
}


void RichHist2D::Write()
{
	m_hist->Write(m_hist->GetName());
}


