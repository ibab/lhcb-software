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

static const InterfaceID IID_IMuonMatchTool ( "IMuonMatchTool", 1, 0 );

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

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonMatchTool; }

  enum MuonMatchType {NoMatch=0, Uncrossed=1, Good=2};

  virtual StatusCode run(const LHCb::Track* track, 
                         std::vector<TrackMuMatch>* bestMatches,
                         std::vector<TrackMuMatch>* spareMatches=NULL) =0;
  virtual void getListofCommonMuonHits(CommonConstMuonHits& matchedMuonHits, int station=-1, bool onlybest=true) =0;
  virtual void getListofMuonTiles(std::vector<LHCb::MuonTileID>& matchedTiles, int station=-1, bool onlybest=true) =0;
  virtual double getChisquare(int& ndof) =0;
  virtual MuonMatchType getMatch(int station)=0;
  virtual double getMatchSigma(int station)=0;

  virtual double muonMatchPropertyD(const char* , int =-1) {return 0.;}
  virtual int muonMatchPropertyI(const char* , int =-1) {return 0;}


protected:
  unsigned nStations;
  unsigned nRegions;  
  
};
#endif // MUONID_IMUONMATCHTOOL_H
