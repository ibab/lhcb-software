// $Id: L0MuonAlgComparison.h,v 1.3 2008-09-15 07:46:40 jucogan Exp $
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

  virtual ~L0MuonAlgComparison( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum Counter {FOUND, NOT_FOUND, ADDITIONAL, PROCESSED, TOTAL, NCounters};
  

protected:

private:

  unsigned int m_counters[NCounters];
  
  void printCand(std::string location);
  void printCand(LHCb::L0MuonCandidates* cand, std::string tab="");
  
  StatusCode compare(std::string loc, bool histo=false);

  std::string m_extension_0;
  std::string m_extension_1;
  
  void filterCand(LHCb::L0MuonCandidates* originals, LHCb::L0MuonCandidates* filtered);
  void L0MuonAlgComparison::printMuonTile(LHCb::MuonTileID mid);

  L0MuonCandHistos*     m_candHistosPU0;
  L0MuonCandHistos*     m_candHistosPU1;


};
#endif // COMPONENT_L0MUONALGCOMPARISON_H
