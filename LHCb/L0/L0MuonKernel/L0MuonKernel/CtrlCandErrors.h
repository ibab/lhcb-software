// $Id: CtrlCandErrors.h,v 1.2 2008-07-11 15:30:48 jucogan Exp $
#ifndef CTRLCANDERRORS_H 
#define CTRLCANDERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class CtrlCandErrors
  {
  
  public:
  
  
    CtrlCandErrors();
    virtual ~CtrlCandErrors();  

    std::string counters(std::string tab="");

    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected in the L0MuonCtrlCand Bank";
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

    const int sumCounters() const ;

    const bool inError() const;
    const bool inError(int ib) const ;
    const int decodingError() const;
    const int hardwareError() const;
    const int hardwareError(int ib) const;
    const int bcidError() const ;
    const int bcidError(int ib) const;
    const int statusError()const ;
    const int statusError(int ib) const;
    
    ErrorHandler present;
    ErrorHandler decoding;
    ErrorHandler l0EventNumber;  
    ErrorHandler l0_B_Id;  
    ErrorHandler ctrl_bcid[2];
    ErrorHandler best_bcid;
    ErrorHandler bcsu_bcid[12];
    ErrorHandler bcsu_bcid_mismatch[12];
    ErrorHandler best_status;
    ErrorHandler bcsu_status[12];
    ErrorHandler errJ,errK;
    ErrorHandler link[12][2];
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::CtrlCandErrors &pberr);

} // namespace L0Muon



#endif // CTRLCANDERRORS_H
