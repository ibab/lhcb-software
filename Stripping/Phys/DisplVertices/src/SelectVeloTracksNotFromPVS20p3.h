#ifndef HLTDISPLVERTICES_SELECTVELOTRACKSNOTFROMPVS20p3_H
#define HLTDISPLVERTICES_SELECTVELOTRACKSNOTFROMPVS20p3_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "Kernel/ITrackUniqueSegmentSelector.h"

#include "LoKi/UniqueKeeper.h"

/**
 * @class SelectVeloTracksNotFromPVS20p3 HltDisplVertices/SelectVeloTracksNotFromPVS20p3.h
 *
 * Filter tracks that are not from a primary vertex, by applying a minimum IP
 * cut to those vertices using LoKi::FastVertex
 *
 * @author Neal Gauvin
 * @author Victor Coco
 * @author Pieter David
 * @date   2012-03-27
 */
class SelectVeloTracksNotFromPVS20p3 : public GaudiAlgorithm 
{

public:

  /// Standard constructor
  SelectVeloTracksNotFromPVS20p3( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SelectVeloTracksNotFromPVS20p3(); ///< Destructor

  virtual StatusCode initialize();      ///< Algorithm initialization

  virtual StatusCode execute   ();      ///< Algorithm execution

  virtual StatusCode finalize  ();      ///< Algorithm finalization

private:

  std::vector<std::string> m_TracksLocations; ///< where the input tracks are located

  std::string m_PVLocation;                   ///< Location of primary vertices for MinIP cut or discarding tracks

  std::string m_WithIPTrackLocation;          ///< where the tracks are saved

  LHCb::Track::ConstVector m_inputTracks;
  LHCb::Track::ConstVector m_tracksWithUniqueVelo;

  bool m_removeBackwardTracks;                ///< Remove backward tracks
  double m_ipcut;                             ///< Minimum IP cut value
  double m_ipchi2cut;                         ///< Minimum IPChi2 cut value
  bool m_removeVeloClones;                    ///< Velo clones
  bool m_removePVTracks;                      ///< Remove tracks that are in a PV
  LoKi::UniqueKeeper<LHCb::Track> m_allPVTracks; // PV track keeper

  unsigned int m_minNumTracks;                ///< Minimal number of tracks that have to pass before accepting the event

  unsigned int m_maxNumOutputTracks;          ///< Reject events with more than this number of tracks after the filter

  // debug level flags
  bool m_debug;
  bool m_verbose;

  // unique segment selector
  ToolHandle<ITrackUniqueSegmentSelector> m_uniqueSegmentSelector;
};
#endif // HLTDISPLVERTICES_SELECTVELOTRACKSNOTFROMPVS20p3_H
