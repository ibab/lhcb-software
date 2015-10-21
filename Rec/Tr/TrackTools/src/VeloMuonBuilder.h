#ifndef VELOMUONBUILDER_H
#define VELOMUONBUILDER_H 1

#include <memory>

#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/Tuples.h"
#include <string>

#include "Event/Track.h"
#include "Event/MuonPID.h"
#include "Event/MuonDigit.h"
#include "Event/MuonCoord.h"

#include"MuonDet/IMuonFastPosTool.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackFitter.h"

/** @class VeloMuonBuilder VeloMuonBuilder.h
 *
 * \brief  Make a ValoMuonTrack: match velo and muon tracks. afterwards apply kalman fit.
 *
 * Parameters:
 * - MuonLocation: where the muon stubs come from
 * -  VeloLocation: where the velo tracks come from
 * -  OutputLocation: where the velomuontracks go to
 * -  zmagnet: where the magnet's bending plane is (optimised for matching)
 * -  zmatch: where in the y-z plane matching is done
 * -  chamberhit: if for y-z matching the first muon measurement shall be used instead of zmatch
 * -  distancecut: obsolete, kept for compatibility. hard coded.
 * -  lhcbids: obsolete used for elaborate chi^2 fit in the muon stations with unknown hit pattern
 *             not applicable for existing muon algorithms but for private code
 * -  cut: if the hard coded distancecut shall be scaled
 * -  MaxVeloTracks: if there are more velo tracks, don't do anything (saving cpu time in hopeless events)
 * -  MaxMuonTracks: if there are more muon tracks, don't do anything (saving cpu time in hopeless events)
 *
 *  @author Paul Seyfert
 *  @date   2010-09-16
 */

class VeloMuonBuilder : public GaudiTupleAlg {

public:
  VeloMuonBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~VeloMuonBuilder();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  StatusCode buildVeloMuon(LHCb::Tracks& veloTracks, LHCb::Tracks& muonTracks, LHCb::Tracks* trackvector);
  LHCb::Tracks* buildVeloMuon(LHCb::Tracks& veloTracks, LHCb::Tracks& muonTracks);

private:
  //  Gaudi::XYZPoint VeloMuonBuilder::autoextra(LHCb::Track* muontrack);
  float m_distcutmultiplyer;
  float m_fieldfactor;
  bool m_chamberhit;
  double m_zmagnet;
  float m_zmatch;
  const float m_xscale[4] = {0.06f,0.1f,0.15f,0.15f};
  std::string m_muonpath;
  std::string m_velopath;
  std::string m_output;

  unsigned int m_maxmuons;
  unsigned int m_maxvelos;

  int n_lhcbids;

  IMuonFastPosTool* m_iPosTool;
  ITrackExtrapolator* m_linearextrapolator;
  ITrackFitter* m_tracksFitter;

};

#endif
