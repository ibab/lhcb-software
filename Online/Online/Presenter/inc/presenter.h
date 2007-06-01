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
    NEVER = 0,
    SEVERE = 1,
    ALL = 2    
  };
}

#endif /*PRESENTER_H_*/
