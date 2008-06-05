// $Id: ProcDataErrors.h,v 1.1 2008-06-05 08:20:28 jucogan Exp $
#ifndef PROCDATAERRORS_H 
#define PROCDATAERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class ProcDataErrors
  {
  
  public:
  
  
    ProcDataErrors();
    virtual ~ProcDataErrors();  
    
    bool inError();
    bool decodingError() const {return decoding.inError();};
    std::string counters(std::string tab="");

    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected in the L0MuonProcData bank";
      h+="\n"+tab;
      return h;
    } 

    void printCounters(std::string &os, std::string tab="") const;
    int sumCounters() const ;
    

    ErrorHandler decoding;
    ErrorHandler opt_link[4];
    ErrorHandler par_link[4];
    ErrorHandler ser_link[4];
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::ProcDataErrors &pberr);

} // namespace L0Muon



#endif // PROCDATAERRORS_H
