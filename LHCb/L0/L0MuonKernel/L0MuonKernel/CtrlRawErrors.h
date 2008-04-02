// $Id: CtrlRawErrors.h,v 1.1 2008-04-02 12:45:49 jucogan Exp $
#ifndef INCLUDE_CTRLRAWERROR_H 
#define INCLUDE_CTRLRAWERROR_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  class CtrlRawErrors
  {
  
  public:
  
  
    CtrlRawErrors();
    virtual ~CtrlRawErrors();  
    
    bool inError();
    bool decodingError() const {return decoding.inError();};
    
    std::string counters(std::string tab="");

    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected on the controller board";
      h+="\n"+tab;
      h+=" Pos   L0EvtNum  L0_B_Id ";
      h+="  ---------------------------- BCIDs ----------------------------  --------- Status ---------";
      h+="\n"+tab;
      h+=" 0  1   0    1    0    1 ";
      h+=" SU CU err CU 0...<- from CU ->....12  SU 0...<- from SU ->....12  SU 0....................12";
      return h;
    } 

    void printCounters(std::string &os, std::string tab="") const;
    int sumCounters() const ;

    ErrorHandler decoding;
    ErrorHandler board_index[2];
    ErrorHandler l0EventNumber[2];  
    ErrorHandler l0_B_Id[2];  
    ErrorHandler su_bcid;
    ErrorHandler cu_bcid;
    ErrorHandler su_bcid_best;
    ErrorHandler cu_bcid_best;
    ErrorHandler err_bcid;
    ErrorHandler bcsus_bcid_su[12];
    ErrorHandler bcsus_bcid_cu[12];
    ErrorHandler su_status;
    ErrorHandler bcsus_status[12];
    ErrorHandler pb_link_su[12];
    ErrorHandler pb_link_cu[12];
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::CtrlRawErrors &pberr);

} // namespace L0Muon



#endif // INCLUDE_CTRLRAWERROR_H
