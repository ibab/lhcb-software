#ifndef ALIGNMENT_ALIGNMENTMONITORING_ALIGNMENTMONITORING_H
#define ALIGNMENT_ALIGNMENTMONITORING_ALIGNMENTMONITORING_H

// USER
#include "HistHelper.h"
// STL
#include <map>
#include <string>
#include <vector>
#include <list>
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
      AlignmentMonitoring(const char* filename, const char* outputdir = ".");
      // Destructor
      ~AlignmentMonitoring() {}
      
      // Methods
      void CheckResiduals();
      void CheckOverlaps();
      void CheckITOverlapResiduals();
      void CheckTTOverlapResiduals();
      void CheckTTResidualsInOverlapRegion();
      void CheckVeloTTandTMatch();
      void CheckVeloTTandTMatchCurvature();
      void CheckVeloTMatchKickPosition();
      void CheckD0();
      void Run();
      
      // Verbosity
      void SetVerbosity(bool verb){m_verbose=verb;}
      
      // Fitters
      //  TFitResult GaussFit(TH1* h);
      void LoadGausFitReferences(const char* fName="");
      void PrintWarnings();
      WarningLevel CheckFitPar(double x, double xerr, double xmin, double xmax);
      WarningLevel CheckFitPar(double x, double xmin, double xmax);
      
      // Vars
    private:
      std::string m_inputFileName;
      std::string m_outputDirectory;
      std::map<std::string, int> m_mapWarnings;
      std::list<std::string> m_insertOrder;
      std::map<std::string, std::vector<double>> m_mapGausFitReference;
      HistHelper m_hhelp;
      bool m_verbose;
      std::map<std::string, TCanvas*> m_pages;
    };
    
  }// namespace AlignmentMonitoring
}// namespace Alignment

#endif
