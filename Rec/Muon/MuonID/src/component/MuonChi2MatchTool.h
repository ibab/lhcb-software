// $Id: $
#ifndef COMPONENT_MUONCHI2MATCHTOOL_H 
#define COMPONENT_MUONCHI2MATCHTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonID/IMuonMatchTool.h"            // Interface

class DeMuonDetector;

/** @class MuonChi2MatchTool MuonChi2MatchTool.h component/MuonChi2MatchTool.h
 *  
 * 
 *  the tool chooses the combination of muon detector hits (1 per station) that best fits a track extrapolation
 *  based on a global chi^2 that takes properly into account correlations in the track extrapolation errors
 *
 *  input hits are given through a vector of TrackMuMatch, containing a CommonMuonHit and related track extrapolation position
 *  optionally, a second list of "low-quality" hits (uncrossed or from large clusters) can be given if no matches are found in the first list
 *
 *  use run() to perform the calculation, and then:
 *    getChisquare() to get the best chi2
 *    getListofCommonMuonHits() or getListofMuonTiles() to get the associated muon hits
 *
 *  @author Violetta Cogoni, Marianna Fontana, Rolf Odeman, ported here by Giacomo Graziani
 *  @date   2015-11-11
 */
class MuonChi2MatchTool : public GaudiTool, virtual public IMuonMatchTool {
public: 
  /// Standard constructor
  MuonChi2MatchTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~MuonChi2MatchTool() {} ///< Destructor
  virtual StatusCode initialize();

  virtual StatusCode run(const LHCb::Track* track, 
                         std::vector<TrackMuMatch>* bestMatches,
                         std::vector<TrackMuMatch>* spareMatches=NULL);
  virtual void getListofCommonMuonHits(CommonConstMuonHits& matchedMuonHits, int station=-1, bool onlybest=true);
  virtual void getListofMuonTiles(std::vector<LHCb::MuonTileID>& matchedTiles, int station=-1, bool onlybest=true);
  virtual double getChisquare(int& ndof) const;
  virtual MuonMatchType getMatch(int station) const;
  virtual double getMatchSigma(int station) const;

private:
  static const int nSt = 4; //this algorithm uses stations M2 to M5  
  DeMuonDetector*  m_mudet;
  std::vector<double> m_zm;
  int m_myFirstStation;

  std::vector<std::vector<double> > m_xpadsize;
  std::vector<std::vector<double> > m_ypadsize;
  std::vector<std::pair<double,double> > m_mcs;
  std::vector<std::vector<std::pair<double,double> > > m_xhit_comb;
	std::vector<std::vector<std::pair<double,double> > > m_yhit_comb;
	std::vector<CommonConstMuonHits>  m_muhit_comb;
  int m_maxcomb;

  const LHCb::Track* m_lasttrack;
  std::vector< TrackMuMatch > * m_lastMatchesBest;
  std::vector< TrackMuMatch > * m_lastMatchesSpare;
  std::vector<std::vector<std::pair<const CommonMuonHit*, double> > > m_dhit;
  std::vector<std::pair<double,double> > m_trkExtrap;
  int m_missedStations;
  std::vector<int> m_nhit;
  int m_ord;
  std::vector<int> m_matchedStationIndex;

  // store output:
  int m_bestcomb;
  double m_chisq_min;
  std::vector<bool> m_hitOnStation;
  CommonConstMuonHits m_matchedHits;
  CommonConstMuonHits m_bestmatchedHits;
  

  void initCombs();
  double calcChi2(std::vector<std::pair<double,double> > combination,double vextraM[],double p);
  void getResidual(TrackMuMatch& match);
  void makeComb(std::vector<int>* c , int& comb);

  // options
  bool m_OnlyCrossedHits;
  int m_maxnhitinFOI;
  double m_InputMaxSigmaMatch;

};
#endif // COMPONENT_MUONCHI2MATCHTOOL_H
