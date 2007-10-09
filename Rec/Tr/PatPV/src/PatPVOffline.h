// $Id: PatPVOffline.h,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
#ifndef PATPVOFFLINE_H 
#define PATPVOFFLINE_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Interfaces
#include "PVFitterTool.h"
#include "IPVFitterTool.h"
#include "PVUtils.h"
#include "PVSeedTool.h"

// Track info
#include "VeloDet/DeVelo.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"


class IPVFitterTool;
class ITrackExtrapolator;

class PVVertex {
public:
  PVTrackPtrs pvTracks;
  LHCb::RecVertex primVtx;
};

typedef std::vector<PVVertex> PVVertices;

class PatPVOffline : public GaudiAlgorithm {
public:
  // Standard constructor
  PatPVOffline(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  ~PatPVOffline();

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute();       // Algorithm execution

private:

  bool m_useVelo;       // Option to use VELO tracks in vtx finding
  bool m_useLong;       // Option to use LONG tracks in vtx finding
  bool m_useUpstream;   // Option to use Upstream tracks in vtx finding
  bool m_saveSeedsAsPV; // Save seeds as PVs (for monitoring)

  // Tools
  IPVFitterTool* m_pvfit;                // PV fitting tool
  PVSeedTool* m_pvSeedTool;              // Seeding tool

  std::vector<vtxCluster> m_vclusters;

  // Member functions
  StatusCode readTracks(PVTracks& pvTracks) ;
  StatusCode collectTracks(PVTracks& pvTracks, 
                      PVVertex& pvVertex, 
                      double zseed);

  void copyTrackVertex(LHCb::RecVertex* inTrackVertex,
                       LHCb::RecVertex* outTrackVertex);

  void storeDummyVertices(std::vector<double> &zseeds, 
                          std::vector<PVTrack>& pvTracks, 
                          LHCb::RecVertices* v2tes);


  std::vector< std::string > m_inputTracks;
  std::string m_outputVertices;



};
#endif // PATPVOFFLINE_H
