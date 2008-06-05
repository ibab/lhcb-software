// $Id: ProcCandErrors.h,v 1.1 2008-06-05 08:20:28 jucogan Exp $
#ifndef PROCCANDERRORS_H 
#define PROCCANDERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class ProcCandErrors
  {
  
  public:
  
  
    ProcCandErrors();
    virtual ~ProcCandErrors();  
    
    bool inError();
    bool decodingError() const {return decoding.inError();};
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

    void printCounters(std::string &os, std::string tab="") const;
    int sumCounters() const ;

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
