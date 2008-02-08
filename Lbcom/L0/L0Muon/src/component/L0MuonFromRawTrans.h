// $Id: L0MuonFromRawTrans.h,v 1.3 2008-02-08 11:17:00 jucogan Exp $
#ifndef COMPONENT_L0MUONFROMRAWTRANS_H 
#define COMPONENT_L0MUONFROMRAWTRANS_H 1

// Include files
// from Gaudi
// #include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
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
// class L0MuonFromRawTrans : public GaudiAlgorithm {
class L0MuonFromRawTrans : public GaudiHistoAlg {
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
  StatusCode writeOnTES(int procVersion, std::string extension="");
  /// Reset the registers used by the converters
  StatusCode releaseRegisters();

private:
  
  void fillHisto();
  
  std::string  m_configfile;          // Config file name

  unsigned int m_ievt;
  
  LHCb::L0MuonCandidate* l0muoncandidate(L0Muon::PMuonCandidate cand, int procVersion);

  L0Muon::CtrlRawCnv  m_ctrlRaw[2];
  L0Muon::ProcRawCnv  m_procRaw[4];

  int m_bankEventCounter;
};
#endif // COMPONENT_L0MUONFROMRAWTRANS_H
