#ifndef ALIGNMENT_ALIGNMENTMONITORING_MONITORINGPAGE_H
#define ALIGNMENT_ALIGNMENTMONITORING_MONITORINGPAGE_H 1
// STL
#include <vector>
// ROOT
#include "TCanvas.h"
#include "TString.h"

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
      MonitoringPage(const char* atitle,
		     std::vector<std::string>);
      ~MonitoringPage() {}

      void draw(const std::vector<TString>& filenames, TCanvas* canvas, bool normalize );
      TString GetName() const { return m_title ; }
      
    private:
      TString m_title ;
      std::vector<TString> m_h ;
      
    };
  } // AlignmentMonitoring 
} // Alignment
#endif
