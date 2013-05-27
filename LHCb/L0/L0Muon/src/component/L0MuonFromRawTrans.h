// $Id: L0MuonFromRawTrans.h,v 1.6 2008-09-04 08:57:02 jucogan Exp $
#ifndef COMPONENT_L0MUONFROMRAWTRANS_H 
#define COMPONENT_L0MUONFROMRAWTRANS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/L0MuonCandidate.h"

#include "L0MuonKernel/ProcRawCnv.h"
#include "L0MuonKernel/CtrlRawCnv.h"
#include "L0MuonKernel/MuonCandidate.h"


/** @class L0MuonFromRawTrans L0MuonFromRawTrans.h component/L0MuonFromRawTrans.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-01-10
 */


class L0MuonFromRawTrans : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0MuonFromRawTrans( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonFromRawTrans( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /// Decode the L0Muon banks and fill the registers of the converters
  StatusCode decodeRawBanks();
  /// From the registers of the converters, fill the L0MuonCandidates
  /// and L0MuonData containers on the TES
  StatusCode writeOnTES();
  /// Reset the registers used by the converters
  StatusCode releaseRegisters();
  /// Print out errors
  StatusCode dumpErrors();

private:
  

  static inline std::string timeSlot(int bx)
  {
    std::string ts;
    switch (bx) {
    case -7 : return "Prev7/";
    case -6 : return "Prev6/";
    case -5 : return "Prev5/";
    case -4 : return "Prev4/";
    case -3 : return "Prev3/";
    case -2 : return "Prev2/";
    case -1 : return "Prev1/";
    case  0 : return "";
    case  1 : return "Next1/";
    case  2 : return "Next2/";
    case  3 : return "Next3/";
    case  4 : return "Next4/";
    case  5 : return "Next5/";
    case  6 : return "Next6/";
    case  7 : return "Next7/";
    default : return "Unknown";
    };
    
  };

  int m_version;

  int m_l0EventNumber;
  int m_l0_B_Id;

  std::string  m_configfile;          // Config file name
  bool m_dumpError  ;
  
  LHCb::L0MuonCandidate* l0muoncandidate(L0Muon::PMuonCandidate cand, int procVersion);

  L0Muon::CtrlRawCnv*  m_ctrlRaw[2];
  L0Muon::ProcRawCnv*  m_procRaw[4];
    
};
 

#endif // COMPONENT_L0MUONFROMRAWTRANS_H
