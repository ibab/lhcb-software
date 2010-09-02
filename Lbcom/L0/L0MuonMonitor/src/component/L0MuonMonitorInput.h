// $Id: L0MuonMonitorInput.h,v 1.1 2010-03-08 15:12:53 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORINPUT_H 
#define COMPONENT_L0MUONMONITORINPUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "Kernel/MuonTileID.h"

#include "MuonDAQ/IMuonRawBuffer.h"
#include "L0Interfaces/IL0MuonInputTool.h"
#include "L0Interfaces/IL0MuonOLErrorTool.h"

/** @class L0MuonMonitorInput L0MuonMonitorInput.h component/L0MuonMonitorInput.h
 *  
 *  Algorithm comparing the hits recorded by the Muon and the L0Muon systems. 
 *  Options : 
 *  - UseNZS    : if set to true, muon hits will be taken from non 0-supp banks
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
class L0MuonMonitorInput : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitorInput( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorInput( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // Options
  std::string m_l0Context;  ///< Suffix appended to data location in TES
  bool m_enableTAE;         ///< Enable TAE mode
  bool m_useNZS;            ///< Use muon NZS banks

  // Histogram
  AIDA::IHistogram1D * m_summary;
  AIDA::IHistogram2D * m_pads[5][4];

private:

  /// Compare the hits in muon and l0muon data and fill histogram 
  StatusCode Compare();
  /// Compare the hits in muon and l0muon data in TAE mode and fill histogram
  StatusCode CompareTAE();
  /// Get the hits seen by the muon
  StatusCode getMuonTiles(std::vector<LHCb::MuonTileID> & tiles, bool & truncated, std::string rootInTes = "");
  /// Check if both list contains the same muon tiles
  bool areDifferent(std::vector<LHCb::MuonTileID> & muontiles, std::vector<LHCb::MuonTileID> & l0muontiles);
  /// Fill maps with unmatched tiles
  void fillMaps(std::vector<LHCb::MuonTileID> & tiles);

  std::map<int,std::string> m_tae_items;    // Definitions of tae slots
  std::vector<LHCb::MuonTileID> m_optlinks; // List of optical links in error 

  IMuonRawBuffer*      m_muonBuffer;  // Muon decoding tool
  IL0MuonInputTool *   m_inputTool;   // L0Muon hits tool
  IL0MuonOLErrorTool * m_olerrorTool; // L0Muon hits tool

  MuonLayout m_padsLayout[5]; // Layouts of logical pads in each station

  bool m_offline;
  std::set<LHCb::MuonTileID> m_bad_tiles;
  
};
#endif // COMPONENT_L0MUONMONITORINPUT_H
