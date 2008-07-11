// $Id: ProcDataErrors.h,v 1.2 2008-07-11 15:30:48 jucogan Exp $
#ifndef PROCDATAERRORS_H 
#define PROCDATAERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class ProcDataErrors
  {
  
  public:
  
    ProcDataErrors();
    virtual ~ProcDataErrors();  
    
    std::string counters(std::string tab="");

    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected in the L0MuonProcData bank";
      h+="\n"+tab;
      return h;
    } 

    const int sumCounters() const ;
    
    const bool inError(int ipu) const;
    const int decodingError() const;
    const int hardwareError(int ipu) const;
    
    ErrorHandler present;
    ErrorHandler decoding;
    ErrorHandler opt_link[4];
    ErrorHandler par_link[4];
    ErrorHandler ser_link[4];
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::ProcDataErrors &pberr);

} // namespace L0Muon



#endif // PROCDATAERRORS_H
