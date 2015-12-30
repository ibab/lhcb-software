// $Id: PVOfflineTool.h,v 1.7 2010-02-09 10:44:20 witekma Exp $
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
#include "PVOfflineRecalculate.h"
// Track info
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "GaudiAlg/ISequencerTimerTool.h"

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
					   LHCb::RecVertex& outvtx) ;

  StatusCode reconstructMultiPVFromTracks(std::vector<const LHCb::Track*>& tracks2use,
					  std::vector<LHCb::RecVertex>& outvtxVec);

  StatusCode reconstructMultiPV(std::vector<LHCb::RecVertex>& outvtxVec);

  StatusCode reconstructSinglePV(const Gaudi::XYZPoint xyzseed,
			         LHCb::RecVertex& outvtx);

  StatusCode removeTracksAndRecalculatePV(const LHCb::RecVertex* pvin,
					  const std::vector<const LHCb::Track*>& tracks2remove,
					  LHCb::RecVertex& vtx);


private:

  bool m_requireVelo;   // Option to use tracks with VELO segment only
  bool m_saveSeedsAsPV; // Save seeds as PVs (for monitoring)
  bool m_lookForDisplaced;
  std::string        m_outputVertices;
  std::string        m_inputVerticesName;
  // Tools
  IPVFitter* m_pvfit;                // PV fitting tool
  IPVSeeding* m_pvSeedTool;              // Seeding tool
  PVOfflineRecalculate* m_pvRecalc;

  // Member functions
  std::vector<std::string> m_inputTracks;
  std::string m_pvFitterName;
  std::string m_pvSeedingName;
  double m_pvsChi2Separation;
  double m_pvsChi2SeparationLowMult;

  bool m_useBeamSpotRCut;
  double m_beamSpotRCut;
  double m_beamSpotX;
  double m_beamSpotY;
  std::string m_beamSpotCond;
  double m_resolverBound;
  bool m_veloClosed;

  StatusCode matchVtxByTracks(const LHCb::RecVertex& invtx,
			      std::vector<LHCb::RecVertex>& outvtxvec,
			      LHCb::RecVertex& outvtx);

  std::vector<const LHCb::Track*> readTracks( ) const;

  std::vector<Gaudi::XYZPoint> getSeeds(std::vector<const LHCb::Track*>& rtracks) const;

  void removeTracksByLHCbIDs(std::vector<const LHCb::Track*>& tracks,
		             const std::vector<const LHCb::Track*>& tracks2remove) const;

  void removeTracks(std::vector<const LHCb::Track*>& tracks,
		    const std::vector<const LHCb::Track*>& tracks2remove) const;

  void removeTracksUsedByVertex(std::vector<const LHCb::Track*>& tracks,
				LHCb::RecVertex& rvtx) const;

  bool separatedVertex( const LHCb::RecVertex& rvtx, const std::vector<LHCb::RecVertex>& outvtxvec) const;
  StatusCode UpdateBeamSpot();

  // timing
  bool  m_doTiming;
  ISequencerTimerTool* m_timerTool;
  int   m_timeTotal;
  int   m_timeSeeding;
  int   m_timeFitting;

};
#endif // PVOFFLINETOOL_H
