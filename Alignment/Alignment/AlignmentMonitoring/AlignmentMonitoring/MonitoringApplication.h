#ifndef ALIGNMENT_ALIGNMENTMONITORING_MONITORINGAPPLICATION_H
#define ALIGNMENT_ALIGNMENTMONITORING_MONITORINGAPPLICATION_H 1

// ALIGNMENTMONITORING
#include "AlignmentMonitoring/MonitoringPage.h"
#include "AlignmentMonitoring/HistHelper.h"
// STL
#include <string>
#include <vector>
// ROOT
#include "TString.h"

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    class MonitoringApplication
    {
    public:
      // Constructor
      MonitoringApplication(const char *outputName="plots");
      // Destructor
      ~MonitoringApplication() {}
      
      // Methods
      void addPage( MonitoringPage* page) { m_pages.push_back(page) ; }
      void addFile( const TString& filenametag);
      void drawPage(TString pagename, bool normalize=false);
      void draw(bool print=false, bool normalize=false);
  
    private:
      std::string m_outputName;
      std::vector<MonitoringPage*> m_pages ;
      std::vector<TString> m_filenames ;
      std::vector<TString> m_tags ;
      HistHelper m_hhelp;
      
    } ;
  } // AlignmentMonitoring
} // Alignment

 #endif
