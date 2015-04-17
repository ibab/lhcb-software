#ifndef PVSPLIT_H
#define PVSPLIT_H 1

// From Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <GaudiKernel/RndmGenerators.h>
// Interfaces
#include "TrackInterfaces/IPVOfflineTool.h"

// From Event
#include "Event/Track.h"
#include "Event/RecVertex.h"

class DeVelo;

/** @class PVSplit PVSplit.h
 *  Algorithm to split tracks belonging to found vertices, fit them and
 *  write split vertices to TES.
 *
 *  For each vertex in m_inputVerticesLocation, the following is executed.
 *  0) The tracks associated with the vertex are randomly shuffled according to option
 *  1) They are split into two parts according to split method:
 *     a) Middle - split into two halves with equal number of tracks
 *        if total number of tracks is even. Otherwise, (if total number of
 *        tracks is odd), one of the split containers (randomly chosen) has
 *        one track more.
 *     b) VeloHalf - a track is put in first (second) container if number of
 *        hits in left (right) sensors is larger. If number of left and right hits
 *        are the same, container is randomly chosen.
 *     c) MiddlePerVeloHalf - split into two nearly equal parts each having
 *        nearly equal number of left and right tracks
 *     d) VeloTopBottom - a track is put in first (second) container if number of
 *        hits in top (bottom) phi sensors is larger.
 *  2) The two track containers are fitted with
 *     IPVOfflineTool::reconstructSinglePVFromTracks() (using the PV as seed) or
 *     with IPVOfflineTool::reconstructMultiPVFromTracks() (making a new seed).
 *  3) Fitted vertices are written to a TES location. If CopyPV is true, each
 *     input vertex is copied before the two split vertices. Each split vertex is
 *     'tagged' with the parent vertex' index and Z. Additionally, for each split
 *     vertex, the sum of the weights (from the parent vertex) of all
 *     constituting tracks is stored. To extract parent vertex index and sum of
 *     weights do:
 *       int index = (int)vertex.info(PVSplit::ParentVertexIndex, -1);
 *       double wsum = vertex.info(PVSplit::SumOfParentWeights, 0.);
 *     If a split vertex is not successfully reconstructed, a vertex is still
 *     written with a technique=Unknown and an empty tracks container.
 *
 *  @author Colin Barschel, Rosen Matev
 *  @date   2012-07-17, 2015-04-15
 */

//-----------------------------------------------------------------------------
class PVSplit : public GaudiAlgorithm
{

public:

  PVSplit(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~PVSplit();    ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void clearSplitTracks();
  void randomShuffleTracks();
  void splitTracksByMiddle();
  void splitTracksByVeloHalf();
  void splitTracksByVeloTopBottom();
  void splitTracksByMiddlePerVeloHalf();

  unsigned int randomMiddle(unsigned int n, int oddShift);
  void countVeloLhcbIDsLeftRight(const LHCb::Track* track, int& left, int& right) const;
  void countVeloLhcbIDsTopBottom(const LHCb::Track* track, int& top, int& bottom) const;
  bool isLeftTrack(const LHCb::Track* track);
  bool isTopTrack(const LHCb::Track* track);

  void debugVertex(const LHCb::RecVertex* vx) const;

private:

  typedef void (PVSplit::*SplitTracksFun)();

  std::string m_inputVerticesLocation;  ///< Location of input vertices
  std::string m_outputVerticesLocation; ///< Location of split vertices
  bool m_randomShuffle; ///< Whether to shuffle tracks first
  std::string m_splitMethodStr; ///< How to split track container (see enum SplitMethod for possible values)
  bool m_usePVForSeed; ///< Use the PV for seed of the split vertex fit
  bool m_copyPV; ///< Whether to copy PVs to the output container
  int m_keyOfParentVertexIndex;
  int m_keyOfSumOfParentWeights;
  int m_keyOfParentVertexZ;

  SplitTracksFun m_splitTracksFun;
  IPVOfflineTool* m_pvsfit;
  Rndm::Numbers m_rndm;
  DeVelo* m_velo;
  std::vector<const LHCb::Track*> m_tracks;
  std::vector<double> m_weights;
  std::vector< std::vector<const LHCb::Track*> > m_splitTracks;
  std::vector<double> m_splitTracksWSum;

};
#endif // PVSPLIT_H
