// $Id: PVSeedTool.h,v 1.2 2008-06-11 19:28:25 witekma Exp $
#ifndef PATPV_PVSEEDTOOL_H
#define PATPV_PVSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IPVSeeding.h"            // Interface
#include "Event/Track.h"


// auxiliary class for searching of clusters of tracks
class vtxCluster {

public:
  double  z;            // z of the cluster
  double  sigsq;        // sigma**2 of the cluster
  double  sigsqmin;     // minimum sigma**2 of the tracks forming cluster
  int     ntracks;      // number of tracks in the cluster
  int     not_merged;   // flag for iterative merging

  vtxCluster() {
    z           = 0.0;
    sigsq       = 0.0;
    sigsqmin    = 0.0;
    ntracks     = 0;
    not_merged  = 0;
  }

};


/** @class PVSeedTool PVSeedTool.h tmp/PVSeedTool.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2005-11-19
 */
class PVSeedTool : public extends<GaudiTool, IPVSeeding> {
public:

  /// Standard constructor
  PVSeedTool( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  ~PVSeedTool( ) override; ///< Destructor

  std::vector<Gaudi::XYZPoint>
  getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
		   const Gaudi::XYZPoint& beamspot) const override;


private:

  std::vector<double> findClusters(std::vector<vtxCluster>& vclus) const;
  void errorForPVSeedFinding(double tx, double ty, double &sigzaq) const;

  double zCloseBeam(const LHCb::Track* track, const Gaudi::XYZPoint& beamspot) const;
  void print_clusters(std::vector<vtxCluster*>& pvclus);


  double m_maxChi2Merge;
  double m_factorToIncreaseErrors;
									
  int    m_minClusterMult;
  double m_dzCloseTracksInCluster;
  int    m_minCloseTracksInCluster;
  int    m_highMult;
  double m_ratioSig2HighMult;
  double m_ratioSig2LowMult;

  double  m_x0MS = 0;         // X0 (tunable) of MS to add for extrapolation of
                          // track parameters to PV
  double  m_scatCons = 0;     // calculated from m_x0MS

};
#endif // PATPV_PVSEEDTOOL_H
