// $Id: L0MuonOutputs.h,v 1.9 2008-07-23 07:56:59 jucogan Exp $
#ifndef COMPONENT_L0MUONOUTPUTS_H 
#define COMPONENT_L0MUONOUTPUTS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/L0MuonCandidate.h"

#include "L0MuonKernel/CtrlCandCnv.h"
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
  StatusCode decodeRawBanks();
  /// From the registers of the converters, fill the L0Muon banks
  StatusCode writeRawBanks();
  /// From the registers of the converters, fill the L0MuonCandidates
  /// and L0MuonData containers on the TES
  StatusCode writeOnTES();
  /// From the registers of the converters, fill the L0ProcessorDatas
  StatusCode writeL0ProcessorData();
  /// Reset the registers used by the converters
  StatusCode releaseRegisters();

  void setDecodingMode(){for (int i=0; i<4; ++i) m_procCand[i].setDecodingMode();}
  
  /// Configure the emulator version
  void setVersion(int version, int mode, bool compression){
    m_version=version; 
    m_mode=mode; 
    m_compression=compression; 
  }
  void setVersion(int version)  { m_version=version;}
  void setCompression(bool compression)  { m_compression=compression;}
  void setMode(int mode)  { m_mode=mode;}

  /// Output event error 
  void errors(MsgStream & msg) const;
  /// Display stat
  void statTot(MsgStream & os) const;
  /// Output error counters
  void errorSummary(MsgStream & msg) const;
  /// Monitor the L0Muon Banks
  void monitorBanks(MsgStream & msg);


private:

  int  m_version;      // L0Muon version to be emulated
  bool m_compression; // Apply compression on raw banks
  int  m_mode;         // Raw banks output mode 
  
  enum Quarters {Q1=0,Q2,Q3,Q4,NQuarters};
  enum Sides {A=0,C,NSides};

  int procSourceID(int srcID)
  {
    switch(m_version) {
    case 1:
      return srcID;
    case 2:
      switch(srcID){
      case 1:
        return Q4;
      case 2:
        return Q3;
      case 3:
        return Q1;
      case 4:
        return Q2;
      }
    }
    return 0;
  }

  int ctrlSourceID_inv(int side)
  {
    switch(m_version) {
    case 1:
      return side;
    case 2:
      switch(side){
      case 0:
        return 5;
      case 1:
        return 0;
      }
    }
    return 0;
  }
  int procSourceID_inv(int quarter)
  {
    switch(m_version) {
    case 1:
      return quarter;
    case 2:
      switch(quarter){
      case 0:
        return 3;
      case 1:
        return 4;
      case 2:
        return 2;
      case 3:
        return 1;
      }
    }
    return 0;
  }
  int ctrlSourceID(int srcID)
  {
    switch(m_version) {
    case 1:
      return srcID;
    case 2:
      switch(srcID){
      case 0:
        return C;
      case 5:
        return A;
      }
    }
    return 0;
  }

  std::vector<unsigned int> DC06RawBanks();
  LHCb::L0MuonCandidate* l0muoncandidate(L0Muon::PMuonCandidate cand);
  
  /// Converters for the L0Muon banks
  L0Muon::CtrlCandCnv  m_ctrlCand[2];
  L0Muon::ProcCandCnv  m_procCand[4];
  L0Muon::ProcDataCnv  m_procData[4];

  int m_l0EventNumber;
  int m_l0_B_Id;

  bool m_ctrlCandFlag;
  bool m_ctrlCandFlagBCSU;
  bool m_procCandFlag;
  bool m_procDataFlag;

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
