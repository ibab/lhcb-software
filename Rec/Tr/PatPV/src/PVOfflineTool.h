// $Id: PVOfflineTool.h,v 1.4 2008-06-11 19:28:24 witekma Exp $
#ifndef PVOFFLINETOOL_H 
#define PVOFFLINETOOL_H 1
// Include files:
// from STL
#include <string>
#include <vector>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include "TrackInterfaces/IPVOfflineTool.h"
#include "IPVFitter.h"
#include "IPVSeeding.h"
// Track info
#include "Event/Track.h"
#include "Event/RecVertex.h"

class PVOfflineTool : public GaudiTool, virtual public IPVOfflineTool {
public:
  // Standard constructor
  PVOfflineTool(const std::string& type, 
                const std::string& name,
                const IInterface* parent);
  // Destructor
  ~PVOfflineTool();
  virtual StatusCode initialize();
  // PV fitting 

  StatusCode reDoSinglePV(const Gaudi::XYZPoint xyzseed,
			  std::vector<const LHCb::Track*>& tracks2exclude,
			  LHCb::RecVertex& outvtx);

  StatusCode reDoMultiPV(const LHCb::RecVertex& invtx, 
			 std::vector<const LHCb::Track*>& tracks2exclude, 
			 LHCb::RecVertex& outvtx);


  StatusCode reconstructSinglePVFromTracks(const Gaudi::XYZPoint xyzseed,
					   std::vector<const LHCb::Track*>& tracks2use,
					   LHCb::RecVertex& outvtx);

  StatusCode reconstructMultiPVFromTracks(std::vector<const LHCb::Track*>& tracks2use,
					  std::vector<LHCb::RecVertex>& outvtxVec);



  StatusCode reconstructMultiPV(std::vector<LHCb::RecVertex>& outvtxVec); 

  StatusCode reconstructSinglePV(const Gaudi::XYZPoint xyzseed,
			         LHCb::RecVertex& outvtx);



private:
  bool m_useVelo;       // Option to use VELO tracks in vtx finding
  bool m_useLong;       // Option to use LONG tracks in vtx finding
  bool m_useUpstream;   // Option to use Upstream tracks in vtx finding
  bool m_saveSeedsAsPV; // Save seeds as PVs (for monitoring)
  std::string m_outputVertices;
  // Tools
  IPVFitter* m_pvfit;                // PV fitting tool
  IPVSeeding* m_pvSeedTool;              // Seeding tool
  // Member functions
  std::vector<std::string> m_inputTracks;
  std::string m_pvFitterName;
  std::string m_pvSeedingName;

  StatusCode matchVtxByTracks(const LHCb::RecVertex& invtx,  
			      std::vector<LHCb::RecVertex>& outvtxvec, 
			      LHCb::RecVertex& outvtx);

  void readTracks( std::vector<const LHCb::Track*>& rtracks);

  void getSeeds(std::vector<const LHCb::Track*>& rtracks, 
                std::vector<Gaudi::XYZPoint>& seeds);

  void removeTracksByLHCbIDs(std::vector<const LHCb::Track*>& tracks, 
		             std::vector<const LHCb::Track*>& tracks2remove);

  void removeTracks(std::vector<const LHCb::Track*>& tracks, 
		    std::vector<const LHCb::Track*>& tracks2remove);

  void removeTracksUsedByVertex(std::vector<const LHCb::Track*>& tracks,
				LHCb::RecVertex& rvtx);

  void storeDummyVertices(std::vector<Gaudi::XYZPoint>& zseeds,
			  std::vector<const LHCb::Track*> rtracks, 
			  std::vector<LHCb::RecVertex>& outvtx);

  double zCloseBeam(const LHCb::Track* track);
};
#endif // PVOFFLINETOOL_H
