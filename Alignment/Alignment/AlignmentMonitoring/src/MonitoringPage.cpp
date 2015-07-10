#include "AlignmentMonitoring/MonitoringPage.h"
#include "AlignmentMonitoring/HistHelper.h"
// STL
#include <iostream>

using namespace Alignment::AlignmentMonitoring;

MonitoringPage::MonitoringPage(const char* atitle,
			       const char* h1,
			       const char* h2,
			       const char* h3,
			       const char* h4,
			       const char* h5,
			       const char* h6,
			       const char* h7,
			       const char* h8,
			       const char* h9)
  : m_title(atitle)
{
  // should use a 'variadic' function here
  if(h1) m_h.push_back(h1) ;
  if(h2) m_h.push_back(h2) ;
  if(h3) m_h.push_back(h3) ;
  if(h4) m_h.push_back(h4) ;
  if(h5) m_h.push_back(h5) ;
  if(h6) m_h.push_back(h6) ;
  if(h7) m_h.push_back(h7) ;
  if(h8) m_h.push_back(h8) ;
  if(h9) m_h.push_back(h9) ;
}

MonitoringPage::MonitoringPage(const char* atitle,
			       std::vector<std::string> hnames)
  : m_title(atitle)
{
  if (hnames.size() < 9)
    for (auto hname : hnames) m_h.push_back(hname);
  else
    std::cout << "This software can handle at most 9 histograms on the same page! Do nothing." << std::endl;
}

void
MonitoringPage::draw(const std::vector<TString>& filenames, TCanvas* canvas, bool normalize )
{
  HistHelper hh;
  size_t ny = 2 ;
  size_t nx = std::max(int(m_h.size()/ny),2) ;
  if( nx*ny < m_h.size()) ++nx ;
  TCanvas* c1 = canvas ? canvas : hh.getCanvas(m_title,m_title) ;
  hh.createPage(c1,m_title,nx,ny) ;
  for( size_t i=0; i< m_h.size() ; ++i) {
    c1->cd(i+1) ;
    hh.drawH1(filenames,m_h[i],"",normalize) ;
  }
  return;
}
