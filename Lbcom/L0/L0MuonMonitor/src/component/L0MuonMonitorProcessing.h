// $Id: L0MuonMonitorProcessing.h,v 1.1 2010-03-08 15:12:53 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORPROCESSING_H 
#define COMPONENT_L0MUONMONITORPROCESSING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class L0MuonMonitorProcessing L0MuonMonitorProcessing.h component/L0MuonMonitorProcessing.h
 *  
 *  Algorithm comparing the L0MuonCandidates found in 2 different processing.
 *  Options : 
 *  - L0Context1, L0Context2 : suffixes to be appended to the standard candidate's locations
 *  - EnableTAE : if set to true, all available time slots will be analysed
 *  Used in the online monitoring to compare the hardware and the emulator.
 *  Fills an histogram with 2 bins : 
 *   - bin 0 : nb of events processed
 *   - bin 1 : nb of event with differences
 *  Filter : accept event with differences (for further analysis)
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class L0MuonMonitorProcessing : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitorProcessing( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorProcessing( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // Options
  bool m_enableTAE;         ///< Enable TAE mode
  std::string m_l0Context1; ///< Suffix appended to the location in Tes
  std::string m_l0Context2; ///< Suffix appended to the location in Tes

  // Histogram
  AIDA::IHistogram1D * m_summary;
  
private:

  /// Compare the hits in muon and l0muon data and fill histogram
  StatusCode Compare();
  /// Compare the hits in muon and l0muon data in TAE mode and fill histogram
  StatusCode CompareTAE();
  /// Check if both list contains the same muon tiles
  bool areDifferent(std::string location1, std::string location2);

  std::map<int,std::string> m_tae_items; // Definitions of tae slots


};
#endif // COMPONENT_L0MUONMONITORPROCESSING_H
