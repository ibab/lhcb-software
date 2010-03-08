// $Id: L0MuonMonitorData.h,v 1.1 2010-03-08 15:12:53 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORDATA_H 
#define COMPONENT_L0MUONMONITORDATA_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "L0Interfaces/IL0MuonInputTool.h"

/** @class L0MuonMonitorData L0MuonMonitorData.h component/L0MuonMonitorData.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class L0MuonMonitorData : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitorData( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // Options
  std::string m_l0Context; ///< Suffix appended to data location in TES
  bool m_enableTAE;        ///< Enable TAE mode

  // Histograms
  AIDA::IHistogram1D * m_multi_pads[5];
  AIDA::IHistogram1D * m_padsX[5];
  AIDA::IHistogram1D * m_padsY[5];

  AIDA::IHistogram2D * m_pads[5][4];

private:

  std::map<int,std::string> m_tae_items;  // Definitions of tae slots

  IL0MuonInputTool *   m_inputTool;   // L0Muon hits tool
  
};
#endif // COMPONENT_L0MUONMONITORDATA_H
