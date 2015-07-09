#ifndef COMPARECONSTANTS_H
#define COMPARECONSTANTS_H

// STL
#include <map>
#include <string>
#include <vector>
#include <cstdlib> 
// ROOT
#include "TH1.h"
// USR
#include "AlignmentMonitoring.h"

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    class CompareConstants
    {
    public:
      // Constructor
      CompareConstants (const char* system="IT/ITGlobal/", const char* ref="v8", const char* dir="files/alignment/");
      // Destructor
      ~CompareConstants () {}
      // Methods
      void Compare(const char* ver);
      WarningLevel CheckConstant(double delta, double d1, double d2);
      std::vector<double> GetLimits(const char* element);
      void FillDeltaHist(const char* element);
      void SetRanges(const char* filename="");
      void PrintWarnings();
      void SaveHists(const char* filename);
      
      // Verbosity
      void SetVerbosity(bool verb){m_verbose=verb;}
      
    private:
      bool m_verbose;
      std::string m_system;
      std::string m_ref;
      std::string m_dir;
      std::map<std::string, std::vector<double>> m_rngs;
      std::map<std::string, int> m_mapWarnings;
      std::map<std::string, double> m_constRef;
      std::map<std::string, double> m_constAlt;
      std::map<std::string, TH1D*> m_hists;
    };
    
  }// namespace AlignmentMonitoring
}// namespace Alignment

#endif
