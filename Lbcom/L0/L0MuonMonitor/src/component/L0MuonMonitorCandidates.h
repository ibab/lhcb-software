// $Id: L0MuonMonitorCandidates.h,v 1.1 2010-03-08 15:12:53 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORCANDIDATES_H 
#define COMPONENT_L0MUONMONITORCANDIDATES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class L0MuonMonitorCandidates L0MuonMonitorCandidates.h component/L0MuonMonitorCandidates.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class L0MuonMonitorCandidates : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitorCandidates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorCandidates( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // Options
  std::string m_l0Context;  ///< Suffix appended to data location in TES
  bool m_enableTAE;         ///< Enable TAE mode
  bool m_allCandidates;     ///< Flag to enable the monitoring of all candidates found by the PU

  // Histograms
  AIDA::IHistogram1D * m_ptGev;
  AIDA::IHistogram1D * m_ptBin;
  AIDA::IHistogram1D * m_charge;
  AIDA::IHistogram1D * m_multi;
  AIDA::IHistogram1D * m_seedX;
  AIDA::IHistogram1D * m_seedY;

private:

  std::string m_location; ///< Location of the candidates in TES
  std::map<int,std::string> m_tae_items;  // Definitions of tae slots
  
};
#endif // COMPONENT_L0MUONMONITORCANDIDATES_H
