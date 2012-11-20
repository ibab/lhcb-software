#ifndef TrackClusterFinder_H
#define TrackClusterFinder_H 1
// Include files:
// from Gaudi
#include "Kernel/DaVinciAlgorithm.h"
// from Event
#include "Event/Track.h"
#include "Event/RecVertex.h"

using namespace LHCb;

/** @class TrackClusterFinder TrackClusterFinder.h
 *
 *
 *  @author Marcin Kucharczyk
 *  @date   2010-04-03
 */

class TrackClusterFinder : public DaVinciAlgorithm 
{

public:

  TrackClusterFinder(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  virtual ~TrackClusterFinder();
  // Algorithm initialization
  virtual StatusCode initialize();
  // Algorithm execution
  virtual StatusCode execute();

private:

  class tmpTrack {
  public:
    int index;
    int weight;
    double phi;
    double eta;
    double pt;
  };

  class tmpJet {
  public:
    std::vector<tmpTrack> tracks;
  };

private:

  void getPeak(std::vector<tmpTrack> tmpTracks, int& index, int& multPeak);
  std::vector<std::string> m_inputTracks;
  std::string m_inputPVsName;
  LHCb::RecVertices* m_inputPVs;
  int m_minWeightSeed;
  double m_coneExtFactor;
  double m_maxDeltaPhi;
  double m_maxDeltaEta;
  double m_min1stJetPt;
  double m_min2ndJetPt;
  unsigned int m_jetMult;
  double m_maxRPV;
  unsigned int m_nrPVs;
  double m_minSumEt;
  
};
#endif // TrackClusterFinder_H
