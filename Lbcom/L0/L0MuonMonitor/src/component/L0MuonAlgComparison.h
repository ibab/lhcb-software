#ifndef COMPONENT_L0MUONALGCOMPARISON_H 
#define COMPONENT_L0MUONALGCOMPARISON_H 1

// Include files


// from Gaudi

#include "Event/L0MuonCandidate.h"

#include "L0MuonCandHistos.h"

#include "L0MuonMonitorBase.h"

/** @class L0MuonAlgComparison L0MuonAlgComparison.h component/L0MuonAlgComparison.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-04-08
 */
class L0MuonAlgComparison : public L0MuonMonitorBase {
public: 
  /// Standard constructor
  L0MuonAlgComparison( const std::string& name, ISvcLocator* pSvcLocator );

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution
  StatusCode finalize  () override;    ///< Algorithm finalization

private:

  enum Counter {FOUND, NOT_FOUND, ADDITIONAL, PROCESSED, TOTAL, BUNCHES, NCounters};
  

  unsigned int m_counters[NCounters];
  
  void printCand(std::string location) const;
  void printCand(const LHCb::L0MuonCandidates& cand, std::string tab="") const;
  
  StatusCode compare(std::string loc, bool histo=false, int ts=0);

  std::string m_extension_0;
  std::string m_extension_1;
  
  void filterCand(const LHCb::L0MuonCandidates& originals, LHCb::L0MuonCandidates& filtered) const;
  void printMuonTile(LHCb::MuonTileID mid) const;

  L0MuonCandHistos*     m_candHistosPU0;
  L0MuonCandHistos*     m_candHistosPU1;

  bool m_diff;
  
};
#endif // COMPONENT_L0MUONALGCOMPARISON_H
