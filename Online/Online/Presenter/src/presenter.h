#ifndef PRESENTER_H_
#define PRESENTER_H_

// slots/signals cint hates namespaces
enum FilterCriteria
{
  ByFolderAndPage = 0,
  ByTask = 1,
  BySubsystem = 2,
  ByHistogramsWithAnalysis = 3,
  ByAnalysisHistograms = 4,
  AllHistos = 5
};

namespace pres
{  
  enum MsgLevel
  { 
    Silent  = 0,
    Verbose = 1,
    Debug   = 2           
  };

  enum ExceptionLevel {
    NoExceptions     = 0,
    SevereExceptions = 1,
    AllExceptions    = 2    
  };

  enum MouseButton {
    LeftMouseButton = 1,
    MiddleMouseButton = 2,
    RightMouseButton = 3           
  };

  static const bool WithHistograms    = true;
  static const bool WithoutHistograms = false;
  
  static const bool CheckTreeItems    = true;
  static const bool UncheckTreeItems  = false;
  
  static const std::string H1D = "H1D";
  static const std::string H2D = "H2D";
  static const std::string P1D = "P1D";
  static const std::string HPD = "HPD"; // legacy compat == P1D
  static const std::string P2D = "P2D";
  static const std::string CNT = "CNT";
  
}
#endif /*PRESENTER_H_*/
