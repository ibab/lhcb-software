#ifndef PRESENTER_H_
#define PRESENTER_H_

class TGString;

// slots/signals cint hates namespaces
enum FilterCriteria
    {
      byFolderAndPage,
      bySubsystem,
      byTask,
      byHistogramsWithAnalysis,
      allHistos,
      byAnalysisHistograms
    };

namespace pres
{  
  enum MsgLevel
  { 
    SILENT = 0,
    VERBOSE = 1,
    DEBUG = 2           
  };
  
  enum ExceptionLevel {
    NO_EXCEPTIONS = 0,
    SEVERE_EXCEPTIONS = 1,
    ALL_EXCEPTIONS = 2    
  };
  
  static const bool withHistograms = true;
  static const bool withoutHistograms = false;
  
}

#endif /*PRESENTER_H_*/
