#ifndef ALIGNMENT_ALIGNMENTMONITORING_MONITORINGPAGE_H
#define ALIGNMENT_ALIGNMENTMONITORING_MONITORINGPAGE_H 1
// STL
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
// ROOT
#include "TCanvas.h"
#include "TString.h"
#include "TLine.h"

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    class MonitoringPage
    {
    public:
      MonitoringPage(const char* atitle,
		     const char* h1=0,
		     const char* h2=0,
		     const char* h3=0,
		     const char* h4=0,
		     const char* h5=0,
		     const char* h6=0,
		     const char* h7=0,
		     const char* h8=0,
		     const char* h9=0);
      MonitoringPage(const char* atitle, std::vector<std::string>);
      ~MonitoringPage() {}

      void draw(const std::vector<TString>& filenames, TCanvas* canvas, bool normalize );
      void drawMonitorStyle(const std::vector<TString>& filenames, TCanvas* canvas, bool normalize );
      TString GetName() const { return m_title ; }
      void setTitle(char* hname, char* htitle);
      void setVLine(char* hname, std::vector<double> vlines);
      void setHLine(char* hname, std::vector<double> hlines);
      void setReferenceLine(char* hname, char* axis, double value);
      void setMLine(char* hname, char* axis);
      void setXRange(char* hname, double xmin, double xhigh);
      void setYRange(char* hname, double ymin, double yhigh);
      void setLines(char* hname, std::vector<TLine*> lines);
      
    private:
      TString m_title ;
      std::vector<TString> m_h ;
      std::map<std::string, char*> m_titles;
      std::map<std::string, std::vector<double> > m_vlines;
      std::map<std::string, std::vector<double> > m_hlines;
      std::map<std::string, std::pair<std::string,double> > m_reflines;
      std::map<std::string, std::string> m_mlines;
      std::map<std::string, std::pair<double,double> > m_xranges;
      std::map<std::string, std::pair<double,double> > m_yranges;
      std::map<std::string, std::vector<TLine*> > m_lines;
      
    };
  } // AlignmentMonitoring 
} // Alignment
#endif
