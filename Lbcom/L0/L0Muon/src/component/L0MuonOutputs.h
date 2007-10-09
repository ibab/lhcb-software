// $Id: L0MuonOutputs.h,v 1.3 2007-10-09 23:37:24 jucogan Exp $
#ifndef COMPONENT_L0MUONOUTPUTS_H 
#define COMPONENT_L0MUONOUTPUTS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/L0MuonCandidate.h"

#include "L0MuonKernel/CtrlFinalCnv.h"
#include "L0MuonKernel/CtrlAllCnv.h"
#include "L0MuonKernel/ProcCandCnv.h"
#include "L0MuonKernel/ProcDataCnv.h"
#include "L0MuonKernel/MuonCandidate.h"

static const InterfaceID IID_L0MuonOutputs ( "L0MuonOutputs", 1, 0 );

/** @class L0MuonOutputs L0MuonOutputs.h component/L0MuonOutputs.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2007-07-09
 */
class L0MuonOutputs : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonOutputs; }

  /// Standard constructor
  L0MuonOutputs( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~L0MuonOutputs( ); ///< Destructor

  /// Decode the L0Muon banks and fill the registers of the converters
  StatusCode decodeRawBanks(int mode=0);
  /// From the registers of the converters, fill the L0Muon banks
  StatusCode writeRawBanks(int mode, int bankVersion);
  /// From the registers of the converters, fill the L0MuonCandidates
  /// and L0MuonData containers on the TES
  StatusCode writeOnTES(int procVersion, std::string extension);
  /// From the registers of the converters, fill the L0ProcessorDatas
  StatusCode writeL0ProcessorData(std::string extension);
  /// Reset the registers used by the converters
  StatusCode releaseRegisters();
  /// Monitor the L0Muon Banks
  StatusCode monitorBanks();

  /// Display stat
  void statTot(MsgStream & os) const;

protected:
  
  

private:

  std::vector<unsigned int> DC06RawBanks();
  LHCb::L0MuonCandidate* l0muoncandidate(L0Muon::PMuonCandidate cand, int procVersion);
  
  /// Converters for the L0Muon banks
  L0Muon::CtrlFinalCnv m_ctrlFinal[2];
  L0Muon::CtrlAllCnv   m_ctrlAll[2];
  L0Muon::ProcCandCnv  m_procCand[4];
  L0Muon::ProcDataCnv  m_procData[4];
  
  int m_rawBankNorm;
  int m_nCandFinalNorm;
  
  int m_rawBankSizeTot;
  int m_nCandFinalTot;
  int m_nTriggeredEvt;

  double m_averagePt;
  double m_averageCh;
  
  int m_bankEventCounter;
  
};
#endif // COMPONENT_L0MUONOUTPUTS_H
