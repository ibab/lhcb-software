// $Id: ProcCandErrors.h,v 1.3 2008-11-07 16:23:39 jucogan Exp $
#ifndef PROCCANDERRORS_H 
#define PROCCANDERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  /** @class ProcCandErrors ProcCandErrors.h L0MuonKernel/ProcCandErrors.h

      Class holding the error related to the L0MuonProcCand bank.

      It deals with the error encountered in one processing board, i.e. :
      - discrepancies observed while removing the redundant information in the BCSU 
      (see the processing board specifications; EDMS 339143)
      - discrepancies observed while removing the redundant information in the TELL1
      (see the L0Muon TELL1 board specifications ; EDMS 818559)
      The error handlers of this class are filled in ProcCandCnv::decodeBank. 
      - status bits reported by the PUs and the BCSU of the board
      - inconsistent bcid reported by the PUs and the BCSU of the board

      @author Julien Cogan
      @date 2008 June the 5th
  */
  class ProcCandErrors
  {
  
  public:
  
  
    ProcCandErrors();
    virtual ~ProcCandErrors();  
    
    //std::string counters(std::string tab="");

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
