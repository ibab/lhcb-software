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

  // Conventions (see HistogramIdentifier for parsing)
  static const std::string H1D("H1D");
  static const std::string H2D("H2D");
  static const std::string P1D("P1D");
  static const std::string HPD("HPD"); // legacy compat == P1D
  static const std::string P2D("P2D");
  static const std::string CNT("CNT");
  static const std::string PAGE("PAGE");
  static const std::string TASK("TASK");
  static const std::string ALGORITHM("ALGORITHM");
  static const std::string SET("SET");
  static const std::string LEVEL("LEVEL");

  // Tunables:
  static const int EstimatedDimServiceCount = 1000;
  static const int EstimatedHistosOnPage = 200;
  static const int EstimatedDbHistoCount = 5000;
  static const int EstimatedDbFolderCount = 500;
  static const int EstimatedDbPageCount = 500;
  static const int TimeoutOfMainTimer = 2000;
}
#endif /*PRESENTER_H_*/
