#ifndef ALIGNMENTMONITORING_H
#define ALIGNMENTMONITORING_H

// USER
#include "AlignmentMonitoringHistHelper.h"
// STL
#include <map>
#include <string>
#include <vector>
// ROOT
#include "TCanvas.h"
#include "TFitResult.h"

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    typedef enum WarningLevel
    {
      OK=1,
      WARNING=2,
      SEVERE=3
    } WarningLevel;
    
    class AlignmentMonitoring
    {
    public:
      // Constructors
      AlignmentMonitoring(const char* fName);
      // Destructor
      ~AlignmentMonitoring() {}
      
      // Methods
      void CheckResiduals();
      void CheckOverlaps();
      void Run();
      
      // Verbosity
      void SetVerbosity(bool verb){m_verbose=verb;}
      
      // Fitters
      //  TFitResult GaussFit(TH1* h);
      void LoadGausFitReferences(const char* fName);
      void PrintWarnings();
      WarningLevel CheckFitPar(double x, double xerr, double xmin, double xmax);
      
      // Vars
    private:
      std::string m_inputFileName;
      std::map<std::string, int> m_mapWarnings;
      std::map<std::string, std::vector<double>> m_mapGausFitReference;
      AlignmentMonitoringHistHelper m_hhelp;
      bool m_verbose;
      std::map<std::string, TCanvas*> m_pages;
    };
    
  }// namespace AlignmentMonitoring
}// namespace Alignment

#endif
