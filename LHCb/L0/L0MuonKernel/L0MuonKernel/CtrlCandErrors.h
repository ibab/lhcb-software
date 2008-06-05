// $Id: CtrlCandErrors.h,v 1.1 2008-06-05 08:20:28 jucogan Exp $
#ifndef CTRLCANDERRORS_H 
#define CTRLCANDERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class CtrlCandErrors
  {
  
  public:
  
  
    CtrlCandErrors();
    virtual ~CtrlCandErrors();  
    
    bool inError();
    bool decodingError() const {return decoding.inError();};
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

    void printCounters(std::string &os, std::string tab="") const;
    int sumCounters() const ;

    ErrorHandler decoding;
    ErrorHandler l0EventNumber;  
    ErrorHandler l0_B_Id;  
    ErrorHandler ctrl_bcid[2];
    ErrorHandler best_bcid;
    ErrorHandler bcsu_bcid[12];
    ErrorHandler best_status;
    ErrorHandler bcsu_status[12];
    ErrorHandler errJ,errK;
    ErrorHandler link[12][2];
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::CtrlCandErrors &pberr);

} // namespace L0Muon



#endif // CTRLCANDERRORS_H
