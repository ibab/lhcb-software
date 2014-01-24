#ifndef HLTDISPLVERTICES_SELECTVELOTRACKSNOTFROMPV_H
#define HLTDISPLVERTICES_SELECTVELOTRACKSNOTFROMPV_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/**
 * @class SelectVeloTracksNotFromPV HltDisplVertices/SelectVeloTracksNotFromPV.h
 *
 * Filter tracks that are not from a primary vertex, by applying a minimum IP
 * cut to those vertices using LoKi::FastVertex
 *
 * @author Neal Gauvin
 * @author Victor Coco
 * @author Pieter David
 * @date   2012-03-27
 */
class SelectVeloTracksNotFromPV : public GaudiAlgorithm
{
  public:
    /// Standard constructor
    SelectVeloTracksNotFromPV( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~SelectVeloTracksNotFromPV(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization

    virtual StatusCode execute(); ///< Algorithm execution

    virtual StatusCode finalize(); ///< Algorithm finalization

  private:
    std::vector<std::string> m_TracksLocations; ///< where the input tracks are
                                                ///located

    std::string m_PVLocation; ///< Location of primary vertices for MinIP cut or
                              ///discarding tracks

    std::string m_WithIPTrackLocation; ///< where the tracks are saved

    bool m_removeBackwardTracks; ///< Remove backward
    double m_ipcut;              ///< Minimum IP cut value
    double m_ipchi2cut;          ///< Minimum IPChi2 cut value

    unsigned int m_minNumTracks; ///< Minimal number of tracks that have to pass
                                 ///before accepting the event

    bool m_rejectSplashEvents; ///< Reject all events where the FastVelo
                               ///clone-killing fired
    int m_maxNumInputTracks; ///< Reject events with more than this number of tracks
                             ///before the filter
    unsigned int m_maxNumOutputTracks; ///< Reject events with more than this number
                                       ///of tracks after the filter

    // debug level flags
    bool m_debug;
    bool m_verbose;
};
#endif // HLTDISPLVERTICES_SELECTVELOTRACKSNOTFROMPV_H
