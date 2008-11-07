// $Id: ProcRawErrors.h,v 1.2 2008-11-07 16:23:39 jucogan Exp $
#ifndef PROCRAWERROR_H 
#define PROCRAWERROR_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  /** @class ProcRawErrors ProcRawErrors.h L0MuonKernel/ProcRawErrors.h

      Class holding the error related to the L0MuonRaw bank (written by the controller boards).

      The error handlers of this class are filled in ProcRawCnv::decodeBank.

      @author Julien Cogan
      @date 2008 June the 5th
  */
  class ProcRawErrors
  {
  
  public:
  
    /// Constructor.
    ProcRawErrors();
    
    /// Destructor.
    virtual ~ProcRawErrors();  
    
    bool inError();
    bool decodingError() const {return decoding.inError();};
    //std::string counters(std::string tab="");

    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected on the processing boards";
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
    ErrorHandler board_index[2];
    ErrorHandler l0EventNumber[2];  
    ErrorHandler l0_B_Id[2];  
    ErrorHandler bcsu_bcid;
    ErrorHandler pus_bcid[4];
    ErrorHandler bcsu_status;
    ErrorHandler pus_status[4];
    ErrorHandler add_link[4];
    ErrorHandler opt_link[4];
    ErrorHandler par_link[4];
    ErrorHandler ser_link[4];
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::ProcRawErrors &pberr);

} // namespace L0Muon



#endif // PROCRAWERROR_H
