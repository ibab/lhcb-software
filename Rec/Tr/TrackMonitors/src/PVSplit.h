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
 *  0) The track container is randomly suffled according to option
 *  1) The track container is split into two parts according to split method:
 *     a) Middle - split into two halves with equal number of tracks
 *        if total number of tracks is even. Otherwise, (if total number of
 *        tracks is odd), one of the split containers (randomly chosen) has
 *        one track more.
 *     b) VeloHalf - a track is put in first (second) container if number of
 *        hit left (right) sensors is larger. If number of left and right hits
 *        are the same, container is randomly chosen.
 *     c) MiddlePerVeloHalf - split into two nearly equal parts each having
 *        nearly equal number of left and right tracks
 *  2) The two track containers are fitted with 
 *     PVOfflineTool::reconstructSinglePVFromTracks().
 *  3) Fitted vertices are written to a TES location. Each split vertex is
 *    'tagged' with the parent vertex' index. Additionally, for each split
 *    vertex, the sum of the weights (from the parent vertex) of all
 *    constituting tracks is stored. To extract parent vertex index and sum of
 *    weights do:
 *      int index = (int)vertex.info(PVSplit::ParentVertexIndex, -1);
 *      double wsum = vertex.info(PVSplit::SumOfParentWeights, 0.);
 *
 *  @author Colin Barschel, Rosen Matev
 *  @date   2012-07-17
 */

//-----------------------------------------------------------------------------
class PVSplit : public GaudiAlgorithm {
public:
  enum ExtraInfoKey { ParentVertexIndex  = 1000001,
                      SumOfParentWeights = 1000002
    };
  
  PVSplit(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~PVSplit();    ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:
  enum SplitMethod { Unknown = 0,
                     Middle,
                     VeloHalf,
                     MiddlePerVeloHalf
    };

  void clearSplitTracks();
  void randomShuffleTracks();
  void splitTracksByMiddle();
  void splitTracksByVeloHalf();
  void splitTracksByMiddlePerVeloHalf();

  int randomMiddle(unsigned int n);
  void countVeloLhcbIDs(const LHCb::Track* track, int& left, int& right) const;
  bool isLeftTrack(const LHCb::Track* track);

  void debugVertex(const LHCb::RecVertex* vx) const;

  std::string m_inputVerticesLocation;  ///< Location of input vertices
  std::string m_outputVerticesLocation; ///< Location of split vertices
  bool m_randomShuffle; ///< Whether to shuffle tracks first
  std::string m_splitMethodStr; ///< How to split track container (see enum SplitMethod for possible values)
  
  SplitMethod m_splitMethod;
  IPVOfflineTool* m_pvsfit;
  Rndm::Numbers m_rndm;
  DeVelo* m_velo;
  std::vector<const LHCb::Track*> m_tracks;
  std::vector<double> m_weights;
  std::vector< std::vector<const LHCb::Track*> > m_splitTracks;
  std::vector<double> m_splitTracksWSum;

};
#endif // PVSPLIT_H
