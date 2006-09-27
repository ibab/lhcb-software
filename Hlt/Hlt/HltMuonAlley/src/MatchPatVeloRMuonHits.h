// $Id: MatchPatVeloRMuonHits.h,v 1.2 2006-09-27 14:18:04 cattanem Exp $
#ifndef MATCHPATVELORMUONHITS_H 
#define MATCHPATVELORMUONHITS_H 1

// Include files

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

#include "MuonDet/IMuonPosTool.h"

/** @class MatchPatVeloRMuonHits MatchPatVeloRMuonHits.h
 *   
 *
 *  @author Sandra Amato 
 *  @date   2006-03-07
 */
class MatchPatVeloRMuonHits : public HltAlgorithm {
public: 
  /// Standard constructor
  MatchPatVeloRMuonHits( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MatchPatVeloRMuonHits( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  bool acceptTrack(const LHCb::Track& track);

private:


  StatusCode match2dMuon(LHCb::Track* ptr);
  std::vector<float> m_x_cut_track;
  float z_magnet_center;
  double m_yTol;
  IMuonPosTool      *m_iPosTool;

  HltHisto m_histoXdist;

};
#endif // MATCHPATVELORMUONHITS_H
