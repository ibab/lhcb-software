#ifndef PRESENTER_H_
#define PRESENTER_H_

class TGString;
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
}

#endif /*PRESENTER_H_*/
