#include "RichHist1D.h"
#include <iostream>


using namespace std;

RichHist1D::RichHist1D(std::string histName, std::string varName, int bins, double xlow, double xhigh)
{
	m_varName = varName;
	m_hist = new TH1D(histName.c_str(),histName.c_str(),bins,xlow,xhigh);
}

RichHist1D::RichHist1D(const RichHist1D& copy)
{
	m_varName = copy.m_varName;
	m_hist = copy.m_hist;
}

const RichHist1D& RichHist1D::operator= (const RichHist1D& copy)
{
	if (this == &copy) return copy;
	m_varName = copy.m_varName;
	m_hist = copy.m_hist;
	return *this;
}

void RichHist1D::Fill(map<string,double>& vars)
{
	m_hist->Fill(vars[VariableName()]);
}


void RichHist1D::Write()
{
	m_hist->Write(m_hist->GetName());
}

std::string RichHist1D::VariableName()
{
	return m_varName;
}
