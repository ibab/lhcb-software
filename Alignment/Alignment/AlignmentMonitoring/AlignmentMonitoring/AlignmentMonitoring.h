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

class TH2D;

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    typedef enum WarningLevel
    {
      FAILED=-1,
      UNCHECKED=0,
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
      void CheckITOverlaps();
      void CheckITOverlapResiduals();
      void CheckTTOverlapResiduals();
      void CheckTTResidualsInOverlapRegion();
      void CheckVeloTTandTMatch();
      void CheckVeloTTandTMatchCurvature();
      void CheckVeloTMatchKickPosition();
      void CheckD0();
      void Run();
      void CheckITSectorResiduals();
      void CheckTTSectorResiduals();
      
      // Verbosity
      void SetVerbosity(bool verb){m_verbose=verb;}
      
      // Fitters
      //  TFitResult GaussFit(TH1* h);
      void LoadGausFitReferences(const char* fName="");
      std::vector<double> GetReference(const char* hname);
      void PrintWarnings();
      WarningLevel CheckFitPar(double x, double xerr, double xmin, double xmax);
      WarningLevel CheckFitPar(double x, double xmin, double xmax);
      
      // Vars
    private:
      std::pair<double,double> ITMapping(int uniqueSector);
      void PlotITLabels(TH2D* hist);
      void PlotITBoxes(TH2D* hist, std::vector<std::string> hNames, std::map< int, std::pair<double,double> > m_mapping);
      std::pair<double,double> TTMapping(int uniqueSector);
      int TTNumberOfSensors(int uniqueSector);
      void PlotTTLabels(TH2D* hist, std::vector<std::string> hNames, std::map< int, std::pair<double,double> > m_mapping, std::map<int,int>);
      void PlotTTBoxes(TH2D* hist, std::vector<std::string> hNames, std::map< int, std::pair<double,double> > m_mapping, std::map<int,int>);

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
