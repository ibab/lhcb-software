#ifndef MUONID_IMUONMATCHTOOL_H 
#define MUONID_IMUONMATCHTOOL_H 1
#include <vector>
#include <tuple>

#include "GaudiKernel/IAlgTool.h"
#include "MuonID/CommonMuonHit.h"

namespace LHCb {
  class Track;
  class MuonTileID;
}

/** @class IMuonMatchTool IMuonMatchTool.h MuonMatch/IMuonMatchTool.h
 *  
 *  tool to match tracks to the muon detector, starting from pre-selected matching tables
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-10
 */

/// TrackMuMatch contains MuonCommonHit, sigma_match, track extrap X, track extrap Y
typedef std::tuple<const CommonMuonHit*,float,double,double> TrackMuMatch;

class IMuonMatchTool : virtual public IAlgTool {
public: 

  DeclareInterfaceID( IMuonMatchTool, 2, 0);

  enum MuonMatchType {NoMatch=0, Uncrossed=1, Good=2};

  virtual StatusCode run(const LHCb::Track* track, 
                         std::vector<TrackMuMatch>* bestMatches,
                         std::vector<TrackMuMatch>* spareMatches=NULL) =0;
  virtual void getListofCommonMuonHits(CommonConstMuonHits& matchedMuonHits, int station=-1, bool onlybest=true) =0;
  virtual void getListofMuonTiles(std::vector<LHCb::MuonTileID>& matchedTiles, int station=-1, bool onlybest=true) =0;
  virtual double getChisquare(int& ndof) const =0;
  virtual MuonMatchType getMatch(int station) const=0;
  virtual double getMatchSigma(int station) const=0;

protected:
  unsigned nStations;
  unsigned nRegions;  
  
};
#endif // MUONID_IMUONMATCHTOOL_H
