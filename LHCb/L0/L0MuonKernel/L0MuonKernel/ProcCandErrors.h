// $Id: ProcCandErrors.h,v 1.2 2008-07-11 15:30:48 jucogan Exp $
#ifndef PROCCANDERRORS_H 
#define PROCCANDERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class ProcCandErrors
  {
  
  public:
  
  
    ProcCandErrors();
    virtual ~ProcCandErrors();  
    
    std::string counters(std::string tab="");

    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected in the L0MuonProcCand Bank";
      h+="\n"+tab;
      h+=" Pos   L0EvtNum  L0_B_Id     ";
      h+="-- BCIDs -  ";
      h+="  - Status -";
      h+="\n"+tab;
      h+=" 0  1   0    1    0    1   ";
      h+="BCSU 0.....4  ";
      h+="BCSU 0.....4";
      return h;
    } 

    //void printCounters(std::string &os, std::string tab="") const;
    const int sumCounters() const ;

    const bool inError() const;
    const bool inError(int ipu) const ;
    const int decodingError() const ;
    const int hardwareError() const ;
    const int bcidError() const ;
    const int bcidError(int ipu) const ;
    const int statusError() const ;
    const int statusError(int ipu) const;
    
    ErrorHandler present;
    ErrorHandler decoding;
    ErrorHandler l0EventNumber;  
    ErrorHandler l0_B_Id;  
    ErrorHandler bcsu_bcid;
    ErrorHandler pus_bcid[4];
    ErrorHandler bcsu_status;
    ErrorHandler pus_status[4];
    ErrorHandler errJ,errK,errH,errF;
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::ProcCandErrors &pberr);

} // namespace L0Muon



#endif // PROCCANDERRORS_H
