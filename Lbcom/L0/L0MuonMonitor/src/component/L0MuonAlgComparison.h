// $Id: L0MuonAlgComparison.h,v 1.1 2008-04-16 07:13:44 jucogan Exp $
#ifndef COMPONENT_L0MUONALGCOMPARISON_H 
#define COMPONENT_L0MUONALGCOMPARISON_H 1

// Include files


// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "Event/L0MuonCandidate.h"

/** @class L0MuonAlgComparison L0MuonAlgComparison.h component/L0MuonAlgComparison.h
 *  
 *
 *  @author 
 *  @date   2008-04-08
 */
class L0MuonAlgComparison : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonAlgComparison( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonAlgComparison( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum Counter {FOUND, NOT_FOUND, NCounters};
  

protected:

private:
  
  std::vector<int> m_quarters;

  unsigned int m_counters[NCounters];
  
  void printCand(std::string location);
  void printCand(LHCb::L0MuonCandidates* cand, std::string tab="");
  
  StatusCode compare(std::string loc);

  std::string m_extension_0;
  std::string m_extension_1;
  
  LHCb::L0MuonCandidates* filterCand(LHCb::L0MuonCandidates* originals);
  void L0MuonAlgComparison::printMuonTile(LHCb::MuonTileID mid);

};
#endif // COMPONENT_L0MUONALGCOMPARISON_H
