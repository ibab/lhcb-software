#ifndef PRESENTER_H_
#define PRESENTER_H_

// slots/signals cint hates namespaces
enum FilterCriteria
{
  byFolderAndPage = 0,
  byTask = 1,
  bySubsystem = 2,
  byHistogramsWithAnalysis = 3,
  byAnalysisHistograms = 4,
  allHistos = 5
};

namespace pres
{  
  enum MsgLevel
  { 
    SILENT  = 0,
    VERBOSE = 1,
    DEBUG   = 2           
  };
  
  enum ExceptionLevel {
    NO_EXCEPTIONS     = 0,
    SEVERE_EXCEPTIONS = 1,
    ALL_EXCEPTIONS    = 2    
  };
  
  enum MouseButton {
    LEFT = 1, 
    RIGHT = 3           
  };
  
  static const bool withHistograms    = true;
  static const bool withoutHistograms = false;

  enum HistoServiceType {
    Unknown,
    H1D,
    H2D,
    P1D,
    P2D,
    CNT
  };

}
#endif /*PRESENTER_H_*/
