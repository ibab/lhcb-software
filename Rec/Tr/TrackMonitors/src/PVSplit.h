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

/** @class PVSplit PVSplit.h
 *  Algorithm to split tracks belonging to found vertices, fit them and
 *  write split vertices to TES.
 *
 *  For each vertex in m_inputVerticesLocation, the following is executed.
 *  The track container is randomly split into two halves with equal number
 *  of tracks if total number of tracks is even. Otherwise, (if total number of
 *  tracks is odd), one of the split containers (randomly chosen) has one track
 *  more. Afterwards, the two track containers are fitted with 
 *  PVOfflineTool::reconstructSinglePVFromTracks(). 
 *  Fitted vertices are written to a TES location. Each split vertex is 'tagged'
 *  with the parent vertex' index. Additionally, for each split vertex,
 *  the sum of the weights (from the parent vertex) of all constituting tracks
 *  is stored. To extract parent vertex index and sum of weights do:
 *    int index = (int)vertex.info(PVSplit::ParentVertexIndex, -1);
 *    double wsum = vertex.info(PVSplit::SumOfParentWeights, 0.);
 *
 *  @author Colin Barschel, Rosen Matev
 *  @date   2012-07-05
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
  void random_shuffle_tracks();
  void split_tracks();

  void debug_vertex(const LHCb::RecVertex* vx) const;

  std::string m_inputVerticesLocation;  ///< Location of input vertices
  std::string m_outputVerticesLocation; ///< Location of split vertices

  IPVOfflineTool* m_pvsfit;
  Rndm::Numbers m_rndm;
  std::vector<const LHCb::Track*> m_tracks;
  std::vector<double> m_weights;
  std::vector< std::vector<const LHCb::Track*> > m_splitTracks;
  std::vector<double> m_splitTracksWSum;

};
#endif // PVSPLIT_H
