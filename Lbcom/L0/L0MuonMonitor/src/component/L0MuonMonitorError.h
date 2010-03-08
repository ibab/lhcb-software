// $Id: L0MuonMonitorError.h,v 1.1 2010-03-08 15:12:53 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORERROR_H 
#define COMPONENT_L0MUONMONITORERROR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class L0MuonMonitorError L0MuonMonitorError.h component/L0MuonMonitorError.h
 *  
 *  Monitor the error flags raised by the L0Muon trigger 
 *
 *  @author Julien Cogan
 *  @date   2010-02-16
 */
class L0MuonMonitorError : public GaudiHistoAlg {

public:
  static const int NQuarters =   4; ///< Nb of quarters
  static const int NTell1s   =   6; ///< Nb of Tell1s
  static const int NBCSUs    =  12; ///< Nb of BCSUs
  static const int NPUs      =   4; ///< Nb of PUs

public: 
  /// Standard constructor
  L0MuonMonitorError( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorError( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  
  // Options
  std::string m_l0Context; ///< Suffix appended to data location in TES
  bool m_enableTAE;        ///< Enable TAE mode

  // Histograms
  AIDA::IHistogram1D * m_summary;                      ///< Error summary
  AIDA::IHistogram1D * m_tx_summary[NQuarters];        ///< Transmission error summary
  AIDA::IHistogram1D * m_sync_summary[NQuarters];      ///< Synchronisation error summary
  AIDA::IHistogram1D * m_derandom_summary[NQuarters];  ///< Derandomiser error summary

  AIDA::IHistogram1D * m_sync_ctrl_src;                   ///< Synchronisation error sources in Ctrl board 
  AIDA::IHistogram1D * m_tell1_int_src[NTell1s];          ///< Tell1 error sources
  AIDA::IHistogram1D * m_tell1_ref_src[NTell1s];          ///< Tell1 error sources
  AIDA::IHistogram1D * m_decoding_src;                    ///< Decoding error sources
  AIDA::IHistogram1D * m_bcsu_src[NQuarters][NBCSUs];     ///< BCSU error sources
  AIDA::IHistogram1D * m_pu_src[NQuarters][NBCSUs][NPUs]; ///< PU error sources

private:

  std::map<int,std::string> m_tae_items;    // Definitions of tae slots

};
#endif // COMPONENT_L0MUONMONITORERROR_H
